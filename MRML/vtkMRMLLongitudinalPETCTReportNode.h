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

#include "vtkMRMLLongitudinalPETCTFindingNode.h"
#include <vtkSmartPointer.h>
#include <vtkMRMLModelHierarchyNode.h>

class vtkMRMLLongitudinalPETCTStudyNode;
class vector;
class string;
class vtkMRMLColorNode;
class vtkMRMLColorTableNode;



class vtkEventForwarderCommand;

/// \ingroup Slicer_QtModules_LongitudinalPETCTReportNode
class VTK_SLICER_LONGITUDINALPETCT_MODULE_MRML_EXPORT vtkMRMLLongitudinalPETCTReportNode : public vtkMRMLNode
{
  public:   

  static vtkMRMLLongitudinalPETCTReportNode *New();
  vtkTypeMacro(vtkMRMLLongitudinalPETCTReportNode,vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual vtkMRMLNode* CreateNodeInstance();

  /// Set node attributes
  virtual void ReadXMLAttributes( const char** atts);

  /// Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  /// Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node);

  /// Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {return "Longitudinal PET/CT Report";};

  void Initialize();

  int AddStudy(vtkMRMLLongitudinalPETCTStudyNode* study);
  //bool RemoveStudy(vtkMRMLLongitudinalPETCTStudyNode study);

  void AddFinding(vtkMRMLLongitudinalPETCTFindingNode* finding);

  int GetStudiesCount() const;
  int GetSelectedStudiesCount() const;
  int GetStudiesSelectedForAnalysisCount() const;
  int GetFindingsCount() const;

  vtkMRMLLongitudinalPETCTFindingNode* GetFinding(int index) const;

  bool FindingNameInList(const std::string& name);

  vtkMRMLLongitudinalPETCTStudyNode* GetStudy(int index) const;

  vtkMRMLLongitudinalPETCTStudyNode* GetSelectedStudy(int index);
  vtkMRMLLongitudinalPETCTStudyNode* GetStudySelectedForAnalysis(int index);
  vtkMRMLLongitudinalPETCTStudyNode* GetSelectedStudyFirst();
  vtkMRMLLongitudinalPETCTStudyNode* GetSelectedStudyLast();

  std::vector<vtkMRMLLongitudinalPETCTStudyNode*> GetSelectedStudies();
  std::vector<vtkMRMLLongitudinalPETCTStudyNode*> GetStudiesSelectedForAnalysis();

  int GetIndexOfStudy(const vtkMRMLLongitudinalPETCTStudyNode* study);
  int GetIndexOfSelectedStudy(const vtkMRMLLongitudinalPETCTStudyNode* study);
  int GetIndexOfStudySelectedForAnalysis(const vtkMRMLLongitudinalPETCTStudyNode* study);
  int GetIndexOfFinding(const vtkMRMLLongitudinalPETCTFindingNode* finding);

  vtkGetMacro(UserSelectedStudy,vtkMRMLLongitudinalPETCTStudyNode*);
  vtkSetMacro(UserSelectedStudy,vtkMRMLLongitudinalPETCTStudyNode*);

  vtkGetMacro(UserSelectedFinding,vtkMRMLLongitudinalPETCTFindingNode*);
  vtkSetMacro(UserSelectedFinding,vtkMRMLLongitudinalPETCTFindingNode*);

  vtkGetMacro(FindingTypesColorTable,vtkMRMLColorTableNode*);
  vtkSetMacro(FindingTypesColorTable,vtkMRMLColorTableNode*);

  void SetModelHierarchyNode(vtkMRMLModelHierarchyNode* modelHierarchy);
  vtkGetMacro(ModelHierarchyNode, vtkMRMLModelHierarchyNode*);


  const vtkMRMLColorNode* GetColorNode();

  int GetFindingTypesCount();
  void AddFindingType(std::string name, double color[4]);
  void RemoveLastFindingType();

  const char* GetFindingTypeName(int colorID);
  int GetFindingTypeColorID(const std::string& typeName);

  bool IsFindingTypeInUse(int colorID);
  bool IsStudyInUse(const vtkMRMLLongitudinalPETCTStudyNode* study);

  void RemoveFinding(vtkMRMLLongitudinalPETCTFindingNode* finding);

  int GetNumberOfDefaultFindingTypes();

  void SetScene(vtkMRMLScene* scene);

protected:
  vtkMRMLLongitudinalPETCTReportNode();
  ~vtkMRMLLongitudinalPETCTReportNode();
  vtkMRMLLongitudinalPETCTReportNode(const vtkMRMLLongitudinalPETCTReportNode&);
  void operator=(const vtkMRMLLongitudinalPETCTReportNode&);

  std::vector<vtkMRMLLongitudinalPETCTStudyNode*> Studies;
  std::vector<vtkMRMLLongitudinalPETCTFindingNode*> Findings;

  vtkMRMLLongitudinalPETCTStudyNode* UserSelectedStudy;
  vtkMRMLLongitudinalPETCTFindingNode* UserSelectedFinding;

  vtkSmartPointer<vtkEventForwarderCommand> studyModifiedForwarder;
  vtkSmartPointer<vtkEventForwarderCommand> findingModifiedForwarder;

  vtkMRMLColorTableNode* FindingTypesColorTable;

  vtkSmartPointer<vtkMRMLModelHierarchyNode> ModelHierarchyNode;

private:
  int NumberOfDefaultFindingTypes;

};

#endif

