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

// LongPETCTAnalysisSettings Widgets includes
#include "qSlicerLongPETCTAnalysisSettingsWidget.h"
#include "ui_qSlicerLongPETCTAnalysisSettingsWidget.h"


#include <QList>
#include <QCheckBox>
#include <QDate>
#include <QTime>

#include <vtkSmartPointer.h>
#include <vtkMRMLLongPETCTReportNode.h>
#include <vtkMRMLLongPETCTStudyNode.h>
#include <vtkCommand.h>


#include <qMRMLNodeComboBox.h>


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongPETCT
class qSlicerLongPETCTAnalysisSettingsWidgetPrivate :
  public Ui_qSlicerLongPETCTAnalysisSettingsWidget
//  : public QWidget
{
  Q_DECLARE_PUBLIC(qSlicerLongPETCTAnalysisSettingsWidget);
protected:
  qSlicerLongPETCTAnalysisSettingsWidget* const q_ptr;

public:
  qSlicerLongPETCTAnalysisSettingsWidgetPrivate(
    qSlicerLongPETCTAnalysisSettingsWidget& object);

  virtual ~qSlicerLongPETCTAnalysisSettingsWidgetPrivate();
  virtual void setupUi(qSlicerLongPETCTAnalysisSettingsWidget* widget);

  vtkSmartPointer<vtkMRMLLongPETCTReportNode> ReportNode;


};

// --------------------------------------------------------------------------
qSlicerLongPETCTAnalysisSettingsWidgetPrivate
::qSlicerLongPETCTAnalysisSettingsWidgetPrivate(
  qSlicerLongPETCTAnalysisSettingsWidget& object)
  : q_ptr(&object), ReportNode(NULL)
{
}

// --------------------------------------------------------------------------
qSlicerLongPETCTAnalysisSettingsWidgetPrivate
::~qSlicerLongPETCTAnalysisSettingsWidgetPrivate()
{
}

// --------------------------------------------------------------------------
void qSlicerLongPETCTAnalysisSettingsWidgetPrivate
::setupUi(qSlicerLongPETCTAnalysisSettingsWidget* widget)
{
  Q_Q(qSlicerLongPETCTAnalysisSettingsWidget);

  this->Ui_qSlicerLongPETCTAnalysisSettingsWidget::setupUi(widget);

  this->WidgetButtonsPanel->setVisible(false);
  this->TableStudyAnalysisSelection->setTableMode(qSlicerLongPETCTStudySelectionTableWidget::AnalysisSelection);

  QObject::connect(this->ButtonQualitativeAnalysis, SIGNAL(toggled(bool)), q, SIGNAL(qualitativeAnalysisChecked(bool)) );
  QObject::connect(this->ButtonQuantitativeAnalysis, SIGNAL(toggled(bool)), q, SIGNAL(quantitativeAnalysisChecked(bool)) );
  QObject::connect(this->ButtonVolumeRendering, SIGNAL(toggled(bool)), q, SIGNAL(volumeRenderingToggled(bool)) );
  QObject::connect(this->ButtonSpinView, SIGNAL(toggled(bool)), q, SIGNAL(spinViewToggled(bool)) );

  QObject::connect(this->TableStudyAnalysisSelection, SIGNAL(studySelectionChanged(int, bool)), q, SLOT(studySelectedInTable(int, bool)) );
}




//-----------------------------------------------------------------------------
// qSlicerLongPETCTAnalysisSettingsWidget methods

