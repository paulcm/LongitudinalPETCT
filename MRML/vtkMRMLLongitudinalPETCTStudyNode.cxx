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

#include <vtkMRMLLinearTransformNode.h>
#include <vtkMRMLAnnotationROINode.h>
#include <vtkMRMLModelHierarchyNode.h>
#include <vtkMRMLModelNode.h>

#include <vtkNew.h>
#include <vtkMatrix4x4.h>
#include <vtkCollection.h>

#include <vtkMRMLVolumeRenderingDisplayNode.h>

// STD includes


//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLLongitudinalPETCTStudyNode);

//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTStudyNode::vtkMRMLLongitudinalPETCTStudyNode()
{
  this->SetHideFromEditors(true);
  this->SelectedForSegmentation = false;
  this->SelectedForAnalysis = true;
  this->CenteredVolumes = true;

  this->PETVolumeNode = NULL;
  this->PETVolumeNodeID = NULL;

  this->CTVolumeNode = NULL;
  this->CTVolumeNodeID = NULL;

  this->PETLabelVolumeNode = NULL;
  this->PETLabelVolumeNodeID = NULL;

  this->SegmentationROINode = NULL;
  this->SegmentationROINodeID = NULL;

  this->CenteringTransformNode = NULL;
  this->CenteringTransformNodeID = NULL;

  this->VolumeRenderingDisplayNode = NULL;
  this->VolumeRenderingDisplayNodeID = NULL;
}


//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTStudyNode::~vtkMRMLLongitudinalPETCTStudyNode()
{
  /*
  this->PETVolumeNode = NULL;
  this->CTVolumeNode = NULL;
  this->PETLabelVolumeNode = NULL;
  this->SegmentationROINode = NULL;
  this->CenteringTransformNode = NULL;
  this->VolumeRenderingDisplayNode = NULL;


  if(this->PETVolumeNodeID)
    delete [] this->PETVolumeNodeID;

  if(this->CTVolumeNodeID)
    delete [] this->CTVolumeNodeID;

  if(this->PETLabelVolumeNodeID)
    delete [] this->PETLabelVolumeNodeID;

  if(this->SegmentationROINodeID)
    delete [] this->PETLabelVolumeNodeID;

  if(this->CenteringTransformNodeID)
    delete [] this->CenteringTransformNodeID;

  if(this->VolumeRenderingDisplayNodeID)
    delete [] this->VolumeRenderingDisplayNodeID;
    */
}

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTStudyNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  Superclass::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;

  while (*atts != NULL)
    {
      attName = *(atts++);
      attValue = *(atts++);

      if (!strcmp(attName, "SelectedForSegmentation"))
        {
          if (!strcmp(attValue, "true"))
            this->SetSelectedForSegmentation(true);
          else
            this->SetSelectedForSegmentation(false);
        }
      else if (!strcmp(attName, "SelectedForAnalysis"))
        {
          if (!strcmp(attValue, "true"))
            this->SetSelectedForAnalysis(true);
          else
            this->SetSelectedForAnalysis(false);
        }
      else if (!strcmp(attName, "VolumesCentered"))
        {
          if (!strcmp(attValue, "true"))
            this->SetCenteredVolumes(true);
          else
            this->SetCenteredVolumes(false);
        }
      else if (!strcmp(attName, "PETVolumeNodeID"))
        {
          this->SetAndObservePETVolumeNodeID(attValue);
        }
      else if (!strcmp(attName, "CTVolumeNodeID"))
        {
          this->SetAndObserveCTVolumeNodeID(attValue);
        }
      else if (!strcmp(attName, "PETLabelVolumeNodeID"))
        {
          this->SetAndObservePETLabelVolumeNodeID(attValue);
        }
      else if (!strcmp(attName, "SegmentationROINodeID"))
        {
          this->SetAndObserveSegmentationROINodeID(attValue);
        }
      else if (!strcmp(attName, "CenteringTransformNodeID"))
        {
          this->SetAndObserveCenteringTransformNodeID(attValue);
        }
      else if (!strcmp(attName, "VolumeRenderingDisplayNodeID"))
        {
          this->SetAndObserveVolumeRenderingDisplayNodeID(attValue);
        }
    }

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTStudyNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);

  vtkIndent indent(nIndent);

  of << indent << " SelectedForSegmentation=\"" << (this->SelectedForSegmentation ? "true" : "false") << "\"";
  of << indent << " SelectedForAnalysis=\"" << (this->SelectedForAnalysis ? "true" : "false") << "\"";
  of << indent << " VolumesCentered=\"" << (this->CenteredVolumes ? "true" : "false") << "\"";

  if (this->PETVolumeNodeID)
      of << indent << " PETVolumeNodeID=\"" << this->PETVolumeNodeID << "\"";
  if (this->CTVolumeNodeID)
      of << indent << " CTVolumeNodeID=\"" << this->CTVolumeNodeID << "\"";
  if (this->PETLabelVolumeNodeID)
      of << indent << " PETLabelVolumeNodeID=\"" << this->PETLabelVolumeNodeID << "\"";
  if (this->SegmentationROINodeID)
      of << indent << " SegmentationROINodeID=\"" << this->SegmentationROINodeID << "\"";
  if (this->CenteringTransformNodeID)
      of << indent << " CenteringTransformNodeID=\"" << this->CenteringTransformNodeID << "\"";
  if (this->VolumeRenderingDisplayNodeID)
      of << indent << " VolumeRenderingDisplayNodeID=\"" << this->VolumeRenderingDisplayNodeID << "\"" << std::endl;
}

