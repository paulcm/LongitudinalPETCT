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

  this->ActiveStudyNodeID = NULL;
  this->ActiveFindingNodeID = NULL;
  this->RegistrationFixedStudyNodeID = NULL;

  this->ColorTableNodeID = NULL;

  this->ObservedEvents = vtkSmartPointer<vtkIntArray>::New();
  this->ObservedEvents->InsertNextValue(vtkCommand::ModifiedEvent);


  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTReportNode::~vtkMRMLLongitudinalPETCTReportNode()
{

  this->RemoveAllStudyNodeIDs();
  this->RemoveAllFindingNodeIDs();


  if(this->ObservedEvents)
    this->ObservedEvents = NULL;

  if(this->ActiveStudyNodeID)
    delete [] this->ActiveStudyNodeID;

  if(this->ActiveFindingNodeID)
    delete [] this->ActiveFindingNodeID;


  if(this->ColorTableNodeID)
    delete [] this->ColorTableNodeID;

  if(this->RegistrationFixedStudyNodeID)
    delete [] this->RegistrationFixedStudyNodeID;
}

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTReportNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  Superclass::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;

  while (*atts != NULL)
    {
      attName = *(atts++);
      attValue = *(atts++);

      if (!strcmp(attName, "ActiveStudyNodeID"))
      {
        this->SetActiveStudyNodeID(attValue);
      }
      else if (!strcmp(attName, "ActiveFindingNodeID"))
      {
        this->SetActiveFindingNodeID(attValue);
      }
      else if (!strcmp(attName, "ActiveFindingNodeID"))
      {
        this->SetActiveFindingNodeID(attValue);
      }
      else if (!strcmp(attName, "ColorTableNodeID"))
      {
        this->SetColorTableNodeID(attValue);
      }
      else if (!strcmp(attName, "RegistrationFixedStudyNodeID"))
      {
        this->SetRegistrationFixedStudyNodeID(attValue);
      }
      else if (!strcmp(attName, "StudyNodeIDs"))
        {
          std::stringstream ss(attValue);
          while (!ss.eof())
            {
              std::string id;
              ss >> id;
              this->AddStudyNodeID(id.c_str());
            }
        }
      else if (!strcmp(attName, "FindingNodeIDs"))
        {
          std::stringstream ss(attValue);
          while (!ss.eof())
            {
              std::string id;
              ss >> id;
              this->AddFindingNodeID(id.c_str());
            }
        }
    }

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTReportNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);

  vtkIndent indent(nIndent);

  if (this->ActiveStudyNodeID)
    of << indent << " ActiveStudyNodeID=\""
        << this->ActiveStudyNodeID << "\"";
  if (this->ActiveFindingNodeID)
    of << indent << " ActiveFindingNodeID=\""
        << this->ActiveFindingNodeID << "\"";
  if (this->ColorTableNodeID)
    of << indent << " ColorTableNodeID=\"" << this->ColorTableNodeID << "\"";
  if (this->RegistrationFixedStudyNodeID)
     of << indent << " RegistrationFixedStudyNodeID=\"" << this->RegistrationFixedStudyNodeID << "\"";

  std::stringstream stdyss;
  unsigned int n;
  for (n = 0; n < this->StudyNodeIDs.size(); n++)
    {
      stdyss << this->StudyNodeIDs[n];
      if (n < StudyNodeIDs.size() - 1)
        stdyss << " ";
    }
  if (this->StudyNodeIDs.size() > 0)
    {
      of << indent << " StudyNodeIDs=\"" << stdyss.str().c_str() << "\"";
    }

  std::stringstream fndss;

  for (n = 0; n < this->FindingNodeIDs.size(); n++)
    {
      fndss << this->FindingNodeIDs[n];
      if (n < FindingNodeIDs.size() - 1)
        fndss << " ";
    }
  if (this->FindingNodeIDs.size() > 0)
    {
      of << indent << " FindingNodeIDs=\"" << fndss.str().c_str() << "\"";
    }
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTReportNode::Copy(vtkMRMLNode *anode)
{
  int disabledModify = this->StartModify();
  
  Superclass::Copy(anode);

  vtkMRMLLongitudinalPETCTReportNode* node = vtkMRMLLongitudinalPETCTReportNode::SafeDownCast(anode);

  if (node)
    {
      this->SetActiveStudyNodeID(node->GetActiveStudyNodeID());
      this->SetActiveFindingNodeID(node->GetActiveFindingNodeID());
      this->SetColorTableNodeID(node->GetColorTableNodeID());
      this->SetRegistrationFixedStudyNodeID(node->GetRegistrationFixedStudyNodeID());

      this->RemoveAllStudyNodeIDs();

      for (int i = 0; i < node->GetNumberOfStudyNodeIDs(); ++i)
        {
          this->AddStudyNodeID(node->GetNthStudyNodeID(i));
        }

      this->UpdateStudyNodesObservation();


      this->RemoveAllFindingNodeIDs();

      for (int i = 0; i < node->GetNumberOfFindingNodeIDs(); ++i)
        {
          this->AddFindingNodeID(node->GetNthFindingNodeID(i));
        }

      this->UpdateFindingNodesObservation();

    }

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
bool
vtkMRMLLongitudinalPETCTReportNode::RemoveAllStudyNodeIDs()
{
  IDsVectorType::iterator it;
  while (this->GetNumberOfStudyNodeIDs() > 0)
    {
      it = this->StudyNodeIDs.begin();
      this->RemoveStudyNodeID((*it).c_str());
    }

  return this->StudyNodeIDs.empty();
}

//----------------------------------------------------------------------------
bool vtkMRMLLongitudinalPETCTReportNode::RemoveAllFindingNodeIDs()
{
  IDsVectorType::iterator it;
  while (this->GetNumberOfFindingNodeIDs() > 0)
    {
      it = this->FindingNodeIDs.begin();
      this->RemoveFindingNodeID((*it).c_str());
    }

  return this->FindingNodeIDs.empty();
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTReportNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);

  os << indent << "ActiveStudyNodeID: " << (this->ActiveStudyNodeID ? this->ActiveStudyNodeID : "(none)") << "\n";
  os << indent << "ActiveFindingNodeID: " << (this->ActiveFindingNodeID ? this->ActiveFindingNodeID : "(none)") << "\n";
  os << indent << "DefaultColorTableNodeID: " << (this->ColorTableNodeID ? this->ColorTableNodeID : "(none)") << "\n";

  os << indent << "RegistrationFixedStudyNodeID: " << (this->RegistrationFixedStudyNodeID ? this->RegistrationFixedStudyNodeID : "(none)") << "\n";


  IDsVectorType::iterator it;

  os << indent << "StudyNodeIDs: " << (this->StudyNodeIDs.empty() ? "(none)" : "") << "\n";
  for (it = this->StudyNodeIDs.begin(); it != StudyNodeIDs.end(); ++it)
    {
      if(! (*it).empty())
              os << indent << indent << (*it).c_str() << "\n";
    }
  os << indent << "FindingNodeIDs: " << (this->FindingNodeIDs.empty() ? "(none)" : "") << "\n";

  for (it = this->FindingNodeIDs.begin(); it != FindingNodeIDs.end(); ++it)
    {
      if(! (*it).empty())
        os << indent << indent << (*it).c_str() << "\n";
    }
}

