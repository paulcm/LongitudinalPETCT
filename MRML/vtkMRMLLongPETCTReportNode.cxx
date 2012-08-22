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

#include "vtkMRMLLongPETCTFindingNode.h"
#include <vtkMRMLColorNode.h>
// STD includes

#include <cassert>


//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLLongPETCTReportNode);

//----------------------------------------------------------------------------
vtkMRMLLongPETCTReportNode::vtkMRMLLongPETCTReportNode()
{
  this->Initialize();
}

//----------------------------------------------------------------------------
vtkMRMLLongPETCTReportNode::~vtkMRMLLongPETCTReportNode()
{

}

//----------------------------------------------------------------------------
void vtkMRMLLongPETCTReportNode::Initialize()
{
    int disabledModify = this->StartModify();

    this->SetHideFromEditors(false);

    this->UserSelectedStudy = NULL;
    this->UserSelectedFinding = NULL;

    this->AddFindingType(std::make_pair("Tumor",7));
    this->AddFindingType(std::make_pair("Lymph Node",23));
    this->AddFindingType(std::make_pair("Liver",216));
    this->AddFindingType(std::make_pair("Aorta",191));

    this->EndModify(disabledModify);
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
  
  this->SetAttribute("DICOM.PatientName",anode->GetAttribute("DICOM.PatientName"));
  this->SetAttribute("DICOM.PatientBirthDate",anode->GetAttribute("DICOM.PatientBirthDate"));
  this->SetAttribute("DICOM.PatientSex",anode->GetAttribute("DICOM.PatientSex"));

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
int vtkMRMLLongPETCTReportNode::GetFindingsCount() const
{
  return this->Findings.size();
}

//----------------------------------------------------------------------------
int vtkMRMLLongPETCTReportNode::GetSelectedStudiesCount() const
{
  int count = 0;

  for(int i=0; i < this->Studies.size(); ++i)
    {
      if(this->Studies.at(i)->GetSelected())
        count++;
    }

  return count;
}


//----------------------------------------------------------------------------
int vtkMRMLLongPETCTReportNode::AddStudy(vtkMRMLLongPETCTStudyNode* study)
{
  int disabledModify = this->StartModify();

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

  this->EndModify(disabledModify);

  this->InvokeEvent(vtkMRMLLongPETCTReportNode::StudiesChangedEvent);

  return i;
}

//----------------------------------------------------------------------------
void vtkMRMLLongPETCTReportNode::AddFinding(vtkMRMLLongPETCTFindingNode* finding)
{
  int disabledModify = this->StartModify();

  this->Findings.push_back(finding);

  this->EndModify(disabledModify);

  this->InvokeEvent(vtkMRMLLongPETCTReportNode::FindingsChangedEvent);
}


//----------------------------------------------------------------------------
vtkMRMLLongPETCTStudyNode* vtkMRMLLongPETCTReportNode::GetStudy(int index) const
{
  if(index >= 0 && index < this->Studies.size())
    return this->Studies[index];

  else
    return NULL;
}


//----------------------------------------------------------------------------
vtkMRMLLongPETCTFindingNode* vtkMRMLLongPETCTReportNode::GetFinding(int index) const
{
  if(index >= 0 && index < this->Findings.size())
    return this->Findings[index];

  else
    return NULL;
}


//----------------------------------------------------------------------------
bool vtkMRMLLongPETCTReportNode::FindingNameInList(const std::string& name)
{
  for(int i=0; i < this->Findings.size(); ++i)
    {
      std::string findingName = this->Findings.at(i)->GetName();
      if(findingName.compare(name) == 0)
        return true;
    }

  return false;
}

//----------------------------------------------------------------------------
std::vector<vtkMRMLLongPETCTStudyNode*> vtkMRMLLongPETCTReportNode::GetSelectedStudies()
{
  std::vector<vtkMRMLLongPETCTStudyNode*> selectedStudies;

  for(int i=0; i < this->Studies.size();++i)
    {
      if(this->Studies.at(i)->GetSelected())
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

//----------------------------------------------------------------------------
int vtkMRMLLongPETCTReportNode::GetIndexOfFinding(const vtkMRMLLongPETCTFindingNode* finding)
{

  if(finding == NULL)
    return -1;

  for(int i=0; i < this->Findings.size(); ++i)
    {
      if (this->Findings[i] == finding)
        return i;
    }

  return -1;
}

//----------------------------------------------------------------------------
const vtkMRMLColorNode* vtkMRMLLongPETCTReportNode::GetColorNode()
{
  if(this->GetScene() == NULL)
    return NULL;


  return vtkMRMLColorNode::SafeDownCast(this->GetScene()->GetNodeByID(this->GetAttribute("ColorNodeID")));
}

//----------------------------------------------------------------------------
int vtkMRMLLongPETCTReportNode::GetFindingTypeColorID(const std::string& typeName)
{
  int index = this->GetIndexOfFindingTypeName(typeName);

  if(index == -1)
    return index;

  return this->FindingTypes.at(index).second;
}

//----------------------------------------------------------------------------
std::string vtkMRMLLongPETCTReportNode::GetFindingTypeName(int colorID)
{
  int index = this->GetIndexOfFindingColorID(colorID);

  if(index == -1)
    {
      std::string emptyString;
      return emptyString;
    }
  return this->FindingTypes.at(index).first;
}

//----------------------------------------------------------------------------
int vtkMRMLLongPETCTReportNode::GetIndexOfFindingColorID(int colorID)
{
  for(int i=0; i < this->FindingTypes.size(); ++i)
    {
      if(this->FindingTypes.at(i).second == colorID)
        return i;
    }

  return -1;
}

//----------------------------------------------------------------------------
int vtkMRMLLongPETCTReportNode::GetIndexOfFindingTypeName(const std::string& typeName)
{
  for(int i=0; i < this->FindingTypes.size(); ++i)
    {
      if(this->FindingTypes.at(i).first.compare(typeName) == 0)
        return i;
    }

  return -1;
}



//----------------------------------------------------------------------------
void vtkMRMLLongPETCTReportNode::AddFindingType(std::pair<std::string, int> type)
{
  int disabledModify = this->StartModify();

  int indexName = this->GetIndexOfFindingTypeName(type.first);
  int indexColor = this->GetIndexOfFindingColorID(type.second);

  vtkMRMLLongPETCTReportNode::FindingTypes.push_back(type);

  this->EndModify(disabledModify);
  this->InvokeEvent(FindingTypesChangedEvent);
}

//----------------------------------------------------------------------------
void vtkMRMLLongPETCTReportNode::SetFindingTypeColorID(const std::string& typeName, int newColorID)
{
  int disabledModify = this->StartModify();

  int index = this->GetIndexOfFindingTypeName(typeName);

  if(index >= 0 && index < this->FindingTypes.size())
    this->FindingTypes.at(index).second = newColorID;

  this->EndModify(disabledModify);
  this->InvokeEvent(FindingTypesChangedEvent);
}

//----------------------------------------------------------------------------
int vtkMRMLLongPETCTReportNode::GetFindingTypesCount()
{
  return this->FindingTypes.size();
}

//----------------------------------------------------------------------------
std::pair<std::string,int> vtkMRMLLongPETCTReportNode::GetFindingType(int index)
{
  if(index >= 0 && index < this->FindingTypes.size())
    return this->FindingTypes.at(index);

  return std::make_pair("None",-1);
}

//----------------------------------------------------------------------------
void vtkMRMLLongPETCTReportNode::DeleteFinding(vtkMRMLLongPETCTFindingNode* finding)
{
  int disabledModify = this->StartModify();

  int index = this->GetIndexOfFinding(finding);

  if(index >= 0 && index < this->GetFindingsCount())
    this->Findings.erase(this->Findings.begin()+index);

  this->EndModify(disabledModify);
  this->InvokeEvent(FindingsChangedEvent);
}
