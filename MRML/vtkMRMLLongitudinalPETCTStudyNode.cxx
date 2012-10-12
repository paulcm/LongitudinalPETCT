/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women\"s Hospital (BWH) All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLReprtingStudyNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

// VTK includes
#include <vtkCommand.h>
#include <vtkObjectFactory.h>
#


// MRML includes
#include "vtkMRMLLongitudinalPETCTStudyNode.h"

#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLLinearTransformNode.h>
#include <vtkMRMLAnnotationROINode.h>
#include <vtkMRMLModelHierarchyNode.h>
#include <vtkMRMLModelNode.h>

#include <vtkNew.h>
#include <vtkMatrix4x4.h>
#include <vtkCollection.h>

// STD includes


//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLLongitudinalPETCTStudyNode);

//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTStudyNode::vtkMRMLLongitudinalPETCTStudyNode()
{
  this->Initialize();
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTStudyNode::Initialize()
{
  this->SetHideFromEditors(true);
  this->SelectedForSegmentation = false;
  this->SelectedForAnalysis = true;
  this->CenteredVolumes = true;

  this->PETVolumeNode = NULL;
  this->CTVolumeNode = NULL;
  this->PETLabelVolumeNode = NULL;

  this->CenteringTransform = NULL;
  this->SegmentationROI = NULL;

  this->VolumeRenderingDisplayNode = NULL;

}

//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTStudyNode::~vtkMRMLLongitudinalPETCTStudyNode()
{

}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTStudyNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  Superclass::ReadXMLAttributes(atts);


    this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTStudyNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);
}

//----------------------------------------------------------------------------
// Copy the node\"s attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, SliceID
void vtkMRMLLongitudinalPETCTStudyNode::Copy(vtkMRMLNode *anode)
{
  int disabledModify = this->StartModify();
  
  Superclass::Copy(anode);

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTStudyNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTStudyNode::ObserveCenteringTransform(bool observe)
{
  if (observe && this->CenteringTransform != NULL)
    {
      if (this->PETVolumeNode
          && this->PETVolumeNode->GetParentTransformNode()
              != this->CenteringTransform)
        this->PETVolumeNode->SetAndObserveTransformNodeID(
            this->CenteringTransform->GetID());
      if (this->CTVolumeNode
          && this->CTVolumeNode->GetParentTransformNode()
              != this->CenteringTransform)
        this->CTVolumeNode->SetAndObserveTransformNodeID(
            this->CenteringTransform->GetID());
      if (this->PETLabelVolumeNode
          && this->PETLabelVolumeNode->GetParentTransformNode()
              != this->CenteringTransform)
        this->PETLabelVolumeNode->SetAndObserveTransformNodeID(
            this->CenteringTransform->GetID());
      if (this->SegmentationROI
          && this->SegmentationROI->GetParentTransformNode()
              != this->CenteringTransform)
        this->SegmentationROI->SetAndObserveTransformNodeID(
            this->CenteringTransform->GetID());
    }
  else
    {
      if (this->PETVolumeNode)
        this->PETVolumeNode->SetAndObserveTransformNodeID(NULL);
      if (this->CTVolumeNode)
        this->CTVolumeNode->SetAndObserveTransformNodeID(NULL);
      if (this->PETLabelVolumeNode)
        this->PETLabelVolumeNode->SetAndObserveTransformNodeID(NULL);
      if (this->SegmentationROI)
        this->SegmentationROI->SetAndObserveTransformNodeID(NULL);
    }
}


void vtkMRMLLongitudinalPETCTStudyNode::SetSegmentationROI(vtkMRMLAnnotationROINode* roi)
{
    this->SegmentationROI = roi;

    if(roi == NULL)
      return;

    if(this->CenteredVolumes)
      {
        if(this->SegmentationROI->GetParentTransformNode() != this->CenteringTransform)
          {
            this->SegmentationROI->SetAndObserveTransformNodeID(this->CenteringTransform->GetID());
            this->InvokeEvent(vtkCommand::ModifiedEvent);
          }
      }
    else
      {
        this->SegmentationROI->SetAndObserveTransformNodeID(NULL);
        this->InvokeEvent(vtkCommand::ModifiedEvent);
      }
}


void vtkMRMLLongitudinalPETCTStudyNode::SetCenteredVolumes(bool centered)
{
  if(this->CenteredVolumes == centered)
    return;

  this->CenteredVolumes = centered;

  this->ObserveCenteringTransform(centered);
}