//----------------------------------------------------------------------------
// Copy the node\"s attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, SliceID
void vtkMRMLLongitudinalPETCTStudyNode::Copy(vtkMRMLNode *anode)
{
  int disabledModify = this->StartModify();
  
  Superclass::Copy(anode);

  vtkSmartPointer<vtkMRMLLongitudinalPETCTStudyNode> node = vtkMRMLLongitudinalPETCTStudyNode::SafeDownCast(anode);
    if (node)
      {
        this->SetSelectedForSegmentation(node->GetSelectedForSegmentation());
        this->SetSelectedForAnalysis(node->GetSelectedForAnalysis());
        this->SetCenteredVolumes(node->GetCenteredVolumes());

        this->SetAndObservePETVolumeNodeID(node->GetPETVolumeNodeID());
        this->SetAndObserveCTVolumeNodeID(node->GetCTVolumeNodeID());
        this->SetAndObservePETLabelVolumeNodeID(node->GetPETLabelVolumeNodeID());
        this->SetAndObserveSegmentationROINodeID(node->GetSegmentationROINodeID());
        this->SetAndObserveCenteringTransformNodeID(node->GetCenteringTransformNodeID());
        this->SetAndObserveVolumeRenderingDisplayNodeID(node->GetVolumeRenderingDisplayNodeID());
      }

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTStudyNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);

  os << indent << "Selected for Segmentation: " << this->SelectedForSegmentation << "\n";
  os << indent << "Selected for Analysis: " << this->SelectedForAnalysis << "\n";
  os << indent << "Volumes centered: " << this->CenteredVolumes << "\n";

  os << indent << "PETVolumeNodeID: " << (this->PETVolumeNodeID ? this->PETVolumeNodeID : "(none)") << "\n";
  os << indent << "CTVolumeNodeID: " << (this->CTVolumeNodeID ? this->CTVolumeNodeID : "(none)") << "\n";
  os << indent << "PETLabelVolumeNodeID: " << (this->PETLabelVolumeNodeID ? this->PETLabelVolumeNodeID : "(none)") << "\n";
  os << indent << "SegmentationROINodeID: " << (this->SegmentationROINodeID ? this->SegmentationROINodeID : "(none)") << "\n";
  os << indent << "CenteringTransformNodeID: " << (this->CenteringTransformNodeID ? this->CenteringTransformNodeID : "(none)") << "\n";
  os << indent << "VolumeRenderingDisplayNodeID: " << (this->VolumeRenderingDisplayNodeID ? this->VolumeRenderingDisplayNodeID : "(none)") << "\n";

}


