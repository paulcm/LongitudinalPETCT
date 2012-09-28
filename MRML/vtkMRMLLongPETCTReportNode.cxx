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
#include <vtkCallbackCommand.h>
// STD includes
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkEventForwarderCommand.h>
#include <vtkMRMLColorNode.h>
#include <vtkMRMLColorTableNode.h>
#include <cassert>

#include <vector>


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
    this->FindingTypesColorTable = NULL;

    this->studyModifiedForwarder = vtkSmartPointer<vtkEventForwarderCommand>::New();
    studyModifiedForwarder->SetTarget(this);
    this->findingModifiedForwarder = vtkSmartPointer<vtkEventForwarderCommand>::New();
    findingModifiedForwarder->SetTarget(this);

    // TODO dynamically
    this->NumberOfDefaultFindingTypes = 6;

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

  for(unsigned int i=0; i < this->Studies.size(); ++i)
    {
      if(this->Studies.at(i)->GetSelectedForSegmentation())
        count++;
    }

  return count;
}

//----------------------------------------------------------------------------
int vtkMRMLLongPETCTReportNode::GetSelectedStudiesForAnalysisCount() const
{
  int count = 0;

  for(unsigned int i=0; i < this->Studies.size(); ++i)
    {
      if(this->Studies.at(i)->GetSelectedForSegmentation() && this->Studies.at(i)->GetSelectedForAnalysis())
        count++;
    }

  return count;
}

//----------------------------------------------------------------------------
int vtkMRMLLongPETCTReportNode::AddStudy(vtkMRMLLongPETCTStudyNode* study)
{

  std::string studyDate = study->GetAttribute("DICOM.StudyDate");
  std::string studyTime = study->GetAttribute("DICOM.StudyTime");

  unsigned int i=0;
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


  if(studyModifiedForwarder != NULL)
    study->AddObserver(vtkCommand::ModifiedEvent,this->studyModifiedForwarder);

  Studies.insert(Studies.begin()+i, study);

  this->InvokeEvent(vtkCommand::ModifiedEvent);

  return static_cast<int>(i);
}

//----------------------------------------------------------------------------
void vtkMRMLLongPETCTReportNode::AddFinding(vtkMRMLLongPETCTFindingNode* finding)
{
  if(findingModifiedForwarder != NULL)
    finding->AddObserver(vtkCommand::ModifiedEvent,this->findingModifiedForwarder);

  this->Findings.push_back(finding);


  this->InvokeEvent(vtkCommand::ModifiedEvent);
}


//----------------------------------------------------------------------------
vtkMRMLLongPETCTStudyNode* vtkMRMLLongPETCTReportNode::GetStudy(int index) const
{
  if(index >= 0 && index < static_cast<int>(this->Studies.size()))
    return this->Studies[index];

  else
    return NULL;
}


//----------------------------------------------------------------------------
vtkMRMLLongPETCTFindingNode* vtkMRMLLongPETCTReportNode::GetFinding(int index) const
{
  if(index >= 0 && index < static_cast<int>(this->Findings.size()))
    return this->Findings[index];

  else
    return NULL;
}


//----------------------------------------------------------------------------
bool vtkMRMLLongPETCTReportNode::FindingNameInList(const std::string& name)
{
  for(int i=0; i < static_cast<int>(this->Findings.size()); ++i)
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

  for(unsigned int i=0; i < this->Studies.size();++i)
    {
      if(this->Studies.at(i)->GetSelectedForSegmentation())
        selectedStudies.push_back(this->Studies[i]);
    }

  return selectedStudies;
}

//----------------------------------------------------------------------------
std::vector<vtkMRMLLongPETCTStudyNode*> vtkMRMLLongPETCTReportNode::GetSelectedStudiesForAnalysis()
{
  std::vector<vtkMRMLLongPETCTStudyNode*> selectedStudiesAnalysis;

  for(unsigned int i=0; i < this->Studies.size();++i)
    {
      if(this->Studies.at(i)->GetSelectedForSegmentation() && this->Studies.at(i)->GetSelectedForAnalysis())
        selectedStudiesAnalysis.push_back(this->Studies[i]);
    }

  return selectedStudiesAnalysis;
}


//----------------------------------------------------------------------------
vtkMRMLLongPETCTStudyNode* vtkMRMLLongPETCTReportNode::GetSelectedStudy(int index)
{
  std::vector<vtkMRMLLongPETCTStudyNode*> selectedStudies = this->GetSelectedStudies();

  if(index >= 0 && index < static_cast<int>(selectedStudies.size()))
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

  for(unsigned int i=0; i < selectedStudies.size(); ++i)
    {
      if (selectedStudies[i] == study)
        return static_cast<int>(i);
    }

  return -1;
}

