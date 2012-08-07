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
  virtual void setupUi(QWidget* mrmlNodeComboboxReports, qSlicerLongPETCTReportSelectionWidget* widget);

  QFormLayout* Layout;
  QLabel* LabelSelectReport;
  QWidget* MRMLNodeComboboxReports;

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
::setupUi(QWidget* mrmlNodeComboboxReports, qSlicerLongPETCTReportSelectionWidget* widget)
{
  Q_Q(qSlicerLongPETCTReportSelectionWidget);

  this->Layout = new QFormLayout(widget);
  this->Layout->setSpacing(10);


  this->LabelSelectReport = new QLabel("Select Report:",widget);

  this->MRMLNodeComboboxReports = mrmlNodeComboboxReports;

  this->Layout->addRow(this->LabelSelectReport,this->MRMLNodeComboboxReports);
}

//-----------------------------------------------------------------------------
// qSlicerLongPETCTReportSelectionWidget methods

//-----------------------------------------------------------------------------
qSlicerLongPETCTReportSelectionWidget
::qSlicerLongPETCTReportSelectionWidget(QWidget* mrmlNodeComboboxReports, QWidget* parentWidget)
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

QWidget* qSlicerLongPETCTReportSelectionWidget::mrmlNodeComboboxReports()
{
  Q_D(qSlicerLongPETCTReportSelectionWidget);
  return d->MRMLNodeComboboxReports;
}

