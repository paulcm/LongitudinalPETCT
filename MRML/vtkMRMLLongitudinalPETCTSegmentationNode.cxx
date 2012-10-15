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
  this->SetHideFromEditors(false);

  this->LabelVolumeNodeID = NULL;
  this->LabelVolumeNode = NULL;

  this->ModelHierarchyNodeID = NULL;
  this->ModelHierarchyNode = NULL;

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
}


//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTSegmentationNode::~vtkMRMLLongitudinalPETCTSegmentationNode()
{
  /*
  this->LabelVolumeNode = NULL;
  this->ModelHierarchyNode = NULL;

  if(this->LabelVolumeNodeID)
    delete [] this->LabelVolumeNodeID;

  if(this->ModelHierarchyNodeID)
    delete [] this->ModelHierarchyNodeID;

    */
}

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTSegmentationNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  Superclass::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;

  while (*atts != NULL)
    {
      attName = *(atts++);
      attValue = *(atts++);

      if (!strcmp(attName, "LabelVolumeNodeID"))
        {
          this->SetAndObserveLabelVolumeNodeID(attValue);
        }
      else if (!strcmp(attName, "ModelHierarchyNodeID"))
        {
          this->SetAndObserveModelHierarchyNodeID(attValue);
        }
      else if (!strcmp(attName, "ModelVisible"))
        {
          if (!strcmp(attValue, "true"))
            this->SetModelVisible(true);
          else
            this->SetModelVisible(false);
        }
      else if (!strcmp(attName, "SUVMax"))
        {
          this->SetSUVMax(atof(attValue));
        }
      else if (!strcmp(attName, "SUVMean"))
        {
          this->SetSUVMean(atof(attValue));
        }
      else if (!strcmp(attName, "SUVMin"))
        {
          this->SetSUVMin(atof(attValue));
        }
      else if (!strcmp(attName, "ROIxyz"))
        {
          std::stringstream ss;
          ss << attValue;
          ss >> this->ROIxyz[0];
          ss >> this->ROIxyz[1];
          ss >> this->ROIxyz[2];
        }
      else if (!strcmp(attName, "ROIxyz"))
        {
          std::stringstream ss;
          ss << attValue;
          ss >> this->ROIRadius[0];
          ss >> this->ROIRadius[1];
          ss >> this->ROIRadius[2];
        }

    }

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTSegmentationNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);

  vtkIndent indent(nIndent);

  if (this->LabelVolumeNodeID)
    of << indent << " LabelVolumeNodeID=\"" << this->LabelVolumeNodeID << "\"";
  if (this->ModelHierarchyNodeID)
    of << indent << " ModelHierarchyNodeID=\"" << this->ModelHierarchyNodeID
        << "\"";

  of << indent << " ModelVisible=\"" << (this->ModelVisible ? "true" : "false") << "\"";

  of << indent << " SUVMax=\"" << this->SUVMax << "\"";
  of << indent << " SUVMean=\"" << this->SUVMean << "\"";
  of << indent << " SUVMin=\"" << this->SUVMin << "\"";


  of << indent << " ROIxyz=\"" << this->ROIxyz[0] << " " << this->ROIxyz[1]
      << " " << this->ROIxyz[2] << "\"";

  of << indent << " ROIRadius=\"" << this->ROIRadius[0] << " "
      << this->ROIRadius[1] << " " << this->ROIRadius[2] << "\"" <<  std::endl;

}

