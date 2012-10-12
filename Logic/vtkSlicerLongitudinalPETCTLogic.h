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

// .NAME vtkSlicerLongitudinalPETCTLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerLongitudinalPETCTLogic_h
#define __vtkSlicerLongitudinalPETCTLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"


// MRML includes


// STD includes
#include <cstdlib>

//Qt inlcudes
#include <QStringList>

// CTK forward declarations
class ctkDICOMDatabase;

#include "vtkSlicerLongitudinalPETCTModuleLogicExport.h"

class vtkMRMLLongitudinalPETCTReportNode;
class vtkMRMLLongitudinalPETCTStudyNode;
class vtkMRMLVolumeNode;
class vtkMRMLColorNode;
class vtkMRMLColorTableNode;
class vtkMRMLScene;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_LONGITUDINALPETCT_MODULE_LOGIC_EXPORT vtkSlicerLongitudinalPETCTLogic :
  public vtkSlicerModuleLogic
{
public:

  static vtkSlicerLongitudinalPETCTLogic *New();
  vtkTypeMacro(vtkSlicerLongitudinalPETCTLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);

  void Initialize();

  static bool CenterStudyVolumeNodes(vtkMRMLLongitudinalPETCTStudyNode* study, vtkMRMLScene* scene);

  const char* GetDirectoryOfDICOMSeries(const char* sopInstanceUID);

  /**
   * Returns a list with UIDs of all patients from the DICOM database with PET/CT studies.
   *
   * \return List with patient UIDs as strings. Empty list if none have been found or no connection to DICOM database available.
   */
  QStringList GetPETCTPatientUIDs();

  /**
   * Returns a list with UIDs of all PET/CT studies for a patient from the DICOM database.
   *
   * \param UID of the patient
   * \return List with study UIDs of PET/CT studies. Empty list if no PET/CT has been found or no connection to DICOM database available.
   */
  QStringList GetPETCTStudyUIDs(const QString& patientUID);

  /**
   * Checks whether a connection to the DICOM database exists.
   *
   * \return true if connection exists and is open; otherwise false
   */
  bool IsDICOMDataBaseAvailable();

  /**
   * Checks whether all the data needed for a Report is in the DICOM database.
   *
   * \return true if all data exists; otherwise false
   */
  bool IsRequiredDataInDICOMDatabase(const QString& patientUID, const QStringList& studyUIDs, const QStringList& seriesUIDs);


  void AddReportNode(vtkMRMLLongitudinalPETCTReportNode* reportNode);

  vtkGetMacro(SelectedReportNode, vtkMRMLLongitudinalPETCTReportNode*);
  vtkSetMacro(SelectedReportNode, vtkMRMLLongitudinalPETCTReportNode*);


  vtkMRMLColorTableNode* GetDefaultFindingTypesColorTable(vtkMRMLColorNode* defaultEditorColorNode);

protected:

  static const QString DATABASEDIRECTORY; // for DatabaseDirectory path
  static const QString DATABASEFILENAME; // for creating connections
  static const QString DATABASECONNECTIONNAME; // for name of DICOM DB connectio

  vtkSlicerLongitudinalPETCTLogic();
  virtual ~vtkSlicerLongitudinalPETCTLogic();

  ctkDICOMDatabase* GetDICOMDatabase();
  vtkSetMacro(DICOMDatabase,ctkDICOMDatabase*);

  vtkMRMLLongitudinalPETCTReportNode*  SelectedReportNode;

  /**
   * Returns whether a study contains of at least one PET and one CT image series.
   *
   * \param UID of study
   * \return true if study modality is PT\CT; otherwise false
   */
  bool IsPETCTStudy(const QString& studyUID);


  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();
  virtual void UpdateFromMRMLScene();
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);
private:

  vtkSlicerLongitudinalPETCTLogic(const vtkSlicerLongitudinalPETCTLogic&); // Not implemented
  void operator=(const vtkSlicerLongitudinalPETCTLogic&);               // Not implemented

  ctkDICOMDatabase *DICOMDatabase;

  std::vector<vtkMRMLLongitudinalPETCTReportNode*> ReportNodes;
};

#endif
