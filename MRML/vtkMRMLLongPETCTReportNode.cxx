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

#include <vtkMRMLScalarVolumeNode.h>

// STD includes


//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLLongPETCTReportNode);

//----------------------------------------------------------------------------
vtkMRMLLongPETCTReportNode::vtkMRMLLongPETCTReportNode()
{

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
void vtkMRMLLongPETCTReportNode::SetPatientInformation(const std::string& name, const std::string& birthDate, const std::string& sex)
{
  PatientOfReport.Name = name;
  PatientOfReport.BirthDate = birthDate;
  PatientOfReport.Sex = sex;
}

//----------------------------------------------------------------------------
const vtkMRMLLongPETCTReportNode::Patient* vtkMRMLLongPETCTReportNode::GetPatient()
{
  return &PatientOfReport;
}

//----------------------------------------------------------------------------
bool vtkMRMLLongPETCTReportNode::AddStudy(const std::string& uid, const std::string& date, vtkMRMLScalarVolumeNode* petVolume, vtkMRMLScalarVolumeNode* ctVolume)
{
  int studiesCount = Studies.size();

  Study study;
  study.StudyInstanceUID = uid;
  study.StudyDate = date;

  if(petVolume)
    study.PETVolume = petVolume;
  else
    study.PETVolume = NULL;

  if(ctVolume)
    study.CTVolume = ctVolume;
  else
    study.CTVolume = NULL;

  Studies.push_back(study);

  if(Studies.size() == studiesCount+1 && this->GetIndexOfStudy(uid) >= 0)
    return true;
  else
    return false;
}


//----------------------------------------------------------------------------
bool vtkMRMLLongPETCTReportNode::RemoveStudy(const std::string& uid)
{
  int studiesCount = Studies.size();

  int index = this->GetIndexOfStudy(uid);

  if(index != -1)
    Studies.erase(Studies.begin() + index);

  if(Studies.size() == studiesCount-1 && this->GetIndexOfStudy(uid) == -1)
      return true;
    else
      return false;
}


//----------------------------------------------------------------------------
int vtkMRMLLongPETCTReportNode::GetIndexOfStudy(const std::string& uid)
{
  for(unsigned int i=0; i < Studies.size(); ++i)
      {
         if(Studies.at(i).StudyInstanceUID == uid)
             return i;
      }

  return -1;
}


//----------------------------------------------------------------------------
const vtkMRMLLongPETCTReportNode::Study* vtkMRMLLongPETCTReportNode::GetStudy(const std::string& uid)
{
  Study* study = NULL;

  int index = this->GetIndexOfStudy(uid);


  return this->GetStudy(index);
}

const vtkMRMLLongPETCTReportNode::Study* vtkMRMLLongPETCTReportNode::GetStudy(int index) const
{

  if(index >= 0 && index < Studies.size())
    return &(Studies.at(index));

  return NULL;
}