//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTStudyNode::SetAndObservePETVolumeNodeID(
    const char* petVolumeNodeID)
{

  // first remove references and observers from old node
  if (this->PETVolumeNode)
    {
      vtkUnObserveMRMLObjectMacro(this->PETVolumeNode);

      if (this->Scene
          && this->Scene->IsNodeReferencingNodeID(this,
              this->PETVolumeNode->GetID()))
        this->Scene->RemoveReferencedNodeID(this->PETVolumeNode->GetID(),
            this);
    }

  vtkSmartPointer<vtkMRMLScalarVolumeNode> petNode = NULL;

  if (this->GetScene() && petVolumeNodeID)
    {
      petNode = vtkMRMLScalarVolumeNode::SafeDownCast(
          this->GetScene()->GetNodeByID(petVolumeNodeID));
    }

  vtkSetAndObserveMRMLObjectMacro(this->PETVolumeNode, petNode);
  this->SetPETVolumeNodeID(petVolumeNodeID);

  // TODO: check why not possible VolumeRenderingDisplayNode observes the set PETLabelVolume
  //if(this->PETVolumeNode && this->VolumeRenderingDisplayNode)
    //this->VolumeRenderingDisplayNode->SetAndObserveVolumeNodeID(this->PETVolumeNode->GetID());


  if (this->Scene && this->PETVolumeNode)
    this->Scene->AddReferencedNodeID(this->PETVolumeNodeID, this);

  if(this->PETVolumeNode && this->CenteringTransformNode && this->CenteredVolumes)
    this->PETVolumeNode->SetAndObserveTransformNodeID(this->CenteringTransformNode->GetID());

}


//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTStudyNode::SetAndObservePETVolumeNodeID(
    const std::string& petVolumeNodeID)
{
  this->SetAndObservePETVolumeNodeID(petVolumeNodeID.c_str());
}


//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTStudyNode::SetAndObserveCTVolumeNodeID(
    const char* ctVolumeNodeID)
{

  // first remove references and observers from old node
  if (this->CTVolumeNode)
    {
      vtkUnObserveMRMLObjectMacro(this->CTVolumeNode);

      if (this->Scene
          && this->Scene->IsNodeReferencingNodeID(this,
              this->CTVolumeNode->GetID()))
        this->Scene->RemoveReferencedNodeID(this->CTVolumeNode->GetID(),
            this);
    }

  vtkSmartPointer<vtkMRMLScalarVolumeNode> ctNode = NULL;

  if (this->GetScene() && ctVolumeNodeID)
    {
      ctNode = vtkMRMLScalarVolumeNode::SafeDownCast(
          this->GetScene()->GetNodeByID(ctVolumeNodeID));
    }

  vtkSetAndObserveMRMLObjectMacro(this->CTVolumeNode, ctNode);
  this->SetCTVolumeNodeID(ctVolumeNodeID);

  if (this->Scene && this->CTVolumeNode)
    this->Scene->AddReferencedNodeID(this->CTVolumeNodeID, this);

  if(this->CTVolumeNode && this->CenteringTransformNode && this->CenteredVolumes)
    this->CTVolumeNode->SetAndObserveTransformNodeID(this->CenteringTransformNode->GetID());

}


