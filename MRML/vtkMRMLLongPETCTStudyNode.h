/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLVolumeRenderingParametersNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/
// .NAME vtkMRMLLongPETCTStudyNode
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

#ifndef __vtkMRMLLongPETCTStudyNode_h
#define __vtkMRMLLongPETCTStudyNode_h

// MRML includes
//#include "vtkDoubleArray.h"
#include "vtkMRML.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLScene.h"

#include <vtkSmartPointer.h>

#include <vtkSlicerLongPETCTModuleMRMLExport.h>

class vtkMRMLScalarVolumeNode;
class vtkMRMLLinearTransformNode;
class vtkMRMLAnnotationROINode;
class vtkMRMLVolumeRenderingDisplayNode;



/// \ingroup Slicer_QtModules_LongPETCTStudyNode
class VTK_SLICER_LONGPETCT_MODULE_MRML_EXPORT vtkMRMLLongPETCTStudyNode : public vtkMRMLNode
{
  public:   


  static vtkMRMLLongPETCTStudyNode *New();
  vtkTypeMacro(vtkMRMLLongPETCTStudyNode,vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual vtkMRMLNode* CreateNodeInstance();

  /// Set node attributes
  virtual void ReadXMLAttributes( const char** atts);

  /// Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  /// Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node);

  /// Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {return "Study";};

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
  vtkMRMLLongPETCTStudyNode();
  ~vtkMRMLLongPETCTStudyNode();
  vtkMRMLLongPETCTStudyNode(const vtkMRMLLongPETCTStudyNode&);
  void operator=(const vtkMRMLLongPETCTStudyNode&);

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

