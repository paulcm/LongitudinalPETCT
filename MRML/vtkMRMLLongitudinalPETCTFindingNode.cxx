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

// STD includes


//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLLongitudinalPETCTFindingNode);

//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTFindingNode::vtkMRMLLongitudinalPETCTFindingNode()
{
  this->SetHideFromEditors(false);
  this->TypeName = NULL;
  this->ColorID = -1;

  this->SegmentationROINode = NULL;
  this->SegmentationROINodeID = NULL;

  this->ModelHierarchyNode = NULL;
  this->ModelHierarchyNodeID = NULL;

  this->ObservedEvents = vtkSmartPointer<vtkIntArray>::New();
  this->ObservedEvents->InsertNextValue(vtkMRMLLongitudinalPETCTSegmentationNode::ModelHierarchyChangedEvent);
}

//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTFindingNode::~vtkMRMLLongitudinalPETCTFindingNode()
{
  this->SegmentationROINode = NULL;
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

  os << indent << "ColorID: " << this->ColorID << "\n";
  os << indent << "TypeName: " << (this->TypeName ? this->TypeName : "(none)")<< "\n";
  os << indent << "SegmentationROINodeID: " << (this->SegmentationROINodeID ? this->SegmentationROINodeID : "(none)") << "\n";
  os << indent << "ModelHierarchyNodeID: " << (this->ModelHierarchyNodeID ? this->ModelHierarchyNodeID : "(none)") << "\n";
  os << indent << "StudyNodeIDs to SegmentationNodeIDs mapping: "<< (this->GetNumberOfSegmentations() == 0 ? "(none)" : "" ) <<"\n";

  std::map<std::string,std::string>::iterator it;
  for ( it=this->StudyIDToSegmentationIDMap.begin() ; it != this->StudyIDToSegmentationIDMap.end(); it++ )
    os << indent << (*it).first.c_str() << " => " << (*it).second.c_str() << "\n";

}

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTFindingNode::SetAndObserveSegmentationROINodeID(
    const char* segmentationROINodeID)
{

  if (segmentationROINodeID && this->SegmentationROINodeID
      && !strcmp(segmentationROINodeID, this->SegmentationROINodeID))
    return;

  // first remove references and observers from old node
  if (this->SegmentationROINode)
    {
      vtkUnObserveMRMLObjectMacro(this->SegmentationROINode);

      if (this->Scene
          && this->Scene->IsNodeReferencingNodeID(this,
              this->SegmentationROINode->GetID()))
        this->Scene->RemoveReferencedNodeID(this->SegmentationROINode->GetID(),
            this);
    }

  vtkSmartPointer<vtkMRMLAnnotationROINode> roiNode = NULL;

  if (this->GetScene() && segmentationROINodeID)
    {
      roiNode = vtkMRMLAnnotationROINode::SafeDownCast(
          this->GetScene()->GetNodeByID(segmentationROINodeID));
    }

  vtkSetAndObserveMRMLObjectMacro(this->SegmentationROINode, roiNode);
  this->SetSegmentationROINodeID(segmentationROINodeID);

  if (this->Scene && this->SegmentationROINode)
    this->Scene->AddReferencedNodeID(this->SegmentationROINodeID, this);
}


//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTFindingNode::SetAndObserveSegmentationROINodeID(
    const std::string& segmentationROINodeID)
{
  this->SetAndObserveSegmentationROINodeID(segmentationROINodeID.c_str());
}


//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTFindingNode::SetAndObserveModelHierarchyNodeID(
    const char* modelHierarchyNodeID)
{

  if (modelHierarchyNodeID && this->ModelHierarchyNodeID
      && !strcmp(modelHierarchyNodeID, this->ModelHierarchyNodeID))
    return;

  // first remove references and observers from old node
  if (this->ModelHierarchyNode)
    {
      vtkUnObserveMRMLObjectMacro(this->ModelHierarchyNode);

      if (this->Scene
          && this->Scene->IsNodeReferencingNodeID(this,
              this->ModelHierarchyNode->GetID()))
        this->Scene->RemoveReferencedNodeID(this->ModelHierarchyNode->GetID(),
            this);
    }

  vtkSmartPointer<vtkMRMLModelHierarchyNode> mhnode = NULL;

  if (this->GetScene() && modelHierarchyNodeID)
    {
      mhnode = vtkMRMLModelHierarchyNode::SafeDownCast(
          this->GetScene()->GetNodeByID(modelHierarchyNodeID));
    }

  vtkSetAndObserveMRMLObjectMacro(this->ModelHierarchyNode, mhnode);
  this->SetModelHierarchyNodeID(modelHierarchyNodeID);

  if (this->Scene && this->ModelHierarchyNode)
    this->Scene->AddReferencedNodeID(this->ModelHierarchyNodeID, this);
}


//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTFindingNode::SetAndObserveModelHierarchyNodeID(
    const std::string& modelHierarchyNodeID)
{
  this->SetAndObserveModelHierarchyNodeID(modelHierarchyNodeID.c_str());
}

