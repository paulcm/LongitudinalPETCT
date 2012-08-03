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


#include <QHBoxLayout>
#include <QLabel>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongitudinalPETCT
class qSlicerLongPETCTReportSelectionWidgetPrivate
  : public QWidget
{
  Q_DECLARE_PUBLIC(qSlicerLongPETCTReportSelectionWidget);
protected:
  qSlicerLongPETCTReportSelectionWidget* const q_ptr;

public:
  qSlicerLongPETCTReportSelectionWidgetPrivate(
    qSlicerLongPETCTReportSelectionWidget& object);
  virtual void setupUi(qMRMLNodeComboBox* mrmlNodeComboboxReports, qSlicerLongPETCTReportSelectionWidget*);

  QHBoxLayout* Layout;
  QLabel* LabelSelectReport;
  qMRMLNodeComboBox* MRMLNodeComboboxReport;

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
::setupUi(qMRMLNodeComboBox* mrmlNodeComboboxReports, qSlicerLongPETCTReportSelectionWidget* widget)
{
  Q_Q(qSlicerLongPETCTReportSelectionWidget);

  this->Layout = new QHBoxLayout(widget);
  this->Layout->setContentsMargins(8,0,8,26);

  this->LabelSelectReport = new QLabel("Select Report:",widget);

  this->MRMLNodeComboboxReport = mrmlNodeComboboxReports;


  this->Layout->addWidget(this->LabelSelectReport,0,Qt::AlignLeft);
  this->Layout->addWidget(this->MRMLNodeComboboxReport,2,Qt::AlignLeft);
  //this->Ui_qSlicerLongPETCTReportSelectionWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerLongPETCTReportSelectionWidget methods

//-----------------------------------------------------------------------------
qSlicerLongPETCTReportSelectionWidget
::qSlicerLongPETCTReportSelectionWidget(qMRMLNodeComboBox* mrmlNodeComboboxReports, QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerLongPETCTReportSelectionWidgetPrivate(*this) )
{
  Q_D(qSlicerLongPETCTReportSelectionWidget);
  d->setupUi(mrmlNodeComboboxReports, this);
}

//-----------------------------------------------------------------------------
qSlicerLongPETCTReportSelectionWidget
::~qSlicerLongPETCTReportSelectionWidget()
{
}

qMRMLNodeComboBox* qSlicerLongPETCTReportSelectionWidget::mrmlNodeComboboxReports()
{
  Q_D(qSlicerLongPETCTReportSelectionWidget);
  return d->MRMLNodeComboboxReport;
}

void qSlicerLongPETCTReportSelectionWidget::setMrmlNodeComboboxReports(qMRMLNodeComboBox* mrmlNodeComboboxReports)
{
  Q_D(qSlicerLongPETCTReportSelectionWidget);
  Q_ASSERT(d->MRMLNodeComboboxReport);

  d->MRMLNodeComboboxReport = mrmlNodeComboboxReports;
}
