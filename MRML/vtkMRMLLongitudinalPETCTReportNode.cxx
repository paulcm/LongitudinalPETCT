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
#include "vtkMRMLLongitudinalPETCTReportNode.h"
#include "vtkMRMLLongitudinalPETCTStudyNode.h"

#include "vtkMRMLLongitudinalPETCTFindingNode.h"
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
vtkMRMLNodeNewMacro(vtkMRMLLongitudinalPETCTReportNode);

//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTReportNode::vtkMRMLLongitudinalPETCTReportNode()
{
  this->Initialize();
}

//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTReportNode::~vtkMRMLLongitudinalPETCTReportNode()
{
  this->ModelHierarchyNode = NULL;
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTReportNode::Initialize()
{

    int disabledModify = this->StartModify();
    this->SetHideFromEditors(false);

    this->UserSelectedStudy = NULL;
    this->UserSelectedFinding = NULL;
    this->FindingTypesColorTable = NULL;

    this->ModelHierarchyNode = vtkSmartPointer<vtkMRMLModelHierarchyNode>::New();
    this->ModelHierarchyNode->SetName("Longitudinalitudinal PET/CT Analysis Models");

    this->studyModifiedForwarder = vtkSmartPointer<vtkEventForwarderCommand>::New();
    studyModifiedForwarder->SetTarget(this);
    this->findingModifiedForwarder = vtkSmartPointer<vtkEventForwarderCommand>::New();
    findingModifiedForwarder->SetTarget(this);

    // TODO dynamically
    this->NumberOfDefaultFindingTypes = 6;

    this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTReportNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  Superclass::ReadXMLAttributes(atts);

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTReportNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);
}

