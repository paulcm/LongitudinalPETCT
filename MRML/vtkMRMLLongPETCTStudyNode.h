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
  vtkSetMacro(Selected, bool);

  vtkGetMacro(PETVolumeNode, vtkMRMLScalarVolumeNode*);
  vtkGetMacro(CTVolumeNode, vtkMRMLScalarVolumeNode*);
  vtkGetMacro(Selected, int);

protected:
  vtkMRMLLongPETCTStudyNode();
  ~vtkMRMLLongPETCTStudyNode();
  vtkMRMLLongPETCTStudyNode(const vtkMRMLLongPETCTStudyNode&);
  void operator=(const vtkMRMLLongPETCTStudyNode&);

  bool Selected;

  vtkMRMLScalarVolumeNode* PETVolumeNode;
  vtkMRMLScalarVolumeNode* CTVolumeNode;

};

#endif

