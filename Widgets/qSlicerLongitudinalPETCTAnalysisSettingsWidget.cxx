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

// LongitudinalPETCTAnalysisSettings Widgets includes
#include "qSlicerLongitudinalPETCTAnalysisSettingsWidget.h"
#include "ui_qSlicerLongitudinalPETCTAnalysisSettingsWidget.h"


#include <QList>
#include <QCheckBox>
#include <QDate>
#include <QTime>

#include <vtkSmartPointer.h>
#include <vtkMRMLLongitudinalPETCTReportNode.h>
#include <vtkMRMLLongitudinalPETCTStudyNode.h>
#include <vtkCommand.h>

#include <qMRMLNodeComboBox.h>


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongitudinalPETCT
class qSlicerLongitudinalPETCTAnalysisSettingsWidgetPrivate :
  public Ui_qSlicerLongitudinalPETCTAnalysisSettingsWidget
//  : public QWidget
{
  Q_DECLARE_PUBLIC(qSlicerLongitudinalPETCTAnalysisSettingsWidget);
protected:
  qSlicerLongitudinalPETCTAnalysisSettingsWidget* const q_ptr;

public:
  qSlicerLongitudinalPETCTAnalysisSettingsWidgetPrivate(
    qSlicerLongitudinalPETCTAnalysisSettingsWidget& object);

  virtual ~qSlicerLongitudinalPETCTAnalysisSettingsWidgetPrivate();
  virtual void setupUi(qSlicerLongitudinalPETCTAnalysisSettingsWidget* widget);

  vtkSmartPointer<vtkMRMLLongitudinalPETCTReportNode> ReportNode;


};

// --------------------------------------------------------------------------
qSlicerLongitudinalPETCTAnalysisSettingsWidgetPrivate
::qSlicerLongitudinalPETCTAnalysisSettingsWidgetPrivate(
  qSlicerLongitudinalPETCTAnalysisSettingsWidget& object)
  : q_ptr(&object), ReportNode(NULL)
{
}

// --------------------------------------------------------------------------
qSlicerLongitudinalPETCTAnalysisSettingsWidgetPrivate
::~qSlicerLongitudinalPETCTAnalysisSettingsWidgetPrivate()
{
}

// --------------------------------------------------------------------------
void qSlicerLongitudinalPETCTAnalysisSettingsWidgetPrivate
::setupUi(qSlicerLongitudinalPETCTAnalysisSettingsWidget* widget)
{
  Q_Q(qSlicerLongitudinalPETCTAnalysisSettingsWidget);

  this->Ui_qSlicerLongitudinalPETCTAnalysisSettingsWidget::setupUi(widget);

  this->WidgetButtonsPanel->setVisible(false);
  this->TableStudyAnalysisSelection->setTableMode(qSlicerLongitudinalPETCTStudySelectionTableWidget::AnalysisSelection);

  this->LabelInfo->setToolTip("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\"></style></head><body style=\"font-family:\'Lucida Grande\',sans-serif; font-size: 12pt; font-weight: 400; font-style: normal;border: 1px solid black;margin-top:0px;\"><table cellspacing=\"5\"><tbody><tr><td>-</td><td>Switch between qualitative and quantiative display of Analysis results.</td></tr><tr><td>-</td><td>Remove/add Studies for comparison</td></tr><tr><td><img src=\":/Icons/help/expdbtn.png\" /></td><td>Open expandable advanced settings panel to modify/disable volume rendering</td></tr><tr><td>-</td><td>Export results to a csv file</td></tr></tbody></table></body></html>");

  QObject::connect(this->ButtonQualitativeAnalysis, SIGNAL(clicked(bool)), q, SIGNAL(qualitativeAnalysisClicked(bool)) );
  QObject::connect(this->ButtonQuantitativeAnalysis, SIGNAL(clicked(bool)), q, SIGNAL(quantitativeAnalysisClicked(bool)) );
  QObject::connect(this->ButtonVolumeRendering, SIGNAL(toggled(bool)), q, SIGNAL(volumeRenderingToggled(bool)) );
  QObject::connect(this->ButtonSpinView, SIGNAL(toggled(bool)), q, SIGNAL(spinViewToggled(bool)) );

  QObject::connect(this->ButtonExportCSV, SIGNAL(clicked()), q, SIGNAL(exportCSV()) );


  QObject::connect(this->TableStudyAnalysisSelection, SIGNAL(studySelectionChanged(int, bool)), q, SLOT(studySelectedInTable(int, bool)) );
}




//-----------------------------------------------------------------------------
// qSlicerLongitudinalPETCTAnalysisSettingsWidget methods

