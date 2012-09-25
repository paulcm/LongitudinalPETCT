/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women\"s Hospital (BWH) All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLReprtingSegmentationNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

// VTK includes
#include <vtkCommand.h>
#include <vtkObjectFactory.h>
#


// MRML includes
#include "vtkMRMLLongPETCTSegmentationNode.h"

#include <vtkMRMLScalarVolumeNode.h>

// STD includes


//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLLongPETCTSegmentationNode);

//----------------------------------------------------------------------------
vtkMRMLLongPETCTSegmentationNode::vtkMRMLLongPETCTSegmentationNode()
{
  this->Initialize();
}

//----------------------------------------------------------------------------
void vtkMRMLLongPETCTSegmentationNode::Initialize()
{
  this->SetHideFromEditors(false);

  this->LabelVolume = NULL;

  this->ROIxyz[0] = 0.;
  this->ROIxyz[1] = 0.;
  this->ROIxyz[2] = 0.;

  this->ROIRadius[0] = 0.;
  this->ROIRadius[1] = 0.;
  this->ROIRadius[2] = 0.;
}

//----------------------------------------------------------------------------
vtkMRMLLongPETCTSegmentationNode::~vtkMRMLLongPETCTSegmentationNode()
{

}

//----------------------------------------------------------------------------
void vtkMRMLLongPETCTSegmentationNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  Superclass::ReadXMLAttributes(atts);


    this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLLongPETCTSegmentationNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);
}

//----------------------------------------------------------------------------
// Copy the node\"s attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, SliceID
void vtkMRMLLongPETCTSegmentationNode::Copy(vtkMRMLNode *anode)
{
  int disabledModify = this->StartModify();
  
  Superclass::Copy(anode);

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLLongPETCTSegmentationNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkMRMLLongPETCTSegmentationNode::SetROIxyz(double roiXYZ[3])
{
  this->ROIxyz[0] = roiXYZ[0];
  this->ROIxyz[1] = roiXYZ[1];
  this->ROIxyz[2] = roiXYZ[2];

  this->InvokeEvent(vtkCommand::ModifiedEvent);
}

//----------------------------------------------------------------------------
void vtkMRMLLongPETCTSegmentationNode::GetROIxyz(double xyz[3])
{
  xyz[0] = this->ROIxyz[0];
  xyz[1] = this->ROIxyz[1];
  xyz[2] = this->ROIxyz[2];
}


//----------------------------------------------------------------------------
void vtkMRMLLongPETCTSegmentationNode::SetROIRadius(double roiRadius[3])
{
  this->ROIRadius[0] = roiRadius[0];
  this->ROIRadius[1] = roiRadius[1];
  this->ROIRadius[2] = roiRadius[2];

  this->InvokeEvent(vtkCommand::ModifiedEvent);
}

//----------------------------------------------------------------------------
void vtkMRMLLongPETCTSegmentationNode::GetROIRadius(double radius[3])
{
  radius[0] = this->ROIRadius[0];
  radius[1] = this->ROIRadius[1];
  radius[2] = this->ROIRadius[2];
}



