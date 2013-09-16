/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// LongitudinalPETCTFindingSelection Widgets includes
#include "qMRMLLongitudinalPETCTFindingWidget.h"
#include "ui_qMRMLLongitudinalPETCTFindingWidget.h"


#include <QMessageBox>


#include <qMRMLNodeComboBox.h>
#include <vtkMRMLLongitudinalPETCTFindingNode.h>

#include <vtkSmartPointer.h>
#include <vtkMRMLLongitudinalPETCTReportNode.h>


#include <QWidgetItem>


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongitudinalPETCT
class qMRMLLongitudinalPETCTFindingWidgetPrivate :
  public Ui_qMRMLLongitudinalPETCTFindingWidget
//  : public QWidget
{
  Q_DECLARE_PUBLIC(qMRMLLongitudinalPETCTFindingWidget);
protected:
  qMRMLLongitudinalPETCTFindingWidget* const q_ptr;

public:
  qMRMLLongitudinalPETCTFindingWidgetPrivate(
    qMRMLLongitudinalPETCTFindingWidget& object);

  virtual ~qMRMLLongitudinalPETCTFindingWidgetPrivate();

  virtual void setupUi(qMRMLLongitudinalPETCTFindingWidget* widget);


};

// --------------------------------------------------------------------------
qMRMLLongitudinalPETCTFindingWidgetPrivate
::qMRMLLongitudinalPETCTFindingWidgetPrivate(
  qMRMLLongitudinalPETCTFindingWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
qMRMLLongitudinalPETCTFindingWidgetPrivate
::~qMRMLLongitudinalPETCTFindingWidgetPrivate()
{
}

// --------------------------------------------------------------------------
void qMRMLLongitudinalPETCTFindingWidgetPrivate
::setupUi(qMRMLLongitudinalPETCTFindingWidget* widget)
{
  Q_Q(qMRMLLongitudinalPETCTFindingWidget);

  this->Ui_qMRMLLongitudinalPETCTFindingWidget::setupUi(widget);

  this->MRMLNodeComboBoxFinding->setNodeTypes(QStringList("vtkMRMLLongitudinalPETCTFindingNode"));
  this->MRMLNodeComboBoxFinding->setShowHidden(false);

  this->LabelInfo->setToolTip("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\"></style></head><body style=\"font-family:\'Lucida Grande\',sans-serif; font-size: 12pt; font-weight: 400; font-style: normal;border: 1px solid black;margin-top:0px;\"><table cellspacing=\"5\"><tbody><tr><td>-</td><td>Select/create a Finding</td></tr><tr><td>-</td><td>Place ROI in the slice views around a lesion</td></tr><tr><td>-</td><td>Open built in Editor module with <strong>Edit Segmentation</strong> to perform the segmentation of the lesion</td></tr><tr><td>-</td><td>Add segmentation to selected Finding in order to confirm the segmentation</td></tr></tbody></table></body></html>");

  this->ButtonAddSegmentationToFinding->setVisible(false);

  QObject::connect( this->MRMLNodeComboBoxFinding, SIGNAL(nodeAddedByUser(vtkMRMLNode*)), q, SLOT(findingNodeAdded(vtkMRMLNode*)) );
  QObject::connect( this->MRMLNodeComboBoxFinding, SIGNAL(currentNodeChanged (vtkMRMLNode*)), q, SLOT(findingNodeChanged(vtkMRMLNode*)) );
  QObject::connect( this->MRMLNodeComboBoxFinding, SIGNAL(nodeAboutToBeRemoved(vtkMRMLNode*)), q, SLOT(findingNodeToBeRemoved(vtkMRMLNode*)) );

  QObject::connect( this->CheckBoxROIVisiblity, SIGNAL(toggled(bool)), q, SIGNAL(roiVisibilityChanged(bool)) );
  QObject::connect( this->ButtonPlaceROI, SIGNAL(toggled(bool)), q, SIGNAL(placeROIChecked(bool)) );
  QObject::connect( this->ButtonAddSegmentationToFinding, SIGNAL(clicked()), q, SIGNAL(addSegmentationToFinding()) );
}
  //-----------------------------------------------------------------------------
// qMRMLLongitudinalPETCTFindingWidget methods

//-----------------------------------------------------------------------------
qMRMLLongitudinalPETCTFindingWidget
::qMRMLLongitudinalPETCTFindingWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qMRMLLongitudinalPETCTFindingWidgetPrivate(*this) )
{
  Q_D(qMRMLLongitudinalPETCTFindingWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qMRMLLongitudinalPETCTFindingWidget
::~qMRMLLongitudinalPETCTFindingWidget()
{
}

//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTFindingWidget::setMRMLScene(vtkMRMLScene *newScene)
{
  Superclass::setMRMLScene(newScene);

  Q_D(qMRMLLongitudinalPETCTFindingWidget);
  Q_ASSERT(d->MRMLNodeComboBoxFinding);

  d->MRMLNodeComboBoxFinding->setMRMLScene(newScene);
  d->MRMLNodeComboBoxFinding->setAddEnabled(true);
  d->MRMLNodeComboBoxFinding->setRemoveEnabled(true);
  d->MRMLNodeComboBoxFinding->setRenameEnabled(true);
}

//-----------------------------------------------------------------------------
qMRMLNodeComboBox* qMRMLLongitudinalPETCTFindingWidget::mrmlNodeComboBoxFinding()
{
  Q_D(qMRMLLongitudinalPETCTFindingWidget);

  return d->MRMLNodeComboBoxFinding;
}


//-----------------------------------------------------------------------------
void
qMRMLLongitudinalPETCTFindingWidget::updateWidgetFromMRML()
{
  Q_D(qMRMLLongitudinalPETCTFindingWidget);
  Q_ASSERT(d->MRMLNodeComboBoxFinding);
  Q_ASSERT(d->ButtonPlaceROI);
  Q_ASSERT(d->CheckBoxROIVisiblity);
  Q_ASSERT(d->GridLayout);


  vtkSmartPointer<vtkMRMLLongitudinalPETCTFindingNode> finding =
      vtkMRMLLongitudinalPETCTFindingNode::SafeDownCast(
          d->MRMLNodeComboBoxFinding->currentNode());

  if (!finding) // || (finding && finding->GetSegmentationROINode()))
    d->ButtonPlaceROI->setDisabled(true);

  else
    {
      d->ButtonPlaceROI->setEnabled(true);
    }
}


//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTFindingWidget
::setSelectionEnabled(bool enabled)
{
  Q_D(qMRMLLongitudinalPETCTFindingWidget);
  Q_ASSERT(d->LabelSelectFinding);
  Q_ASSERT(d->MRMLNodeComboBoxFinding);
  Q_ASSERT(d->LabelPlaceROI);

  d->LabelSelectFinding->setEnabled(enabled);
  d->MRMLNodeComboBoxFinding->setEnabled(enabled);
  d->LabelPlaceROI->setEnabled(enabled);
  d->ButtonPlaceROI->setEnabled(enabled);

}

//-----------------------------------------------------------------------------
bool qMRMLLongitudinalPETCTFindingWidget
::selectionEnabled()
{
  Q_D(qMRMLLongitudinalPETCTFindingWidget);
  Q_ASSERT(d->LabelSelectFinding);
  Q_ASSERT(d->MRMLNodeComboBoxFinding);
  Q_ASSERT(d->LabelPlaceROI);

  return d->LabelSelectFinding->isEnabled() && d->MRMLNodeComboBoxFinding->isEnabled() && d->LabelPlaceROI->isEnabled();
}

//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTFindingWidget
::setROIVisibility(bool visibility)
{
  Q_D(qMRMLLongitudinalPETCTFindingWidget);
  Q_ASSERT(d->CheckBoxROIVisiblity);

  d->CheckBoxROIVisiblity->setChecked(visibility);
}

//-----------------------------------------------------------------------------
bool qMRMLLongitudinalPETCTFindingWidget
::roiVisibility()
{
  Q_D(qMRMLLongitudinalPETCTFindingWidget);
  Q_ASSERT(d->CheckBoxROIVisiblity);

  return d->CheckBoxROIVisiblity->isChecked();
}

//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTFindingWidget
::hideAddButton(bool hide)
{
  Q_D(qMRMLLongitudinalPETCTFindingWidget);
  Q_ASSERT(d->ButtonAddSegmentationToFinding);

  d->ButtonAddSegmentationToFinding->setHidden(hide);
}

//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTFindingWidget
::setPlaceROIChecked(bool checked)
{
  Q_D(qMRMLLongitudinalPETCTFindingWidget);
  Q_ASSERT(d->ButtonPlaceROI);

  d->ButtonPlaceROI->setChecked(checked);
}

//-----------------------------------------------------------------------------
bool qMRMLLongitudinalPETCTFindingWidget
::placeROIChecked()
{
  Q_D(qMRMLLongitudinalPETCTFindingWidget);
  Q_ASSERT(d->ButtonPlaceROI);

  return d->ButtonPlaceROI->isChecked();
}


//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTFindingWidget
::setEditorWidget(QWidget* editorWidget)
{
  Q_D(qMRMLLongitudinalPETCTFindingWidget);
  Q_ASSERT(d->ButtonAddSegmentationToFinding);

  d->GridLayout->addWidget(editorWidget,3,0,1,3);

}


//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTFindingWidget::findingNodeAdded(vtkMRMLNode* node)
{
  vtkMRMLLongitudinalPETCTFindingNode* finding =
      vtkMRMLLongitudinalPETCTFindingNode::SafeDownCast(node);

  if(finding)
    emit findingNodeAddedByUser(finding);
}


//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTFindingWidget::findingNodeChanged(vtkMRMLNode* node)
{
  vtkMRMLLongitudinalPETCTFindingNode* finding =
      vtkMRMLLongitudinalPETCTFindingNode::SafeDownCast(node);

  if(finding)
    emit findingNodeSelectionChanged(finding);
}

//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTFindingWidget::findingNodeToBeRemoved(vtkMRMLNode* node)
{
  vtkMRMLLongitudinalPETCTFindingNode* finding =
      vtkMRMLLongitudinalPETCTFindingNode::SafeDownCast(node);

  if(finding)
    emit findingNodeAboutToBeRemoved(finding);
}


