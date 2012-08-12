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


#include <iostream>

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

  QStringList StudyDates;

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
  connect( this->Slider, SIGNAL(valueChanged(int)), widget, SLOT(setSelectedValue(int)) );

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
::updateSliderValues(vtkMRMLNode* node)
{
  Q_D(qSlicerLongPETCTStudySliderWidget);
  Q_ASSERT(d->StudyDates);
  Q_ASSERT(d->Slider);
  Q_ASSERT(d->LabelSelectedTimepoint);

  d->LabelSelectedTimepoint->setText("");

  vtkMRMLLongPETCTReportNode* selectedReportNode = vtkMRMLLongPETCTReportNode::SafeDownCast(node);

  if( ! selectedReportNode)
      return;

  int studiesCount = selectedReportNode->GetSelectedStudiesCount();
  d->StudyDates.clear();

  std::cout << "UPDATING STUDYDATES" << std::endl;
  for(int i=0; i < studiesCount; ++i)
    {
      vtkMRMLLongPETCTStudyNode* study = selectedReportNode->GetSelectedStudy(i);
      if(study != NULL)
        std::cout << "GOT SELECTED STUDY " << i << std::endl;
      else
        std::cout << "NO SELECTED STUDY " << i << std::endl;
      QDate date = QDate::fromString(QString(study->GetAttribute("DICOM.StudyDate")).trimmed(),"yyyyMMdd");
      QTime time = QTime::fromString(QString(study->GetAttribute("DICOM.StudyTime")).trimmed().left(6),"hhmmss");

      QStringList text;
      text << date.toString(Qt::SystemLocaleLongDate);
      text << time.toString(Qt::ISODate);

      d->StudyDates.append(text.join("\t"));

      std::cout << "APPENDED TO STUDYDATES: "<< text.join(" ").toStdString() << std::endl;
    }

  this->setSelectedValue(d->Slider->value());

  if(d->StudyDates.size() != studiesCount)
    return;

  if(studiesCount < 2)
    {
      disconnect( d->Slider, SIGNAL(valueChanged(int)), this, SLOT(setSelectedValue(int)) );
      d->Slider->setValue(0);
      connect( d->Slider, SIGNAL(valueChanged(int)), this, SLOT(setSelectedValue(int)) );

      d->Slider->setEnabled(false);
      d->Slider->setMaximum(1);
    }
  else
    {
      d->Slider->setEnabled(true);
      d->Slider->setMaximum(studiesCount-1);
    }

  this->setSelectedValue(d->Slider->value());
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTStudySliderWidget
::setSelectedValue(int value)
{
  Q_D(qSlicerLongPETCTStudySliderWidget);
  Q_ASSERT(d->LabelSelectedTimepoint);
  Q_ASSERT(d->StudyDates);
  Q_ASSERT(d->Slider);

  if(value < 0 || value > d->Slider->maximum() || value >= d->StudyDates.size())
    return;

  disconnect( d->Slider, SIGNAL(valueChanged(int)), this, SLOT(setSelectedValue(int)) );
  d->Slider->setValue(value);
  connect( d->Slider, SIGNAL(valueChanged(int)), this, SLOT(setSelectedValue(int)) );

  if(value < d->StudyDates.size())
    d->LabelSelectedTimepoint->setText(d->StudyDates.at(value));
  else
    d->LabelSelectedTimepoint->setText("");

  emit selectedValueChanged(value);
}


