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
#include "qSlicerLongitudinalPETCTFindingSelectionWidget.h"
#include "ui_qSlicerLongitudinalPETCTFindingSelectionWidget.h"


#include <QMessageBox>


#include <qMRMLNodeComboBox.h>
#include <vtkMRMLLongitudinalPETCTFindingNode.h>

#include <vtkSmartPointer.h>
#include <vtkMRMLLongitudinalPETCTReportNode.h>

#include "qSlicerLongitudinalPETCTFindingSettingsDialog.h"

#include <QWidgetItem>


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongitudinalPETCT
class qSlicerLongitudinalPETCTFindingSelectionWidgetPrivate :
  public Ui_qSlicerLongitudinalPETCTFindingSelectionWidget
//  : public QWidget
{
  Q_DECLARE_PUBLIC(qSlicerLongitudinalPETCTFindingSelectionWidget);
protected:
  qSlicerLongitudinalPETCTFindingSelectionWidget* const q_ptr;

public:
  qSlicerLongitudinalPETCTFindingSelectionWidgetPrivate(
    qSlicerLongitudinalPETCTFindingSelectionWidget& object);

  virtual ~qSlicerLongitudinalPETCTFindingSelectionWidgetPrivate();

  virtual void setupUi(qSlicerLongitudinalPETCTFindingSelectionWidget* widget);

  vtkSmartPointer<vtkMRMLLongitudinalPETCTReportNode> ReportNode;

};

// --------------------------------------------------------------------------
qSlicerLongitudinalPETCTFindingSelectionWidgetPrivate
::qSlicerLongitudinalPETCTFindingSelectionWidgetPrivate(
  qSlicerLongitudinalPETCTFindingSelectionWidget& object)
  : q_ptr(&object), ReportNode(NULL)
{
}

// --------------------------------------------------------------------------
qSlicerLongitudinalPETCTFindingSelectionWidgetPrivate
::~qSlicerLongitudinalPETCTFindingSelectionWidgetPrivate()
{
}

// --------------------------------------------------------------------------
void qSlicerLongitudinalPETCTFindingSelectionWidgetPrivate
::setupUi(qSlicerLongitudinalPETCTFindingSelectionWidget* widget)
{
  Q_Q(qSlicerLongitudinalPETCTFindingSelectionWidget);

  this->Ui_qSlicerLongitudinalPETCTFindingSelectionWidget::setupUi(widget);

  this->MRMLNodeComboBoxFinding->setNodeTypes(QStringList("vtkMRMLLongitudinalPETCTFindingNode"));

  QObject::connect( this->MRMLNodeComboBoxFinding, SIGNAL(nodeAddedByUser(vtkMRMLNode*)), q, SIGNAL(findingNodeAddedByUser(vtkMRMLNode*)) );
  QObject::connect( this->CheckBoxROIVisiblity, SIGNAL(toggled(bool)), q, SIGNAL(roiVisibilityChanged(bool)) );
  QObject::connect( this->ButtonPlaceROI, SIGNAL(toggled(bool)), q, SIGNAL(placeROIChecked(bool)) );
  QObject::connect( this->ButtonAddSegmentationToFinding, SIGNAL(clicked()), q, SIGNAL(addSegmentationToFinding()) );
}
  //-----------------------------------------------------------------------------
// qSlicerLongitudinalPETCTFindingSelectionWidget methods

