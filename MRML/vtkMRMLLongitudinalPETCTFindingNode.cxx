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
  this->ColorID = -1;

  this->SegmentationROISpecified = false;

  this->ROIxyz[0] = 0.;
  this->ROIxyz[1] = 0.;
  this->ROIxyz[2] = 0.;

  this->ROIRadius[0] = 0.;
  this->ROIRadius[1] = 0.;
  this->ROIRadius[2] = 0.;

}

//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTFindingNode::~vtkMRMLLongitudinalPETCTFindingNode()
{

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

      if (!strcmp(attName, "SegmentationROISpecified"))
        {
          if (!strcmp(attValue, "true"))
            this->SetSegmentationROISpecified(true);
          else
            this->SetSegmentationROISpecified(false);
        }
      else if (!strcmp(attName, "ColorID"))
        {
          this->SetColorID(atoi(attValue));
        }
      else if (!strcmp(attName, "ROIxyz"))
        {
          std::stringstream ss;
          ss << attValue;
          ss >> this->ROIxyz[0];
          ss >> this->ROIxyz[1];
          ss >> this->ROIxyz[2];
        }
      else if (!strcmp(attName, "ROIRadius"))
        {
          std::stringstream ss;
          ss << attValue;
          ss >> this->ROIRadius[0];
          ss >> this->ROIRadius[1];
          ss >> this->ROIRadius[2];
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

  of << indent << " SegmentationROISpecified=\"" << (this->GetSegmentationROISpecified() ? "true" : "false") << "\"";

  of << indent << " ROIxyz=\"" << this->ROIxyz[0] << " " << this->ROIxyz[1]
      << " " << this->ROIxyz[2] << "\"";

  of << indent << " ROIRadius=\"" << this->ROIRadius[0] << " "
      << this->ROIRadius[1] << " " << this->ROIRadius[2] << "\"" << std::endl;

  if (this->ColorID)
    of << indent << " ColorID=\"" << this->ColorID << "\"";

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
      this->SetSegmentationROISpecified(node->GetSegmentationROISpecified());

      node->GetROIxyz(this->ROIxyz);
      node->GetROIRadius(this->ROIRadius);

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
  os << indent << "StudyNodeIDs to SegmentationNodeIDs mapping: "<< (this->GetNumberOfSegmentations() == 0 ? "(none)" : "" ) <<"\n";

  os << indent << "ROIxyz: [" << this->ROIxyz[0] <<","<< this->ROIxyz[1] <<"," << this->ROIxyz[2] << "]\n";
  os << indent << "ROIRadius: [" << this->ROIRadius[0] <<","<< this->ROIRadius[1] <<"," << this->ROIRadius[2] << "]\n";

  StudyIDSegIDMap::iterator it;
  for ( it=this->StudyIDToSegmentationIDMap.begin() ; it != this->StudyIDToSegmentationIDMap.end(); it++ )
    os << indent << (*it).first.c_str() << " => " << (*it).second.c_str() << "\n";

}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTFindingNode::SetROIxyz(double roiXYZ[3])
{
  this->ROIxyz[0] = roiXYZ[0];
  this->ROIxyz[1] = roiXYZ[1];
  this->ROIxyz[2] = roiXYZ[2];

  this->SetSegmentationROISpecified(true);

  this->Modified();
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTFindingNode::GetROIxyz(double xyz[3])
{
  xyz[0] = this->ROIxyz[0];
  xyz[1] = this->ROIxyz[1];
  xyz[2] = this->ROIxyz[2];
}


//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTFindingNode::SetROIRadius(double roiRadius[3])
{
  this->ROIRadius[0] = roiRadius[0];
  this->ROIRadius[1] = roiRadius[1];
  this->ROIRadius[2] = roiRadius[2];

  this->SetSegmentationROISpecified(true);

  this->Modified();
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTFindingNode::GetROIRadius(double radius[3])
{
  radius[0] = this->ROIRadius[0];
  radius[1] = this->ROIRadius[1];
  radius[2] = this->ROIRadius[2];
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

//----------------------------------------------------------------------------
const vtkMRMLLongitudinalPETCTFindingNode::StudyIDSegIDMap&
vtkMRMLLongitudinalPETCTFindingNode::GetStudyNodeIDToSegmentationNodeIDMap() const
{
  return this->StudyIDToSegmentationIDMap;
}



//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTFindingNode::SetScene(vtkMRMLScene* scene)
{
  bool update = this->Scene != scene;

  Superclass::SetScene(scene);

  if(update)
    this->UpdateScene(this->Scene);
}

//-----------------------------------------------------------
void vtkMRMLLongitudinalPETCTFindingNode::UpdateScene(vtkMRMLScene *scene)
{
   Superclass::UpdateScene(scene);
}


//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTFindingNode::UpdateReferences()
{
  this->Superclass::UpdateReferences();

  if(this->Scene)
    {

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

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTFindingNode::ProcessMRMLEvents(vtkObject *caller,
    unsigned long event, void *callData)
{
  Superclass::ProcessMRMLEvents(caller, event, callData);

  vtkMRMLLongitudinalPETCTSegmentationNode* segCaller = vtkMRMLLongitudinalPETCTSegmentationNode::SafeDownCast(caller);

  if(segCaller && event == vtkCommand::ModifiedEvent)
        this->Modified(); // forwarding Modified
}


