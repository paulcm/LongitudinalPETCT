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

#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QTableWidget>
#include <QFrame>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongitudinalPETCT
class qSlicerLongPETCTStudySelectionWidgetPrivate
  : public QWidget
{
  Q_DECLARE_PUBLIC(qSlicerLongPETCTStudySelectionWidget);
protected:
  qSlicerLongPETCTStudySelectionWidget* const q_ptr;

public:
  qSlicerLongPETCTStudySelectionWidgetPrivate(
    qSlicerLongPETCTStudySelectionWidget& object);
  virtual void setupUi(qSlicerLongPETCTStudySelectionWidget*);

  QVBoxLayout* Layout;

  QLabel* LabelSelectPatient;
  QComboBox* ComboBoxPatientNames;

  QFrame* FrameHorizontalSeparator;

  QTableWidget* TablePETCTStudies;

};

// --------------------------------------------------------------------------
qSlicerLongPETCTStudySelectionWidgetPrivate
::qSlicerLongPETCTStudySelectionWidgetPrivate(
  qSlicerLongPETCTStudySelectionWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidgetPrivate
::setupUi(qSlicerLongPETCTStudySelectionWidget* widget)
{
  Q_Q(qSlicerLongPETCTStudySelectionWidget);

  this->Layout = new QVBoxLayout(widget);
  this->Layout->setSpacing(8);

  this->LabelSelectPatient = new QLabel("Select patient from DICOM database",widget);

  this->ComboBoxPatientNames = new QComboBox(widget);

  this->FrameHorizontalSeparator = new QFrame(widget);
  this->FrameHorizontalSeparator->setFrameStyle(QFrame::HLine | QFrame::Sunken);

  this->TablePETCTStudies = new QTableWidget(0,4,widget);
  // set table column headers
  this->TablePETCTStudies->setColumnWidth(0,18);
  this->TablePETCTStudies->setHorizontalHeaderItem(0, new QTableWidgetItem(""));
  this->TablePETCTStudies->setHorizontalHeaderItem(1, new QTableWidgetItem("Date"));
  this->TablePETCTStudies->setHorizontalHeaderItem(2, new QTableWidgetItem("Time"));
  this->TablePETCTStudies->setHorizontalHeaderItem(3, new QTableWidgetItem("Modality"));
  // set size policy
  this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);


  this->Layout->addWidget(this->LabelSelectPatient);
  this->Layout->addWidget(this->ComboBoxPatientNames);
  this->Layout->addWidget(this->FrameHorizontalSeparator);
  this->Layout->addWidget(this->TablePETCTStudies);

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
QStringList qSlicerLongPETCTStudySelectionWidget::patientNames()
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  Q_ASSERT(d->ComboBoxPatientNames);

  QStringList patientNamesList;

  for(int i=0; i < d->ComboBoxPatientNames->count(); ++i)
    {
      patientNamesList.append(d->ComboBoxPatientNames->itemText(i));
    }

  return patientNamesList;
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidget::setPatientNames(const QStringList& patientNames)
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  Q_ASSERT(d->ComboBoxPatientNames);

  d->ComboBoxPatientNames->clear();

  for(int i=0; i < patientNames.size(); ++i)
    {
      d->ComboBoxPatientNames->addItem(patientNames.at(i));
    }
}



