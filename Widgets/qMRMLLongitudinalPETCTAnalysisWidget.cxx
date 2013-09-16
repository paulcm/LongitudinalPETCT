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
#include "qMRMLLongitudinalPETCTAnalysisWidget.h"
#include "ui_qMRMLLongitudinalPETCTAnalysisWidget.h"


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
class qMRMLLongitudinalPETCTAnalysisWidgetPrivate :
  public Ui_qMRMLLongitudinalPETCTAnalysisWidget
//  : public QWidget
{
  Q_DECLARE_PUBLIC(qMRMLLongitudinalPETCTAnalysisWidget);
protected:
  qMRMLLongitudinalPETCTAnalysisWidget* const q_ptr;

public:
  qMRMLLongitudinalPETCTAnalysisWidgetPrivate(
    qMRMLLongitudinalPETCTAnalysisWidget& object);

  virtual ~qMRMLLongitudinalPETCTAnalysisWidgetPrivate();
  virtual void setupUi(qMRMLLongitudinalPETCTAnalysisWidget* widget);

  vtkSmartPointer<vtkMRMLLongitudinalPETCTReportNode> ReportNode;


};

// --------------------------------------------------------------------------
qMRMLLongitudinalPETCTAnalysisWidgetPrivate
::qMRMLLongitudinalPETCTAnalysisWidgetPrivate(
  qMRMLLongitudinalPETCTAnalysisWidget& object)
  : q_ptr(&object), ReportNode(NULL)
{
}

// --------------------------------------------------------------------------
qMRMLLongitudinalPETCTAnalysisWidgetPrivate
::~qMRMLLongitudinalPETCTAnalysisWidgetPrivate()
{
}

// --------------------------------------------------------------------------
void qMRMLLongitudinalPETCTAnalysisWidgetPrivate
::setupUi(qMRMLLongitudinalPETCTAnalysisWidget* widget)
{
  Q_Q(qMRMLLongitudinalPETCTAnalysisWidget);

  this->Ui_qMRMLLongitudinalPETCTAnalysisWidget::setupUi(widget);

  this->TableStudyAnalysisSelection->setTableMode(qMRMLLongitudinalPETCTStudyTableWidget::AnalysisSelection);

  this->LabelInfo->setToolTip("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\"></style></head><body style=\"font-family:\'Lucida Grande\',sans-serif; font-size: 12pt; font-weight: 400; font-style: normal;border: 1px solid black;margin-top:0px;\"><table cellspacing=\"5\"><tbody><tr><td>-</td><td>Switch between qualitative and quantiative display of Analysis results.</td></tr><tr><td>-</td><td>Remove/add Studies for comparison</td></tr><tr><td>-</td><td>Export results to a csv file</td></tr></tbody></table></body></html>");

  QObject::connect(this->ButtonQualitativeAnalysis, SIGNAL(clicked(bool)), q, SIGNAL(qualitativeAnalysisClicked(bool)) );
  QObject::connect(this->ButtonQuantitativeAnalysis, SIGNAL(clicked(bool)), q, SIGNAL(quantitativeAnalysisClicked(bool)) );
  QObject::connect(this->ButtonExportCSV, SIGNAL(clicked()), q, SIGNAL(exportCSV()) );


  QObject::connect(this->TableStudyAnalysisSelection, SIGNAL(studySelectionChanged(int, bool)), q, SLOT(studySelectedInTable(int, bool)) );
}




//-----------------------------------------------------------------------------
// qMRMLLongitudinalPETCTAnalysisWidget methods

//-----------------------------------------------------------------------------
qMRMLLongitudinalPETCTAnalysisWidget
::qMRMLLongitudinalPETCTAnalysisWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qMRMLLongitudinalPETCTAnalysisWidgetPrivate(*this) )
{
  Q_D(qMRMLLongitudinalPETCTAnalysisWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qMRMLLongitudinalPETCTAnalysisWidget
::~qMRMLLongitudinalPETCTAnalysisWidget()
{
}


//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTAnalysisWidget::setReportNode(vtkMRMLLongitudinalPETCTReportNode* reportNode)
{
  Superclass::setReportNode(reportNode);

  Q_D (qMRMLLongitudinalPETCTAnalysisWidget);

  d->TableStudyAnalysisSelection->setReportNode(reportNode);


}


//-----------------------------------------------------------------------------
bool qMRMLLongitudinalPETCTAnalysisWidget::qualitativeChecked()
{
  Q_D(qMRMLLongitudinalPETCTAnalysisWidget);
  Q_ASSERT(d->ButtonQualitativeAnalysis);

  return d->ButtonQualitativeAnalysis->isChecked();
}

//-----------------------------------------------------------------------------
bool qMRMLLongitudinalPETCTAnalysisWidget::quantitativeChecked()
{
  Q_D(qMRMLLongitudinalPETCTAnalysisWidget);
  Q_ASSERT(d->ButtonQuantitativeAnalysis);

  return d->ButtonQuantitativeAnalysis->isChecked();
}

//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTAnalysisWidget::setQualitativeChecked(bool checked)
{
  Q_D(qMRMLLongitudinalPETCTAnalysisWidget);
  Q_ASSERT(d->ButtonQualitativeAnalysis);

  d->ButtonQualitativeAnalysis->setChecked(checked);
}

//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTAnalysisWidget::setQuantitativeChecked(bool checked)
{
  Q_D(qMRMLLongitudinalPETCTAnalysisWidget);
  Q_ASSERT(d->ButtonQuantitativeAnalysis);

  d->ButtonQuantitativeAnalysis->setChecked(checked);
}

//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTAnalysisWidget::studySelectedInTable(int index, bool selected)
{
  vtkMRMLLongitudinalPETCTReportNode* report = this->reportNode();

  if(report)
    {
      vtkMRMLLongitudinalPETCTStudyNode* study = report->GetSelectedStudy(index);

      if(study)
        emit studySelectedForAnalysis(study, selected);
    }
}

