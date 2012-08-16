/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women\"s Hospital (BWH) All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLReprtingReportNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

// VTK includes
#include <vtkCommand.h>
#include <vtkObjectFactory.h>


// MRML includes
#include "vtkMRMLLongPETCTReportNode.h"
#include "vtkMRMLLongPETCTStudyNode.h"

// STD includes

//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLLongPETCTReportNode);

//----------------------------------------------------------------------------
vtkMRMLLongPETCTReportNode::vtkMRMLLongPETCTReportNode()
{
  this->SetHideFromEditors(false);
  this->UserSelectedStudy = NULL;
}

//----------------------------------------------------------------------------
vtkMRMLLongPETCTReportNode::~vtkMRMLLongPETCTReportNode()
{

}

//----------------------------------------------------------------------------
void vtkMRMLLongPETCTReportNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  Superclass::ReadXMLAttributes(atts);


    this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLLongPETCTReportNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);
}

//----------------------------------------------------------------------------
// Copy the node\"s attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, SliceID
void vtkMRMLLongPETCTReportNode::Copy(vtkMRMLNode *anode)
{
  int disabledModify = this->StartModify();
  
  Superclass::Copy(anode);

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLLongPETCTReportNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
int vtkMRMLLongPETCTReportNode::GetStudiesCount() const
{
  return this->Studies.size();
}

//----------------------------------------------------------------------------
int vtkMRMLLongPETCTReportNode::GetSelectedStudiesCount()
{
  int count = 0;

  for(int i=0; i < this->Studies.size(); ++i)
    {
      if(this->Studies.at(i)->IsSelected())
        count++;
    }

  return count;
}


//----------------------------------------------------------------------------
int vtkMRMLLongPETCTReportNode::AddStudy(vtkMRMLLongPETCTStudyNode* study)
{
  std::string studyDate = study->GetAttribute("DICOM.StudyDate");
  std::string studyTime = study->GetAttribute("DICOM.StudyTime");

  int i=0;
  for(; i < this->Studies.size(); ++i)
    {
      std::string listStudyDate = this->Studies.at(i)->GetAttribute("DICOM.StudyDate");
      std::string listStudyTime = this->Studies.at(i)->GetAttribute("DICOM.StudyTime");

      if(studyDate.compare(listStudyDate) < 0)
        break;

      else if(studyDate.compare(listStudyDate) == 0)
        if(studyTime.compare(listStudyTime) < 0)
          break;
    }

  Studies.insert(Studies.begin()+i, study);

  return i;
}


//----------------------------------------------------------------------------
vtkMRMLLongPETCTStudyNode* vtkMRMLLongPETCTReportNode::GetStudy(int index)
{
  if(index >= 0 && index < this->Studies.size())
    return this->Studies[index];

  else
    return NULL;
}

//----------------------------------------------------------------------------
std::vector<vtkMRMLLongPETCTStudyNode*> vtkMRMLLongPETCTReportNode::GetSelectedStudies()
{
  std::vector<vtkMRMLLongPETCTStudyNode*> selectedStudies;

  for(int i=0; i < this->Studies.size();++i)
    {
      if(this->Studies.at(i)->IsSelected())
        selectedStudies.push_back(this->Studies[i]);
    }

  return selectedStudies;
}


//----------------------------------------------------------------------------
vtkMRMLLongPETCTStudyNode* vtkMRMLLongPETCTReportNode::GetSelectedStudy(int index)
{
  std::vector<vtkMRMLLongPETCTStudyNode*> selectedStudies = this->GetSelectedStudies();

  if(index >= 0 && index < selectedStudies.size())
    return selectedStudies[index];


  return NULL;
}

//----------------------------------------------------------------------------
vtkMRMLLongPETCTStudyNode* vtkMRMLLongPETCTReportNode::GetSelectedStudyFirst()
{
  if(this->GetSelectedStudiesCount() > 0)
    return this->GetSelectedStudy(0);

  return NULL;
}

//----------------------------------------------------------------------------
vtkMRMLLongPETCTStudyNode* vtkMRMLLongPETCTReportNode::GetSelectedStudyLast()
{
  int selectedStudies = this->GetSelectedStudiesCount();

  if(selectedStudies > 0)
    return this->GetSelectedStudy(selectedStudies-1);

  return NULL;
}


//----------------------------------------------------------------------------
int vtkMRMLLongPETCTReportNode::GetIndexOfSelectedStudy(const vtkMRMLLongPETCTStudyNode* study)
{

  if(study == NULL)
    return -1;

  std::vector<vtkMRMLLongPETCTStudyNode*> selectedStudies = this->GetSelectedStudies();

  for(int i=0; i < selectedStudies.size(); ++i)
    {
      if (selectedStudies[i] == study)
        return i;
    }

  return -1;
}

//----------------------------------------------------------------------------
int vtkMRMLLongPETCTReportNode::GetIndexOfStudy(const vtkMRMLLongPETCTStudyNode* study)
{

  if(study == NULL)
    return -1;

  for(int i=0; i < this->Studies.size(); ++i)
    {
      if (this->Studies[i] == study)
        return i;
    }

  return -1;
}

