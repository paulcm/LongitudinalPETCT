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

#include <vtkNew.h>
#include <vtkSmartPointer.h>

#include <vtkMRMLAnnotationROINode.h>
#include <vtkMRMLScalarVolumeNode.h>
// STD includes


//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLLongPETCTFindingNode);

//----------------------------------------------------------------------------
vtkMRMLLongPETCTFindingNode::vtkMRMLLongPETCTFindingNode()
{
  this->SetHideFromEditors(false);
  this->FindingType.first = "None";
  this->FindingType.second = -1;

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
bool vtkMRMLLongPETCTFindingNode::AddROIForStudy(vtkMRMLLongPETCTStudyNode* study, vtkMRMLAnnotationROINode* roi)
{
    return this->StudyAndROIMap.insert(std::make_pair(study,roi)).second; // "second" points to bool in std::pair return value of insert
}

//----------------------------------------------------------------------------
vtkMRMLAnnotationROINode* vtkMRMLLongPETCTFindingNode::RemoveROIForStudy(const vtkMRMLLongPETCTStudyNode* study)
{
  vtkMRMLAnnotationROINode* roiToRemove = this->GetROIForStudy(study);

  if( roiToRemove != NULL)
      this->StudyAndROIMap.erase(const_cast<vtkMRMLLongPETCTStudyNode*>(study));

  return roiToRemove;
}


//----------------------------------------------------------------------------
bool vtkMRMLLongPETCTFindingNode::AddLabelMapForROI(vtkMRMLAnnotationROINode* roi, vtkMRMLScalarVolumeNode* labelMapVolume)
{
    return this->ROIandLabelsMap.insert(std::make_pair(roi,labelMapVolume)).second; // "second" points to bool in std::pair return value of insert
}

//----------------------------------------------------------------------------
vtkMRMLScalarVolumeNode* vtkMRMLLongPETCTFindingNode::RemoveLabelMapForROI(const vtkMRMLAnnotationROINode* roi)
{
  vtkMRMLScalarVolumeNode* labelMapVolumeToRemove = this->GetLabelMapVolumeForROI(roi);

  if( labelMapVolumeToRemove != NULL )
    this->ROIandLabelsMap.erase(const_cast<vtkMRMLAnnotationROINode*>(roi));

  return labelMapVolumeToRemove;
}



//----------------------------------------------------------------------------
vtkMRMLAnnotationROINode* vtkMRMLLongPETCTFindingNode::GetROIForStudy(const vtkMRMLLongPETCTStudyNode* study)
{
  vtkMRMLAnnotationROINode* roi = NULL;

  if(this->StudyAndROIMap.find(const_cast<vtkMRMLLongPETCTStudyNode*>(study)) != this->StudyAndROIMap.end())
    {
        roi = this->StudyAndROIMap[const_cast<vtkMRMLLongPETCTStudyNode*>(study)];
    }

  return roi;
}

//----------------------------------------------------------------------------
vtkMRMLScalarVolumeNode* vtkMRMLLongPETCTFindingNode::GetLabelMapVolumeForROI(const vtkMRMLAnnotationROINode* roi)
{
  vtkMRMLScalarVolumeNode* labelMapVolume = NULL;

  if(this->ROIandLabelsMap.find(const_cast<vtkMRMLAnnotationROINode*>(roi)) != this->ROIandLabelsMap.end())
    {
      labelMapVolume = this->ROIandLabelsMap[const_cast<vtkMRMLAnnotationROINode*>(roi)];
    }

  return labelMapVolume;
}

//----------------------------------------------------------------------------
std::pair<std::string, int> vtkMRMLLongPETCTFindingNode::GetFindingType()
{
  return this->FindingType;
}

//----------------------------------------------------------------------------
void vtkMRMLLongPETCTFindingNode::SetFindingType(std::pair<std::string,int> type)
{
  this->SetFindingType(type.first, type.second);
}

//----------------------------------------------------------------------------
void vtkMRMLLongPETCTFindingNode::SetFindingType(std::string typeName, int colorID)
{
  this->FindingType.first = typeName;
  this->FindingType.second = colorID;
}