//-----------------------------------------------------------------------------
qSlicerLongitudinalPETCTFindingSelectionWidget
::qSlicerLongitudinalPETCTFindingSelectionWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerLongitudinalPETCTFindingSelectionWidgetPrivate(*this) )
{
  Q_D(qSlicerLongitudinalPETCTFindingSelectionWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerLongitudinalPETCTFindingSelectionWidget
::~qSlicerLongitudinalPETCTFindingSelectionWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTFindingSelectionWidget::setMRMLScene(vtkMRMLScene* mrmlScene)
{
  Q_D(qSlicerLongitudinalPETCTFindingSelectionWidget);
  Q_ASSERT(d->MRMLNodeComboBoxFinding);

  d->MRMLNodeComboBoxFinding->setMRMLScene(mrmlScene);
  d->MRMLNodeComboBoxFinding->setAddEnabled(true);
  d->MRMLNodeComboBoxFinding->setRemoveEnabled(true);
  d->MRMLNodeComboBoxFinding->setRenameEnabled(false);
  d->MRMLNodeComboBoxFinding->setEditEnabled(true);
}


//-----------------------------------------------------------------------------
vtkMRMLScene* qSlicerLongitudinalPETCTFindingSelectionWidget::mrmlScene()
{
  Q_D(qSlicerLongitudinalPETCTFindingSelectionWidget);

  return d->MRMLNodeComboBoxFinding->mrmlScene();
}


//-----------------------------------------------------------------------------
qMRMLNodeComboBox* qSlicerLongitudinalPETCTFindingSelectionWidget::mrmlNodeComboBoxFinding()
{
  Q_D(qSlicerLongitudinalPETCTFindingSelectionWidget);

  return d->MRMLNodeComboBoxFinding;
}


//-----------------------------------------------------------------------------
void
qSlicerLongitudinalPETCTFindingSelectionWidget::updateView()
{
  Q_D(qSlicerLongitudinalPETCTFindingSelectionWidget);
  Q_ASSERT(d->MRMLNodeComboBoxFinding);
  Q_ASSERT(d->ButtonPlaceROI);
  Q_ASSERT(d->CheckBoxROIVisiblity);
  Q_ASSERT(d->FormLayout);

  vtkSmartPointer<vtkMRMLLongitudinalPETCTFindingNode> finding =
      vtkMRMLLongitudinalPETCTFindingNode::SafeDownCast(
          d->MRMLNodeComboBoxFinding->currentNode());

  if (!finding || (finding && finding->GetSegmentationROINode()))
    d->ButtonPlaceROI->setDisabled(true);

  else
    d->ButtonPlaceROI->setEnabled(true);

}


//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTFindingSelectionWidget
::setSelectionEnabled(bool enabled)
{
  Q_D(qSlicerLongitudinalPETCTFindingSelectionWidget);
  Q_ASSERT(d->LabelSelectFinding);
  Q_ASSERT(d->MRMLNodeComboBoxFinding);
  Q_ASSERT(d->LabelPlaceROI);

  d->LabelSelectFinding->setEnabled(enabled);
  d->MRMLNodeComboBoxFinding->setEnabled(enabled);
  d->LabelPlaceROI->setEnabled(enabled);

  if(d->ReportNode)
    {
      if(d->ReportNode->GetUserSelectedFindingNode())
          d->ButtonPlaceROI->setEnabled(enabled && !(d->ReportNode->GetUserSelectedFindingNode()->GetSegmentationROINode()));
    }
  }

//-----------------------------------------------------------------------------
bool qSlicerLongitudinalPETCTFindingSelectionWidget
::selectionEnabled()
{
  Q_D(qSlicerLongitudinalPETCTFindingSelectionWidget);
  Q_ASSERT(d->LabelSelectFinding);
  Q_ASSERT(d->MRMLNodeComboBoxFinding);
  Q_ASSERT(d->LabelPlaceROI);

  return d->LabelSelectFinding->isEnabled() && d->MRMLNodeComboBoxFinding->isEnabled() && d->LabelPlaceROI->isEnabled();
}

//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTFindingSelectionWidget
::setROIVisibility(bool visibility)
{
  Q_D(qSlicerLongitudinalPETCTFindingSelectionWidget);
  Q_ASSERT(d->CheckBoxROIVisiblity);

  d->CheckBoxROIVisiblity->setChecked(visibility);
}

//-----------------------------------------------------------------------------
bool qSlicerLongitudinalPETCTFindingSelectionWidget
::roiVisibility()
{
  Q_D(qSlicerLongitudinalPETCTFindingSelectionWidget);
  Q_ASSERT(d->CheckBoxROIVisiblity);

  return d->CheckBoxROIVisiblity->isChecked();
}

//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTFindingSelectionWidget
::hideAddButton(bool hide)
{
  Q_D(qSlicerLongitudinalPETCTFindingSelectionWidget);
  Q_ASSERT(d->ButtonAddSegmentationToFinding);

  d->ButtonAddSegmentationToFinding->setHidden(hide);
}

//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTFindingSelectionWidget
::setPlaceROIChecked(bool checked)
{
  Q_D(qSlicerLongitudinalPETCTFindingSelectionWidget);
  Q_ASSERT(d->ButtonPlaceROI);

  d->ButtonPlaceROI->setChecked(checked);
}

//-----------------------------------------------------------------------------
bool qSlicerLongitudinalPETCTFindingSelectionWidget
::placeROIChecked()
{
  Q_D(qSlicerLongitudinalPETCTFindingSelectionWidget);
  Q_ASSERT(d->ButtonPlaceROI);

  return d->ButtonPlaceROI->isChecked();
}

//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTFindingSelectionWidget::setReportNode(vtkMRMLLongitudinalPETCTReportNode* reportNode)
{
  Q_D(qSlicerLongitudinalPETCTFindingSelectionWidget);

  qvtkReconnect(d->ReportNode.GetPointer(), reportNode, vtkCommand::ModifiedEvent, this, SLOT(updateView()) );
  d->ReportNode = reportNode;

  this->updateView();
}

//-----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTReportNode* qSlicerLongitudinalPETCTFindingSelectionWidget::reportNode()
{
  Q_D(qSlicerLongitudinalPETCTFindingSelectionWidget);

  return d->ReportNode.GetPointer();
}


//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTFindingSelectionWidget
::setEditorWidget(QWidget* editorWidget)
{
  Q_D(qSlicerLongitudinalPETCTFindingSelectionWidget);
  Q_ASSERT(d->FormLayout);
  Q_ASSERT(d->ButtonAddSegmentationToFinding);

  d->ButtonAddSegmentationToFinding->setVisible(false);

  d->FormLayout->setWidget(2,QFormLayout::SpanningRole,editorWidget);
  d->FormLayout->setWidget(3,QFormLayout::SpanningRole,d->ButtonAddSegmentationToFinding);

  int minWidth = editorWidget->minimumWidth();
  this->setMinimumWidth(minWidth);
}