//----------------------------------------------------------------------------
bool
vtkMRMLLongitudinalPETCTFindingNode::AddSegmentationNodeIDForStudy(
    const char* studyNodeID, const char* segmentationNodeID)
{
  bool ok = this->StudyIDToSegmentationIDMap.insert(
      std::make_pair(studyNodeID, segmentationNodeID)).second;

  if (ok && this->Scene)
    {
       vtkObserveMRMLObjectMacro(this->Scene->GetNodeByID(segmentationNodeID));
    }

  this->InvokeEvent(vtkCommand::ModifiedEvent);

  return ok;
}

//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTSegmentationNode* vtkMRMLLongitudinalPETCTFindingNode::RemoveSegmentationNodeIDForStudy(const char* studyNodeID)
{
  vtkSmartPointer<vtkMRMLLongitudinalPETCTSegmentationNode> segmentationToRemove = this->GetSegmentationForStudy(studyNodeID);

  int removed = 0;

  if( segmentationToRemove )
    {
      removed = this->StudyIDToSegmentationIDMap.erase(studyNodeID);
    }

  if(removed > 0)
    {
      if(this->Scene)
        vtkUnObserveMRMLObjectMacro(segmentationToRemove);

      this->InvokeEvent(vtkCommand::ModifiedEvent);
    }
  else
    segmentationToRemove = NULL;


  return segmentationToRemove;
}


//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTSegmentationNode* vtkMRMLLongitudinalPETCTFindingNode::GetSegmentationForStudy(const char* studyNodeID)
{
  vtkSmartPointer<vtkMRMLLongitudinalPETCTSegmentationNode> segmentation = NULL;

  if(this->StudyIDToSegmentationIDMap.find(studyNodeID) != this->StudyIDToSegmentationIDMap.end())
    {
      std::string segmentationID = this->StudyIDToSegmentationIDMap[studyNodeID];
      if(this->Scene && !segmentationID.empty())
        segmentation = vtkMRMLLongitudinalPETCTSegmentationNode::SafeDownCast(this->Scene->GetNodeByID(segmentationID.c_str()));
    }

  return segmentation;
}


//----------------------------------------------------------------------------
int vtkMRMLLongitudinalPETCTFindingNode::GetNumberOfSegmentations()
{
  return this->StudyIDToSegmentationIDMap.size();
}


//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTFindingNode::SetScene(vtkMRMLScene* scene)
{
  Superclass::SetScene(scene);

  if(this->Scene && this->Scene->GetNodeByID(this->ModelHierarchyNodeID))
      this->SetAndObserveModelHierarchyNodeID(this->ModelHierarchyNodeID);

  if(this->Scene && this->Scene->GetNodeByID(this->SegmentationROINodeID))
        this->SetAndObserveSegmentationROINodeID(this->SegmentationROINodeID);

}


//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTFindingNode::UpdateSegmentationModelHierarchyParent(
    vtkMRMLLongitudinalPETCTSegmentationNode* segmentation)
{
  if (segmentation && segmentation->GetModelHierarchyNode())
    {
      if (segmentation->GetModelHierarchyNode() != this->ModelHierarchyNode)
        {
          segmentation->GetModelHierarchyNode()->SetParentNodeID(
              this->GetModelHierarchyNode()->GetID());
          this->InvokeEvent(vtkCommand::ModifiedEvent);
        }
    }

}

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTFindingNode::UpdateReferences()
{
  this->Superclass::UpdateReferences();

  if(this->Scene)
    {
      if(this->SegmentationROINodeID && this->Scene->GetNodeByID(this->SegmentationROINodeID) == NULL)
        this->SetAndObserveSegmentationROINodeID(NULL);

      else if(this->ModelHierarchyNodeID && this->GetScene()->GetNodeByID(this->ModelHierarchyNodeID) == NULL)
        this->SetAndObserveModelHierarchyNodeID(NULL);
    }
}

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTFindingNode::UpdateReferenceID(const char *oldID, const char *newID)
{
  this->Superclass::UpdateReferenceID(oldID,newID);

  if(this->SegmentationROINode && !strcmp(oldID,this->SegmentationROINodeID))
    this->SetAndObserveSegmentationROINodeID(newID);

  else if(this->ModelHierarchyNode && !strcmp(oldID,this->ModelHierarchyNodeID))
    this->SetAndObserveModelHierarchyNodeID(newID);
}

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTFindingNode::ProcessMRMLEvents(vtkObject *caller,
    unsigned long event, void *callData)
{
  Superclass::ProcessMRMLEvents(caller, event, callData);

  vtkSmartPointer<vtkMRMLLongitudinalPETCTSegmentationNode> segCaller = vtkMRMLLongitudinalPETCTSegmentationNode::SafeDownCast(caller);

  if(segCaller)
    {
      if(event == vtkMRMLLongitudinalPETCTSegmentationNode::ModelHierarchyChangedEvent)
        this->UpdateSegmentationModelHierarchyParent(segCaller);

      else if(event == vtkCommand::ModifiedEvent)
        this->InvokeEvent(vtkCommand::ModifiedEvent); // forwarding Modified
    }
}


