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
#include "qSlicerLongitudinalPETCTReportSelectionWidget.h"
#include "ui_qSlicerLongitudinalPETCTReportSelectionWidget.h"


#include <QDate>

#include <vtkSmartPointer.h>
#include <vtkMRMLLongitudinalPETCTReportNode.h>
#include <vtkCommand.h>


#include <qMRMLNodeComboBox.h>


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongitudinalPETCT
class qSlicerLongitudinalPETCTReportSelectionWidgetPrivate :
  public Ui_qSlicerLongitudinalPETCTReportSelectionWidget
//  : public QWidget
{
  Q_DECLARE_PUBLIC(qSlicerLongitudinalPETCTReportSelectionWidget);
protected:
  qSlicerLongitudinalPETCTReportSelectionWidget* const q_ptr;

public:
  qSlicerLongitudinalPETCTReportSelectionWidgetPrivate(
    qSlicerLongitudinalPETCTReportSelectionWidget& object);

  virtual ~qSlicerLongitudinalPETCTReportSelectionWidgetPrivate();
  virtual void setupUi(qSlicerLongitudinalPETCTReportSelectionWidget* widget);


  vtkSmartPointer<vtkMRMLLongitudinalPETCTReportNode> currentSelectedReportNode();
  vtkSmartPointer<vtkMRMLLongitudinalPETCTReportNode> ReportNode;
};

// --------------------------------------------------------------------------
qSlicerLongitudinalPETCTReportSelectionWidgetPrivate
::qSlicerLongitudinalPETCTReportSelectionWidgetPrivate(
  qSlicerLongitudinalPETCTReportSelectionWidget& object)
  : q_ptr(&object), ReportNode(NULL)
{
}

// --------------------------------------------------------------------------
qSlicerLongitudinalPETCTReportSelectionWidgetPrivate
::~qSlicerLongitudinalPETCTReportSelectionWidgetPrivate()
{
}

// --------------------------------------------------------------------------
void qSlicerLongitudinalPETCTReportSelectionWidgetPrivate
::setupUi(qSlicerLongitudinalPETCTReportSelectionWidget* widget)
{
  Q_Q(qSlicerLongitudinalPETCTReportSelectionWidget);

  this->Ui_qSlicerLongitudinalPETCTReportSelectionWidget::setupUi(widget);

  this->ExpandButton->setOrientation(Qt::Vertical);

  this->MRMLNodeComboBoxReport->setNodeTypes(QStringList("vtkMRMLLongitudinalPETCTReportNode"));
  this->MRMLNodeComboBoxReport->setAddEnabled(false);
  this->MRMLNodeComboBoxReport->setRemoveEnabled(false);
  this->MRMLNodeComboBoxReport->setRenameEnabled(true);

  this->LabelInformation->setText("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\"font-family:\'LucidaGrande\'; font-size:13pt; font-weight:400; font-style:normal; text-align:justify;\"><p style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">There are no Longitudinalitudinal PET/CT Analysis Reports in the scene.<br/>Please import PET/CT image data using the <span style=\"font-style: italic;\">Longitudinalitudinal PET/CT Analysis Reader</span> in the <img src=\":/Icons/SlicerLoadDICOM.png\" />  <span style=\"font-weight:600;\">DICOM Module</span>.</p></body></html>");

  QObject::connect(this->MRMLNodeComboBoxReport, SIGNAL(currentNodeChanged(vtkMRMLNode*)), q, SLOT(selectionChanged(vtkMRMLNode*)) );
  //QObject::connect(this->MRMLNodeComboBoxReport, SIGNAL(nodeAdded()), q, SLOT(updateView()) );

}


//-----------------------------------------------------------------------------
vtkSmartPointer<vtkMRMLLongitudinalPETCTReportNode> qSlicerLongitudinalPETCTReportSelectionWidgetPrivate::currentSelectedReportNode()
{
  Q_ASSERT(this->MRMLNodeComboBoxReport);

  return vtkMRMLLongitudinalPETCTReportNode::SafeDownCast(this->MRMLNodeComboBoxReport->currentNode());
}