//----------------------------------------------------------------------------
// Copy the node\"s attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, SliceID
void vtkMRMLLongitudinalPETCTReportNode::Copy(vtkMRMLNode *anode)
{
  int disabledModify = this->StartModify();
  
  this->SetAttribute("DICOM.PatientName",anode->GetAttribute("DICOM.PatientName"));
  this->SetAttribute("DICOM.PatientBirthDate",anode->GetAttribute("DICOM.PatientBirthDate"));
  this->SetAttribute("DICOM.PatientSex",anode->GetAttribute("DICOM.PatientSex"));

  Superclass::Copy(anode);

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTReportNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
int vtkMRMLLongitudinalPETCTReportNode::GetStudiesCount() const
{
  return this->Studies.size();
}

//----------------------------------------------------------------------------
int vtkMRMLLongitudinalPETCTReportNode::GetFindingsCount() const
{
  return this->Findings.size();
}

//----------------------------------------------------------------------------
int vtkMRMLLongitudinalPETCTReportNode::GetSelectedStudiesCount() const
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
int vtkMRMLLongitudinalPETCTReportNode::GetStudiesSelectedForAnalysisCount() const
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
int vtkMRMLLongitudinalPETCTReportNode::AddStudy(vtkMRMLLongitudinalPETCTStudyNode* study)
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
void vtkMRMLLongitudinalPETCTReportNode::AddFinding(vtkMRMLLongitudinalPETCTFindingNode* finding)
{
  if(findingModifiedForwarder != NULL)
    finding->AddObserver(vtkCommand::ModifiedEvent,this->findingModifiedForwarder);

  this->Findings.push_back(finding);

  if(this->ModelHierarchyNode && finding->GetModelHierarchyNode())
    finding->GetModelHierarchyNode()->SetParentNodeID(this->ModelHierarchyNode->GetID());


  this->InvokeEvent(vtkCommand::ModifiedEvent);
}


//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTStudyNode* vtkMRMLLongitudinalPETCTReportNode::GetStudy(int index) const
{
  if(index >= 0 && index < static_cast<int>(this->Studies.size()))
    return this->Studies[index];

  else
    return NULL;
}


//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTFindingNode* vtkMRMLLongitudinalPETCTReportNode::GetFinding(int index) const
{
  if(index >= 0 && index < static_cast<int>(this->Findings.size()))
    return this->Findings[index];

  else
    return NULL;
}


//----------------------------------------------------------------------------
bool vtkMRMLLongitudinalPETCTReportNode::FindingNameInList(const std::string& name)
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
std::vector<vtkMRMLLongitudinalPETCTStudyNode*> vtkMRMLLongitudinalPETCTReportNode::GetSelectedStudies()
{
  std::vector<vtkMRMLLongitudinalPETCTStudyNode*> selectedStudies;

  for(unsigned int i=0; i < this->Studies.size();++i)
    {
      if(this->Studies.at(i)->GetSelectedForSegmentation())
        selectedStudies.push_back(this->Studies[i]);
    }

  return selectedStudies;
}

//----------------------------------------------------------------------------
std::vector<vtkMRMLLongitudinalPETCTStudyNode*> vtkMRMLLongitudinalPETCTReportNode::GetStudiesSelectedForAnalysis()
{
  std::vector<vtkMRMLLongitudinalPETCTStudyNode*> studiesSelectedForAnalysis;

  for(unsigned int i=0; i < this->Studies.size();++i)
    {
      if(this->Studies.at(i)->GetSelectedForSegmentation() && this->Studies.at(i)->GetSelectedForAnalysis())
        studiesSelectedForAnalysis.push_back(this->Studies[i]);
    }

  return studiesSelectedForAnalysis;
}


//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTStudyNode* vtkMRMLLongitudinalPETCTReportNode::GetSelectedStudy(int index)
{
  std::vector<vtkMRMLLongitudinalPETCTStudyNode*> selectedStudies = this->GetSelectedStudies();

  if(index >= 0 && index < static_cast<int>(selectedStudies.size()))
    return selectedStudies[index];


  return NULL;
}

//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTStudyNode* vtkMRMLLongitudinalPETCTReportNode::GetStudySelectedForAnalysis(int index)
{
  std::vector<vtkMRMLLongitudinalPETCTStudyNode*> studiesSelectedForAnalysis = this->GetStudiesSelectedForAnalysis();

  if(index >= 0 && index < static_cast<int>(studiesSelectedForAnalysis.size()))
    return studiesSelectedForAnalysis[index];


  return NULL;
}


//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTStudyNode* vtkMRMLLongitudinalPETCTReportNode::GetSelectedStudyFirst()
{
  if(this->GetSelectedStudiesCount() > 0)
    return this->GetSelectedStudy(0);

  return NULL;
}

//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTStudyNode* vtkMRMLLongitudinalPETCTReportNode::GetSelectedStudyLast()
{
  int selectedStudies = this->GetSelectedStudiesCount();

  if(selectedStudies > 0)
    return this->GetSelectedStudy(selectedStudies-1);

  return NULL;
}


//----------------------------------------------------------------------------
int vtkMRMLLongitudinalPETCTReportNode::GetIndexOfSelectedStudy(const vtkMRMLLongitudinalPETCTStudyNode* study)
{

  if(study == NULL)
    return -1;

  std::vector<vtkMRMLLongitudinalPETCTStudyNode*> selectedStudies = this->GetSelectedStudies();

  for(unsigned int i=0; i < selectedStudies.size(); ++i)
    {
      if (selectedStudies[i] == study)
        return static_cast<int>(i);
    }

  return -1;
}

//----------------------------------------------------------------------------
int vtkMRMLLongitudinalPETCTReportNode::GetIndexOfStudySelectedForAnalysis(const vtkMRMLLongitudinalPETCTStudyNode* study)
{
  if(study == NULL)
    return -1;

  std::vector<vtkMRMLLongitudinalPETCTStudyNode*> studiesSelectedForAnalysis = this->GetStudiesSelectedForAnalysis();

  for(unsigned int i=0; i < studiesSelectedForAnalysis.size(); ++i)
    {
      if (studiesSelectedForAnalysis[i] == study)
        return static_cast<int>(i);
    }

  return -1;
}

//----------------------------------------------------------------------------
int vtkMRMLLongitudinalPETCTReportNode::GetIndexOfStudy(const vtkMRMLLongitudinalPETCTStudyNode* study)
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
int vtkMRMLLongitudinalPETCTReportNode::GetIndexOfFinding(const vtkMRMLLongitudinalPETCTFindingNode* finding)
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
const vtkMRMLColorNode* vtkMRMLLongitudinalPETCTReportNode::GetColorNode()
{
  if(this->GetScene() == NULL)
    return NULL;


  return vtkMRMLColorNode::SafeDownCast(this->GetScene()->GetNodeByID(this->GetAttribute("ColorNodeID")));
}

//----------------------------------------------------------------------------
int vtkMRMLLongitudinalPETCTReportNode::GetFindingTypeColorID(const std::string& typeName)
{
  return this->FindingTypesColorTable->GetColorIndexByName(typeName.c_str());
}

//----------------------------------------------------------------------------
const char* vtkMRMLLongitudinalPETCTReportNode::GetFindingTypeName(int colorID)
{
  return this->FindingTypesColorTable->GetColorName(colorID);
}



//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTReportNode::AddFindingType(std::string name, double color[4])
{
  int size = this->FindingTypesColorTable->GetNumberOfColors();
  this->FindingTypesColorTable->SetNumberOfColors(size+1);

  this->FindingTypesColorTable->SetColor(size,name.c_str(),color[0],color[1],color[2],color[3]);

  this->InvokeEvent(vtkCommand::ModifiedEvent);
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTReportNode::RemoveLastFindingType()
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
bool vtkMRMLLongitudinalPETCTReportNode::IsFindingTypeInUse(int colorID)
{
  for(unsigned int i=0; i < this->Findings.size(); ++i)
    {
      if(this->GetFinding(i)->GetColorID() == colorID)
        return true;
    }

  return false;
}

//----------------------------------------------------------------------------
int vtkMRMLLongitudinalPETCTReportNode::GetFindingTypesCount()
{
  return this->FindingTypesColorTable->GetNumberOfColors();
}

//----------------------------------------------------------------------------
int vtkMRMLLongitudinalPETCTReportNode::GetNumberOfDefaultFindingTypes()
{
  return this->NumberOfDefaultFindingTypes;
}


//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTReportNode::RemoveFinding(vtkMRMLLongitudinalPETCTFindingNode* finding)
{
  int index = this->GetIndexOfFinding(finding);

  if(index >= 0 && index < this->GetFindingsCount())
    {
      this->Findings.erase(this->Findings.begin()+index);

      if(this->GetFindingsCount() > 0 && index > 0)
        this->SetUserSelectedFinding(this->GetFinding(index-1));
      else
        this->SetUserSelectedFinding(NULL);

      this->InvokeEvent(vtkCommand::ModifiedEvent);

    }
}

//----------------------------------------------------------------------------
bool vtkMRMLLongitudinalPETCTReportNode::IsStudyInUse(const vtkMRMLLongitudinalPETCTStudyNode* study)
{
  for(unsigned int i=0; i < this->Findings.size(); ++i)
    {
      if(this->GetFinding(i)->GetSegmentationForStudy(study) != NULL)
        return true;
    }

  return false;
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTReportNode::SetModelHierarchyNode(vtkMRMLModelHierarchyNode* modelHierarchy)
{
  if(this->ModelHierarchyNode.GetPointer() == modelHierarchy)
    return;

  vtkSetMRMLObjectMacro(this->ModelHierarchyNode, modelHierarchy);

  for(int i=0; i < this->GetFindingsCount(); ++i)
    {
      vtkSmartPointer<vtkMRMLModelHierarchyNode> tempMH = this->GetFinding(i)->GetModelHierarchyNode();

      if(tempMH)
        {
           if(this->ModelHierarchyNode)
             tempMH->SetParentNodeID(this->ModelHierarchyNode->GetID());
           else
             tempMH->SetParentNodeID(NULL);
        }
    }
}


//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTReportNode::SetScene(vtkMRMLScene* scene)
{
  if(scene && this->ModelHierarchyNode && ! scene->GetNodeByID(this->ModelHierarchyNode->GetID()))
    scene->AddNode(this->ModelHierarchyNode);

  Superclass::SetScene(scene);
}