//-----------------------------------------------------------------------------
qSlicerLongPETCTAnalysisSettingsWidget
::qSlicerLongPETCTAnalysisSettingsWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerLongPETCTAnalysisSettingsWidgetPrivate(*this) )
{
  Q_D(qSlicerLongPETCTAnalysisSettingsWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerLongPETCTAnalysisSettingsWidget
::~qSlicerLongPETCTAnalysisSettingsWidget()
{
}


//-----------------------------------------------------------------------------
void qSlicerLongPETCTAnalysisSettingsWidget::setReportNode(vtkMRMLLongPETCTReportNode* reportNode)
{
  Q_D(qSlicerLongPETCTAnalysisSettingsWidget);

  qvtkReconnect(d->ReportNode.GetPointer(), reportNode, vtkCommand::ModifiedEvent, this, SLOT(updateView()) );
  d->ReportNode = reportNode;

  this->updateView();

}


//-----------------------------------------------------------------------------
vtkMRMLLongPETCTReportNode* qSlicerLongPETCTAnalysisSettingsWidget::reportNode()
{
  Q_D(qSlicerLongPETCTAnalysisSettingsWidget);

  return d->ReportNode.GetPointer();
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTAnalysisSettingsWidget
::updateView()
{
  Q_D(qSlicerLongPETCTAnalysisSettingsWidget);
  Q_ASSERT(d->TableStudyAnalysisSelection);

  d->TableStudyAnalysisSelection->clearRows();

  if(d->ReportNode == NULL)
    return;


  bool lastSelected = d->ReportNode->GetStudiesSelectedForAnalysisCount() == 1;

  for(int i=0; i < d->ReportNode->GetSelectedStudiesCount(); ++i)
    {
      vtkSmartPointer<vtkMRMLLongPETCTStudyNode> study = d->ReportNode->GetSelectedStudy(i);

      bool disabled = study->GetSelectedForAnalysis() && lastSelected;
      d->TableStudyAnalysisSelection->addStudyToTable(study, disabled);
    }

}


//-----------------------------------------------------------------------------
bool qSlicerLongPETCTAnalysisSettingsWidget::qualitativeChecked()
{
  Q_D(qSlicerLongPETCTAnalysisSettingsWidget);
  Q_ASSERT(d->ButtonQualitativeAnalysis);

  return d->ButtonQualitativeAnalysis->isChecked();
}

//-----------------------------------------------------------------------------
bool qSlicerLongPETCTAnalysisSettingsWidget::quantitativeChecked()
{
  Q_D(qSlicerLongPETCTAnalysisSettingsWidget);
  Q_ASSERT(d->ButtonQuantitativeAnalysis);

  return d->ButtonQuantitativeAnalysis->isChecked();
}


//-----------------------------------------------------------------------------
bool qSlicerLongPETCTAnalysisSettingsWidget::volumeRendering()
{
  Q_D(qSlicerLongPETCTAnalysisSettingsWidget);
  Q_ASSERT(d->ButtonVolumeRendering);

  return d->ButtonVolumeRendering->isChecked();
}

//-----------------------------------------------------------------------------
bool qSlicerLongPETCTAnalysisSettingsWidget::spinView()
{
  Q_D(qSlicerLongPETCTAnalysisSettingsWidget);
  Q_ASSERT(d->ButtonSpinView);

  return d->ButtonSpinView->isChecked();
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTAnalysisSettingsWidget::setQualitativeChecked(bool checked)
{
  Q_D(qSlicerLongPETCTAnalysisSettingsWidget);
  Q_ASSERT(d->ButtonQualitativeAnalysis);

  d->ButtonQualitativeAnalysis->setChecked(checked);
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTAnalysisSettingsWidget::setQuantitativeChecked(bool checked)
{
  Q_D(qSlicerLongPETCTAnalysisSettingsWidget);
  Q_ASSERT(d->ButtonQuantitativeAnalysis);

  d->ButtonQuantitativeAnalysis->setChecked(checked);
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTAnalysisSettingsWidget::setVolumeRendering(bool checked)
{
  Q_D(qSlicerLongPETCTAnalysisSettingsWidget);
  Q_ASSERT(d->ButtonVolumeRendering);

  d->ButtonVolumeRendering->setChecked(checked);
}


//-----------------------------------------------------------------------------
void qSlicerLongPETCTAnalysisSettingsWidget::setSpinView(bool checked)
{
  Q_D(qSlicerLongPETCTAnalysisSettingsWidget);
  Q_ASSERT(d->ButtonSpinView);

  d->ButtonSpinView->setChecked(checked);
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTAnalysisSettingsWidget::studySelectedInTable(int index, bool selected)
{
  Q_D(qSlicerLongPETCTAnalysisSettingsWidget);

  if(d->ReportNode)
    {
      vtkSmartPointer<vtkMRMLLongPETCTStudyNode> study = d->ReportNode->GetSelectedStudy(index);

      if(study)
        study->SetSelectedForAnalysis(selected);
    }

    emit studySelectedForAnalysis(index, selected);
}