//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTStudyNode::SetAndObserveCTVolumeNodeID(
    const std::string& ctVolumeNodeID)
{
  this->SetAndObserveCTVolumeNodeID(ctVolumeNodeID.c_str());
}

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTStudyNode::SetAndObservePETLabelVolumeNodeID(
    const char* petLabelVolumeNodeID)
{

  // first remove references and observers from old node
  if (this->PETLabelVolumeNode)
    {
      vtkUnObserveMRMLObjectMacro(this->PETLabelVolumeNode);

      if (this->Scene
          && this->Scene->IsNodeReferencingNodeID(this,
              this->PETLabelVolumeNode->GetID()))
        this->Scene->RemoveReferencedNodeID(this->PETLabelVolumeNode->GetID(),
            this);
    }

  vtkSmartPointer<vtkMRMLScalarVolumeNode> petLblNode = NULL;

  if (this->GetScene() && petLabelVolumeNodeID)
    {
      petLblNode = vtkMRMLScalarVolumeNode::SafeDownCast(
          this->GetScene()->GetNodeByID(petLabelVolumeNodeID));
    }

  vtkSetAndObserveMRMLObjectMacro(this->PETLabelVolumeNode, petLblNode);
  this->SetPETLabelVolumeNodeID(petLabelVolumeNodeID);

  if (this->Scene && this->PETLabelVolumeNode)
    this->Scene->AddReferencedNodeID(this->PETLabelVolumeNodeID, this);

  if(this->PETLabelVolumeNode && this->CenteringTransformNode && this->CenteredVolumes)
    this->PETLabelVolumeNode->SetAndObserveTransformNodeID(this->CenteringTransformNode->GetID());
}


//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTStudyNode::SetAndObservePETLabelVolumeNodeID(
    const std::string& petLabelVolumeNodeID)
{
  this->SetAndObservePETLabelVolumeNodeID(petLabelVolumeNodeID.c_str());
}

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTStudyNode::SetAndObserveCenteringTransformNodeID(
    const char* centeringTransformNodeID)
{
  // first remove references and observers from old node
  if (this->CenteringTransformNode)
    {
      vtkUnObserveMRMLObjectMacro(this->CenteringTransformNode);

      if (this->Scene
          && this->Scene->IsNodeReferencingNodeID(this,
              this->CenteringTransformNode->GetID()))
        this->Scene->RemoveReferencedNodeID(this->CenteringTransformNode->GetID(),
            this);
    }

  vtkSmartPointer<vtkMRMLLinearTransformNode> tnode = NULL;

  if (this->GetScene() && centeringTransformNodeID)
    {
      tnode = vtkMRMLLinearTransformNode::SafeDownCast(
          this->GetScene()->GetNodeByID(centeringTransformNodeID));
    }

  vtkSetAndObserveMRMLObjectMacro(this->CenteringTransformNode, tnode);
  this->SetCenteringTransformNodeID(centeringTransformNodeID);

  this->ObserveCenteringTransform(this->CenteredVolumes);

  if (this->Scene && this->CenteringTransformNode)
    this->Scene->AddReferencedNodeID(this->CenteringTransformNodeID, this);
}


//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTStudyNode::SetAndObserveCenteringTransformNodeID(
    const std::string& centeringTransformNodeID)
{
  this->SetAndObserveCenteringTransformNodeID(centeringTransformNodeID.c_str());
}


//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTStudyNode::SetAndObserveVolumeRenderingDisplayNodeID(
    const char* volumeRenderingDisplayNodeID)
{

  // first remove references and observers from old node
  if (this->VolumeRenderingDisplayNode)
    {
      vtkUnObserveMRMLObjectMacro(this->VolumeRenderingDisplayNode);

      if (this->Scene
          && this->Scene->IsNodeReferencingNodeID(this,
              this->VolumeRenderingDisplayNode->GetID()))
        this->Scene->RemoveReferencedNodeID(this->VolumeRenderingDisplayNode->GetID(),
            this);
    }

  vtkSmartPointer<vtkMRMLVolumeRenderingDisplayNode> vrdNode = NULL;

  if (this->GetScene() && volumeRenderingDisplayNodeID)
    {
      vrdNode = vtkMRMLVolumeRenderingDisplayNode::SafeDownCast(
          this->GetScene()->GetNodeByID(volumeRenderingDisplayNodeID));
    }

  vtkSetAndObserveMRMLObjectMacro(this->VolumeRenderingDisplayNode, vrdNode);
  this->SetVolumeRenderingDisplayNodeID(volumeRenderingDisplayNodeID);

  // TODO: check why not possible: observe the current PETLabelVolume
  //if(this->VolumeRenderingDisplayNode && this->PETVolumeNode)
    //this->VolumeRenderingDisplayNode->SetAndObserveVolumeNodeID(this->PETVolumeNode->GetID());

  if (this->Scene && this->VolumeRenderingDisplayNode)
    this->Scene->AddReferencedNodeID(this->VolumeRenderingDisplayNodeID, this);
}


