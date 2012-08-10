/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLVolumeRenderingParametersNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/
// .NAME vtkMRMLLongPETCTReportNode
// This node keeps pointers to the two elements of the report: annotation
// and markup.
//
// Annotation element will (eventually) contain the report template and the
// user-initialized fields of the template.
//
// Markup element will point to the hierarchy of the markup elements.
//
// On export/import, this node will be used by the IO logic to determine how
// to code the formatted report.
// .SECTION Description
// This node keeps pointers to the two elements of the report: annotation
// and markup.
//

#ifndef __vtkMRMLLongPETCTReportNode_h
#define __vtkMRMLLongPETCTReportNode_h

// MRML includes
//#include "vtkDoubleArray.h"
#include "vtkMRML.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLScene.h"

#include <vtkSlicerLongPETCTModuleMRMLExport.h>


class vtkMRMLScalarVolumeNode;


/// \ingroup Slicer_QtModules_LongPETCTReportNode
class VTK_SLICER_LONGPETCT_MODULE_MRML_EXPORT vtkMRMLLongPETCTReportNode : public vtkMRMLNode
{
  public:   

  static vtkMRMLLongPETCTReportNode *New();
  vtkTypeMacro(vtkMRMLLongPETCTReportNode,vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual vtkMRMLNode* CreateNodeInstance();

  /// Set node attributes
  virtual void ReadXMLAttributes( const char** atts);

  /// Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  /// Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node);

  /// Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {return "Report";};


  struct Patient
  {
    std::string Name;
    std::string BirthDate;
    std::string Sex;
  };

  struct Study
  {
    std::string StudyInstanceUID;
    std::string StudyDate;
    vtkMRMLScalarVolumeNode* PETVolume;
    vtkMRMLScalarVolumeNode* CTVolume;
  };
  

  void SetPatientInformation(const std::string& name, const std::string& birthDate, const std::string& sex);

  const Patient* GetPatient();

  bool AddStudy(const std::string& uid, const std::string& date, vtkMRMLScalarVolumeNode* petVolume, vtkMRMLScalarVolumeNode* ctVolume);
  bool RemoveStudy(const std::string& uid);


  const Study* GetStudy(const std::string& uid);
  const Study* GetStudy(int index) const;


protected:
  vtkMRMLLongPETCTReportNode();
  ~vtkMRMLLongPETCTReportNode();
  vtkMRMLLongPETCTReportNode(const vtkMRMLLongPETCTReportNode&);
  void operator=(const vtkMRMLLongPETCTReportNode&);


  int GetIndexOfStudy(const std::string& uid);


  Patient PatientOfReport;
  std::vector<Study> Studies;




};

#endif