//----------------------------------------------------------------------------
// Copy the node\"s attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, SliceID
void vtkMRMLLongitudinalPETCTSegmentationNode::Copy(vtkMRMLNode *anode)
{
  int disabledModify = this->StartModify();
  
  Superclass::Copy(anode);

  vtkSmartPointer<vtkMRMLLongitudinalPETCTSegmentationNode> node = vtkMRMLLongitudinalPETCTSegmentationNode::SafeDownCast(anode);
  if (node)
    {
      this->SetAndObserveLabelVolumeNodeID(node->GetLabelVolumeNodeID());
      this->SetAndObserveModelHierarchyNodeID(node->GetModelHierarchyNodeID());
      this->SetModelVisible(node->GetModelVisible());
      this->SetSUVMax(node->GetSUVMax());
      this->SetSUVMean(node->GetSUVMean());
      this->SetSUVMin(node->GetSUVMin());
      node->GetROIxyz(this->ROIxyz);
      node->GetROIRadius(this->ROIRadius);
    }
  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTSegmentationNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);

  os << indent << "LabelVolumeNodeID: " << (this->LabelVolumeNodeID ? this->LabelVolumeNodeID : "(none)") << "\n";
  os << indent << "ModelHierarchyNodeID: " << (this->ModelHierarchyNodeID ? this->ModelHierarchyNodeID : "(none)") << "\n";
  os << indent << "ModelVisible: " << this->ModelVisible << "\n";
  os << indent << "SUVMax: " << this->SUVMax << "\n";
  os << indent << "SUVMean: " << this->SUVMean << "\n";
  os << indent << "SUVMin: " << this->SUVMin << "\n";
  os << indent << "ROIxyz: [" << this->ROIxyz[0] <<","<< this->ROIxyz[1] <<"," << this->ROIxyz[2] << "]\n";
  os << indent << "ROIRadius: [" << this->ROIRadius[0] <<","<< this->ROIRadius[1] <<"," << this->ROIRadius[2] << "]\n";

}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTSegmentationNode::SetROIxyz(double roiXYZ[3])
{
  this->ROIxyz[0] = roiXYZ[0];
  this->ROIxyz[1] = roiXYZ[1];
  this->ROIxyz[2] = roiXYZ[2];

  this->Modified();
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

  this->Modified();
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


////----------------------------------------------------------------------------
//void
//vtkMRMLLongitudinalPETCTSegmentationNode::RemoveCurrentModelHierarchyFromScene()
//{
//  if (this->ModelHierarchyNode)
//    {
//      vtkNew<vtkCollection> cmn;
//      this->ModelHierarchyNode->GetChildrenModelNodes(cmn.GetPointer());
//
//      for (int i = 0; i < cmn->GetNumberOfItems(); ++i)
//        {
//          vtkSmartPointer<vtkMRMLModelNode> tempModelNode =
//              vtkMRMLModelNode::SafeDownCast(cmn->GetItemAsObject(i));
//
//          if (!tempModelNode)
//            continue;
//
//          if (this->Scene)
//            this->Scene->RemoveNode(tempModelNode);
//        }
//
//      if (this->Scene)
//        {
//          vtkUnObserveMRMLObjectMacro(this->ModelHierarchyNode);
//          this->Scene->RemoveNode(this->ModelHierarchyNode);
//        }
//
//      this->ModelHierarchyNode = NULL;
//    }
//}


//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTSegmentationNode::SetAndObserveLabelVolumeNodeID(
    const char* labelVolumeNodeID)
{

  if (labelVolumeNodeID && this->LabelVolumeNodeID
      && !strcmp(labelVolumeNodeID, this->LabelVolumeNodeID))
    return;

  // first remove references and observers from old node
  if (this->LabelVolumeNode)
    {
      vtkUnObserveMRMLObjectMacro(this->LabelVolumeNode);

      if (this->Scene
          && this->Scene->IsNodeReferencingNodeID(this,
              this->LabelVolumeNode->GetID()))
        this->Scene->RemoveReferencedNodeID(this->LabelVolumeNode->GetID(),
            this);
    }

  // than set new node
  vtkSmartPointer<vtkMRMLScalarVolumeNode> lvnode = NULL;

  if (this->GetScene() && labelVolumeNodeID)
    {
      lvnode = vtkMRMLScalarVolumeNode::SafeDownCast(
          this->GetScene()->GetNodeByID(labelVolumeNodeID));
    }

  vtkSetAndObserveMRMLObjectMacro(this->LabelVolumeNode, lvnode);
  this->SetLabelVolumeNodeID(labelVolumeNodeID);

  this->AdjustModelTransformToLabelVolume();

  if (this->Scene)
    this->Scene->AddReferencedNodeID(this->LabelVolumeNodeID, this);

}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTSegmentationNode::SetAndObserveLabelVolumeNodeID(const std::string& labelVolumeNodeID)
{
  this->SetAndObserveLabelVolumeNodeID(labelVolumeNodeID.c_str());
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTSegmentationNode::SetAndObserveModelHierarchyNodeID(const char* modelHierarchyNodeID)
{

  if(modelHierarchyNodeID && this->ModelHierarchyNodeID && !strcmp(modelHierarchyNodeID, this->ModelHierarchyNodeID))
        return;

      // first remove references and observers from old node
      if(this->ModelHierarchyNode)
        {
          vtkUnObserveMRMLObjectMacro(this->ModelHierarchyNode);

          if(this->Scene && this->Scene->IsNodeReferencingNodeID(this,this->ModelHierarchyNode->GetID()))
                this->Scene->RemoveReferencedNodeID(this->ModelHierarchyNode->GetID(),this);
        }

  vtkSmartPointer<vtkMRMLModelHierarchyNode> mhnode = NULL;

  if(this->GetScene() && modelHierarchyNodeID)
    {
      mhnode = vtkMRMLModelHierarchyNode::SafeDownCast(this->GetScene()->GetNodeByID(modelHierarchyNodeID));
    }

  vtkSetAndObserveMRMLObjectMacro(this->ModelHierarchyNode, mhnode);
  this->SetModelHierarchyNodeID(modelHierarchyNodeID);

  this->InvokeEvent(vtkMRMLLongitudinalPETCTSegmentationNode::ModelHierarchyChangedEvent);

  this->AdjustModelTransformToLabelVolume();

  if (this->Scene)
    {
      this->Scene->AddReferencedNodeID(this->ModelHierarchyNodeID, this);
    }
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTSegmentationNode::SetAndObserveModelHierarchyNodeID(const std::string& modelHierarchyNodeID)
{
  this->SetAndObserveModelHierarchyNodeID(modelHierarchyNodeID.c_str());
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

  this->Modified();
}


//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTSegmentationNode::ProcessMRMLEvents(vtkObject *caller,
    unsigned long event, void *callData)
{
  Superclass::ProcessMRMLEvents(caller, event, callData);

  if (caller == this->LabelVolumeNode.GetPointer()
      && event == vtkCommand::ModifiedEvent)
    this->AdjustModelTransformToLabelVolume();

}

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTSegmentationNode::UpdateReferences()
{
  this->Superclass::UpdateReferences();

  if(this->Scene)
    {
      if(this->LabelVolumeNodeID && this->Scene->GetNodeByID(this->LabelVolumeNodeID) == NULL)
        this->SetAndObserveLabelVolumeNodeID(NULL);

      else if(this->ModelHierarchyNodeID && this->GetScene()->GetNodeByID(this->ModelHierarchyNodeID) == NULL)
        this->SetAndObserveModelHierarchyNodeID(NULL);
    }
}

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTSegmentationNode::UpdateReferenceID(const char *oldID, const char *newID)
{
  this->Superclass::UpdateReferenceID(oldID,newID);

  if(this->LabelVolumeNode && !strcmp(oldID,this->LabelVolumeNodeID))
    this->SetAndObserveLabelVolumeNodeID(newID);

  else if(this->ModelHierarchyNode && !strcmp(oldID,this->ModelHierarchyNodeID))
    this->SetAndObserveModelHierarchyNodeID(newID);
}


//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTSegmentationNode::SetScene(vtkMRMLScene* scene)
{
  Superclass::SetScene(scene);

  if(this->Scene && this->Scene->GetNodeByID(this->LabelVolumeNodeID))
    this->SetAndObserveModelHierarchyNodeID(this->LabelVolumeNodeID);

  if(this->Scene && this->Scene->GetNodeByID(this->ModelHierarchyNodeID))
      this->SetAndObserveModelHierarchyNodeID(this->ModelHierarchyNodeID);

}




