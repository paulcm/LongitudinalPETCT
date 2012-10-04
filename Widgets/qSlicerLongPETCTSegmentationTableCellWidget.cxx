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

// LongPETCTStudySlider Widgets includes
#include "qSlicerLongPETCTSegmentationTableCellWidget.h"
#include "ui_qSlicerLongPETCTSegmentationTableCellWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongitudinalPETCT
class qSlicerLongPETCTSegmentationTableCellWidgetPrivate
  : public Ui_qSlicerLongPETCTSegmentationTableCellWidget
{
  Q_DECLARE_PUBLIC(qSlicerLongPETCTSegmentationTableCellWidget);
protected:
  qSlicerLongPETCTSegmentationTableCellWidget* const q_ptr;

public:
  qSlicerLongPETCTSegmentationTableCellWidgetPrivate(qSlicerLongPETCTSegmentationTableCellWidget& object);

  virtual ~qSlicerLongPETCTSegmentationTableCellWidgetPrivate();

  virtual void setupUi(qSlicerLongPETCTSegmentationTableCellWidget*);
};

// --------------------------------------------------------------------------
qSlicerLongPETCTSegmentationTableCellWidgetPrivate
::qSlicerLongPETCTSegmentationTableCellWidgetPrivate(
  qSlicerLongPETCTSegmentationTableCellWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
qSlicerLongPETCTSegmentationTableCellWidgetPrivate
::~qSlicerLongPETCTSegmentationTableCellWidgetPrivate()
{
}


// --------------------------------------------------------------------------
void qSlicerLongPETCTSegmentationTableCellWidgetPrivate
::setupUi(qSlicerLongPETCTSegmentationTableCellWidget* widget)
{
  Q_Q(qSlicerLongPETCTSegmentationTableCellWidget);

  this->Ui_qSlicerLongPETCTSegmentationTableCellWidget::setupUi(widget);


  QObject::connect( this->CheckBoxModelVisibility, SIGNAL(toggled(bool)), q, SIGNAL(modelVisibilityCheckBoxChecked(bool)) );

}

//-----------------------------------------------------------------------------
// qSlicerLongPETCTSegmentationTableCellWidget methods

//-----------------------------------------------------------------------------
qSlicerLongPETCTSegmentationTableCellWidget
::qSlicerLongPETCTSegmentationTableCellWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerLongPETCTSegmentationTableCellWidgetPrivate(*this) )
{
  Q_D(qSlicerLongPETCTSegmentationTableCellWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerLongPETCTSegmentationTableCellWidget
::~qSlicerLongPETCTSegmentationTableCellWidget()
{
}


//-----------------------------------------------------------------------------
bool qSlicerLongPETCTSegmentationTableCellWidget::modelVisibilityChecked()
{
  Q_D(qSlicerLongPETCTSegmentationTableCellWidget);
  Q_ASSERT(d->CheckBoxModelVisibility);

  return d->CheckBoxModelVisibility->isChecked();
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTSegmentationTableCellWidget::setModelVisibilityChecked(bool checked)
{
  Q_D(qSlicerLongPETCTSegmentationTableCellWidget);
  Q_ASSERT(d->CheckBoxModelVisibility);

  d->CheckBoxModelVisibility->setChecked(checked);
}














