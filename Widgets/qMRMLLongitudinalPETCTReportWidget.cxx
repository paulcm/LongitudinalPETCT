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

// LongitudinalPETCTReportSelection Widgets includes
#include "qMRMLLongitudinalPETCTReportWidget.h"
#include "ui_qMRMLLongitudinalPETCTReportWidget.h"


#include <vtkMRMLLongitudinalPETCTReportNode.h>

// Qt
#include <QDate>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongitudinalPETCT
class qMRMLLongitudinalPETCTReportWidgetPrivate :
  public Ui_qMRMLLongitudinalPETCTReportWidget
//  : public QWidget
{
  Q_DECLARE_PUBLIC(qMRMLLongitudinalPETCTReportWidget);
protected:
  qMRMLLongitudinalPETCTReportWidget* const q_ptr;

public:
  qMRMLLongitudinalPETCTReportWidgetPrivate(
    qMRMLLongitudinalPETCTReportWidget& object);

  virtual ~qMRMLLongitudinalPETCTReportWidgetPrivate();
  virtual void setupUi(qMRMLLongitudinalPETCTReportWidget* widget);

};

// --------------------------------------------------------------------------
qMRMLLongitudinalPETCTReportWidgetPrivate
::qMRMLLongitudinalPETCTReportWidgetPrivate(
  qMRMLLongitudinalPETCTReportWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
qMRMLLongitudinalPETCTReportWidgetPrivate
::~qMRMLLongitudinalPETCTReportWidgetPrivate()
{
}

// --------------------------------------------------------------------------
void qMRMLLongitudinalPETCTReportWidgetPrivate
::setupUi(qMRMLLongitudinalPETCTReportWidget* widget)
{
  Q_Q(qMRMLLongitudinalPETCTReportWidget);

  this->Ui_qMRMLLongitudinalPETCTReportWidget::setupUi(widget);

  this->ExpandButton->setOrientation(Qt::Vertical);

  this->MRMLNodeComboBoxReport->setNodeTypes(QStringList("vtkMRMLLongitudinalPETCTReportNode"));
  this->MRMLNodeComboBoxReport->setAddEnabled(false);
  this->MRMLNodeComboBoxReport->setRemoveEnabled(false);
  this->MRMLNodeComboBoxReport->setRenameEnabled(true);

  this->LabelInfo->setToolTip("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\"></style></head><body style=\"font-family:\'Lucida Grande\',sans-serif; font-size: 12pt; font-weight: 400; font-style: normal;border: 1px solid black;margin-top:0px;\"><table cellspacing=\"5\"><tbody><tr><td><img src=\":/Icons/SlicerLoadDICOM.png\" /></td><td>Import valid PET/CT image data with the DICOM Module using the <strong>Longitudinal PET/CT Analysis Reader<strong></td></tr><tr><td>-</td><td>Select a <strong>Report</strong> to perform longitudinal PET/CT analysis</td></tr></tbody></table></body></html>");
  this->MRMLNodeComboBoxReport->setToolTip("Select a Report for the PET/CT Analysis");
  QObject::connect(this->MRMLNodeComboBoxReport, SIGNAL(currentNodeChanged(vtkMRMLNode*)), q, SLOT(reportNodeChanged(vtkMRMLNode*)) );
}

//-----------------------------------------------------------------------------
// qMRMLLongitudinalPETCTReportWidget methods

//-----------------------------------------------------------------------------
qMRMLLongitudinalPETCTReportWidget
::qMRMLLongitudinalPETCTReportWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qMRMLLongitudinalPETCTReportWidgetPrivate(*this) )
{
  Q_D(qMRMLLongitudinalPETCTReportWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qMRMLLongitudinalPETCTReportWidget
::~qMRMLLongitudinalPETCTReportWidget()
{
}

//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTReportWidget::setMRMLScene(vtkMRMLScene* mrmlScene)
{
  Superclass::setMRMLScene(mrmlScene);

  Q_D(qMRMLLongitudinalPETCTReportWidget);
  Q_ASSERT(d->MRMLNodeComboBoxReport);

  d->MRMLNodeComboBoxReport->setMRMLScene(mrmlScene);
}

//-----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTReportNode*
qMRMLLongitudinalPETCTReportWidget::currentSelectedReportNode()
{
  Q_D(qMRMLLongitudinalPETCTReportWidget);
  Q_ASSERT(d->MRMLNodeComboBoxReport);

  vtkMRMLLongitudinalPETCTReportNode* reportNode =
      vtkMRMLLongitudinalPETCTReportNode::SafeDownCast(
          d->MRMLNodeComboBoxReport->currentNode());

  return reportNode;
}


//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTReportWidget
::updateWidgetFromMRML()
{
  Q_D(qMRMLLongitudinalPETCTReportWidget);
  Q_ASSERT(d->LabelNameInfo);
  Q_ASSERT(d->LabelDoBInfo);
  Q_ASSERT(d->LabelSexInfo);
  Q_ASSERT(d->LabelSizeInfo);

  vtkMRMLLongitudinalPETCTReportNode* report = this->reportNode();

  if(report)
    {
        d->LabelNameInfo->setText(report->GetAttribute("DICOM.PatientName"));
        QDate dob = QDate::fromString(QString(report->GetAttribute("DICOM.PatientBirthDate")).trimmed(),"yyyyMMdd");
        d->LabelDoBInfo->setText(dob.toString(Qt::SystemLocaleLongDate));
        d->LabelSexInfo->setText(report->GetAttribute("DICOM.PatientSex"));
        QString patientHeight = QString(report->GetAttribute("DICOM.PatientHeight")).trimmed();

        // add "0" if only one digit after decimal point
        patientHeight = patientHeight.right(2).contains(".") ? patientHeight+"0" : patientHeight;

        if( ! patientHeight.isEmpty())
          patientHeight.append(" m");
        d->LabelSizeInfo->setText(patientHeight);

    }
  else
    {
        d->LabelNameInfo->setText(NULL);
        d->LabelDoBInfo->setText(NULL);
        d->LabelSexInfo->setText(NULL);
        d->LabelSizeInfo->setText(NULL);
    }

}

//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTReportWidget
::reportNodeChanged(vtkMRMLNode* node)
{
    vtkMRMLLongitudinalPETCTReportNode* reportNode = vtkMRMLLongitudinalPETCTReportNode::SafeDownCast(node);

    emit reportNodeChanged(reportNode);
}