//----------------------------------------------------------------------------
int vtkMRMLLongPETCTReportNode::GetIndexOfSelectedForAnalysisStudy(const vtkMRMLLongPETCTStudyNode* study)
{
  if(study == NULL)
    return -1;

  std::vector<vtkMRMLLongPETCTStudyNode*> selectedStudiesAnalysis = this->GetSelectedStudiesForAnalysis();

  for(unsigned int i=0; i < selectedStudiesAnalysis.size(); ++i)
    {
      if (selectedStudiesAnalysis[i] == study)
        return static_cast<int>(i);
    }

  return -1;
}

//----------------------------------------------------------------------------
int vtkMRMLLongPETCTReportNode::GetIndexOfStudy(const vtkMRMLLongPETCTStudyNode* study)
{

  if(study == NULL)
    return -1;

  for(unsigned int i=0; i < this->Studies.size(); ++i)
    {
      if (this->Studies[i] == study)
        return static_cast<int>(i);
    }

  return -1;
}

//----------------------------------------------------------------------------
int vtkMRMLLongPETCTReportNode::GetIndexOfFinding(const vtkMRMLLongPETCTFindingNode* finding)
{

  if(finding == NULL)
    return -1;

  for(unsigned int i=0; i < this->Findings.size(); ++i)
    {
      if (this->Findings[i] == finding)
        return static_cast<int>(i);
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
  return this->FindingTypesColorTable->GetColorIndexByName(typeName.c_str());
}

//----------------------------------------------------------------------------
std::string vtkMRMLLongPETCTReportNode::GetFindingTypeName(int colorID)
{
  return this->FindingTypesColorTable->GetColorName(colorID);
}



//----------------------------------------------------------------------------
void vtkMRMLLongPETCTReportNode::AddFindingType(std::string name, double color[4])
{
  int size = this->FindingTypesColorTable->GetNumberOfColors();
  this->FindingTypesColorTable->SetNumberOfColors(size+1);

  this->FindingTypesColorTable->SetColor(size,name.c_str(),color[0],color[1],color[2],color[3]);

  this->InvokeEvent(vtkCommand::ModifiedEvent);
}

//----------------------------------------------------------------------------
void vtkMRMLLongPETCTReportNode::RemoveLastFindingType()
{

  if(this->IsFindingTypeInUse(this->FindingTypesColorTable->GetNumberOfColors()-1) || this->FindingTypesColorTable->GetNumberOfColors() == this->GetNumberOfDefaultFindingTypes())
    return;

  vtkNew<vtkMRMLColorTableNode> workingCopy;
  workingCopy->Copy(this->FindingTypesColorTable);

  workingCopy->SetNumberOfColors(this->FindingTypesColorTable->GetNumberOfColors()-1);


  for(int i=0; i < this->FindingTypesColorTable->GetNumberOfColors()-1; ++i)
    {
      const char* name = this->FindingTypesColorTable->GetColorName(i);

      double color[4];
      this->FindingTypesColorTable->GetColor(i,color);
      workingCopy->SetColor(i,name,color[0],color[1],color[2],color[3]);
    }

  this->FindingTypesColorTable->Copy(workingCopy.GetPointer());

  this->InvokeEvent(vtkCommand::ModifiedEvent);
}

//----------------------------------------------------------------------------
bool vtkMRMLLongPETCTReportNode::IsFindingTypeInUse(int colorID)
{
  for(unsigned int i=0; i < this->Findings.size(); ++i)
    {
      if(this->GetFinding(i)->GetColorID() == colorID)
        return true;
    }

  return false;
}

//----------------------------------------------------------------------------
int vtkMRMLLongPETCTReportNode::GetFindingTypesCount()
{
  return this->FindingTypesColorTable->GetNumberOfColors();
}

//----------------------------------------------------------------------------
int vtkMRMLLongPETCTReportNode::GetNumberOfDefaultFindingTypes()
{
  return this->NumberOfDefaultFindingTypes;
}


//----------------------------------------------------------------------------
void vtkMRMLLongPETCTReportNode::RemoveFinding(vtkMRMLLongPETCTFindingNode* finding)
{
  int index = this->GetIndexOfFinding(finding);

  if(index >= 0 && index < this->GetFindingsCount())
    {
      this->Findings.erase(this->Findings.begin()+index);
//        std::cout << "FINDING REF COUNT BEFORE DELETE" << finding->GetReferenceCount() << std::endl;
//
//      finding->Delete();
//
//      std::cout << "FINDING REF COUNT AFTER DELETE" << finding->GetReferenceCount() << std::endl;

      if(this->GetFindingsCount() > 0 && index > 0)
        this->SetUserSelectedFinding(this->GetFinding(index-1));
      else
        this->SetUserSelectedFinding(NULL);

      this->InvokeEvent(vtkCommand::ModifiedEvent);

    }
}

//----------------------------------------------------------------------------
bool vtkMRMLLongPETCTReportNode::IsStudyInUse(const vtkMRMLLongPETCTStudyNode* study)
{
  for(int i=0; i < this->Findings.size(); ++i)
    {
      if(this->GetFinding(i)->GetSegmentationForStudy(study) != NULL)
        return true;
    }

  return false;
}
