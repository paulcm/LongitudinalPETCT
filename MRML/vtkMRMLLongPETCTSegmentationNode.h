/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLVolumeRenderingParametersNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/
// .NAME vtkMRMLLongPETCTSegmentationNode
// This node keeps pointers to the two elements of the Segmentation: annotation
// and markup.
//
// Annotation element will (eventually) contain the Segmentation template and the
// user-initialized fields of the template.
//
// Markup element will point to the hierarchy of the markup elements.
//
// On export/import, this node will be used by the IO logic to determine how
// to code the formatted Segmentation.
// .SECTION Description
// This node keeps pointers to the two elements of the Segmentation: annotation
// and markup.
//

#ifndef __vtkMRMLLongPETCTSegmentationNode_h
#define __vtkMRMLLongPETCTSegmentationNode_h

// MRML includes
//#include "vtkDoubleArray.h"
#include "vtkMRML.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLScene.h"

#include <vtkSmartPointer.h>

#include <vtkSlicerLongPETCTModuleMRMLExport.h>

#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLModelHierarchyNode.h>
#include <vtkIntArray.h>


/// \ingroup Slicer_QtModules_LongPETCTSegmentationNode
class VTK_SLICER_LONGPETCT_MODULE_MRML_EXPORT vtkMRMLLongPETCTSegmentationNode : public vtkMRMLNode
{
  public:   

  static vtkMRMLLongPETCTSegmentationNode *New();
  vtkTypeMacro(vtkMRMLLongPETCTSegmentationNode,vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual vtkMRMLNode* CreateNodeInstance();

  /// Set node attributes
  virtual void ReadXMLAttributes( const char** atts);

  /// Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  /// Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node);

  /// Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {return "Segmentation";};

  enum
      {
        ModelHierarchyUpdatedEvent
      };

  void SetROIxyz(double roiXYZ[3]);
  void GetROIxyz(double xyz[3]);

  void SetROIRadius(double roiRadius[3]);
  void GetROIRadius(double radius[3]);

  void SetSUVs(double max, double mean, double min);

  void SetLabelVolumeNode(vtkMRMLScalarVolumeNode* labelVolume);
  vtkGetMacro(LabelVolumeNode,vtkMRMLScalarVolumeNode*);

  void SetModelHierarchyNode(vtkMRMLModelHierarchyNode* modelHierarchy);
  vtkGetMacro(ModelHierarchyNode,vtkMRMLModelHierarchyNode*);

  void SetModelVisible(bool visible);
  vtkGetMacro(ModelVisible, bool);

  vtkGetMacro(SUVMax,double);
  vtkGetMacro(SUVMean,double);
  vtkGetMacro(SUVMin,double);

  void Initialize();

  void ProcessMRMLEvents(vtkObject *caller, unsigned long event, void *callData);
  void SetScene(vtkMRMLScene* scene);

protected:
  vtkMRMLLongPETCTSegmentationNode();
  ~vtkMRMLLongPETCTSegmentationNode();
  vtkMRMLLongPETCTSegmentationNode(const vtkMRMLLongPETCTSegmentationNode&);
  void operator=(const vtkMRMLLongPETCTSegmentationNode&);

  void AdjustModelTransformToLabelVolume();

  vtkSetMacro(SUVMax,double);
  vtkSetMacro(SUVMean,double);
  vtkSetMacro(SUVMin,double);

  bool ModelVisible;

  vtkSmartPointer<vtkMRMLScalarVolumeNode> LabelVolumeNode;
  vtkSmartPointer<vtkMRMLModelHierarchyNode> ModelHierarchyNode;

  vtkSmartPointer<vtkIntArray> ObservedEvents;

  double ROIxyz[3];
  double ROIRadius[3];

  double SUVMax;
  double SUVMean;
  double SUVMin;


};

#endif

