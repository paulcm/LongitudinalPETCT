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

// LongPETCTStudySelection Widgets includes
#include "qSlicerLongPETCTStudySelectionWidget.h"
#include "ui_qSlicerLongPETCTStudySelectionWidget.h"

#include <QCheckBox>
#include <QDate>
#include <QTime>


#include <ctkMessageBox.h>



#include <vtkSmartPointer.h>
#include <vtkMRMLLongPETCTReportNode.h>
#include <vtkMRMLLongPETCTStudyNode.h>



//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongitudinalPETCT
class qSlicerLongPETCTStudySelectionWidgetPrivate
  : public Ui_qSlicerLongPETCTStudySelectionWidget
{
  Q_DECLARE_PUBLIC(qSlicerLongPETCTStudySelectionWidget);
protected:
  qSlicerLongPETCTStudySelectionWidget* const q_ptr;

public:
  qSlicerLongPETCTStudySelectionWidgetPrivate(
    qSlicerLongPETCTStudySelectionWidget& object);

  virtual ~qSlicerLongPETCTStudySelectionWidgetPrivate();
  virtual void setupUi(qSlicerLongPETCTStudySelectionWidget*);

  vtkSmartPointer<vtkMRMLLongPETCTReportNode> ReportNode;

};

// --------------------------------------------------------------------------
qSlicerLongPETCTStudySelectionWidgetPrivate
::qSlicerLongPETCTStudySelectionWidgetPrivate(
  qSlicerLongPETCTStudySelectionWidget& object)
  : q_ptr(&object), ReportNode(NULL)
{
}

// --------------------------------------------------------------------------
qSlicerLongPETCTStudySelectionWidgetPrivate
::~qSlicerLongPETCTStudySelectionWidgetPrivate()
{
}


// --------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidgetPrivate
::setupUi(qSlicerLongPETCTStudySelectionWidget* widget)
{
  Q_Q(qSlicerLongPETCTStudySelectionWidget);

  this->Ui_qSlicerLongPETCTStudySelectionWidget::setupUi(widget);

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

  this->TableStudySelection->setTableMode(qSlicerLongPETCTStudySelectionTableWidget::WorkflowSelection);

  QObject::connect(this->TableStudySelection, SIGNAL(studySelectionChanged(int, bool)), q, SLOT(studySelectedInTable(int, bool)) );

  QObject::connect(this->ButtonVolumeRendering, SIGNAL(toggled(bool)), q, SIGNAL(volumeRenderingToggled(bool)) );
  QObject::connect(this->ButtonSpinView, SIGNAL(toggled(bool)), q, SIGNAL(spinViewToggled(bool)) );

  QObject::connect(this->SpinBoxOpacityPow, SIGNAL(valueChanged(double)), this->SliderOpacityPow, SLOT(setValue(double)) );
  QObject::connect(this->SliderOpacityPow, SIGNAL(valueChanged(double)), this->SpinBoxOpacityPow, SLOT(setValue(double)) );
  QObject::connect(this->SpinBoxOpacityPow, SIGNAL(valueChanged(double)), q, SIGNAL(opacityPowChanged(double)) );

  QObject::connect(this->CheckBoxStudiesCentered, SIGNAL(toggled(bool)), q, SIGNAL(showStudiesCentered(bool)) );
}

//-----------------------------------------------------------------------------
// qSlicerLongPETCTStudySelectionWidget methods

//-----------------------------------------------------------------------------
qSlicerLongPETCTStudySelectionWidget
::qSlicerLongPETCTStudySelectionWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerLongPETCTStudySelectionWidgetPrivate(*this) )
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerLongPETCTStudySelectionWidget
::~qSlicerLongPETCTStudySelectionWidget()
{
}


//-----------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidget
::updateView()
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);

  std::cout << "Trying to remove rows" << std::endl;
  d->TableStudySelection->clearRows(); // clear table first

  if(d->ReportNode.GetPointer() == NULL)
    return;

  d->TableStudySelection->deselectTableAll();

  for(int i=0; i < d->ReportNode->GetStudiesCount(); ++i)
    {
      vtkSmartPointer<vtkMRMLLongPETCTStudyNode> study = d->ReportNode->GetStudy(i);
      bool disabled = d->ReportNode->IsStudyInUse(study);
      if(disabled)
        std::cout << "STUDY " << study->GetName() << " IS IN USE" << std::endl;
      d->TableStudySelection->addStudyToTable(study, disabled);
    }
}

//-----------------------------------------------------------------------------
bool qSlicerLongPETCTStudySelectionWidget::volumeRendering()
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  Q_ASSERT(d->ButtonVolumeRendering);

  return d->ButtonVolumeRendering->isChecked();
}

//-----------------------------------------------------------------------------
bool qSlicerLongPETCTStudySelectionWidget::spinView()
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  Q_ASSERT(d->ButtonSpinView);

  return d->ButtonSpinView->isChecked();
}

//-----------------------------------------------------------------------------
double qSlicerLongPETCTStudySelectionWidget::opacityPow()
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  Q_ASSERT(d->SpinBoxOpacityPow);

  return d->SpinBoxOpacityPow->value();
}


//-----------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidget::setVolumeRendering(bool checked)
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  Q_ASSERT(d->ButtonVolumeRendering);

  d->ButtonVolumeRendering->setChecked(checked);
}


//-----------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidget::setSpinView(bool checked)
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  Q_ASSERT(d->ButtonSpinView);

  d->ButtonSpinView->setChecked(checked);
}


//-----------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidget::setOpacityPow(double opacityPow)
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  Q_ASSERT(d->SpinBoxOpacityPow);

  if(opacityPow >= d->SpinBoxOpacityPow->minimum() && opacityPow <= d->SpinBoxOpacityPow->maximum())
    d->SpinBoxOpacityPow->setValue(opacityPow);
}


//-----------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidget::setReportNode(vtkMRMLLongPETCTReportNode* reportNode)
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);

  qvtkReconnect(d->ReportNode.GetPointer(), reportNode, vtkCommand::ModifiedEvent, this, SLOT(updateView()) );
  d->ReportNode = reportNode;

  this->updateView();
}

//-----------------------------------------------------------------------------
vtkMRMLLongPETCTReportNode* qSlicerLongPETCTStudySelectionWidget::reportNode()
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);

  return d->ReportNode.GetPointer();
}

//-----------------------------------------------------------------------------
bool qSlicerLongPETCTStudySelectionWidget::centeredSelected()
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  Q_ASSERT(d->CheckBoxStudiesCentered);

  return d->CheckBoxStudiesCentered->isChecked();
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidget::setCenteredSelected(bool selected)
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  Q_ASSERT(d->CheckBoxStudiesCentered);

  return d->CheckBoxStudiesCentered->setChecked(selected);
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidget::selectStudyInRow(int row)
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  d->TableStudySelection->selectTableRow(row, true);
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidget::studySelectedInTable(int index, bool selected)
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);

  if(d->ReportNode)
    {
      vtkSmartPointer<vtkMRMLLongPETCTStudyNode> study = d->ReportNode->GetStudy(index);

      if(study)
        study->SetSelectedForSegmentation(selected);
    }

  if(selected)
    emit studySelected(index);
  else
    emit studyDeselected(index);

}

