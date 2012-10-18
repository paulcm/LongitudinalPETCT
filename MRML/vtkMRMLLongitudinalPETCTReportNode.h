/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLVolumeRenderingParametersNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/
// .NAME vtkMRMLLongitudinalPETCTReportNode
// This node keeps pointers to the two elements of the report: annotation
// and markup.
//
// Annotation element will (eventually) contain the report template and the
// user-initialized fields of the template.
//
// Markup element will point to the hierarchy of the markup elements.
//
// On export/import, this node will be used by the IO logic to determine how
// to code the formatted report.
// .SECTION Description
// This node keeps pointers to the two elements of the report: annotation
// and markup.
//

#ifndef __vtkMRMLLongitudinalPETCTReportNode_h
#define __vtkMRMLLongitudinalPETCTReportNode_h

// MRML includes
//#include "vtkDoubleArray.h"
#include "vtkMRML.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLScene.h"

#include <vtkSlicerLongitudinalPETCTModuleMRMLExport.h>

#include "vtkMRMLLongitudinalPETCTStudyNode.h"
#include "vtkMRMLLongitudinalPETCTFindingNode.h"
#include <vtkSmartPointer.h>
#include <vtkMRMLModelHierarchyNode.h>
#include <vtkMRMLColorTableNode.h>

class vector;
class string;
class vtkMRMLColorNode;



class vtkEventForwarderCommand;

/// \ingroup Slicer_QtModules_LongitudinalPETCTReportNode
class VTK_SLICER_LONGITUDINALPETCT_MODULE_MRML_EXPORT vtkMRMLLongitudinalPETCTReportNode : public vtkMRMLNode
{
  public:   

  static vtkMRMLLongitudinalPETCTReportNode *New();
  vtkTypeMacro(vtkMRMLLongitudinalPETCTReportNode,vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  typedef std::vector<std::string> IDsVectorType;

  virtual vtkMRMLNode* CreateNodeInstance();

  /// Set node attributes
  virtual void ReadXMLAttributes( const char** atts);

  /// Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  /// Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node);

  /// Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {return "PETCT_Report";};


  int AddStudyNodeID(const char* studyNodeID);
  bool RemoveStudyNodeID(const char* studyNodeID);

  bool AddFindingNodeID(const char* findingNodeID);
  bool RemoveFindingNodeID(const char* findingNodeID);

  int GetNumberOfStudyNodeIDs() const;
  int GetNumberOfSelectedStudies() const;
  int GetNumberOfSelectedStudiesSelectedForAnalysis() const;
  int GetNumberOfFindingNodeIDs() const;

  vtkMRMLLongitudinalPETCTFindingNode* GetFinding(int index) const;

  bool FindingNameInList(const std::string& name);

  const char* GetNthStudyNodeID(int index);
  const char* GetNthFindingNodeID(int index);

  vtkMRMLLongitudinalPETCTStudyNode* GetStudy(int index) const;

  vtkMRMLLongitudinalPETCTStudyNode* GetSelectedStudy(int index);
  vtkMRMLLongitudinalPETCTStudyNode* GetSelectedStudySelectedForAnalysis(int index);
  vtkMRMLLongitudinalPETCTStudyNode* GetSelectedStudyFirst();
  vtkMRMLLongitudinalPETCTStudyNode* GetSelectedStudyLast();

  std::vector<vtkMRMLLongitudinalPETCTStudyNode*> GetSelectedStudies();
  std::vector<vtkMRMLLongitudinalPETCTStudyNode*> GetSelectedStudiesSelectedForAnalysis();

  int GetIndexOfStudyNodeID(const char* studyNodeID);
  int GetIndexOfSelectedStudy(const vtkMRMLLongitudinalPETCTStudyNode* study);
  int GetIndexOfSelectedStudySelectedForAnalysis(const vtkMRMLLongitudinalPETCTStudyNode* study);
  int GetIndexOfFindingNodeID(const char* findingNodeID);

  void SetReportsModelHierarchyNodeID(const char* modelHierarchyNodeID);
  vtkGetStringMacro(ModelHierarchyNodeID);

  void SetAndObserveFindingTypesColorTableNodeID(const char* findingTypesColorTableNodeID);
  vtkGetStringMacro(FindingTypesColorTableNodeID);

  vtkGetMacro(FindingTypesColorTableNode, vtkMRMLColorTableNode*);


  vtkSetStringMacro(UserSelectedStudyNodeID);
  vtkGetStringMacro(UserSelectedStudyNodeID);

  vtkSetStringMacro(UserSelectedFindingNodeID);
  vtkGetStringMacro(UserSelectedFindingNodeID);

  vtkSetStringMacro(ColorNodeID);
  vtkGetStringMacro(ColorNodeID);

  vtkMRMLLongitudinalPETCTStudyNode* GetUserSelectedStudyNode();
  vtkMRMLLongitudinalPETCTFindingNode* GetUserSelectedFindingNode();
  const vtkMRMLColorNode* GetColorNode();

  int GetFindingTypesCount();
  void AddFindingType(const char* name, double color[4]);
  void RemoveLastFindingType();

  const char* GetFindingTypeName(int colorID);
  int GetFindingTypeColorID(const std::string& typeName);

  bool IsFindingTypeInUse(int colorID);
  bool IsStudyInUse(const vtkMRMLLongitudinalPETCTStudyNode* study);


  int GetNumberOfDefaultFindingTypes();

  void UpdateReferences();
  void ProcessMRMLEvents(vtkObject *caller, unsigned long event, void *callData);
  void SetScene(vtkMRMLScene* scene);

protected:
  vtkMRMLLongitudinalPETCTReportNode();
  ~vtkMRMLLongitudinalPETCTReportNode();
  vtkMRMLLongitudinalPETCTReportNode(const vtkMRMLLongitudinalPETCTReportNode&);
  void operator=(const vtkMRMLLongitudinalPETCTReportNode&);

  bool RemoveAllStudyNodeIDs();
  bool RemoveAllFindingNodeIDs();

  IDsVectorType StudyNodeIDs;
  IDsVectorType FindingNodeIDs;

  vtkMRMLColorTableNode* FindingTypesColorTableNode;

  bool IsStudyNodeIDPresent(const char* studyNodeID);
  bool IsFindingNodeIDPresent(const char* findingNodeID);

  vtkSetStringMacro(ModelHierarchyNodeID);
  vtkSetStringMacro(FindingTypesColorTableNodeID);

  char* ModelHierarchyNodeID;
  char* UserSelectedStudyNodeID;
  char* UserSelectedFindingNodeID;
  char* FindingTypesColorTableNodeID;
  char* ColorNodeID;

  vtkSmartPointer<vtkIntArray> ObservedEvents;

private:
  int NumberOfDefaultFindingTypes;

};

#endif

