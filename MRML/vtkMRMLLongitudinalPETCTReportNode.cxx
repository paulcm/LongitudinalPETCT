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
#include <vtkMRMLColorNode.h>
#include <vtkMRMLColorTableNode.h>
#include <cassert>

#include <vector>


//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLLongitudinalPETCTReportNode);

//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTReportNode::vtkMRMLLongitudinalPETCTReportNode()
{
  int disabledModify = this->StartModify();
  this->SetHideFromEditors(false);

  this->UserSelectedStudyNodeID = NULL;
  this->UserSelectedFindingNodeID = NULL;

  this->FindingTypesColorTableNode = NULL;
  this->FindingTypesColorTableNodeID = NULL;

  this->ColorNodeID = NULL;
  this->ModelHierarchyNodeID = NULL;


  this->ObservedEvents = vtkSmartPointer<vtkIntArray>::New();
  this->ObservedEvents->InsertNextValue(vtkCommand::ModifiedEvent);


  // TODO dynamically
  this->NumberOfDefaultFindingTypes = 6;

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTReportNode::~vtkMRMLLongitudinalPETCTReportNode()
{
  /*
  this->FindingTypesColorTableNode = NULL;
  this->ObservedEvents = NULL;

  if(this->ModelHierarchyNodeID)
    delete [] this->ModelHierarchyNodeID;

  if(this->UserSelectedStudyNodeID)
    delete [] this->UserSelectedStudyNodeID;

  if(this->UserSelectedFindingNodeID)
    delete [] this->UserSelectedFindingNodeID;

  if(this->FindingTypesColorTableNodeID)
    delete [] this->FindingTypesColorTableNodeID;

  if(this->ColorNodeID)
    delete [] this->ColorNodeID;
  */
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
int vtkMRMLLongitudinalPETCTReportNode::GetNumberOfStudyNodeIDs() const
{
  return this->StudyNodeIDs.size();
}

//----------------------------------------------------------------------------
int vtkMRMLLongitudinalPETCTReportNode::GetNumberOfFindingNodeIDs() const
{
  return this->FindingNodeIDs.size();
}

//----------------------------------------------------------------------------
int vtkMRMLLongitudinalPETCTReportNode::GetNumberOfSelectedStudies() const
{
  int count = 0;

  for(unsigned int i=0; i < this->StudyNodeIDs.size(); ++i)
    {
      vtkMRMLLongitudinalPETCTStudyNode* study = this->GetStudy(i);
      if(study && study->GetSelectedForSegmentation())
        count++;
    }

  return count;
}

//----------------------------------------------------------------------------
int vtkMRMLLongitudinalPETCTReportNode::GetNumberOfSelectedStudiesSelectedForAnalysis() const
{
  int count = 0;

  for(unsigned int i=0; i < this->StudyNodeIDs.size(); ++i)
    {
      vtkMRMLLongitudinalPETCTStudyNode* study = this->GetStudy(i);
      if(study && study->GetSelectedForSegmentation() && study->GetSelectedForAnalysis())
        count++;
    }

  return count;
}

//----------------------------------------------------------------------------
bool vtkMRMLLongitudinalPETCTReportNode::IsStudyNodeIDPresent(const char* studyNodeID)
{

  for(int i=0; i < this->GetNumberOfStudyNodeIDs(); ++i)
    {
      if(this->StudyNodeIDs.at(i).compare(studyNodeID) == 0)
        return true;
    }

  return false;

}

//----------------------------------------------------------------------------
bool vtkMRMLLongitudinalPETCTReportNode::IsFindingNodeIDPresent(const char* findingNodeID)
{

  for(int i=0; i < this->GetNumberOfFindingNodeIDs(); ++i)
    {
      if(this->FindingNodeIDs.at(i).compare(findingNodeID) == 0)
        return true;
    }

  return false;

}


//----------------------------------------------------------------------------
bool vtkMRMLLongitudinalPETCTReportNode::RemoveFindingNodeID(const char* findingNodeID)
{
  bool removed = false;

  if (!findingNodeID)
    return false;

  int pos = 0;

  for(IDsVectorType::iterator it = this->FindingNodeIDs.begin(); it != this->FindingNodeIDs.end(); ++it)
    {
      if((*it).compare(findingNodeID) == 0)
        {
          vtkMRMLLongitudinalPETCTFindingNode* finding = this->GetFinding(pos);

          this->FindingNodeIDs.erase(it);

          if(finding)
            {
              vtkUnObserveMRMLObjectMacro(finding.GetPointer());
              if(this->Scene)
                this->Scene->RemoveReferencedNodeID(findingNodeID, this);
            }

          removed = true;
          this->Modified();

          break;
        }

      pos++;
    }

  return removed;

}


//----------------------------------------------------------------------------
bool
vtkMRMLLongitudinalPETCTReportNode::RemoveStudyNodeID(const char* studyNodeID)
{
  bool removed = false;

  if (!studyNodeID)
    return false;


  int pos = 0;

  for(IDsVectorType::iterator it = this->StudyNodeIDs.begin(); it != this->StudyNodeIDs.end(); ++it)
    {

      if((*it).compare(studyNodeID) == 0)
        {
          vtkMRMLLongitudinalPETCTStudyNode* study = this->GetStudy(pos);

          this->StudyNodeIDs.erase(it);

          if(study)
            {
              vtkUnObserveMRMLObjectMacro(study.GetPointer());
              if(this->Scene)
                this->Scene->RemoveReferencedNodeID(studyNodeID, this);
            }

          removed = true;
          this->Modified();

          break;
        }

      pos++;
    }

  return removed;
}

//----------------------------------------------------------------------------
bool
vtkMRMLLongitudinalPETCTReportNode::AddFindingNodeID(const char* findingNodeID)
{
  bool added = false;

  if (!this->Scene || !findingNodeID
      || this->IsFindingNodeIDPresent(findingNodeID))
    return false;

  vtkMRMLLongitudinalPETCTFindingNode* finding =
      vtkMRMLLongitudinalPETCTFindingNode::SafeDownCast(
          this->Scene->GetNodeByID(findingNodeID));

  if (finding)
    {
      this->FindingNodeIDs.push_back(findingNodeID);
      added = true;

      vtkObserveMRMLObjectEventsMacro(finding,
          this->ObservedEvents.GetPointer());

      if (this->Scene)
        this->Scene->AddReferencedNodeID(findingNodeID, this);

      if (finding->GetModelHierarchyNode()
          && this->Scene->GetNodeByID(this->ModelHierarchyNodeID))
        finding->GetModelHierarchyNode()->SetParentNodeID(
            this->ModelHierarchyNodeID);

      this->InvokeEvent(vtkCommand::ModifiedEvent);
    }

  return added;
}

//----------------------------------------------------------------------------
int
vtkMRMLLongitudinalPETCTReportNode::AddStudyNodeID(const char* studyNodeID)
{
  int pos = -1;

  if (!this->Scene || !studyNodeID || this->IsStudyNodeIDPresent(studyNodeID))
    return pos;

  vtkMRMLLongitudinalPETCTStudyNode* study =
      vtkMRMLLongitudinalPETCTStudyNode::SafeDownCast(
          this->Scene->GetNodeByID(studyNodeID));

  if (study)
    {
      const char* studyDate = study->GetAttribute("DICOM.StudyDate");
      const char* studyTime = study->GetAttribute("DICOM.StudyTime");

      pos = this->GetNumberOfStudyNodeIDs();

      if (studyDate)
        {
          int i = 0;

          for (; i < this->GetNumberOfStudyNodeIDs(); ++i)
            {
              const char* listStudyDate = this->GetStudy(i)->GetAttribute(
                  "DICOM.StudyDate");
              const char* listStudyTime = this->GetStudy(i)->GetAttribute(
                  "DICOM.StudyTime");

              if (!listStudyDate || (listStudyDate && strcmp(studyDate, listStudyDate) < 0))
                break;

              if (studyTime)
                {
                  if (!listStudyTime
                      || (listStudyDate && strcmp(studyDate, listStudyDate) == 0))
                    if (!listStudyTime
                        || (listStudyTime
                            && strcmp(studyTime, listStudyTime) < 0))
                      break;
                }
            }

          pos = i;
        }

      IDsVectorType::iterator it = StudyNodeIDs.begin() + pos;
      StudyNodeIDs.insert(it, studyNodeID);
      vtkObserveMRMLObjectEventsMacro(study,this->ObservedEvents.GetPointer());

      if (this->Scene)
        {
          this->Scene->AddReferencedNodeID(studyNodeID, this);
        }
    }

  return pos;
}




//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTStudyNode* vtkMRMLLongitudinalPETCTReportNode::GetStudy(int index) const
{
  vtkMRMLLongitudinalPETCTStudyNode* node = NULL;

  if(this->Scene && index >= 0 && index < this->GetNumberOfStudyNodeIDs())
    node = vtkMRMLLongitudinalPETCTStudyNode::SafeDownCast(this->Scene->GetNodeByID(this->StudyNodeIDs[index]));

  return node;
}


//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTFindingNode* vtkMRMLLongitudinalPETCTReportNode::GetFinding(int index) const
{
  vtkMRMLLongitudinalPETCTFindingNode* node = NULL;

    if(this->Scene && index >= 0 && index < this->GetNumberOfFindingNodeIDs())
      node = vtkMRMLLongitudinalPETCTFindingNode::SafeDownCast(this->Scene->GetNodeByID(this->FindingNodeIDs[index]));

    return node;

}


//----------------------------------------------------------------------------
bool vtkMRMLLongitudinalPETCTReportNode::FindingNameInList(const std::string& name)
{
  for(int i=0; i < this->GetNumberOfFindingNodeIDs(); ++i)
    {
      if(this->FindingNodeIDs.at(i).compare(name) == 0)
        return true;
    }

  return false;
}

//----------------------------------------------------------------------------
std::vector<vtkMRMLLongitudinalPETCTStudyNode*> vtkMRMLLongitudinalPETCTReportNode::GetSelectedStudies()
{
  std::vector<vtkMRMLLongitudinalPETCTStudyNode*> selectedStudies;

  for(int i=0; i < this->GetNumberOfStudyNodeIDs();++i)
    {
      vtkMRMLLongitudinalPETCTStudyNode* study = this->GetStudy(i);
      if(study && study->GetSelectedForSegmentation())
        selectedStudies.push_back(study);
    }

  return selectedStudies;
}

//----------------------------------------------------------------------------
std::vector<vtkMRMLLongitudinalPETCTStudyNode*> vtkMRMLLongitudinalPETCTReportNode::GetSelectedStudiesSelectedForAnalysis()
{
  std::vector<vtkMRMLLongitudinalPETCTStudyNode*> studiesSelectedForAnalysis;

  for(int i=0; i < this->GetNumberOfStudyNodeIDs();++i)
    {
      vtkMRMLLongitudinalPETCTStudyNode* study = this->GetStudy(i);
      if(study && study->GetSelectedForSegmentation() && study->GetSelectedForAnalysis())
        studiesSelectedForAnalysis.push_back(study);
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
vtkMRMLLongitudinalPETCTStudyNode* vtkMRMLLongitudinalPETCTReportNode::GetSelectedStudySelectedForAnalysis(int index)
{
  std::vector<vtkMRMLLongitudinalPETCTStudyNode*> studiesSelectedForAnalysis = this->GetSelectedStudiesSelectedForAnalysis();

  if(index >= 0 && index < static_cast<int>(studiesSelectedForAnalysis.size()))
    return studiesSelectedForAnalysis[index];


  return NULL;
}


//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTStudyNode* vtkMRMLLongitudinalPETCTReportNode::GetSelectedStudyFirst()
{
  if(this->GetNumberOfSelectedStudies() > 0)
    return this->GetSelectedStudy(0);

  return NULL;
}

//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTStudyNode* vtkMRMLLongitudinalPETCTReportNode::GetSelectedStudyLast()
{
  int selectedStudies = this->GetNumberOfSelectedStudies();

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
int vtkMRMLLongitudinalPETCTReportNode::GetIndexOfSelectedStudySelectedForAnalysis(const vtkMRMLLongitudinalPETCTStudyNode* study)
{
  if(study == NULL)
    return -1;

  std::vector<vtkMRMLLongitudinalPETCTStudyNode*> studiesSelectedForAnalysis = this->GetSelectedStudiesSelectedForAnalysis();

  for(unsigned int i=0; i < studiesSelectedForAnalysis.size(); ++i)
    {
      if (studiesSelectedForAnalysis[i] == study)
        return static_cast<int>(i);
    }

  return -1;
}

//----------------------------------------------------------------------------
int vtkMRMLLongitudinalPETCTReportNode::GetIndexOfStudyNodeID(const char* studyNodeID)
{
  int index = -1;

  if( ! studyNodeID)
    return index;


  for(int i=0; i < this->GetNumberOfStudyNodeIDs(); ++i)
    {
      if (this->StudyNodeIDs.at(i).compare(studyNodeID) == 0)
        {
          index = i;
          break;
        }
    }

  return index;
}

//----------------------------------------------------------------------------
int vtkMRMLLongitudinalPETCTReportNode::GetIndexOfFindingNodeID(const char* findingNodeID)
{
  int index = -1;

  if( ! findingNodeID)
    return index;


  for(int i=0; i < this->GetNumberOfFindingNodeIDs(); ++i)
     {
      if (this->FindingNodeIDs.at(i).compare(findingNodeID) == 0)
        {
          index = i;
          break;
        }
    }

  return index;
}

//----------------------------------------------------------------------------
const vtkMRMLColorNode* vtkMRMLLongitudinalPETCTReportNode::GetColorNode()
{
  vtkMRMLColorNode* node = NULL;
  if(this->GetScene() && this->ColorNodeID)
    {
      node = vtkMRMLColorNode::SafeDownCast(this->Scene->GetNodeByID(this->ColorNodeID));
    }

  return node;}

//----------------------------------------------------------------------------
int vtkMRMLLongitudinalPETCTReportNode::GetFindingTypeColorID(const std::string& typeName)
{
  int id = -1;

  if(this->FindingTypesColorTableNode)
    id = this->FindingTypesColorTableNode->GetColorIndexByName(typeName.c_str());

  return id;
}

//----------------------------------------------------------------------------
const char* vtkMRMLLongitudinalPETCTReportNode::GetFindingTypeName(int colorID)
{

  if(this->FindingTypesColorTableNode && colorID >= 0 && colorID < this->FindingTypesColorTableNode->GetNumberOfColors())
    return this->FindingTypesColorTableNode->GetColorName(colorID);

  return NULL;
}



//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTReportNode::AddFindingType(const char* name, double color[4])
{
  if( ! this->FindingTypesColorTableNode || ! name)
    return;

  int size = this->FindingTypesColorTableNode->GetNumberOfColors();
  this->FindingTypesColorTableNode->SetNumberOfColors(size+1);

  this->FindingTypesColorTableNode->SetColor(size,name,color[0],color[1],color[2],color[3]);
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTReportNode::RemoveLastFindingType()
{

  if( ! this->FindingTypesColorTableNode || this->FindingTypesColorTableNode->GetNumberOfColors() == 0)
    return;

  if(this->IsFindingTypeInUse(this->FindingTypesColorTableNode->GetNumberOfColors()-1))
    return;

  vtkNew<vtkMRMLColorTableNode> workingCopy;
  workingCopy->Copy(this->FindingTypesColorTableNode);

  workingCopy->SetNumberOfColors(this->FindingTypesColorTableNode->GetNumberOfColors()-1);

  for(int i=0; i < this->FindingTypesColorTableNode->GetNumberOfColors()-1; ++i)
    {
      const char* name = this->FindingTypesColorTableNode->GetColorName(i);

      double color[4];
      this->FindingTypesColorTableNode->GetColor(i,color);
      workingCopy->SetColor(i,name,color[0],color[1],color[2],color[3]);
    }

  this->FindingTypesColorTableNode->Copy(workingCopy.GetPointer());
}

//----------------------------------------------------------------------------
bool vtkMRMLLongitudinalPETCTReportNode::IsFindingTypeInUse(int colorID)
{
  for(int i=0; i < this->GetNumberOfFindingNodeIDs(); ++i)
    {
      if(this->GetFinding(i)->GetColorID() == colorID)
        return true;
    }

  return false;
}

//----------------------------------------------------------------------------
int vtkMRMLLongitudinalPETCTReportNode::GetFindingTypesCount()
{
  int nr = -1;

  if(this->FindingTypesColorTableNode)
    nr = this->FindingTypesColorTableNode->GetNumberOfColors();

  return nr;
}

//----------------------------------------------------------------------------
int vtkMRMLLongitudinalPETCTReportNode::GetNumberOfDefaultFindingTypes()
{
  return this->NumberOfDefaultFindingTypes;
}




//----------------------------------------------------------------------------
bool vtkMRMLLongitudinalPETCTReportNode::IsStudyInUse(const vtkMRMLLongitudinalPETCTStudyNode* study)
{

  for(int i=0; i < this->GetNumberOfFindingNodeIDs(); ++i)
    {
      if(study && this->GetFinding(i)->GetSegmentationMappedByStudyNodeID(const_cast<vtkMRMLLongitudinalPETCTStudyNode*>(study)->GetID()) != NULL)
        return true;
    }

  return false;
}

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTReportNode::SetReportsModelHierarchyNodeID(
    const char* modelHierarchyNodeID)
{

  if (this->Scene  && this->ModelHierarchyNodeID && this->Scene->IsNodeReferencingNodeID(this, this->ModelHierarchyNodeID))
    this->Scene->RemoveReferencedNodeID(this->ModelHierarchyNodeID, this);

  this->SetModelHierarchyNodeID(modelHierarchyNodeID);

  if (this->Scene)
    {
      // update all findings model hierarchy node's parent node to the set node
      for (int i = 0; i < this->GetNumberOfFindingNodeIDs(); ++i)
        {
          vtkMRMLModelHierarchyNode* tempMH = this->GetFinding(
              i)->GetModelHierarchyNode();

          if (tempMH)
            {
              if (this->ModelHierarchyNodeID
                  && this->Scene->GetNodeByID(this->ModelHierarchyNodeID))
                tempMH->SetParentNodeID(this->ModelHierarchyNodeID);
              else
                tempMH->SetParentNodeID(NULL);
            }
        }
      this->Scene->AddReferencedNodeID(this->ModelHierarchyNodeID, this);
    }
}

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTReportNode::SetAndObserveFindingTypesColorTableNodeID(
    const char* findingTypesColorTableNodeID)
{

  // first remove references and observers from old node
  if (this->FindingTypesColorTableNode)
    {
      vtkUnObserveMRMLObjectMacro(this->FindingTypesColorTableNode);

      if (this->Scene
          && this->Scene->IsNodeReferencingNodeID(this,
              this->FindingTypesColorTableNode->GetID()))
        this->Scene->RemoveReferencedNodeID(this->FindingTypesColorTableNode->GetID(),
            this);
    }

  // than set new node
  vtkMRMLColorTableNode* ctnode = NULL;

  if (this->GetScene() && findingTypesColorTableNodeID)
    {
      ctnode = vtkMRMLColorTableNode::SafeDownCast(
          this->GetScene()->GetNodeByID(findingTypesColorTableNodeID));
    }

  vtkSetAndObserveMRMLObjectMacro(this->FindingTypesColorTableNode, ctnode);
  this->SetFindingTypesColorTableNodeID(findingTypesColorTableNodeID);


  if (this->Scene)
    this->Scene->AddReferencedNodeID(this->FindingTypesColorTableNodeID, this);
}

//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTStudyNode* vtkMRMLLongitudinalPETCTReportNode::GetUserSelectedStudyNode()
{
    vtkMRMLLongitudinalPETCTStudyNode* node = NULL;
    if(this->GetScene() && this->UserSelectedStudyNodeID)
      {
        node = vtkMRMLLongitudinalPETCTStudyNode::SafeDownCast(this->Scene->GetNodeByID(this->UserSelectedStudyNodeID));
      }

    return node;
}

//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTFindingNode* vtkMRMLLongitudinalPETCTReportNode::GetUserSelectedFindingNode()
{
    vtkMRMLLongitudinalPETCTFindingNode* node = NULL;
    if(this->GetScene() && this->UserSelectedFindingNodeID)
      {
        node = vtkMRMLLongitudinalPETCTFindingNode::SafeDownCast(this->Scene->GetNodeByID(this->UserSelectedFindingNodeID));
      }

    return node;
}

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTReportNode::UpdateReferences()
{
  this->Superclass::UpdateReferences();

  if(this->Scene)
    {
      IDsVectorType::iterator it;

      for(it = this->StudyNodeIDs.begin(); it != this->StudyNodeIDs.end(); ++it)
        {
          if(this->Scene->GetNodeByID((*it).c_str()) == NULL)
            {
              this->RemoveStudyNodeID((*it).c_str());
              break;
            }
        }
      for(it = this->FindingNodeIDs.begin(); it != this->FindingNodeIDs.end(); ++it)
        {
          if(this->Scene->GetNodeByID((*it).c_str()) == NULL)
            {
              this->RemoveFindingNodeID((*it).c_str());
              break;
            }
        }
    }
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTReportNode::ProcessMRMLEvents(vtkObject *caller, unsigned long event, void *callData)
{
  Superclass::ProcessMRMLEvents(caller, event, callData);

  vtkMRMLNode* node = vtkMRMLNode::SafeDownCast(caller);

  bool callerIsStudy = node->IsA("vtkMRMLLongitudinalPETCTStudyNode");
  bool callerIsFinding = node->IsA("vtkMRMLLongitudinalPETCTFindingNode");
  bool callerIsFindingTypesTable = this->FindingTypesColorTableNode && node.GetPointer() == this->FindingTypesColorTableNode;

  if(callerIsStudy || callerIsFinding || callerIsFindingTypesTable)
    {
      this->Modified();
    }
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTReportNode::SetScene(vtkMRMLScene* scene)
{
  Superclass::SetScene(scene);


}


