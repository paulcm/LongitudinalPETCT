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

class vtkMRMLScalarVolumeNode;
class vtkMRMLLinearTransformNode;
class vtkMRMLAnnotationROINode;
class vtkMRMLVolumeRenderingDisplayNode;


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

  vtkSetMacro(PETVolumeNode, vtkMRMLScalarVolumeNode*);
  vtkSetMacro(CTVolumeNode, vtkMRMLScalarVolumeNode*);
  vtkSetMacro(CenteringTransform, vtkMRMLLinearTransformNode*);
  vtkSetMacro(PETLabelVolumeNode, vtkMRMLScalarVolumeNode*);
  vtkSetMacro(VolumeRenderingDisplayNode,vtkMRMLVolumeRenderingDisplayNode*);

  //vtkSetMacro(SegmentationROI,vtkMRMLAnnotationROINode*);

  void SetSegmentationROI(vtkMRMLAnnotationROINode* roi);

  vtkGetMacro(PETVolumeNode, vtkMRMLScalarVolumeNode*);
  vtkGetMacro(CTVolumeNode, vtkMRMLScalarVolumeNode*);
  vtkGetMacro(CenteringTransform, vtkMRMLLinearTransformNode*);
  vtkGetMacro(PETLabelVolumeNode, vtkMRMLScalarVolumeNode*);
  vtkGetMacro(SegmentationROI,vtkMRMLAnnotationROINode*);
  vtkGetMacro(VolumeRenderingDisplayNode,vtkMRMLVolumeRenderingDisplayNode*);

  vtkBooleanMacro(SelectedForSegmentation, bool);
  vtkGetMacro(SelectedForSegmentation, bool);
  vtkSetMacro(SelectedForSegmentation, bool);

  vtkBooleanMacro(SelectedForAnalysis, bool);
  vtkGetMacro(SelectedForAnalysis, bool);
  vtkSetMacro(SelectedForAnalysis, bool);

  vtkGetMacro(CenteredVolumes, bool);

  void SetCenteredVolumes(bool centered);

  void Initialize();

protected:
  vtkMRMLLongitudinalPETCTStudyNode();
  ~vtkMRMLLongitudinalPETCTStudyNode();
  vtkMRMLLongitudinalPETCTStudyNode(const vtkMRMLLongitudinalPETCTStudyNode&);
  void operator=(const vtkMRMLLongitudinalPETCTStudyNode&);

  void ObserveCenteringTransform(bool observe);

  bool SelectedForSegmentation;
  bool SelectedForAnalysis;
  bool CenteredVolumes;

  vtkMRMLScalarVolumeNode* PETVolumeNode;
  vtkMRMLScalarVolumeNode* CTVolumeNode;

  vtkMRMLScalarVolumeNode* PETLabelVolumeNode;
  vtkMRMLLinearTransformNode* CenteringTransform;

  vtkMRMLAnnotationROINode* SegmentationROI;

  vtkMRMLVolumeRenderingDisplayNode* VolumeRenderingDisplayNode;
};

#endif