//-----------------------------------------------------------------------------
// qSlicerLongitudinalPETCTReportSelectionWidget methods

//-----------------------------------------------------------------------------
qSlicerLongitudinalPETCTReportSelectionWidget
::qSlicerLongitudinalPETCTReportSelectionWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerLongitudinalPETCTReportSelectionWidgetPrivate(*this) )
{
  Q_D(qSlicerLongitudinalPETCTReportSelectionWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerLongitudinalPETCTReportSelectionWidget
::~qSlicerLongitudinalPETCTReportSelectionWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTReportSelectionWidget::setMRMLScene(vtkMRMLScene* mrmlScene)
{
  Q_D(qSlicerLongitudinalPETCTReportSelectionWidget);
  Q_ASSERT(d->MRMLNodeComboBoxReport);

  d->MRMLNodeComboBoxReport->setMRMLScene(mrmlScene);

  this->setReportNode(d->currentSelectedReportNode());

}

//-----------------------------------------------------------------------------
vtkMRMLScene* qSlicerLongitudinalPETCTReportSelectionWidget::mrmlScene()
{
  Q_D(qSlicerLongitudinalPETCTReportSelectionWidget);

  return d->MRMLNodeComboBoxReport->mrmlScene();
}


//-----------------------------------------------------------------------------
qMRMLNodeComboBox* qSlicerLongitudinalPETCTReportSelectionWidget::mrmlNodeComboBoxReport()
{
  Q_D(qSlicerLongitudinalPETCTReportSelectionWidget);

  return d->MRMLNodeComboBoxReport;
}


//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTReportSelectionWidget::setReportNode(vtkSmartPointer<vtkMRMLLongitudinalPETCTReportNode> reportNode)
{
  Q_D(qSlicerLongitudinalPETCTReportSelectionWidget);

  qvtkReconnect(d->ReportNode.GetPointer(), vtkCommand::ModifiedEvent, this, SLOT(updateView()) );
  d->ReportNode = reportNode;

  this->updateView();

}

//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTReportSelectionWidget
::updateView()
{
  Q_D(qSlicerLongitudinalPETCTReportSelectionWidget);
  Q_ASSERT(d->LabelNameInfo);
  Q_ASSERT(d->LabelDoBInfo);
  Q_ASSERT(d->LabelSexInfo);
  Q_ASSERT(d->LabelInformation);
  Q_ASSERT(d->LabelSizeInfo);

  if(d->ReportNode != NULL)
    {
        d->LabelNameInfo->setText(d->ReportNode->GetAttribute("DICOM.PatientName"));
        QDate dob = QDate::fromString(QString(d->ReportNode->GetAttribute("DICOM.PatientBirthDate")).trimmed(),"yyyyMMdd");
        d->LabelDoBInfo->setText(dob.toString(Qt::SystemLocaleLongDate));
        d->LabelSexInfo->setText(d->ReportNode->GetAttribute("DICOM.PatientSex"));
        QString patientSize = QString(d->ReportNode->GetAttribute("DICOM.PatientSize")).trimmed();
        if( ! patientSize.isEmpty())
          patientSize.append(" m");
        d->LabelSizeInfo->setText(patientSize);

    }
  else
    {
        d->LabelNameInfo->setText(NULL);
        d->LabelDoBInfo->setText(NULL);
        d->LabelSexInfo->setText(NULL);
        d->LabelSizeInfo->setText(NULL);
    }

    bool infoVisible = d->MRMLNodeComboBoxReport->nodeCount() == 0;
    d->LabelInformation->setVisible(infoVisible);
}

//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTReportSelectionWidget
::selectionChanged(vtkMRMLNode* node)
{
    vtkSmartPointer<vtkMRMLLongitudinalPETCTReportNode> reportNode = vtkMRMLLongitudinalPETCTReportNode::SafeDownCast(node);
    this->setReportNode(reportNode);
}

