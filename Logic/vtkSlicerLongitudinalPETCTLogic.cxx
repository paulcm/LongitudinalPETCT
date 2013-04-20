/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// LongitudinalPETCT Logic includes
#include "vtkSlicerLongitudinalPETCTLogic.h"



// MRML includes

// VTK includes
#include <vtkNew.h>

#include <vtkMRMLLongitudinalPETCTReportNode.h>
#include <vtkMRMLLongitudinalPETCTStudyNode.h>
#include <vtkMRMLLongitudinalPETCTFindingNode.h>
#include <vtkMRMLLongitudinalPETCTSegmentationNode.h>

// STD includes
#include <cassert>

// VTK includes
#include <vtkNew.h>
#include <vtkMatrix4x4.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkLookupTable.h>

// MRML includes
#include <vtkMRMLVolumeNode.h>
#include <vtkMRMLScalarVolumeNode.h>

#include <vtkMRMLLinearTransformNode.h>
#include <vtkMRMLColorTableNode.h>
#include <vtkMRMLColorLogic.h>




//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerLongitudinalPETCTLogic);

//----------------------------------------------------------------------------
vtkSlicerLongitudinalPETCTLogic::vtkSlicerLongitudinalPETCTLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerLongitudinalPETCTLogic::~vtkSlicerLongitudinalPETCTLogic()
{

}

//----------------------------------------------------------------------------
void vtkSlicerLongitudinalPETCTLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

bool vtkSlicerLongitudinalPETCTLogic::CenterStudyVolumeNodes(vtkMRMLLongitudinalPETCTStudyNode* study, vtkMRMLScene* scene)
{
  if (study == NULL)
    return false;

  vtkMRMLScalarVolumeNode* petVolume = study->GetPETVolumeNode();
  vtkMRMLScalarVolumeNode* ctVolume = study->GetCTVolumeNode();
  vtkMRMLScalarVolumeNode* petLabelVolume = study->GetPETLabelVolumeNode();

  if (petVolume == NULL || ctVolume == NULL || petLabelVolume == NULL || petVolume->GetImageData() == NULL
      || ctVolume->GetImageData() == NULL || petLabelVolume->GetImageData() == NULL)
    {
      //vtkDebugMacro(
          //"vtkSlicerLongitudinalPETCTLogic: PETVolume and/or CTVolume to be centered have NULL value or their image data is NULL");
      return false;
    }

  double petSpacing[3], ctSpacing[3];
  int petDimensions[3], ctDimensions[3];

  petVolume->GetSpacing(petSpacing);
  ctVolume->GetSpacing(ctSpacing);

  petVolume->GetImageData()->GetDimensions(petDimensions);
  ctVolume->GetImageData()->GetDimensions(ctDimensions);

  double petCapacity = petDimensions[0] * petSpacing[0] * petDimensions[1]
      * petSpacing[1] * petDimensions[2] * petSpacing[2];
  double ctCapacity = ctDimensions[0] * ctSpacing[0] * ctDimensions[1]
      * ctSpacing[1] * ctDimensions[2] * ctSpacing[2];

  vtkSmartPointer<vtkMRMLVolumeNode> referenceVolume;
  vtkSmartPointer<vtkMRMLVolumeNode> secondaryVolume;

  if (petCapacity >= ctCapacity)
    {
      referenceVolume = petVolume;
      secondaryVolume = ctVolume;
    }

  else
    {
      referenceVolume = ctVolume;
      secondaryVolume = petVolume;
    }


  vtkNew<vtkMatrix4x4> originalIJKToRAS;
  referenceVolume->GetIJKToRASMatrix(originalIJKToRAS.GetPointer());

  vtkNew<vtkMatrix4x4> referenceIJKToRAS;
  referenceIJKToRAS->DeepCopy(originalIJKToRAS.GetPointer());


  char* scanOrder =
      const_cast<char*>(vtkMRMLVolumeNode::ComputeScanOrderFromIJKToRAS(
          referenceIJKToRAS.GetPointer()));

  bool center =vtkMRMLVolumeNode::ComputeIJKToRASFromScanOrder(scanOrder,
      referenceVolume->GetSpacing(),
      referenceVolume->GetImageData()->GetDimensions(), true,
      referenceIJKToRAS.GetPointer());

  if (!center)
    {
      //vtkDebugMacro(
         // "vtkSlicerLongitudinalPETCTLogic: Centering of reference volume failed!");
      return false;
    }

  double translation[3];
  translation[0] = referenceIJKToRAS->GetElement(0, 3)
      - originalIJKToRAS->GetElement(0, 3);
  translation[1] = referenceIJKToRAS->GetElement(1, 3)
      - originalIJKToRAS->GetElement(1, 3);
  translation[2] = referenceIJKToRAS->GetElement(2, 3)
      - originalIJKToRAS->GetElement(2, 3);

  vtkNew<vtkMatrix4x4> translationMatrix;
  translationMatrix->Identity();

  for (int i = 0; i < 3; ++i)
    {
      translationMatrix->SetElement(i, 3, translation[i]);
    }


  vtkNew<vtkMRMLLinearTransformNode> translationTransform;
  translationTransform->ApplyTransformMatrix(
      translationMatrix.GetPointer());
  translationTransform->SetHideFromEditors(false);

  std::string pre = "CenteringTransform_";
  std::string tag = study->GetAttribute("DICOM.StudyDate");
  std::string name(pre+tag);
  translationTransform->SetName(name.c_str());

  scene->AddNode(translationTransform.GetPointer());

  /*petVolume->SetAndObserveTransformNodeID(
      translationTransform->GetID());
  ctVolume->SetAndObserveTransformNodeID(
      translationTransform->GetID());
  petLabelVolume->SetAndObserveTransformNodeID(translationTransform->GetID());
  */

  study->SetAndObserveCenteringTransformNodeID(translationTransform->GetID());


  return true;
}


