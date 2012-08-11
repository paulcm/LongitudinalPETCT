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

// LongPETCTReportSelection Widgets includes
#include "qSlicerLongPETCTReportSelectionWidget.h"


#include <QFormLayout>
#include <QLabel>
#include <QDate>

#include <qMRMLNodeComboBox.h>

#include <vtkMRMLLongPETCTReportNode.h>


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongPETCT
class qSlicerLongPETCTReportSelectionWidgetPrivate
  : public QWidget
{
  Q_DECLARE_PUBLIC(qSlicerLongPETCTReportSelectionWidget);
protected:
  qSlicerLongPETCTReportSelectionWidget* const q_ptr;

public:
  qSlicerLongPETCTReportSelectionWidgetPrivate(
    qSlicerLongPETCTReportSelectionWidget& object);
  virtual void setupUi(qSlicerLongPETCTReportSelectionWidget* widget);

  QFormLayout* Layout;
  QLabel* LabelSelectReport;
  QLabel* LabelName;
  QLabel* LabelNameInfo;
  QLabel* LabelBirthDate;
  QLabel* LabelBirthDateInfo;
  QLabel* LabelSex;
  QLabel* LabelSexInfo;
  QFrame* Separator;


  QWidget* Placeholder;
  qMRMLNodeComboBox* MRMLNodeComboboxReports;

};

// --------------------------------------------------------------------------
qSlicerLongPETCTReportSelectionWidgetPrivate
::qSlicerLongPETCTReportSelectionWidgetPrivate(
  qSlicerLongPETCTReportSelectionWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerLongPETCTReportSelectionWidgetPrivate
::setupUi(qSlicerLongPETCTReportSelectionWidget* widget)
{
  Q_Q(qSlicerLongPETCTReportSelectionWidget);

  this->Layout = new QFormLayout(widget);
  this->Layout->setLabelAlignment(Qt::AlignRight);
  this->Layout->setSpacing(10);

  this->LabelSelectReport = new QLabel("Select Report:",widget);

  this->Separator = new QFrame(widget);
  this->Separator->setFrameStyle(QFrame::HLine | QFrame::Sunken);

  this->LabelName = new QLabel("Patient Name:",widget);
  this->LabelNameInfo = new QLabel(widget);

  this->LabelBirthDate = new QLabel("Date of Birth:",widget);
  this->LabelBirthDateInfo = new QLabel(widget);

  this->LabelSex = new QLabel("Sex:",widget);
  this->LabelSexInfo = new QLabel(widget);

  this->Placeholder = new QWidget(widget);

  this->MRMLNodeComboboxReports = NULL;


  this->Layout->addRow(this->LabelSelectReport, this->Placeholder);//this->MRMLNodeComboboxReports);
  this->Layout->addRow(this->Separator);
  this->Layout->addRow(this->LabelName, this->LabelNameInfo);
  this->Layout->addRow(this->LabelBirthDate, this->LabelBirthDateInfo);
  this->Layout->addRow(this->LabelSex, this->LabelSexInfo);
}

//-----------------------------------------------------------------------------
// qSlicerLongPETCTReportSelectionWidget methods

//-----------------------------------------------------------------------------
qSlicerLongPETCTReportSelectionWidget
::qSlicerLongPETCTReportSelectionWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerLongPETCTReportSelectionWidgetPrivate(*this) )
{
  Q_D(qSlicerLongPETCTReportSelectionWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerLongPETCTReportSelectionWidget
::~qSlicerLongPETCTReportSelectionWidget()
{
}

void qSlicerLongPETCTReportSelectionWidget::setMRMLNodeComboBoxReports(qMRMLNodeComboBox* reportsComboBox)
{
  Q_D(qSlicerLongPETCTReportSelectionWidget);
  Q_ASSERT(d->Layout);
  Q_ASSERT(d->Placeholder);

  QLayoutItem* fieldItem = d->Layout->itemAt(0,QFormLayout::FieldRole);
  d->Layout->removeItem(fieldItem);

  qMRMLNodeComboBox* currentWidget = qobject_cast<qMRMLNodeComboBox*>(d->MRMLNodeComboboxReports);
  if(currentWidget)
    disconnect( d->MRMLNodeComboboxReports, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT(updateReportInformation(vtkMRMLNode*)) );


  d->MRMLNodeComboboxReports = reportsComboBox;

  if(d->MRMLNodeComboboxReports)
    {
      d->Layout->setWidget(0,QFormLayout::FieldRole, d->MRMLNodeComboboxReports);
      this->updateReportInformation(d->MRMLNodeComboboxReports->currentNode());
      connect( d->MRMLNodeComboboxReports, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT(updateReportInformation(vtkMRMLNode*)) );
    }
  else
    d->Layout->setWidget(0,QFormLayout::FieldRole, d->Placeholder);
}

//qMRMLNodeComboBox* qSlicerLongPETCTReportSelectionWidget::mrmlNodeComboboxReports()
//{
//  Q_D(qSlicerLongPETCTReportSelectionWidget);
//  return d->MRMLNodeComboboxReports;
//}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTReportSelectionWidget
::updateReportInformation(vtkMRMLNode* node)
{
  Q_D(qSlicerLongPETCTReportSelectionWidget);
  Q_ASSERT(d->LabelNameInfo);
  Q_ASSERT(d->LabelBirthDateInfo);
  Q_ASSERT(d->LabelSexInfo);

  vtkMRMLLongPETCTReportNode* selectedReportNode = vtkMRMLLongPETCTReportNode::SafeDownCast(node);

  if(selectedReportNode)
    {
        d->LabelNameInfo->setText(selectedReportNode->GetAttribute("DICOM.PatientName"));

        QDate dob = QDate::fromString(QString(selectedReportNode->GetAttribute("DICOM.PatientBirthDate")).trimmed(),"yyyyMMdd");
        d->LabelBirthDateInfo->setText(dob.toString(Qt::SystemLocaleLongDate));
        d->LabelSexInfo->setText(selectedReportNode->GetAttribute("DICOM.PatientSex"));
    }
  else
    {
        QString noText;
        d->LabelNameInfo->setText(noText);
        d->LabelBirthDateInfo->setText(noText);
        d->LabelSexInfo->setText(noText);
    }
}
