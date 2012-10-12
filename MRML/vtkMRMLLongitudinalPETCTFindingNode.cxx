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
#include "vtkMRMLLongitudinalPETCTFindingNode.h"

#include "vtkMRMLLongitudinalPETCTStudyNode.h"
#include "vtkMRMLLongitudinalPETCTSegmentationNode.h"

#include <vtkNew.h>
#include <vtkMRMLScalarVolumeNode.h>

#include <vtkEventForwarderCommand.h>
// STD includes


//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLLongitudinalPETCTFindingNode);

//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTFindingNode::vtkMRMLLongitudinalPETCTFindingNode()
{
  this->SetHideFromEditors(false);
  this->TypeName = "";
  this->ColorID = -1;
  this->SegmentationROI = NULL;

  this->ModelHierarchyNode = vtkSmartPointer<vtkMRMLModelHierarchyNode>::New();

  this->segmentationModifiedForwarder = vtkSmartPointer<vtkEventForwarderCommand>::New();
  segmentationModifiedForwarder->SetTarget(this);

  this->ObservedEvents = vtkSmartPointer<vtkIntArray>::New();
  this->ObservedEvents->InsertNextValue(vtkMRMLLongitudinalPETCTSegmentationNode::ModelHierarchyChangedEvent);
}

//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTFindingNode::~vtkMRMLLongitudinalPETCTFindingNode()
{
  this->SegmentationROI = NULL;

  if(this->Scene && this->ModelHierarchyNode && this->Scene->GetNodeByID(this->ModelHierarchyNode->GetID()))
    this->Scene->RemoveNode(this->ModelHierarchyNode);

  this->ModelHierarchyNode = NULL;
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTFindingNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  Superclass::ReadXMLAttributes(atts);


    this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTFindingNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);
}

//----------------------------------------------------------------------------
// Copy the node\"s attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, SliceID
void vtkMRMLLongitudinalPETCTFindingNode::Copy(vtkMRMLNode *anode)
{
  int disabledModify = this->StartModify();
  
  Superclass::Copy(anode);

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTFindingNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
bool vtkMRMLLongitudinalPETCTFindingNode::AddSegmentationForStudy(vtkMRMLLongitudinalPETCTStudyNode* study, vtkMRMLLongitudinalPETCTSegmentationNode* segmentation)
{
  bool ok = this->StudyToSegmentationMap.insert(std::make_pair(study,segmentation)).second;

  if(ok && segmentationModifiedForwarder != NULL)
    {
      vtkObserveMRMLObjectEventsMacro(segmentation, this->ObservedEvents);
      segmentation->AddObserver(vtkCommand::ModifiedEvent,this->segmentationModifiedForwarder);

      this->UpdateSegmentationModelHierarchyParent(segmentation);

    }


  this->InvokeEvent(vtkCommand::ModifiedEvent);

  return ok;
}

//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTSegmentationNode* vtkMRMLLongitudinalPETCTFindingNode::RemoveSegmentationForStudy(const vtkMRMLLongitudinalPETCTStudyNode* study)
{
  vtkMRMLLongitudinalPETCTSegmentationNode* segmentationToRemove = this->GetSegmentationForStudy(study);

  int removed = 0;

  if( segmentationToRemove != NULL )
    {
      removed = this->StudyToSegmentationMap.erase(const_cast<vtkMRMLLongitudinalPETCTStudyNode*>(study));
    }

  if(removed > 0)
    this->InvokeEvent(vtkCommand::ModifiedEvent);
  else
    segmentationToRemove = NULL;


  return segmentationToRemove;
}


//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTSegmentationNode* vtkMRMLLongitudinalPETCTFindingNode::GetSegmentationForStudy(const vtkMRMLLongitudinalPETCTStudyNode* study)
{
  vtkMRMLLongitudinalPETCTSegmentationNode* segmentation = NULL;

  if(this->StudyToSegmentationMap.find(const_cast<vtkMRMLLongitudinalPETCTStudyNode*>(study)) != this->StudyToSegmentationMap.end())
    {
      segmentation = this->StudyToSegmentationMap[const_cast<vtkMRMLLongitudinalPETCTStudyNode*>(study)];
    }

  return segmentation;
}

//----------------------------------------------------------------------------
int vtkMRMLLongitudinalPETCTFindingNode::GetColorID()
{
  return this->ColorID;
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTFindingNode::SetColorID(int id)
{
  this->ColorID = id;
  this->InvokeEvent(vtkCommand::ModifiedEvent);
}


//----------------------------------------------------------------------------
int vtkMRMLLongitudinalPETCTFindingNode::GetSegmentationsCount()
{
  return this->StudyToSegmentationMap.size();
}


//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTFindingNode::SetScene(vtkMRMLScene* scene)
{
  if(scene && this->ModelHierarchyNode && ! scene->GetNodeByID(this->ModelHierarchyNode->GetID()))
    scene->AddNode(this->ModelHierarchyNode);

  Superclass::SetScene(scene);
}


//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTFindingNode::UpdateSegmentationModelHierarchyParent(vtkMRMLLongitudinalPETCTSegmentationNode* segmentation)
{
  if(segmentation && segmentation->GetModelHierarchyNode() && this->ModelHierarchyNode)
    segmentation->GetModelHierarchyNode()->SetParentNodeID(this->GetModelHierarchyNode()->GetID());
}

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTFindingNode::ProcessMRMLEvents(vtkObject *caller,
    unsigned long event, void *callData)
{
  Superclass::ProcessMRMLEvents(caller, event, callData);

  vtkSmartPointer<vtkMRMLLongitudinalPETCTSegmentationNode> seg = vtkMRMLLongitudinalPETCTSegmentationNode::SafeDownCast(caller);
  if (seg && event == vtkMRMLLongitudinalPETCTSegmentationNode::ModelHierarchyChangedEvent)
    this->UpdateSegmentationModelHierarchyParent(seg);

}


