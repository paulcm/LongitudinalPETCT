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
#include <QDate>
#include <QTime>

#include <vtkMRMLLongPETCTReportNode.h>
#include <vtkMRMLLongPETCTStudyNode.h>


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
  QFont font = this->LabelDescription->font();
  font.setBold(true);
  this->LabelDescription->setFont(font);

  this->LabelSelectedTimepoint = new QLabel(widget);
  this->LabelSelectedTimepoint->setAlignment(Qt::AlignLeft);

  this->Slider = new QSlider(Qt::Horizontal,widget);
  this->Slider->setMinimum(0);
  this->Slider->setMaximum(1);
  this->Slider->setValue(0);
  this->Slider->setTickPosition(QSlider::TicksBothSides);
  this->Slider->setSingleStep(1);
  this->Slider->setPageStep(1);
  this->Slider->setEnabled(false);
  connect( this->Slider, SIGNAL(valueChanged(int)), widget, SIGNAL(sliderValueChanged(int)) );

  this->Layout->addWidget(this->LabelDescription,0,0,1,1);
  this->Layout->addWidget(this->LabelSelectedTimepoint,0,1,1,1);
  this->Layout->addWidget(this->Slider,1,0,1,2);

  this->Layout->setColumnStretch(0,0);
  this->Layout->setColumnStretch(1,2);

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



//-----------------------------------------------------------------------------
void qSlicerLongPETCTStudySliderWidget
::update(vtkMRMLNode* node)
{
  Q_D(qSlicerLongPETCTStudySliderWidget);
  Q_ASSERT(d->Slider);
  Q_ASSERT(d->LabelSelectedTimepoint);

  disconnect( d->Slider, SIGNAL(valueChanged(int)), this, SIGNAL(sliderValueChanged(int)) );

  d->LabelSelectedTimepoint->setText("");

  vtkMRMLLongPETCTReportNode* selectedReportNode = vtkMRMLLongPETCTReportNode::SafeDownCast(node);

  if( selectedReportNode == NULL )
      return;

  int studiesCount = selectedReportNode->GetSelectedStudiesCount();

  if(studiesCount < 2)
    {
      d->Slider->setValue(0);
      d->Slider->setEnabled(false);
      d->Slider->setMaximum(1);
    }
  else
    {
      d->Slider->setEnabled(true);
      d->Slider->setMaximum(studiesCount-1);
    }

  vtkMRMLLongPETCTStudyNode* study = selectedReportNode->GetUserSelectedStudy();

  if (study != NULL)
    {
      QDate date = QDate::fromString(
          QString(study->GetAttribute("DICOM.StudyDate")).trimmed(),
          "yyyyMMdd");
      QTime time = QTime::fromString(
          QString(study->GetAttribute("DICOM.StudyTime")).trimmed().left(6),
          "hhmmss");

      QStringList text;
      text << date.toString(Qt::SystemLocaleLongDate);
      text << time.toString(Qt::ISODate);

      d->LabelSelectedTimepoint->setText(text.join("   "));
    }

  int sliderVal = selectedReportNode->GetIndexOfSelectedStudy(study);

  if(sliderVal >= 0 && sliderVal <= d->Slider->maximum())
    d->Slider->setValue(sliderVal);

  connect( d->Slider, SIGNAL(valueChanged(int)), this, SIGNAL(sliderValueChanged(int)) );

}





