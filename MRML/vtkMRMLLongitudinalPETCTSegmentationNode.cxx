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
#include "vtkMRMLLongitudinalPETCTSegmentationNode.h"

#include <vtkMRMLModelNode.h>
#include <vtkMRMLModelDisplayNode.h>
#include <vtkMRMLTransformNode.h>

#include <vtkNew.h>
#include <vtkCollection.h>



// STD includes


//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLLongitudinalPETCTSegmentationNode);

//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTSegmentationNode::vtkMRMLLongitudinalPETCTSegmentationNode()
{
  this->Initialize();
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTSegmentationNode::Initialize()
{
  this->SetHideFromEditors(false);

  this->LabelVolumeNode = NULL;
  this->ModelHierarchyNode = vtkSmartPointer<vtkMRMLModelHierarchyNode>::New();
  this->ModelVisible = true;

  this->ROIxyz[0] = 0.;
  this->ROIxyz[1] = 0.;
  this->ROIxyz[2] = 0.;

  this->ROIRadius[0] = 0.;
  this->ROIRadius[1] = 0.;
  this->ROIRadius[2] = 0.;

  this->SUVMax = 0.;
  this->SUVMean = 0.;
  this->SUVMin = 0.;

  this->ObservedEvents = vtkSmartPointer<vtkIntArray>::New();
  this->ObservedEvents->InsertNextValue(vtkCommand::ModifiedEvent);
}

//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTSegmentationNode::~vtkMRMLLongitudinalPETCTSegmentationNode()
{
  vtkUnObserveMRMLObjectMacro(this->LabelVolumeNode);
  this->LabelVolumeNode = NULL;

  if(this->Scene && this->ModelHierarchyNode && this->Scene->GetNodeByID(this->ModelHierarchyNode->GetID()))
      this->Scene->RemoveNode(this->ModelHierarchyNode);

  vtkUnObserveMRMLObjectMacro(this->ModelHierarchyNode);
  this->ModelHierarchyNode == NULL;
}


//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTSegmentationNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  Superclass::ReadXMLAttributes(atts);
  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTSegmentationNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);
}