//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTStudyNode::SetAndObserveVolumeRenderingDisplayNodeID(
    const std::string& volumeRenderingDisplayNodeID)
{
  this->SetAndObserveVolumeRenderingDisplayNodeID(volumeRenderingDisplayNodeID.c_str());
}



//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTStudyNode::SetAndObserveSegmentationROINodeID(
    const char* segmentationROINodeID)
{

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

  if (this->SegmentationROINode)
    {
      if (this->CenteredVolumes)
        {
          if (this->SegmentationROINode->GetParentTransformNode()
              != this->CenteringTransformNode)
            {
              this->SegmentationROINode->SetAndObserveTransformNodeID(
                  this->CenteringTransformNode->GetID());
              this->Modified();
            }
        }
      else
        {
          this->SegmentationROINode->SetAndObserveTransformNodeID(NULL);
          this->Modified();
        }

      if (this->Scene)
        this->Scene->AddReferencedNodeID(this->SegmentationROINodeID, this);

      if(this->SegmentationROINode && this->CenteringTransformNode && this->CenteredVolumes)
        this->SegmentationROINode->SetAndObserveTransformNodeID(this->CenteringTransformNode->GetID());

    }
}

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTStudyNode::SetAndObserveSegmentationROINodeID(
    const std::string& segmentationROINodeID)
{
  this->SetAndObserveSegmentationROINodeID(segmentationROINodeID.c_str());
}


//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTStudyNode::ObserveCenteringTransform(bool observe)
{
  if (observe && this->CenteringTransformNode != NULL)
    {
      if (this->PETVolumeNode
          && this->PETVolumeNode->GetParentTransformNode()
              != this->CenteringTransformNode)
        this->PETVolumeNode->SetAndObserveTransformNodeID(
            this->CenteringTransformNode->GetID());
      if (this->CTVolumeNode
          && this->CTVolumeNode->GetParentTransformNode()
              != this->CenteringTransformNode)
        this->CTVolumeNode->SetAndObserveTransformNodeID(
            this->CenteringTransformNode->GetID());
      if (this->PETLabelVolumeNode
          && this->PETLabelVolumeNode->GetParentTransformNode()
              != this->CenteringTransformNode)
        this->PETLabelVolumeNode->SetAndObserveTransformNodeID(
            this->CenteringTransformNode->GetID());
      if (this->SegmentationROINode
          && this->SegmentationROINode->GetParentTransformNode()
              != this->CenteringTransformNode)
        this->SegmentationROINode->SetAndObserveTransformNodeID(
            this->CenteringTransformNode->GetID());
    }
  else
    {
      if (this->PETVolumeNode)
        this->PETVolumeNode->SetAndObserveTransformNodeID(NULL);
      if (this->CTVolumeNode)
        this->CTVolumeNode->SetAndObserveTransformNodeID(NULL);
      if (this->PETLabelVolumeNode)
        this->PETLabelVolumeNode->SetAndObserveTransformNodeID(NULL);
      if (this->SegmentationROINode)
        this->SegmentationROINode->SetAndObserveTransformNodeID(NULL);
    }


  this->Modified();
}


//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTStudyNode::SetCenteredVolumes(bool centered)
{
  if(this->CenteredVolumes == centered)
    return;

  this->CenteredVolumes = centered;

  this->ObserveCenteringTransform(centered);

}