//---------------------------------------------------------------------------
void vtkSlicerLongitudinalPETCTLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerLongitudinalPETCTLogic::RegisterNodes()
{

  if(!this->GetMRMLScene())
    return;

  vtkNew<vtkMRMLLongitudinalPETCTReportNode> reportNode;
  this->GetMRMLScene()->RegisterNodeClass(reportNode.GetPointer());

  vtkNew<vtkMRMLLongitudinalPETCTStudyNode> studyNode;
  this->GetMRMLScene()->RegisterNodeClass(studyNode.GetPointer());

  vtkNew<vtkMRMLLongitudinalPETCTFindingNode> findingNode;
  this->GetMRMLScene()->RegisterNodeClass(findingNode.GetPointer());

  vtkNew<vtkMRMLLongitudinalPETCTSegmentationNode> segmentationNode;
  this->GetMRMLScene()->RegisterNodeClass(segmentationNode.GetPointer());

}

//---------------------------------------------------------------------------
void vtkSlicerLongitudinalPETCTLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerLongitudinalPETCTLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerLongitudinalPETCTLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}




//---------------------------------------------------------------------------
vtkMRMLColorTableNode* vtkSlicerLongitudinalPETCTLogic::GetDefaultFindingTypesColorTable(vtkMRMLColorNode* defaultEditorColorNode)
{
  if(this->GetMRMLScene() == NULL)
    return NULL;

  //vtkSmartPointer<vtkMRMLColorTableNode> colorTable = vtkMRMLColorLogic::CopyNode(defaultEditorColorNode,"LongitudinalitudinalPETCT_ColorTable");
  vtkNew<vtkMRMLColorTableNode> colorTable;
  //colorTable->Copy(defaultEditorColorNode);

  colorTable->SetType(vtkMRMLColorTableNode::User);
  colorTable->SetSaveWithScene(true);
  colorTable->SetNumberOfColors(6);
  colorTable->GetLookupTable()->SetTableRange(0.,6.);

  double color[4];

  defaultEditorColorNode->GetColor(0,color);
  colorTable->SetColor(0,"None",color[0],color[1],color[2],color[3]);

  defaultEditorColorNode->GetColor(7,color);
  colorTable->SetColor(1,"Tumor",color[0],color[1],color[2],color[3]);

  defaultEditorColorNode->GetColor(23,color);
  colorTable->SetColor(2,"Lymph Node",color[0],color[1],color[2],color[3]);

  defaultEditorColorNode->GetColor(216,color);
  colorTable->SetColor(3,"Liver",color[0],color[1],color[2],color[3]);

  defaultEditorColorNode->GetColor(105,color);
  colorTable->SetColor(4,"Cerebellum",color[0],color[1],color[2],color[3]);

  defaultEditorColorNode->GetColor(191,color);
  colorTable->SetColor(5,"Aorta",color[0],color[1],color[2],color[3]);

  colorTable->SetName("LongitudinalitudinalPETCT_ColorTable");
  colorTable->SetScene(this->GetMRMLScene());
  colorTable->SetHideFromEditors(false);
  this->GetMRMLScene()->AddNode(colorTable.GetPointer());

  return colorTable.GetPointer();
}



