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
#include "qSlicerLongPETCTStudySliderWidget.h"

#include <QGridLayout>
#include <QLabel>
#include <QSlider>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongitudinalPETCT
class qSlicerLongPETCTStudySliderWidgetPrivate
  : public QWidget
{
  Q_DECLARE_PUBLIC(qSlicerLongPETCTStudySliderWidget);
protected:
  qSlicerLongPETCTStudySliderWidget* const q_ptr;

public:
  qSlicerLongPETCTStudySliderWidgetPrivate(
    qSlicerLongPETCTStudySliderWidget& object);
  virtual void setupUi(qSlicerLongPETCTStudySliderWidget*);

  QGridLayout* Layout;

  QLabel* LabelDescription;
  QLabel* LabelSelectedTimepoint;

  QSlider* Slider;

};

// --------------------------------------------------------------------------
qSlicerLongPETCTStudySliderWidgetPrivate
::qSlicerLongPETCTStudySliderWidgetPrivate(
  qSlicerLongPETCTStudySliderWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerLongPETCTStudySliderWidgetPrivate
::setupUi(qSlicerLongPETCTStudySliderWidget* widget)
{
  Q_Q(qSlicerLongPETCTStudySliderWidget);

  this->Layout = new QGridLayout(widget);
  this->Layout->setSpacing(8);

  this->LabelDescription = new QLabel("Selected Study:",widget);
  this->LabelSelectedTimepoint = new QLabel(widget);
  this->LabelSelectedTimepoint->setAlignment(Qt::AlignLeft);

  this->Slider = new QSlider(Qt::Horizontal,widget);
  this->Slider->setMinimum(0);
  this->Slider->setMaximum(1);
  this->Slider->setValue(0);
  this->Slider->setTickPosition(QSlider::TicksBothSides);
  this->Slider->setEnabled(false);

  this->Layout->addWidget(this->LabelDescription,0,0,1,1);
  this->Layout->addWidget(this->LabelSelectedTimepoint,0,1,1,1);
  this->Layout->addWidget(this->Slider,1,0,1,2);

}

//-----------------------------------------------------------------------------
// qSlicerLongPETCTStudySliderWidget methods

//-----------------------------------------------------------------------------
qSlicerLongPETCTStudySliderWidget
::qSlicerLongPETCTStudySliderWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerLongPETCTStudySliderWidgetPrivate(*this) )
{
  Q_D(qSlicerLongPETCTStudySliderWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerLongPETCTStudySliderWidget
::~qSlicerLongPETCTStudySliderWidget()
{
}

int qSlicerLongPETCTStudySliderWidget::maximumValue()
{
  Q_D(qSlicerLongPETCTStudySliderWidget);
  return d->Slider->maximum();
}

int qSlicerLongPETCTStudySliderWidget::currentValue()
{
  Q_D(qSlicerLongPETCTStudySliderWidget);
  return d->Slider->value();
}

QString qSlicerLongPETCTStudySliderWidget::selectedStudy()
{
  Q_D(qSlicerLongPETCTStudySliderWidget);
  return d->LabelSelectedTimepoint->text();
}

void qSlicerLongPETCTStudySliderWidget::setMaximumValue(int value)
{
  Q_D(qSlicerLongPETCTStudySliderWidget);
  Q_ASSERT(d->Slider);

  d->Slider->setMaximum(value);
}

void qSlicerLongPETCTStudySliderWidget::setCurrentValue(int value)
{
  Q_D(qSlicerLongPETCTStudySliderWidget);
  Q_ASSERT(d->Slider);

  d->Slider->setValue(value);
}

void qSlicerLongPETCTStudySliderWidget::setSelectedStudy(const QString& selectedStudy)
{
  Q_D(qSlicerLongPETCTStudySliderWidget);
  Q_ASSERT(d->LabelSelectedTimepoint);

  d->LabelDescription->setText(selectedStudy);
}