//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTStudyNode::UpdateReferences()
{
  this->Superclass::UpdateReferences();

  if(this->Scene)
    {
      if(this->PETVolumeNodeID && this->Scene->GetNodeByID(this->PETVolumeNodeID) == NULL)
        this->SetAndObservePETVolumeNodeID(NULL);

      else if(this->CTVolumeNodeID && this->GetScene()->GetNodeByID(this->CTVolumeNodeID) == NULL)
        this->SetAndObserveCTVolumeNodeID(NULL);

      else if(this->PETLabelVolumeNodeID && this->GetScene()->GetNodeByID(this->PETLabelVolumeNodeID) == NULL)
        this->SetAndObservePETLabelVolumeNodeID(NULL);

      else if(this->SegmentationROINodeID && this->GetScene()->GetNodeByID(this->SegmentationROINodeID) == NULL)
        this->SetAndObserveSegmentationROINodeID(NULL);

      else if(this->CenteringTransformNodeID && this->GetScene()->GetNodeByID(this->CenteringTransformNodeID) == NULL)
        this->SetAndObserveCenteringTransformNodeID(NULL);

      else if(this->VolumeRenderingDisplayNodeID && this->GetScene()->GetNodeByID(this->VolumeRenderingDisplayNodeID) == NULL)
        this->SetAndObserveVolumeRenderingDisplayNodeID(NULL);
    }
}

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTStudyNode::UpdateReferenceID(const char *oldID, const char *newID)
{
  this->Superclass::UpdateReferenceID(oldID,newID);

  if(this->PETVolumeNode && !strcmp(oldID,this->PETLabelVolumeNodeID))
    this->SetAndObservePETVolumeNodeID(newID);

  else if(this->CTVolumeNode && !strcmp(oldID,this->CTVolumeNodeID))
    this->SetAndObserveCTVolumeNodeID(newID);

  else if(this->PETLabelVolumeNode && !strcmp(oldID,this->PETLabelVolumeNodeID))
    this->SetAndObservePETLabelVolumeNodeID(newID);

  else if(this->SegmentationROINode && !strcmp(oldID,this->SegmentationROINodeID))
    this->SetAndObserveSegmentationROINodeID(newID);

  else if(this->CenteringTransformNode && !strcmp(oldID,this->CenteringTransformNodeID))
    this->SetAndObserveCenteringTransformNodeID(newID);

  else if(this->VolumeRenderingDisplayNode && !strcmp(oldID,this->VolumeRenderingDisplayNodeID))
    this->SetAndObserveVolumeRenderingDisplayNodeID(newID);
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTStudyNode::SetScene(vtkMRMLScene* scene)
{
  Superclass::SetScene(scene);

  if(this->Scene && this->Scene->GetNodeByID(this->PETVolumeNodeID))
    this->SetAndObservePETVolumeNodeID(this->PETVolumeNodeID);

  if(this->Scene && this->Scene->GetNodeByID(this->CTVolumeNodeID))
    this->SetAndObserveCTVolumeNodeID(this->CTVolumeNodeID);

  if(this->Scene && this->Scene->GetNodeByID(this->PETLabelVolumeNodeID))
    this->SetAndObservePETLabelVolumeNodeID(this->PETLabelVolumeNodeID);

  if(this->Scene && this->Scene->GetNodeByID(this->SegmentationROINodeID))
      this->SetAndObserveSegmentationROINodeID(this->SegmentationROINodeID);

  if(this->Scene && this->Scene->GetNodeByID(this->CenteringTransformNodeID))
      this->SetAndObserveCenteringTransformNodeID(this->CenteringTransformNodeID);

  if(this->Scene && this->Scene->GetNodeByID(this->VolumeRenderingDisplayNodeID))
      this->SetAndObserveVolumeRenderingDisplayNodeID(this->VolumeRenderingDisplayNodeID);
}




