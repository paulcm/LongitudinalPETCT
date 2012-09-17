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
#include "vtkMRMLLongPETCTStudyNode.h"

#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLLinearTransformNode.h>
#include <vtkMRMLAnnotationROINode.h>

#include <vtkNew.h>
#include <vtkMatrix4x4.h>

// STD includes


//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLLongPETCTStudyNode);

//----------------------------------------------------------------------------
vtkMRMLLongPETCTStudyNode::vtkMRMLLongPETCTStudyNode()
{
  this->Initialize();
}

//----------------------------------------------------------------------------
void vtkMRMLLongPETCTStudyNode::Initialize()
{
  this->SetHideFromEditors(true);
  this->Selected = false;
  this->CenteredVolumes = true;

  this->PETVolumeNode = NULL;
  this->CTVolumeNode = NULL;
  this->PETLabelVolumeNode = NULL;

  this->CenteringTransform = NULL;
  this->SegmentationROI = NULL;



}

//----------------------------------------------------------------------------
vtkMRMLLongPETCTStudyNode::~vtkMRMLLongPETCTStudyNode()
{

}

//----------------------------------------------------------------------------
void vtkMRMLLongPETCTStudyNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  Superclass::ReadXMLAttributes(atts);


    this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLLongPETCTStudyNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);
}

//----------------------------------------------------------------------------
// Copy the node\"s attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, SliceID
void vtkMRMLLongPETCTStudyNode::Copy(vtkMRMLNode *anode)
{
  int disabledModify = this->StartModify();
  
  Superclass::Copy(anode);

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLLongPETCTStudyNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkMRMLLongPETCTStudyNode::ObserveCenteringTransform(bool observe)
{
  if(observe && this->CenteringTransform != NULL)
    {
      if(this->PETVolumeNode != NULL && this->PETVolumeNode->GetParentTransformNode() != this->CenteringTransform)
        this->PETVolumeNode->SetAndObserveTransformNodeID(this->CenteringTransform->GetID());
      if(this->CTVolumeNode != NULL && this->CTVolumeNode->GetParentTransformNode() != this->CenteringTransform)
        this->CTVolumeNode->SetAndObserveTransformNodeID(this->CenteringTransform->GetID());
      if(this->PETLabelVolumeNode != NULL && this->PETLabelVolumeNode->GetParentTransformNode() != this->CenteringTransform)
        this->PETLabelVolumeNode->SetAndObserveTransformNodeID(this->CenteringTransform->GetID());
      if(this->SegmentationROI != NULL && this->SegmentationROI->GetParentTransformNode() != this->CenteringTransform)
        this->SegmentationROI->SetAndObserveTransformNodeID(this->CenteringTransform->GetID());
    }
  else
    {
      if(this->PETVolumeNode)
        this->PETVolumeNode->SetAndObserveTransformNodeID(NULL);
      if(this->CTVolumeNode)
        this->CTVolumeNode->SetAndObserveTransformNodeID(NULL);
      if(this->PETLabelVolumeNode)
        this->PETLabelVolumeNode->SetAndObserveTransformNodeID(NULL);
      if(this->SegmentationROI)
        this->SegmentationROI->SetAndObserveTransformNodeID(NULL);
    }
}

void vtkMRMLLongPETCTStudyNode::SetSegmentationROI(vtkMRMLAnnotationROINode* roi)
{
    this->SegmentationROI = roi;

    if(this->CenteredVolumes)
      {
        if(this->SegmentationROI->GetParentTransformNode() != this->CenteringTransform)
          {
            std::cout << "SETTING CENTERING TRANSFORM" << std::endl;

            if(this->SegmentationROI->GetParentTransformNode() != NULL)
              std::cout << "OLD CENTERING TRANSFORM: " << this->SegmentationROI->GetParentTransformNode()->GetName() << "NEW: " << this->CenteringTransform->GetName() << std::endl;

            this->SegmentationROI->SetAndObserveTransformNodeID(this->CenteringTransform->GetID());
            this->InvokeEvent(vtkCommand::ModifiedEvent);
            std::cout << "SETTING CENTERING TRANSFORM " << std::endl;
          }
      }
    else
      {
        std::cout << "SETTING ROI WITHOUT CENTERING TRANSFORM." << std::endl;
        this->SegmentationROI->SetAndObserveTransformNodeID(NULL);
        this->InvokeEvent(vtkCommand::ModifiedEvent);
      }
}


void vtkMRMLLongPETCTStudyNode::SetCenteredVolumes(bool centered)
{
  if(this->CenteredVolumes == centered)
    return;

  this->CenteredVolumes = centered;

  this->ObserveCenteringTransform(centered);
}