//-----------------------------------------------------------------------------
qSlicerLongitudinalPETCTAnalysisSettingsWidget
::qSlicerLongitudinalPETCTAnalysisSettingsWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerLongitudinalPETCTAnalysisSettingsWidgetPrivate(*this) )
{
  Q_D(qSlicerLongitudinalPETCTAnalysisSettingsWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerLongitudinalPETCTAnalysisSettingsWidget
::~qSlicerLongitudinalPETCTAnalysisSettingsWidget()
{
}


//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTAnalysisSettingsWidget::setReportNode(vtkMRMLLongitudinalPETCTReportNode* reportNode)
{
  Q_D(qSlicerLongitudinalPETCTAnalysisSettingsWidget);

  qvtkReconnect(d->ReportNode.GetPointer(), reportNode, vtkCommand::ModifiedEvent, this, SLOT(updateView()) );
  d->ReportNode = reportNode;

  this->updateView();

}


//-----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTReportNode* qSlicerLongitudinalPETCTAnalysisSettingsWidget::reportNode()
{
  Q_D(qSlicerLongitudinalPETCTAnalysisSettingsWidget);

  return d->ReportNode.GetPointer();
}

//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTAnalysisSettingsWidget
::updateView()
{
  Q_D(qSlicerLongitudinalPETCTAnalysisSettingsWidget);
  Q_ASSERT(d->TableStudyAnalysisSelection);

  std::cout << "UPDATING ANALYSIS SETTINGS" << std::endl;

  d->TableStudyAnalysisSelection->clearRows();

  if(d->ReportNode == NULL)
    return;


  bool lastSelected = d->ReportNode->GetNumberOfSelectedStudiesSelectedForAnalysis() == 1;

  for(int i=0; i < d->ReportNode->GetNumberOfSelectedStudies(); ++i)
    {
      vtkSmartPointer<vtkMRMLLongitudinalPETCTStudyNode> study = d->ReportNode->GetSelectedStudy(i);

      bool disabled = study->GetSelectedForAnalysis() && lastSelected;
      d->TableStudyAnalysisSelection->addStudyToTable(study, disabled);
    }

  std::cout << "END UPDATING ANALYSIS SETTINGS" << std::endl;
}


//-----------------------------------------------------------------------------
bool qSlicerLongitudinalPETCTAnalysisSettingsWidget::qualitativeChecked()
{
  Q_D(qSlicerLongitudinalPETCTAnalysisSettingsWidget);
  Q_ASSERT(d->ButtonQualitativeAnalysis);

  return d->ButtonQualitativeAnalysis->isChecked();
}

//-----------------------------------------------------------------------------
bool qSlicerLongitudinalPETCTAnalysisSettingsWidget::quantitativeChecked()
{
  Q_D(qSlicerLongitudinalPETCTAnalysisSettingsWidget);
  Q_ASSERT(d->ButtonQuantitativeAnalysis);

  return d->ButtonQuantitativeAnalysis->isChecked();
}


//-----------------------------------------------------------------------------
bool qSlicerLongitudinalPETCTAnalysisSettingsWidget::volumeRendering()
{
  Q_D(qSlicerLongitudinalPETCTAnalysisSettingsWidget);
  Q_ASSERT(d->ButtonVolumeRendering);

  return d->ButtonVolumeRendering->isChecked();
}

//-----------------------------------------------------------------------------
bool qSlicerLongitudinalPETCTAnalysisSettingsWidget::spinView()
{
  Q_D(qSlicerLongitudinalPETCTAnalysisSettingsWidget);
  Q_ASSERT(d->ButtonSpinView);

  return d->ButtonSpinView->isChecked();
}

//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTAnalysisSettingsWidget::setQualitativeChecked(bool checked)
{
  Q_D(qSlicerLongitudinalPETCTAnalysisSettingsWidget);
  Q_ASSERT(d->ButtonQualitativeAnalysis);

  d->ButtonQualitativeAnalysis->setChecked(checked);
}

//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTAnalysisSettingsWidget::setQuantitativeChecked(bool checked)
{
  Q_D(qSlicerLongitudinalPETCTAnalysisSettingsWidget);
  Q_ASSERT(d->ButtonQuantitativeAnalysis);

  d->ButtonQuantitativeAnalysis->setChecked(checked);
}

//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTAnalysisSettingsWidget::setVolumeRendering(bool checked)
{
  Q_D(qSlicerLongitudinalPETCTAnalysisSettingsWidget);
  Q_ASSERT(d->ButtonVolumeRendering);

  d->ButtonVolumeRendering->setChecked(checked);
}


//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTAnalysisSettingsWidget::setSpinView(bool checked)
{
  Q_D(qSlicerLongitudinalPETCTAnalysisSettingsWidget);
  Q_ASSERT(d->ButtonSpinView);

  d->ButtonSpinView->setChecked(checked);
}

//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTAnalysisSettingsWidget::studySelectedInTable(int index, bool selected)
{
  Q_D(qSlicerLongitudinalPETCTAnalysisSettingsWidget);

  if(d->ReportNode)
    {
      vtkSmartPointer<vtkMRMLLongitudinalPETCTStudyNode> study = d->ReportNode->GetSelectedStudy(index);

      if(study)
        study->SetSelectedForAnalysis(selected);
    }

    emit studySelectedForAnalysis(index, selected);
}

