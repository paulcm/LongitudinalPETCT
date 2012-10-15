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
  /*
  this->SegmentationROINode = NULL;
  this->ModelHierarchyNode = NULL;
  this->ObservedEvents = NULL;

  if(this->SegmentationROINodeID)
    delete [] this->SegmentationROINodeID;

  if(this->ModelHierarchyNodeID)
    delete [] this->ModelHierarchyNodeID;
    */
}

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTFindingNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  Superclass::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;

  while (*atts != NULL)
    {
      attName = *(atts++);
      attValue = *(atts++);

      if (!strcmp(attName, "ColorID"))
        {
          this->SetColorID(atoi(attValue));
        }
      else if (!strcmp(attName, "TypeName"))
        {
          this->SetTypeName(attValue);
        }
      else if (!strcmp(attName, "SegmentationROINodeID"))
        {
          this->SetAndObserveSegmentationROINodeID(attValue);
        }
      else if (!strcmp(attName, "ModelHierarchyNodeID"))
        {
          this->SetAndObserveModelHierarchyNodeID(attValue);
        }
      else if (!strcmp(attName, "StudyNodeIDSegmentationNodeIDMap"))
        {
          this->StudyIDToSegmentationIDMap.clear(); // just to be sure

          // format is 'StudyNodeID':'SegmentationNodeID'
          // Search for 4 single quotes and pull out the pieces.
          std::string text(attValue);
          const std::string::size_type n = text.length();
          std::string::size_type first = 0, second, third, fourth;
          first = text.find_first_of("'");
          while (first < n)
            {
              second = text.find_first_of("'", first + 1);
              third = text.find_first_of("'", second + 1);
              fourth = text.find_first_of("'", third + 1);
              std::string studyID = text.substr(first + 1, second - first - 1);
              std::string segID = text.substr(third + 1, fourth - third - 1);
              this->StudyIDToSegmentationIDMap.insert(
                  std::make_pair(studyID, segID));
              first = text.find_first_of("'",fourth+1);
            }
        }

      this->EndModify(disabledModify);
    }
}

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTFindingNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);

  vtkIndent indent(nIndent);

  if (this->ColorID)
    of << indent << " ColorID=\"" << this->ColorID << "\"";
  if (this->TypeName)
    of << indent << " TypeName=\"" << this->TypeName << "\"";
  if (this->SegmentationROINodeID)
    of << indent << " SegmentationROINodeID=\"" << this->SegmentationROINodeID
        << "\"";
  if (this->ModelHierarchyNodeID)
    of << indent << " ModelHierarchyNodeID=\"" << this->ModelHierarchyNodeID
        << "\"";

  of << indent << " StudyNodeIDSegmentationNodeIDMap=\"";
  StudyIDSegIDMap::iterator it;
  for (it = this->StudyIDToSegmentationIDMap.begin();
      it != this->StudyIDToSegmentationIDMap.end(); it++)
    {
      if (it != this->StudyIDToSegmentationIDMap.begin())
        {
          of << " ";
        }

      of << "'" << (*it).first.c_str() << "':'" << (*it).second.c_str() << "'";
    }
  of << "\"" << std::endl;

}

