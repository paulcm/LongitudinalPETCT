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

// LongPETCT Logic includes
#include "vtkSlicerLongPETCTLogic.h"



// MRML includes

// VTK includes
#include <vtkNew.h>

#include <vtkMRMLLongPETCTReportNode.h>
#include <vtkMRMLLongPETCTStudyNode.h>

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

// DCMTK includes
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>


const QString vtkSlicerLongPETCTLogic::DATABASEDIRECTORY = "DatabaseDirectory";
const QString vtkSlicerLongPETCTLogic::DATABASECONNECTIONNAME = "LongPETCT";
const QString vtkSlicerLongPETCTLogic::DATABASEFILENAME = "ctkDICOM.sql";

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerLongPETCTLogic);

//----------------------------------------------------------------------------
vtkSlicerLongPETCTLogic::vtkSlicerLongPETCTLogic()
{
  this->SelectedReportNode = NULL;
  this->Initialize();
}

//----------------------------------------------------------------------------
vtkSlicerLongPETCTLogic::~vtkSlicerLongPETCTLogic()
{
  if(this->DICOMDatabase)
    delete this->DICOMDatabase;
}

//----------------------------------------------------------------------------
void vtkSlicerLongPETCTLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkSlicerLongPETCTLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerLongPETCTLogic::RegisterNodes()
{

  if(!this->GetMRMLScene())
    return;

  vtkMRMLLongPETCTReportNode *reportNode = vtkMRMLLongPETCTReportNode::New();
  this->GetMRMLScene()->RegisterNodeClass(reportNode);
  reportNode->Delete();

  vtkMRMLLongPETCTStudyNode *studyNode = vtkMRMLLongPETCTStudyNode::New();
  this->GetMRMLScene()->RegisterNodeClass(studyNode);
  studyNode->Delete();

 // assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerLongPETCTLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerLongPETCTLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerLongPETCTLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
bool vtkSlicerLongPETCTLogic::IsDICOMDataBaseAvailable()
{
   return (this->GetDICOMDatabase());
}

//---------------------------------------------------------------------------
bool vtkSlicerLongPETCTLogic::IsRequiredDataInDICOMDatabase(const QString& patientUID, const QStringList& studyUIDs, const QStringList& seriesUIDs)
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
void vtkSlicerLongPETCTLogic::Initialize()
{
  static bool initialized = false;

  if(!initialized)
    {
      this->DICOMDatabase = NULL;

      initialized = true; // so initialization can be perfomed only once
    }
}

//---------------------------------------------------------------------------
ctkDICOMDatabase* vtkSlicerLongPETCTLogic::GetDICOMDatabase()
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
QStringList vtkSlicerLongPETCTLogic::GetPETCTPatientUIDs()
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
QStringList vtkSlicerLongPETCTLogic::GetPETCTStudyUIDs(const QString& patientUID)
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
bool vtkSlicerLongPETCTLogic::IsPETCTStudy(const QString& studyUID)
{
  ctkDICOMDatabase* tempDatabasePtr = this->GetDICOMDatabase();

  bool isPETCTStudy = true; // assume true because while iterating sudy's serieses it will set to false if a series belongs to a non PET/CT study

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
void vtkSlicerLongPETCTLogic::AddReportNode(vtkMRMLLongPETCTReportNode* reportNode)
{
  if(reportNode)
    ReportNodes.push_back(reportNode);

  std::cout << "//////////// ReportNode added to logic" << std::endl;
//  std::cout << "Report Node Name: " << reportNode->GetName() << " Studies Count: " << reportNode->GetStudyCount() << std::endl;
}
