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
bool vtkMRMLLongPETCTReportNode::AddStudy(vtkMRMLLongPETCTStudyNode* study)
{
  if(study)
    Studies.push_back(study);

  return true;
}
