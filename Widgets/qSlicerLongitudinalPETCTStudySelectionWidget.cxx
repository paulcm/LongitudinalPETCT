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

// LongitudinalPETCTStudySelection Widgets includes
#include "qSlicerLongitudinalPETCTStudySelectionWidget.h"
#include "ui_qSlicerLongitudinalPETCTStudySelectionWidget.h"

#include <QCheckBox>
#include <QDate>
#include <QTime>


#include <ctkMessageBox.h>



#include <vtkSmartPointer.h>
#include <vtkMRMLLongitudinalPETCTReportNode.h>
#include <vtkMRMLLongitudinalPETCTStudyNode.h>



//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongitudinalitudinalPETCT
class qSlicerLongitudinalPETCTStudySelectionWidgetPrivate
  : public Ui_qSlicerLongitudinalPETCTStudySelectionWidget
{
  Q_DECLARE_PUBLIC(qSlicerLongitudinalPETCTStudySelectionWidget);
protected:
  qSlicerLongitudinalPETCTStudySelectionWidget* const q_ptr;

public:
  qSlicerLongitudinalPETCTStudySelectionWidgetPrivate(
    qSlicerLongitudinalPETCTStudySelectionWidget& object);

  virtual ~qSlicerLongitudinalPETCTStudySelectionWidgetPrivate();
  virtual void setupUi(qSlicerLongitudinalPETCTStudySelectionWidget*);

  vtkSmartPointer<vtkMRMLLongitudinalPETCTReportNode> ReportNode;

};

// --------------------------------------------------------------------------
qSlicerLongitudinalPETCTStudySelectionWidgetPrivate
::qSlicerLongitudinalPETCTStudySelectionWidgetPrivate(
  qSlicerLongitudinalPETCTStudySelectionWidget& object)
  : q_ptr(&object), ReportNode(NULL)
{
}

// --------------------------------------------------------------------------
qSlicerLongitudinalPETCTStudySelectionWidgetPrivate
::~qSlicerLongitudinalPETCTStudySelectionWidgetPrivate()
{
}


// --------------------------------------------------------------------------
void qSlicerLongitudinalPETCTStudySelectionWidgetPrivate
::setupUi(qSlicerLongitudinalPETCTStudySelectionWidget* widget)
{
  Q_Q(qSlicerLongitudinalPETCTStudySelectionWidget);

  this->Ui_qSlicerLongitudinalPETCTStudySelectionWidget::setupUi(widget);

  this->WidgetButtonsPanel->setVisible(false);

  //this->SliderOpacityPow->setOrientation(Qt::Vertical);
  double opcValue = this->SpinBoxOpacityPow->value();
  double min = this->SpinBoxOpacityPow->minimum();
  double max = this->SpinBoxOpacityPow->maximum();
  double step = this->SpinBoxOpacityPow->singleStep();

  this->SliderOpacityPow->setValue(opcValue); //this->SpinBoxOpacityPow->value());
  this->SliderOpacityPow->setMinimum(min); //this->SpinBoxOpacityPow->minimum());
  this->SliderOpacityPow->setMaximum(max); //this->SpinBoxOpacityPow->minimum());
  this->SliderOpacityPow->setSingleStep(step); //this->SpinBoxOpacityPow->singleStep());

  this->TableStudySelection->setTableMode(qSlicerLongitudinalPETCTStudySelectionTableWidget::WorkflowSelection);

  QObject::connect(this->TableStudySelection, SIGNAL(studySelectionChanged(int, bool)), q, SLOT(studySelectedInTable(int, bool)) );

  QObject::connect(this->ButtonVolumeRendering, SIGNAL(toggled(bool)), q, SIGNAL(volumeRenderingToggled(bool)) );
  QObject::connect(this->ButtonSpinView, SIGNAL(toggled(bool)), q, SIGNAL(spinViewToggled(bool)) );

  QObject::connect(this->SpinBoxOpacityPow, SIGNAL(valueChanged(double)), this->SliderOpacityPow, SLOT(setValue(double)) );
  QObject::connect(this->SliderOpacityPow, SIGNAL(valueChanged(double)), this->SpinBoxOpacityPow, SLOT(setValue(double)) );
  QObject::connect(this->SpinBoxOpacityPow, SIGNAL(valueChanged(double)), q, SIGNAL(opacityPowChanged(double)) );

  QObject::connect(this->CheckBoxStudiesCentered, SIGNAL(toggled(bool)), q, SIGNAL(showStudiesCentered(bool)) );
}

//-----------------------------------------------------------------------------
// qSlicerLongitudinalPETCTStudySelectionWidget methods

