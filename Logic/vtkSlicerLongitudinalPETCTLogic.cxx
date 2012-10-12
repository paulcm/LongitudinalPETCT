/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// LongitudinalPETCT Logic includes
#include "vtkSlicerLongitudinalPETCTLogic.h"



// MRML includes

// VTK includes
#include <vtkNew.h>

#include <vtkMRMLLongitudinalPETCTReportNode.h>
#include <vtkMRMLLongitudinalPETCTStudyNode.h>
#include <vtkMRMLLongitudinalPETCTFindingNode.h>
#include <vtkMRMLLongitudinalPETCTSegmentationNode.h>

// STD includes
#include <cassert>

// CTK includes
#include <ctkDICOMDatabase.h>
#include <ctkDICOMIndexer.h>

// Qt includes
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMap>
#include <QFileInfo>
#include <QDir>

// DCMTK includes
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>

#include <vtkNew.h>
#include <vtkMatrix4x4.h>
#include <vtkImageData.h>
#include <vtkLookupTable.h>

#include <vtkMRMLVolumeNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLLinearTransformNode.h>
#include <vtkMRMLColorTableNode.h>
#include <vtkMRMLColorLogic.h>


const QString vtkSlicerLongitudinalPETCTLogic::DATABASEDIRECTORY = "DatabaseDirectory";
const QString vtkSlicerLongitudinalPETCTLogic::DATABASECONNECTIONNAME = "LongitudinalPETCT";
const QString vtkSlicerLongitudinalPETCTLogic::DATABASEFILENAME = "ctkDICOM.sql";



//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerLongitudinalPETCTLogic);

//----------------------------------------------------------------------------
vtkSlicerLongitudinalPETCTLogic::vtkSlicerLongitudinalPETCTLogic()
{
  this->SelectedReportNode = NULL;
  this->Initialize();
}

//----------------------------------------------------------------------------
vtkSlicerLongitudinalPETCTLogic::~vtkSlicerLongitudinalPETCTLogic()
{
  if(this->DICOMDatabase)
    delete this->DICOMDatabase;
}

//----------------------------------------------------------------------------
void vtkSlicerLongitudinalPETCTLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