//----------------------------------------------------------------------------
const char* vtkMRMLLongitudinalPETCTReportNode::GetNthStudyNodeID(int index)
{
  const char* nodeID = NULL;

  if(index >= 0 && index < this->GetNumberOfStudyNodeIDs())
    nodeID = this->StudyNodeIDs.at(index).c_str();


  return nodeID;
}

//----------------------------------------------------------------------------
const char* vtkMRMLLongitudinalPETCTReportNode::GetNthFindingNodeID(int index)
{
  const char* nodeID = NULL;

  if(index >= 0 && index < this->GetNumberOfFindingNodeIDs())
    nodeID = this->FindingNodeIDs.at(index).c_str();

  return nodeID;
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

bool vtkMRMLLongitudinalPETCTReportNode::IsStudyInReport(const char* dicomStudyInstanceUID)
{
  for(int i=0; i < this->GetNumberOfStudyNodeIDs(); ++i)
    {
      std::string uid = this->GetStudy(i)->GetAttribute("DICOM.StudyInstanceUID");

      if(uid.compare(dicomStudyInstanceUID) == 0)
        return true;
    }

  return false;
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
              vtkUnObserveMRMLObjectMacro(finding);
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
              vtkUnObserveMRMLObjectMacro(study);
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

  if (!findingNodeID || this->IsFindingNodeIDPresent(findingNodeID))
    return false;

  vtkMRMLLongitudinalPETCTFindingNode* finding = NULL;

  if (this->Scene)
    finding = vtkMRMLLongitudinalPETCTFindingNode::SafeDownCast(
        this->Scene->GetNodeByID(findingNodeID));

  this->FindingNodeIDs.push_back(findingNodeID);
  added = true;

  if (finding)
    vtkObserveMRMLObjectEventsMacro(finding, this->ObservedEvents.GetPointer());

  if (this->Scene)
    this->Scene->AddReferencedNodeID(findingNodeID, this);

  this->Modified();

  return added;
}

//----------------------------------------------------------------------------
int
vtkMRMLLongitudinalPETCTReportNode::AddStudyNodeID(const char* studyNodeID)
{

  if (!studyNodeID || this->IsStudyNodeIDPresent(studyNodeID))
    return -1;

  int pos = this->GetNumberOfStudyNodeIDs();

  vtkMRMLLongitudinalPETCTStudyNode* study = NULL;

  if (this->Scene)
    study = vtkMRMLLongitudinalPETCTStudyNode::SafeDownCast(
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

              if (!listStudyDate
                  || (listStudyDate && strcmp(studyDate, listStudyDate) < 0))
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
    }

  if(pos >= 0 && pos <= this->GetNumberOfStudyNodeIDs())
    {
      IDsVectorType::iterator it = StudyNodeIDs.begin() + pos;
      StudyNodeIDs.insert(it, studyNodeID);
    }

  if(study)
    vtkObserveMRMLObjectEventsMacro(study, this->ObservedEvents.GetPointer());

  if (this->Scene)
    this->Scene->AddReferencedNodeID(studyNodeID, this);

  this->Modified();

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
bool vtkMRMLLongitudinalPETCTReportNode::IsFindingNameInUse(const char* findingName)
{
  for(int i=0; i < this->GetNumberOfFindingNodeIDs(); ++i)
    {
      std::string name = this->GetFinding(i)->GetName();
      if(name.compare(findingName) == 0)
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
const vtkMRMLColorTableNode* vtkMRMLLongitudinalPETCTReportNode::GetColorTableNode()
{
  vtkMRMLColorTableNode* node = NULL;
  if(this->GetScene() && this->ColorTableNodeID)
    {
      node = vtkMRMLColorTableNode::SafeDownCast(this->Scene->GetNodeByID(this->ColorTableNodeID));
    }

  return node;}



//----------------------------------------------------------------------------
bool vtkMRMLLongitudinalPETCTReportNode::IsFindingColorInUse(int colorID)
{
  for(int i=0; i < this->GetNumberOfFindingNodeIDs(); ++i)
    {
      if(this->GetFinding(i)->GetColorID() == colorID)
        return true;
    }

  return false;
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
vtkMRMLLongitudinalPETCTStudyNode* vtkMRMLLongitudinalPETCTReportNode::GetActiveStudyNode()
{
    vtkMRMLLongitudinalPETCTStudyNode* node = NULL;
    if(this->GetScene() && this->ActiveStudyNodeID)
      {
        node = vtkMRMLLongitudinalPETCTStudyNode::SafeDownCast(this->Scene->GetNodeByID(this->ActiveStudyNodeID));
      }

    return node;
}

//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTFindingNode* vtkMRMLLongitudinalPETCTReportNode::GetActiveFindingNode()
{
    vtkMRMLLongitudinalPETCTFindingNode* node = NULL;
    if(this->GetScene() && this->ActiveFindingNodeID)
      {
        node = vtkMRMLLongitudinalPETCTFindingNode::SafeDownCast(this->Scene->GetNodeByID(this->ActiveFindingNodeID));
      }

    return node;
}

//----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTStudyNode* vtkMRMLLongitudinalPETCTReportNode::GetRegistrationFixedStudyNode()
{
    vtkMRMLLongitudinalPETCTStudyNode* node = NULL;
    if(this->GetScene() && this->RegistrationFixedStudyNodeID)
      {
        node = vtkMRMLLongitudinalPETCTStudyNode::SafeDownCast(this->Scene->GetNodeByID(this->RegistrationFixedStudyNodeID));
      }

    return node;
}

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTReportNode::UpdateStudyNodesObservation()
{
  if (!this->Scene)
    return;

  for (int i = 0; i < this->GetNumberOfStudyNodeIDs(); ++i)
    {
      vtkMRMLLongitudinalPETCTStudyNode* study = this->GetStudy(i);

      if (study)
        {
          // avoiding redundant observing
          vtkUnObserveMRMLObjectMacro(study);

          vtkObserveMRMLObjectEventsMacro(study,
              this->ObservedEvents.GetPointer());

          if (!this->Scene->IsNodeReferencingNodeID(this, study->GetID()))
            this->Scene->AddReferencedNodeID(study->GetID(), this);
        }
    }
}

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTReportNode::UpdateFindingNodesObservation()
{
  if (!this->Scene)
    return;

  for (int i = 0; i < this->GetNumberOfFindingNodeIDs(); ++i)
    {
      vtkMRMLLongitudinalPETCTFindingNode* finding = this->GetFinding(i);

      if (finding)
        {
          // avoiding redundant observing
          vtkUnObserveMRMLObjectMacro(finding);

          vtkObserveMRMLObjectEventsMacro(finding,
              this->ObservedEvents.GetPointer());

          if (!this->Scene->IsNodeReferencingNodeID(this, finding->GetID()))
            this->Scene->AddReferencedNodeID(finding->GetID(), this);
        }
    }
}

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTReportNode::UpdateReferences()
{
  this->Superclass::UpdateReferences();

  if (this->Scene)
    {

      IDsVectorType::iterator it;

      for (it = this->StudyNodeIDs.begin(); it != this->StudyNodeIDs.end();
          ++it)
        {
          if (this->Scene->GetNodeByID((*it).c_str()) == NULL)
            {
              this->RemoveStudyNodeID((*it).c_str());
              break;
            }
        }
      for (it = this->FindingNodeIDs.begin(); it != this->FindingNodeIDs.end();
          ++it)
        {
          if (this->Scene->GetNodeByID((*it).c_str()) == NULL)
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

  if(callerIsStudy || callerIsFinding)
    this->Modified();
}

//----------------------------------------------------------------------------
void vtkMRMLLongitudinalPETCTReportNode::SetScene(vtkMRMLScene* scene)
{
  bool update = this->Scene != scene;

  Superclass::SetScene(scene);

  if(update)
    this->UpdateScene(this->Scene);

}

//-----------------------------------------------------------
void
vtkMRMLLongitudinalPETCTReportNode::UpdateScene(vtkMRMLScene *scene)
{
  Superclass::UpdateScene(scene);

  if (this->Scene && this->Scene == scene)
    {
      this->UpdateStudyNodesObservation();
      this->UpdateFindingNodesObservation();
    }

}

//----------------------------------------------------------------------------
void
vtkMRMLLongitudinalPETCTReportNode::UpdateReferenceID(const char *oldID,
    const char *newID)
{
  this->Superclass::UpdateReferenceID(oldID, newID);

  if (this->ColorTableNodeID && !strcmp(oldID, this->ColorTableNodeID))
    this->SetColorTableNodeID(newID);

  else
    {
      bool changed = false;

      for (int i = 0; i < this->GetNumberOfStudyNodeIDs(); ++i)
        {
          if (!strcmp(oldID, StudyNodeIDs[i].c_str()))
            {
              StudyNodeIDs[i] = newID;
              changed = true;
              break;
            }
        }

      if (changed)
        this->UpdateStudyNodesObservation();

      else
        {
          for (int i = 0; i < this->GetNumberOfFindingNodeIDs(); ++i)
            {
              if (!strcmp(oldID, FindingNodeIDs[i].c_str()))
                {
                  FindingNodeIDs[i] = newID;
                  break;
                }
            }

          this->UpdateFindingNodesObservation();
        }
    }
}