//-----------------------------------------------------------------------------
qSlicerLongitudinalPETCTStudySelectionWidget
::qSlicerLongitudinalPETCTStudySelectionWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerLongitudinalPETCTStudySelectionWidgetPrivate(*this) )
{
  Q_D(qSlicerLongitudinalPETCTStudySelectionWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerLongitudinalPETCTStudySelectionWidget
::~qSlicerLongitudinalPETCTStudySelectionWidget()
{
}


//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTStudySelectionWidget
::updateView()
{
  Q_D(qSlicerLongitudinalPETCTStudySelectionWidget);

  d->TableStudySelection->clearRows(); // clear table first

  if(!d->ReportNode)
    return;

  d->TableStudySelection->deselectTableAll();

  for(int i=0; i < d->ReportNode->GetNumberOfStudyNodeIDs(); ++i)
    {
      vtkSmartPointer<vtkMRMLLongitudinalPETCTStudyNode> study = d->ReportNode->GetStudy(i);
      bool disabled = d->ReportNode->IsStudyInUse(study);

      d->TableStudySelection->addStudyToTable(study, disabled);
    }

}

//-----------------------------------------------------------------------------
bool qSlicerLongitudinalPETCTStudySelectionWidget::volumeRendering()
{
  Q_D(qSlicerLongitudinalPETCTStudySelectionWidget);
  Q_ASSERT(d->ButtonVolumeRendering);

  return d->ButtonVolumeRendering->isChecked();
}

//-----------------------------------------------------------------------------
bool qSlicerLongitudinalPETCTStudySelectionWidget::spinView()
{
  Q_D(qSlicerLongitudinalPETCTStudySelectionWidget);
  Q_ASSERT(d->ButtonSpinView);

  return d->ButtonSpinView->isChecked();
}

//-----------------------------------------------------------------------------
double qSlicerLongitudinalPETCTStudySelectionWidget::opacityPow()
{
  Q_D(qSlicerLongitudinalPETCTStudySelectionWidget);
  Q_ASSERT(d->SpinBoxOpacityPow);

  return d->SpinBoxOpacityPow->value();
}


//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTStudySelectionWidget::setVolumeRendering(bool checked)
{
  Q_D(qSlicerLongitudinalPETCTStudySelectionWidget);
  Q_ASSERT(d->ButtonVolumeRendering);

  d->ButtonVolumeRendering->setChecked(checked);
}


//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTStudySelectionWidget::setSpinView(bool checked)
{
  Q_D(qSlicerLongitudinalPETCTStudySelectionWidget);
  Q_ASSERT(d->ButtonSpinView);

  d->ButtonSpinView->setChecked(checked);
}


//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTStudySelectionWidget::setOpacityPow(double opacityPow)
{
  Q_D(qSlicerLongitudinalPETCTStudySelectionWidget);
  Q_ASSERT(d->SpinBoxOpacityPow);

  if(opacityPow >= d->SpinBoxOpacityPow->minimum() && opacityPow <= d->SpinBoxOpacityPow->maximum())
    d->SpinBoxOpacityPow->setValue(opacityPow);
}


//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTStudySelectionWidget::setReportNode(vtkMRMLLongitudinalPETCTReportNode* reportNode)
{
  Q_D(qSlicerLongitudinalPETCTStudySelectionWidget);

  qvtkReconnect(d->ReportNode.GetPointer(), reportNode, vtkCommand::ModifiedEvent, this, SLOT(updateView()) );
  d->ReportNode = reportNode;

  this->updateView();
}

//-----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTReportNode* qSlicerLongitudinalPETCTStudySelectionWidget::reportNode()
{
  Q_D(qSlicerLongitudinalPETCTStudySelectionWidget);

  return d->ReportNode.GetPointer();
}

//-----------------------------------------------------------------------------
bool qSlicerLongitudinalPETCTStudySelectionWidget::centeredSelected()
{
  Q_D(qSlicerLongitudinalPETCTStudySelectionWidget);
  Q_ASSERT(d->CheckBoxStudiesCentered);

  return d->CheckBoxStudiesCentered->isChecked();
}

//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTStudySelectionWidget::setCenteredSelected(bool selected)
{
  Q_D(qSlicerLongitudinalPETCTStudySelectionWidget);
  Q_ASSERT(d->CheckBoxStudiesCentered);

  return d->CheckBoxStudiesCentered->setChecked(selected);
}

//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTStudySelectionWidget::selectStudyInRow(int row)
{
  Q_D(qSlicerLongitudinalPETCTStudySelectionWidget);
  d->TableStudySelection->selectTableRow(row, true);
}

//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTStudySelectionWidget::studySelectedInTable(int index, bool selected)
{
  Q_D(qSlicerLongitudinalPETCTStudySelectionWidget);

  if(d->ReportNode)
    {
      vtkSmartPointer<vtkMRMLLongitudinalPETCTStudyNode> study = d->ReportNode->GetStudy(index);

      if(study)
        study->SetSelectedForSegmentation(selected);
    }

  if(selected)
    emit studySelected(index);
  else
    emit studyDeselected(index);

}