bool vtkSlicerLongitudinalPETCTLogic::CenterStudyVolumeNodes(vtkMRMLLongitudinalPETCTStudyNode* study, vtkMRMLScene* scene)
{
  if (study == NULL)
    return false;

  vtkMRMLScalarVolumeNode* petVolume = study->GetPETVolumeNode();
  vtkMRMLScalarVolumeNode* ctVolume = study->GetCTVolumeNode();
  vtkMRMLScalarVolumeNode* petLabelVolume = study->GetPETLabelVolumeNode();

  if (petVolume == NULL || ctVolume == NULL || petLabelVolume == NULL || petVolume->GetImageData() == NULL
      || ctVolume->GetImageData() == NULL || petLabelVolume->GetImageData() == NULL)
    {
      vtkDebugMacro(
          "vtkSlicerLongitudinalPETCTLogic: PETVolume and/or CTVolume to be centered have NULL value or their image data is NULL");
      return false;
    }

  double petSpacing[3], ctSpacing[3];
  int petDimensions[3], ctDimensions[3];

  petVolume->GetSpacing(petSpacing);
  ctVolume->GetSpacing(ctSpacing);

  petVolume->GetImageData()->GetDimensions(petDimensions);
  ctVolume->GetImageData()->GetDimensions(ctDimensions);

  double petCapacity = petDimensions[0] * petSpacing[0] * petDimensions[1]
      * petSpacing[1] * petDimensions[2] * petSpacing[2];
  double ctCapacity = ctDimensions[0] * ctSpacing[0] * ctDimensions[1]
      * ctSpacing[1] * ctDimensions[2] * ctSpacing[2];

  vtkSmartPointer<vtkMRMLVolumeNode> referenceVolume;
  vtkSmartPointer<vtkMRMLVolumeNode> secondaryVolume;

  if (petCapacity >= ctCapacity)
    {
      referenceVolume = petVolume;
      secondaryVolume = ctVolume;
    }

  else
    {
      referenceVolume = ctVolume;
      secondaryVolume = petVolume;
    }


  vtkNew<vtkMatrix4x4> originalIJKToRAS;
  referenceVolume->GetIJKToRASMatrix(originalIJKToRAS.GetPointer());

  vtkNew<vtkMatrix4x4> referenceIJKToRAS;
  referenceIJKToRAS->DeepCopy(originalIJKToRAS.GetPointer());


  char* scanOrder =
      const_cast<char*>(vtkMRMLVolumeNode::ComputeScanOrderFromIJKToRAS(
          referenceIJKToRAS.GetPointer()));

  bool center =vtkMRMLVolumeNode::ComputeIJKToRASFromScanOrder(scanOrder,
      referenceVolume->GetSpacing(),
      referenceVolume->GetImageData()->GetDimensions(), true,
      referenceIJKToRAS.GetPointer());

  if (!center)
    {
      vtkDebugMacro(
          "vtkSlicerLongitudinalPETCTLogic: Centering of reference volume failed!");
      return false;
    }

  double translation[3];
  translation[0] = referenceIJKToRAS->GetElement(0, 3)
      - originalIJKToRAS->GetElement(0, 3);
  translation[1] = referenceIJKToRAS->GetElement(1, 3)
      - originalIJKToRAS->GetElement(1, 3);
  translation[2] = referenceIJKToRAS->GetElement(2, 3)
      - originalIJKToRAS->GetElement(2, 3);

  vtkNew<vtkMatrix4x4> translationMatrix;
  translationMatrix->Identity();

  for (int i = 0; i < 3; ++i)
    {
      translationMatrix->SetElement(i, 3, translation[i]);
    }


  vtkNew<vtkMRMLLinearTransformNode> translationTransform;
  translationTransform->ApplyTransformMatrix(
      translationMatrix.GetPointer());
  translationTransform->SetHideFromEditors(false);

  std::string pre = "CenteringTransform_";
  std::string tag = study->GetAttribute("DICOM.StudyDate");
  std::string name(pre+tag);
  translationTransform->SetName(name.c_str());

  scene->AddNode(translationTransform.GetPointer());

  petVolume->SetAndObserveTransformNodeID(
      translationTransform->GetID());
  ctVolume->SetAndObserveTransformNodeID(
      translationTransform->GetID());
  petLabelVolume->SetAndObserveTransformNodeID(translationTransform->GetID());

  study->SetCenteringTransform(translationTransform.GetPointer());


  return true;
}
//  vtkNew<vtkMatrix4x4> originalIJKToRAS;
//  referenceVolume->GetIJKToRASMatrix(originalIJKToRAS.GetPointer());
//
//  vtkNew<vtkMatrix4x4> referenceIJKToRAS;
//  referenceIJKToRAS->DeepCopy(originalIJKToRAS.GetPointer());
//
//  char* scanOrder = const_cast<char*>(vtkMRMLVolumeNode::ComputeScanOrderFromIJKToRAS(referenceIJKToRAS.GetPointer()));
//  bool center = vtkMRMLVolumeNode::ComputeIJKToRASFromScanOrder(scanOrder,referenceVolume->GetSpacing(),referenceVolume->GetImageData()->GetDimensions(),true,referenceIJKToRAS.GetPointer());
//
//  if(!center)
//    {
//      vtkDebugMacro("vtkSlicerLongitudinalPETCTLogic: Centering of bigger volume failed");
//      return false;
//    }
//
//  double translation[3];
//  translation[0] = referenceIJKToRAS->GetElement(0,3) - originalIJKToRAS->GetElement(0,3);
//  translation[1] = referenceIJKToRAS->GetElement(1,3) - originalIJKToRAS->GetElement(1,3);
//  translation[2] = referenceIJKToRAS->GetElement(2,3) - originalIJKToRAS->GetElement(2,3);
//
//
//  // centering reference volume
//  referenceVolume->SetIJKToRASMatrix(referenceIJKToRAS.GetPointer());
//  vtkNew<vtkMatrix4x4> referenceRASToIJK;
//
//  referenceRASToIJK->DeepCopy(referenceIJKToRAS.GetPointer());
//  referenceRASToIJK->Invert();
//
//  referenceVolume->SetIJKToRASMatrix(referenceIJKToRAS.GetPointer());
//  referenceVolume->SetRASToIJKMatrix(referenceRASToIJK.GetPointer());
//
//
//  // centering secondary volume
//  vtkNew<vtkMatrix4x4> secondaryIJKToRAS;
//  secondaryVolume->GetIJKToRASMatrix(secondaryIJKToRAS.GetPointer());
//
//  // modify IJKToRAS with translation of reference volume
//  secondaryIJKToRAS->SetElement(0,3, secondaryIJKToRAS->GetElement(0,3)+translation[0]);
//  secondaryIJKToRAS->SetElement(1,3, secondaryIJKToRAS->GetElement(1,3)+translation[1]);
//  secondaryIJKToRAS->SetElement(2,3, secondaryIJKToRAS->GetElement(2,3)+translation[2]);
//
//  vtkNew<vtkMatrix4x4> secondaryRASToIJK;
//  secondaryRASToIJK->DeepCopy(secondaryIJKToRAS.GetPointer());
//  secondaryRASToIJK->Invert();
//
//  secondaryVolume->SetIJKToRASMatrix(secondaryRASToIJK.GetPointer());
//  secondaryVolume->SetRASToIJKMatrix(secondaryRASToIJK.GetPointer());
//
//
//  referenceVolume->Modified();
//  secondaryVolume->Modified();
//
//  return true;