//----------------------------------------------------------------------------
// Copy the node\"s attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, SliceID
void
vtkMRMLLongitudinalPETCTFindingNode::Copy(vtkMRMLNode *anode)
{
  int disabledModify = this->StartModify();

  Superclass::Copy(anode);

  vtkMRMLLongitudinalPETCTFindingNode* node =
      vtkMRMLLongitudinalPETCTFindingNode::SafeDownCast(anode);
  if (node)
    {
      this->SetColorID(node->GetColorID());
      this->SetTypeName(node->GetTypeName());
      this->SetAndObserveSegmentationROINodeID(
          node->GetSegmentationROINodeID());
      this->SetAndObserveModelHierarchyNodeID(node->GetModelHierarchyNodeID());

      // remove all mapped values manually and do not use clear to make sure observer removel and referencing is handled correctly
      StudyIDSegIDMap::iterator it;
      while (this->StudyIDToSegmentationIDMap.size() > 0)
        {
          it = this->StudyIDToSegmentationIDMap.begin();
          this->RemoveStudyNodeIDToSegmentationNodeIDMap((*it).first.c_str());
        }

      StudyIDSegIDMap map = node->GetStudyNodeIDToSegmentationNodeIDMap();
      for (it = map.begin(); it != map.end(); it++)
        {
          std::string studyID = (*it).first;
          std::string segID = (*it).second;

          this->MapStudyNodeIDToSegmentationNodeID(studyID.c_str(),
              segID.c_str());
        }
    }

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

  StudyIDSegIDMap::iterator it;
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

  vtkMRMLAnnotationROINode* roiNode = NULL;

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

  vtkMRMLModelHierarchyNode* mhnode = NULL;

  if (this->GetScene() && modelHierarchyNodeID)
    {
      mhnode = vtkMRMLModelHierarchyNode::SafeDownCast(
          this->GetScene()->GetNodeByID(modelHierarchyNodeID));
    }

  vtkSetAndObserveMRMLObjectMacro(this->ModelHierarchyNode, mhnode);
  this->SetModelHierarchyNodeID(modelHierarchyNodeID);

  if (this->Scene && this->ModelHierarchyNode)
    {

      this->Scene->AddReferencedNodeID(this->ModelHierarchyNodeID, this);

      // update all mapped segmentations model hierarchies
      StudyIDSegIDMap::iterator it;
      for (it = this->StudyIDToSegmentationIDMap.begin();
          it != this->StudyIDToSegmentationIDMap.end(); it++)
        {
          vtkMRMLLongitudinalPETCTSegmentationNode* seg = vtkMRMLLongitudinalPETCTSegmentationNode::SafeDownCast(this->Scene->GetNodeByID((*it).second));
          this->UpdateSegmentationModelHierarchyParent(seg);
        }
    }

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
vtkMRMLLongitudinalPETCTFindingNode::MapStudyNodeIDToSegmentationNodeID(
    const char* studyNodeID, const char* segmentationNodeID)
{
  if(this->IsSegmentationNodeInMap(segmentationNodeID))
    return false;

  bool ok = this->StudyIDToSegmentationIDMap.insert(
      std::make_pair(studyNodeID, segmentationNodeID)).second;

  if (ok && this->Scene)
    {
       vtkObserveMRMLObjectMacro(this->Scene->GetNodeByID(segmentationNodeID));
       this->Scene->AddReferencedNodeID(studyNodeID, this);
       this->Scene->AddReferencedNodeID(segmentationNodeID, this);
    }

  this->Modified();

  return ok;
}

//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTSegmentationNode*
vtkMRMLLongitudinalPETCTFindingNode::RemoveStudyNodeIDToSegmentationNodeIDMap(
    const char* studyNodeID)
{
  vtkMRMLLongitudinalPETCTSegmentationNode* segmentationToRemove =
      this->GetSegmentationMappedByStudyNodeID(studyNodeID);

  int removed = 0;

  if (segmentationToRemove)
    {
      removed = this->StudyIDToSegmentationIDMap.erase(studyNodeID);
    }

  if (removed > 0)
    {
      if (this->Scene)
        {
          vtkUnObserveMRMLObjectMacro(segmentationToRemove);
          this->Scene->RemoveReferencedNodeID(studyNodeID, this);
          this->Scene->RemoveReferencedNodeID(segmentationToRemove->GetID(),
              this);
        }

      this->Modified();
    }
  else
    segmentationToRemove = NULL;

  return segmentationToRemove;
}


//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTSegmentationNode* vtkMRMLLongitudinalPETCTFindingNode::GetSegmentationMappedByStudyNodeID(const char* studyNodeID)
{
  vtkMRMLLongitudinalPETCTSegmentationNode* segmentation = NULL;

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
bool
vtkMRMLLongitudinalPETCTFindingNode::IsSegmentationNodeInMap(
    const char* segmentationNodeID)
{
  StudyIDSegIDMap::iterator it;
  for (it = this->StudyIDToSegmentationIDMap.begin();
      it != this->StudyIDToSegmentationIDMap.end(); it++)
    {
      if(!strcmp(segmentationNodeID, (*it).second.c_str()))
        return true;
    }

  return false;
}

const vtkMRMLLongitudinalPETCTFindingNode::StudyIDSegIDMap&
vtkMRMLLongitudinalPETCTFindingNode::GetStudyNodeIDToSegmentationNodeIDMap() const
{
  return this->StudyIDToSegmentationIDMap;
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
          std::cout << "UPDATED MH" << std::endl;
          segmentation->GetModelHierarchyNode()->SetParentNodeID(
              this->GetModelHierarchyNode()->GetID());
          this->Modified();
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

      StudyIDSegIDMap::iterator it;
      for ( it=this->StudyIDToSegmentationIDMap.begin() ; it != this->StudyIDToSegmentationIDMap.end(); it++ )
        {
          const char* studyNodeID = (*it).first.c_str();
          const char* segNodeID = (*it).second.c_str();

          vtkMRMLNode* studyNode = this->Scene->GetNodeByID(studyNodeID);
          vtkMRMLNode* segNode = this->Scene->GetNodeByID(segNodeID);

          this->Modified();

          if( ! studyNode || ! segNode)
            {
              this->StudyIDToSegmentationIDMap.erase((*it).first);

//              if(!studyNode && segNode)
//                {
//                  this->Scene->RemoveReferencedNodeID(segNodeID, this);
//                  std::cout << "ERASING2" << std::endl;
//                }
//
//              else if(studyNode && !segNode)
//                {
//                  this->Scene->RemoveReferencedNodeID(studyNodeID, this);
//                }
            }
        }
    }
}

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTFindingNode::UpdateReferenceID(const char *oldID,
    const char *newID)
{
  this->Superclass::UpdateReferenceID(oldID, newID);

  if (this->SegmentationROINode && !strcmp(oldID, this->SegmentationROINodeID))
    this->SetAndObserveSegmentationROINodeID(newID);

  else if (this->ModelHierarchyNode
      && !strcmp(oldID, this->ModelHierarchyNodeID))
    this->SetAndObserveModelHierarchyNodeID(newID);

  else
    {
      StudyIDSegIDMap::iterator it;
      for (it = this->StudyIDToSegmentationIDMap.begin();
          it != this->StudyIDToSegmentationIDMap.end(); it++)
        {
          const char* studyNodeID = (*it).first.c_str();
          const char* segNodeID = (*it).second.c_str();

          if (!strcmp(studyNodeID, oldID))
            {
              this->StudyIDToSegmentationIDMap.erase((*it).first);
              this->StudyIDToSegmentationIDMap.insert(
                  std::make_pair(newID, segNodeID));
            }
          else if (!strcmp(segNodeID, oldID))
            {
              this->StudyIDToSegmentationIDMap.erase((*it).first);
              this->StudyIDToSegmentationIDMap.insert(
                  std::make_pair(studyNodeID, newID));
            }
        }
    }
}

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTFindingNode::ProcessMRMLEvents(vtkObject *caller,
    unsigned long event, void *callData)
{
  Superclass::ProcessMRMLEvents(caller, event, callData);

  vtkMRMLLongitudinalPETCTSegmentationNode* segCaller = vtkMRMLLongitudinalPETCTSegmentationNode::SafeDownCast(caller);

  if(segCaller)
    {
      if(event == vtkMRMLLongitudinalPETCTSegmentationNode::ModelHierarchyChangedEvent)
        this->UpdateSegmentationModelHierarchyParent(segCaller);

      else if(event == vtkCommand::ModifiedEvent)
        this->Modified(); // forwarding Modified
    }
}