//----------------------------------------------------------------------------
// Copy the node\"s attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, SliceID
void vtkMRMLLongitudinalPETCTSegmentationNode::Copy(vtkMRMLNode *anode)
{
  int disabledModify = this->StartModify();
  
  Superclass::Copy(anode);

  this->EndModify(disabledModify);
  this->ModelHierarchyNode->GetModelNode();
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTSegmentationNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTSegmentationNode::SetROIxyz(double roiXYZ[3])
{
  this->ROIxyz[0] = roiXYZ[0];
  this->ROIxyz[1] = roiXYZ[1];
  this->ROIxyz[2] = roiXYZ[2];

  this->InvokeEvent(vtkCommand::ModifiedEvent);
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTSegmentationNode::GetROIxyz(double xyz[3])
{
  xyz[0] = this->ROIxyz[0];
  xyz[1] = this->ROIxyz[1];
  xyz[2] = this->ROIxyz[2];
}


//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTSegmentationNode::SetROIRadius(double roiRadius[3])
{
  this->ROIRadius[0] = roiRadius[0];
  this->ROIRadius[1] = roiRadius[1];
  this->ROIRadius[2] = roiRadius[2];

  this->InvokeEvent(vtkCommand::ModifiedEvent);
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTSegmentationNode::GetROIRadius(double radius[3])
{
  radius[0] = this->ROIRadius[0];
  radius[1] = this->ROIRadius[1];
  radius[2] = this->ROIRadius[2];
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTSegmentationNode::SetSUVs(double max, double mean, double min)
{
  int disabledModify = this->StartModify();

  this->SetSUVMax(max);
  this->SetSUVMean(mean);
  this->SetSUVMin(min);

  this->EndModify(disabledModify);
}


//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTSegmentationNode::SetLabelVolumeNode(vtkMRMLScalarVolumeNode* labelVolume)
{
  if(labelVolume == this->LabelVolumeNode)
    return;

   vtkUnObserveMRMLObjectMacro(this->LabelVolumeNode);

   vtkSetAndObserveMRMLObjectEventsMacro(this->LabelVolumeNode,labelVolume, this->ObservedEvents);

   this->AdjustModelTransformToLabelVolume();

}

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTSegmentationNode::SetModelHierarchyNode(
    vtkMRMLModelHierarchyNode* modelHierarchy)
{
  if (modelHierarchy == this->ModelHierarchyNode)
    return;

  if (this->ModelHierarchyNode)
    {
      vtkNew<vtkCollection> cmn;
      this->ModelHierarchyNode->GetChildrenModelNodes(cmn.GetPointer());

      for (int i = 0; i < cmn->GetNumberOfItems(); ++i)
        {
          vtkSmartPointer<vtkMRMLModelNode> tempModelNode =
              vtkMRMLModelNode::SafeDownCast(cmn->GetItemAsObject(i));

          if (!tempModelNode)
            continue;

          if (this->Scene)
            this->Scene->RemoveNode(tempModelNode);
        }

      if (this->Scene)
        this->Scene->RemoveNode(this->ModelHierarchyNode);

      this->ModelHierarchyNode = NULL;
    }

  vtkSetMRMLObjectMacro(this->ModelHierarchyNode, modelHierarchy);
  this->AdjustModelTransformToLabelVolume();

  this->InvokeEvent(vtkMRMLLongitudinalPETCTSegmentationNode::ModelHierarchyUpdatedEvent);
}


//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTSegmentationNode::ProcessMRMLEvents(vtkObject *caller,
    unsigned long event, void *callData)
{
  if (caller == this->LabelVolumeNode.GetPointer()
      && event == vtkCommand::ModifiedEvent)
    this->AdjustModelTransformToLabelVolume();

  Superclass::ProcessMRMLEvents(caller, event, callData);
}


//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTSegmentationNode::AdjustModelTransformToLabelVolume()
{
  if (this->ModelHierarchyNode && this->LabelVolumeNode)
    {

      vtkNew<vtkCollection> cmn;
      this->ModelHierarchyNode->GetChildrenModelNodes(cmn.GetPointer());

      for(int i=0; i < cmn->GetNumberOfItems(); ++i)
        {
          vtkSmartPointer<vtkMRMLModelNode> tempModelNode = vtkMRMLModelNode::SafeDownCast(cmn->GetItemAsObject(i));

          if(!tempModelNode)
            continue;

          if(this->LabelVolumeNode->GetParentTransformNode())
            tempModelNode->SetAndObserveTransformNodeID(this->LabelVolumeNode->GetParentTransformNode()->GetID());
          else
            tempModelNode->SetAndObserveTransformNodeID(NULL);

        }
    }
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTSegmentationNode::SetScene(vtkMRMLScene* scene)
{
  if(scene && this->ModelHierarchyNode && ! scene->GetNodeByID(this->ModelHierarchyNode->GetID()))
    scene->AddNode(this->ModelHierarchyNode);

  Superclass::SetScene(scene);
}

void vtkMRMLLongitudinalPETCTSegmentationNode::SetModelVisible(bool visible)
{
  if(this->ModelVisible == visible)
    return;

  this->ModelVisible = visible;

  if(this->ModelHierarchyNode)
    {
      vtkSmartPointer<vtkMRMLModelNode> tempMN = this->ModelHierarchyNode->GetModelNode();

      if(tempMN)
        {
          vtkSmartPointer<vtkMRMLModelDisplayNode> tempMDN = tempMN->GetModelDisplayNode();
          if(tempMDN)
            tempMDN->SetVisibility(visible);
        }
    }

  this->InvokeEvent(vtkCommand::ModifiedEvent);
}
