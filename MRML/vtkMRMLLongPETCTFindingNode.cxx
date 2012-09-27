/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women\"s Hospital (BWH) All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLReprtingFindingNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

// VTK includes
#include <vtkCommand.h>
#include <vtkObjectFactory.h>



// MRML includes
#include "vtkMRMLLongPETCTFindingNode.h"

#include "vtkMRMLLongPETCTStudyNode.h"
#include "vtkMRMLLongPETCTSegmentationNode.h"

#include <vtkNew.h>
#include <vtkSmartPointer.h>

#include <vtkMRMLAnnotationROINode.h>
#include <vtkMRMLScalarVolumeNode.h>

#include <vtkEventForwarderCommand.h>
// STD includes


//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLLongPETCTFindingNode);

//----------------------------------------------------------------------------
vtkMRMLLongPETCTFindingNode::vtkMRMLLongPETCTFindingNode()
{
  this->SetHideFromEditors(false);
  this->TypeName = "Tumor";
  this->ColorID = 1;
  this->SegmentationROI = NULL;

  this->segmentationModifiedForwarder = vtkSmartPointer<vtkEventForwarderCommand>::New();
  segmentationModifiedForwarder->SetTarget(this);

}

//----------------------------------------------------------------------------
vtkMRMLLongPETCTFindingNode::~vtkMRMLLongPETCTFindingNode()
{

}

//----------------------------------------------------------------------------
void vtkMRMLLongPETCTFindingNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  Superclass::ReadXMLAttributes(atts);


    this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLLongPETCTFindingNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);
}

//----------------------------------------------------------------------------
// Copy the node\"s attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, SliceID
void vtkMRMLLongPETCTFindingNode::Copy(vtkMRMLNode *anode)
{
  int disabledModify = this->StartModify();
  
  Superclass::Copy(anode);

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLLongPETCTFindingNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
bool vtkMRMLLongPETCTFindingNode::AddSegmentationForStudy(vtkMRMLLongPETCTStudyNode* study, vtkMRMLLongPETCTSegmentationNode* segmentation)
{
  bool ok = this->StudyToSegmentationMap.insert(std::make_pair(study,segmentation)).second;

  if(ok && segmentationModifiedForwarder != NULL)
    segmentation->AddObserver(vtkCommand::ModifiedEvent,this->segmentationModifiedForwarder);

  this->InvokeEvent(vtkCommand::ModifiedEvent);

  return ok;
}

//----------------------------------------------------------------------------
vtkMRMLLongPETCTSegmentationNode* vtkMRMLLongPETCTFindingNode::RemoveSegmentationForStudy(const vtkMRMLLongPETCTStudyNode* study)
{
  vtkMRMLLongPETCTSegmentationNode* segmentationToRemove = this->GetSegmentationForStudy(study);

  int removed = 0;

  if( segmentationToRemove != NULL )
    {
      removed = this->StudyToSegmentationMap.erase(const_cast<vtkMRMLLongPETCTStudyNode*>(study));
    }

  if(removed > 0)
    this->InvokeEvent(vtkCommand::ModifiedEvent);
  else
    segmentationToRemove = NULL;


  return segmentationToRemove;
}


//----------------------------------------------------------------------------
vtkMRMLLongPETCTSegmentationNode* vtkMRMLLongPETCTFindingNode::GetSegmentationForStudy(const vtkMRMLLongPETCTStudyNode* study)
{
  vtkMRMLLongPETCTSegmentationNode* segmentation = NULL;

  if(this->StudyToSegmentationMap.find(const_cast<vtkMRMLLongPETCTStudyNode*>(study)) != this->StudyToSegmentationMap.end())
    {
      segmentation = this->StudyToSegmentationMap[const_cast<vtkMRMLLongPETCTStudyNode*>(study)];
    }

  return segmentation;
}

//----------------------------------------------------------------------------
int vtkMRMLLongPETCTFindingNode::GetColorID()
{
  return this->ColorID;
}

//----------------------------------------------------------------------------
void vtkMRMLLongPETCTFindingNode::SetColorID(int id)
{
  this->ColorID = id;
  this->InvokeEvent(vtkCommand::ModifiedEvent);
}


//----------------------------------------------------------------------------
int vtkMRMLLongPETCTFindingNode::GetSegmentationsCount()
{
  return this->StudyToSegmentationMap.size();
}




