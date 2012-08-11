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
#


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
  return Studies.size();
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
const vtkMRMLLongPETCTStudyNode* vtkMRMLLongPETCTReportNode::GetStudy(int index)
{
  if(index >= 0 && index < this->Studies.size())
    return Studies.at(index);

  else
    return NULL;
}


