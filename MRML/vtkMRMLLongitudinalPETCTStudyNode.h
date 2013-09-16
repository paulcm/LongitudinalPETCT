/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLVolumeRenderingParametersNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/
// .NAME vtkMRMLLongitudinalPETCTStudyNode
// This node keeps pointers to the two elements of the Study: annotation
// and markup.
//
// Annotation element will (eventually) contain the Study template and the
// user-initialized fields of the template.
//
// Markup element will point to the hierarchy of the markup elements.
//
// On export/import, this node will be used by the IO logic to determine how
// to code the formatted Study.
// .SECTION Description
// This node keeps pointers to the two elements of the Study: annotation
// and markup.
//

#ifndef __vtkMRMLLongitudinalPETCTStudyNode_h
#define __vtkMRMLLongitudinalPETCTStudyNode_h

// MRML includes
//#include "vtkDoubleArray.h"
#include "vtkMRML.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLScene.h"

#include <vtkSmartPointer.h>

#include <vtkSlicerLongitudinalPETCTModuleMRMLExport.h>

#include <vtkMRMLVolumeRenderingDisplayNode.h>
#include <vtkMRMLLinearTransformNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLAnnotationROINode.h>



/// \ingroup Slicer_QtModules_LongitudinalPETCTStudyNode
class VTK_SLICER_LONGITUDINALPETCT_MODULE_MRML_EXPORT vtkMRMLLongitudinalPETCTStudyNode : public vtkMRMLNode
{
  public:   


  static vtkMRMLLongitudinalPETCTStudyNode *New();
  vtkTypeMacro(vtkMRMLLongitudinalPETCTStudyNode,vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual vtkMRMLNode* CreateNodeInstance();

  /// Set node attributes
  virtual void ReadXMLAttributes( const char** atts);

  /// Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  /// Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node);

  /// Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {return "PETCT_Study";};


  void SetAndObservePETVolumeNodeID(const char* petVolumeNodeID);
  void SetAndObservePETVolumeNodeID(const std::string& petVolumeNodeID);
  vtkGetStringMacro(PETVolumeNodeID);

  vtkGetMacro(PETVolumeNode,vtkMRMLScalarVolumeNode*);


  void SetAndObserveCTVolumeNodeID(const char* ctVolumeNodeID);
  void SetAndObserveCTVolumeNodeID(const std::string& ctVolumeNodeID);
  vtkGetStringMacro(CTVolumeNodeID);

  vtkGetMacro(CTVolumeNode,vtkMRMLScalarVolumeNode*);


  void SetAndObservePETLabelVolumeNodeID(const char* petLabelVolumeNodeID);
  void SetAndObservePETLabelVolumeNodeID(const std::string& petLabelVolumeNodeID);
  vtkGetStringMacro(PETLabelVolumeNodeID);

  vtkGetMacro(PETLabelVolumeNode,vtkMRMLScalarVolumeNode*);


  void SetAndObserveRegistrationTransformNodeID(const char* registrationTransformNodeID);
  void SetAndObserveRegistrationTransformNodeID(const std::string& registrationTransformNodeID);
  vtkGetStringMacro(RegistrationTransformNodeID);

  vtkGetMacro(RegistrationTransformNode,vtkMRMLLinearTransformNode*);


  void SetAndObserveVolumeRenderingDisplayNodeID(const char* volumeRenderingDisplayNodeID);
  void SetAndObserveVolumeRenderingDisplayNodeID(const std::string& volumeRenderingDisplayNodeID);
  vtkGetStringMacro(VolumeRenderingDisplayNodeID);

  vtkGetMacro(VolumeRenderingDisplayNode,vtkMRMLVolumeRenderingDisplayNode*);

  vtkBooleanMacro(SelectedForSegmentation, bool);
  vtkGetMacro(SelectedForSegmentation, bool);
  vtkSetMacro(SelectedForSegmentation, bool);

  vtkBooleanMacro(SelectedForAnalysis, bool);
  vtkGetMacro(SelectedForAnalysis, bool);
  vtkSetMacro(SelectedForAnalysis, bool);

  void UpdateScene(vtkMRMLScene* scene);
  void SetScene(vtkMRMLScene* scene);
  //void ProcessMRMLEvents(vtkObject *caller, unsigned long event, void *callData);
  void UpdateReferences();
  void UpdateReferenceID(const char *oldID, const char *newID);

protected:
  vtkMRMLLongitudinalPETCTStudyNode();
  ~vtkMRMLLongitudinalPETCTStudyNode();
  vtkMRMLLongitudinalPETCTStudyNode(const vtkMRMLLongitudinalPETCTStudyNode&);
  void operator=(const vtkMRMLLongitudinalPETCTStudyNode&);

  void ObserveRegistrationTransform(bool observe);

  vtkSetStringMacro(PETVolumeNodeID);
  vtkSetStringMacro(CTVolumeNodeID);
  vtkSetStringMacro(PETLabelVolumeNodeID);
  vtkSetStringMacro(RegistrationTransformNodeID);
  vtkSetStringMacro(VolumeRenderingDisplayNodeID);


  bool SelectedForSegmentation;
  bool SelectedForAnalysis;

  char* PETVolumeNodeID;
  char* CTVolumeNodeID;
  char* PETLabelVolumeNodeID;
  char* RegistrationTransformNodeID;
  char* VolumeRenderingDisplayNodeID;


  vtkMRMLScalarVolumeNode* PETVolumeNode;
  vtkMRMLScalarVolumeNode* CTVolumeNode;
  vtkMRMLScalarVolumeNode* PETLabelVolumeNode;

  vtkMRMLLinearTransformNode* RegistrationTransformNode;
  vtkMRMLVolumeRenderingDisplayNode* VolumeRenderingDisplayNode;
};

#endif

