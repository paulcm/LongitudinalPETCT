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
#include "ui_qSlicerLongPETCTReportSelectionWidget.h"


#include <QDate>

#include <qMRMLNodeComboBox.h>
#include <vtkMRMLLongPETCTReportNode.h>




//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongPETCT
class qSlicerLongPETCTReportSelectionWidgetPrivate :
  public Ui_qSlicerLongPETCTReportSelectionWidget
//  : public QWidget
{
  Q_DECLARE_PUBLIC(qSlicerLongPETCTReportSelectionWidget);
protected:
  qSlicerLongPETCTReportSelectionWidget* const q_ptr;

public:
  qSlicerLongPETCTReportSelectionWidgetPrivate(
    qSlicerLongPETCTReportSelectionWidget& object);

  virtual ~qSlicerLongPETCTReportSelectionWidgetPrivate();


  virtual void setupUi(qSlicerLongPETCTReportSelectionWidget* widget);

};

// --------------------------------------------------------------------------
qSlicerLongPETCTReportSelectionWidgetPrivate
::qSlicerLongPETCTReportSelectionWidgetPrivate(
  qSlicerLongPETCTReportSelectionWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
qSlicerLongPETCTReportSelectionWidgetPrivate
::~qSlicerLongPETCTReportSelectionWidgetPrivate()
{
}

// --------------------------------------------------------------------------
void qSlicerLongPETCTReportSelectionWidgetPrivate
::setupUi(qSlicerLongPETCTReportSelectionWidget* widget)
{
  Q_Q(qSlicerLongPETCTReportSelectionWidget);

  this->Ui_qSlicerLongPETCTReportSelectionWidget::setupUi(widget);

  this->ExpandButton->setOrientation(Qt::Vertical);

  this->MRMLNodeComboBoxReport->setNodeTypes(QStringList("vtkMRMLLongPETCTReportNode"));
  QObject::connect( this->MRMLNodeComboBoxReport, SIGNAL(currentNodeChanged(vtkMRMLNode*)), widget, SLOT(update(vtkMRMLNode*)) );

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

//-----------------------------------------------------------------------------
void qSlicerLongPETCTReportSelectionWidget::setMRMLScene(vtkMRMLScene* mrmlScene)
{
  Q_D(qSlicerLongPETCTReportSelectionWidget);
  Q_ASSERT(d->MRMLNodeComboBoxReport);

  std::cout << "MRML SCENE: " << mrmlScene << std::endl;
  d->MRMLNodeComboBoxReport->setMRMLScene(mrmlScene);
  std::cout << "MRML SCENE SET" << std::endl;
  d->MRMLNodeComboBoxReport->setAddEnabled(false);
  d->MRMLNodeComboBoxReport->setRemoveEnabled(false);
  d->MRMLNodeComboBoxReport->setRenameEnabled(true);

}

//-----------------------------------------------------------------------------
vtkMRMLScene* qSlicerLongPETCTReportSelectionWidget::mrmlScene()
{
  Q_D(qSlicerLongPETCTReportSelectionWidget);

  return d->MRMLNodeComboBoxReport->mrmlScene();
}


//-----------------------------------------------------------------------------
qMRMLNodeComboBox* qSlicerLongPETCTReportSelectionWidget::mrmlNodeComboBoxReport()
{
  Q_D(qSlicerLongPETCTReportSelectionWidget);

  return d->MRMLNodeComboBoxReport;
}


//-----------------------------------------------------------------------------
void qSlicerLongPETCTReportSelectionWidget
::update(vtkMRMLNode* node)
{
  Q_D(qSlicerLongPETCTReportSelectionWidget);
  Q_ASSERT(d->LabelNameInfo);
  Q_ASSERT(d->LabelDoBInfo);
  Q_ASSERT(d->LabelSexInfo);


  vtkMRMLLongPETCTReportNode* selectedReportNode = vtkMRMLLongPETCTReportNode::SafeDownCast(node);
  std::cout << "SELECTED NODe" << selectedReportNode <<  std::endl;

  if(selectedReportNode)
    {
        d->LabelNameInfo->setText(selectedReportNode->GetAttribute("DICOM.PatientName"));
        QDate dob = QDate::fromString(QString(selectedReportNode->GetAttribute("DICOM.PatientBirthDate")).trimmed(),"yyyyMMdd");
        d->LabelDoBInfo->setText(dob.toString(Qt::SystemLocaleLongDate));
        d->LabelSexInfo->setText(selectedReportNode->GetAttribute("DICOM.PatientSex"));
    }
  else
    {
        QString noText;
        d->LabelNameInfo->setText(noText);
        d->LabelDoBInfo->setText(noText);
        d->LabelSexInfo->setText(noText);
    }

}