//---------------------------------------------------------------------------
void vtkSlicerLongitudinalPETCTLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerLongitudinalPETCTLogic::RegisterNodes()
{

  if(!this->GetMRMLScene())
    return;

  vtkNew<vtkMRMLLongitudinalPETCTReportNode> reportNode;
  this->GetMRMLScene()->RegisterNodeClass(reportNode.GetPointer());

  vtkNew<vtkMRMLLongitudinalPETCTStudyNode> studyNode;
  this->GetMRMLScene()->RegisterNodeClass(studyNode.GetPointer());

  vtkNew<vtkMRMLLongitudinalPETCTFindingNode> findingNode;
  this->GetMRMLScene()->RegisterNodeClass(findingNode.GetPointer());

  vtkNew<vtkMRMLLongitudinalPETCTSegmentationNode> segmentationNode;
  this->GetMRMLScene()->RegisterNodeClass(segmentationNode.GetPointer());

 // assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerLongitudinalPETCTLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerLongitudinalPETCTLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerLongitudinalPETCTLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
bool vtkSlicerLongitudinalPETCTLogic::IsDICOMDataBaseAvailable()
{
   return (this->GetDICOMDatabase());
}

//---------------------------------------------------------------------------
bool vtkSlicerLongitudinalPETCTLogic::IsRequiredDataInDICOMDatabase(const QString& patientUID, const QStringList& studyUIDs, const QStringList& seriesUIDs)
{
  bool dataIsInDICOMDB = true;

  ctkDICOMDatabase* tempDatabasePtr = this->GetDICOMDatabase();

  if (tempDatabasePtr) // only searching if database available
    {
      QStringList tempPatientUIDs = tempDatabasePtr->patients(); // all patient UIDs from DICOM database

      if (tempPatientUIDs.contains(patientUID)) // continue only if requested patient UID in database
        {
          QStringList tempStudyUIDs = tempDatabasePtr->studiesForPatient(patientUID); // all studys from DICOM database for requested patient UID

          for (int i = 0; i < studyUIDs.size(); ++i) // iterate over all given study UIDs
            {
              QString tempRequestedStudyUID = studyUIDs.at(i);

              if (tempStudyUIDs.contains(tempRequestedStudyUID)) // continue only if patient's studies from DB contain requested study
                {
                  QStringList tempSeriesUIDs = tempDatabasePtr->seriesForStudy(tempRequestedStudyUID);

                  for (int j = 0; j < seriesUIDs.size(); ++j) // iterate over all given image series UIDs
                    {
                      QString tempRequestedSeriesUID = seriesUIDs.at(j);

                      if (!tempSeriesUIDs.contains(tempRequestedSeriesUID)) // if requested series is not in database set result false
                        {
                          dataIsInDICOMDB = false;
                          break;
                        }
                    }
                }
              else // requested study is not in database
                {
                  dataIsInDICOMDB = false;
                  break;
                }
            }
        }
    }
  else // no database avaialbe
    return false;

  return dataIsInDICOMDB;
}

//---------------------------------------------------------------------------
void vtkSlicerLongitudinalPETCTLogic::Initialize()
{
  this->DICOMDatabase = NULL;
}

//---------------------------------------------------------------------------
ctkDICOMDatabase* vtkSlicerLongitudinalPETCTLogic::GetDICOMDatabase()
{
  if (this->DICOMDatabase)
    return this->DICOMDatabase;

  else
    {
      QSettings settings;
      QString dbPath = settings.value(DATABASEDIRECTORY, "").toString();

      if (dbPath.isEmpty())
        {
          vtkWarningMacro(
              "InitializeDICOMDatabase: no DatabaseDirectory path found, please update the settings.\nUsing " << qPrintable(dbPath));
          return NULL;
        }

      this->DICOMDatabase = new ctkDICOMDatabase();

      QString fullDatabaseFilename = dbPath + "/" + DATABASEFILENAME;

      this->GetDICOMDatabase()->openDatabase(fullDatabaseFilename,
          DATABASECONNECTIONNAME);

      if (this->GetDICOMDatabase()->isOpen() == false)
        {
          vtkWarningMacro(
              "OpenDICOMDatabase: DICOM database could not be opened using " << qPrintable(fullDatabaseFilename));
          delete this->DICOMDatabase; // free allocated memory
          this->DICOMDatabase = NULL;

          return NULL;
        }

      QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
      db.setDatabaseName(fullDatabaseFilename);

      if (db.open() == false)
        {
          vtkWarningMacro("OpenDICOMQSqlDatabase: QSql DICOM database could not be opened using " << qPrintable(fullDatabaseFilename));
          delete this->DICOMDatabase; // free allocated memory
          this->DICOMDatabase = NULL;

          return NULL;
        }

    }

  return this->DICOMDatabase;
}

//---------------------------------------------------------------------------
QStringList vtkSlicerLongitudinalPETCTLogic::GetPETCTPatientUIDs()
{

  QStringList petCtPatientUIDs;

  ctkDICOMDatabase* tempDatabasePtr = this->GetDICOMDatabase();

  if (tempDatabasePtr)
    {
      QStringList tempAllPatients = tempDatabasePtr->patients();

      for (int i = 0; i < tempAllPatients.size(); ++i)
        {
          QString tempPatientUID = tempAllPatients.at(i);

          if (!this->GetPETCTStudyUIDs(tempPatientUID).empty()) // check if patient has any PET\CT Study
            petCtPatientUIDs.append(tempPatientUID);
        }
    }

  return petCtPatientUIDs;
}


//---------------------------------------------------------------------------
QStringList vtkSlicerLongitudinalPETCTLogic::GetPETCTStudyUIDs(const QString& patientUID)
{
  QStringList petCtStudyUIDs;

  ctkDICOMDatabase* tempDatabasePtr = this->GetDICOMDatabase();

  if (tempDatabasePtr)
    {
      QStringList tempStudiesUIDs = tempDatabasePtr->studiesForPatient(patientUID); // all studies for this patient

      for (int i = 0; i < tempStudiesUIDs.size(); ++i)
        {
          QString tempStudyUID = tempStudiesUIDs.at(i);

          if (this->IsPETCTStudy(tempStudyUID) && !petCtStudyUIDs.contains(tempStudyUID)) // check if study is of modality PT\CT and not in list yet
            {
              petCtStudyUIDs.append(tempStudyUID);

            }
        }
    }

  return petCtStudyUIDs;
}


//---------------------------------------------------------------------------
bool vtkSlicerLongitudinalPETCTLogic::IsPETCTStudy(const QString& studyUID)
{
  ctkDICOMDatabase* tempDatabasePtr = this->GetDICOMDatabase();

  bool isPETCTStudy = true; // assume true because while iterating sudy's serieses it will set to false if a series beLongitudinals to a non PET/CT study

  if (tempDatabasePtr && !studyUID.isEmpty()) // don't continue if no db connection or no StudyUID available
    {
      DcmFileFormat fileformat;
      OFCondition status;
      OFString modality;
      QString qmodality;

      QStringList tempSeriesList = tempDatabasePtr->seriesForStudy(studyUID); // UID's of all series in the study

      if (tempSeriesList.size() >= 2) // at least 1 PET and 1 CT image series needed
        {
          for (int i = 0; i < tempSeriesList.size(); ++i) // iterating through all of study's image series
            {
              qmodality = ""; // reset every iteration step to avoid comparison with old value

              QString tempSeriesUID = tempSeriesList.at(i);
              QStringList tempImagesList = tempDatabasePtr->filesForSeries(tempSeriesUID); // file names of all DICOM images in series

              if (!tempImagesList.isEmpty()) // at least 1 image needed
                {
                  QString tempFirstImageFilename = tempImagesList.at(0);

                  status = fileformat.loadFile(tempFirstImageFilename.toStdString().c_str()); // load DICOM file with dcmtk

                  if (status.good())
                    {

                      if(fileformat.getDataset()->findAndGetOFStringArray(DCM_ModalitiesInStudy, modality).good())
                          qmodality = QString(modality.c_str()); // only convert to QString if modality was found

                      if (!(qmodality.contains("PT") && qmodality.contains("CT")))
                        isPETCTStudy = false;
                    }
                }
              else
                isPETCTStudy = false; // no image in series
            }
        }
      else
        isPETCTStudy = false; // study doesn't contain at least 2 image series
    }
  else
    return false; // no DB connection or no StudyUID


  return isPETCTStudy;
}

//---------------------------------------------------------------------------
void vtkSlicerLongitudinalPETCTLogic::AddReportNode(vtkMRMLLongitudinalPETCTReportNode* reportNode)
{
  if(reportNode)
    ReportNodes.push_back(reportNode);
}

//---------------------------------------------------------------------------
vtkMRMLColorTableNode* vtkSlicerLongitudinalPETCTLogic::GetDefaultFindingTypesColorTable(vtkMRMLColorNode* defaultEditorColorNode)
{
  if(this->GetMRMLScene() == NULL)
    return NULL;

  //vtkSmartPointer<vtkMRMLColorTableNode> colorTable = vtkMRMLColorLogic::CopyNode(defaultEditorColorNode,"LongitudinalitudinalPETCT_ColorTable");
  vtkNew<vtkMRMLColorTableNode> colorTable;
  //colorTable->Copy(defaultEditorColorNode);

  colorTable->SetType(vtkMRMLColorTableNode::User);
  colorTable->SetSaveWithScene(true);
  colorTable->SetNumberOfColors(6);
  colorTable->GetLookupTable()->SetTableRange(0.,6.);

  double color[4];

  defaultEditorColorNode->GetColor(0,color);
  colorTable->SetColor(0,"None",color[0],color[1],color[2],color[3]);

  defaultEditorColorNode->GetColor(7,color);
  colorTable->SetColor(1,"Tumor",color[0],color[1],color[2],color[3]);

  defaultEditorColorNode->GetColor(23,color);
  colorTable->SetColor(2,"Lymph Node",color[0],color[1],color[2],color[3]);

  defaultEditorColorNode->GetColor(216,color);
  colorTable->SetColor(3,"Liver",color[0],color[1],color[2],color[3]);

  defaultEditorColorNode->GetColor(105,color);
  colorTable->SetColor(4,"Cerebellum",color[0],color[1],color[2],color[3]);

  defaultEditorColorNode->GetColor(191,color);
  colorTable->SetColor(5,"Aorta",color[0],color[1],color[2],color[3]);

  colorTable->SetName("LongitudinalitudinalPETCT_ColorTable");
  colorTable->SetScene(this->GetMRMLScene());
  colorTable->SetHideFromEditors(false);
  this->GetMRMLScene()->AddNode(colorTable.GetPointer());

  return colorTable.GetPointer();
}

const char*
vtkSlicerLongitudinalPETCTLogic::GetDirectoryOfDICOMSeries(const char* sopInstanceUID)
{
  QString directoryPath = "";
  ctkDICOMDatabase* tempDatabase = this->GetDICOMDatabase();

  if (tempDatabase)
    {
      QString fileForInstance = tempDatabase->fileForInstance(sopInstanceUID);
      QFileInfo fileInfo(fileForInstance);
      QDir directory = fileInfo.absoluteDir();

      if(directory.exists())
        directoryPath.append(directory.absolutePath());
    }

  return directoryPath.toStdString().c_str();
}
