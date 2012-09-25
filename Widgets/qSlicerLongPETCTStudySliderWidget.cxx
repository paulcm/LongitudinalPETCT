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
#include "ui_qSlicerLongPETCTStudySliderWidget.h"

#include <QDate>
#include <QTime>

#include <vtkMRMLLongPETCTReportNode.h>
#include <vtkMRMLLongPETCTStudyNode.h>

#include <QSlider>
#include <QMouseEvent>



//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongitudinalPETCT
class qSlicerLongPETCTStudySliderWidgetPrivate
  : public Ui_qSlicerLongPETCTStudySliderWidget
{
  Q_DECLARE_PUBLIC(qSlicerLongPETCTStudySliderWidget);
protected:
  qSlicerLongPETCTStudySliderWidget* const q_ptr;

public:
  qSlicerLongPETCTStudySliderWidgetPrivate(
  qSlicerLongPETCTStudySliderWidget& object);

  virtual ~qSlicerLongPETCTStudySliderWidgetPrivate();

  virtual void setupUi(qSlicerLongPETCTStudySliderWidget*);

  vtkSmartPointer<vtkMRMLLongPETCTReportNode> ReportNode;



  class qSlicerLongPETCTJumpSlider : public QSlider
  {
  public:
    qSlicerLongPETCTJumpSlider(QWidget* parent = 0) :
        QSlider(parent)
    {
      this->setOrientation(Qt::Horizontal);
      this->setTickPosition(QSlider::TicksBothSides);
      this->setMinimum(0);
      this->setMaximum(1);
      this->setPageStep(1);
      this->setSingleStep(1);
      this->setEnabled(false);
    }

  protected:

    int
    getNearestSliderValue(int position)
    {
      double min = static_cast<double>(minimum());
      double max = static_cast<double>(maximum());
      double pos = static_cast<double>(position);
      double wdt = static_cast<double>(width());

      int val = static_cast<int>(min + (max - min) * pos / wdt + 0.5);
      return val;
    }

    void
    mousePressEvent(QMouseEvent* event)
    {
      if (event->button() == Qt::LeftButton)
        {
          if (orientation() == Qt::Horizontal)
            setValue(this->getNearestSliderValue(event->x()));
          else
            QSlider::mousePressEvent(event);

          event->accept();
        }
    }

    void
    mouseReleaseEvent(QMouseEvent* event)
    {
      if (event->button() == Qt::LeftButton)
        {
          if (orientation() == Qt::Horizontal)
            setValue(this->getNearestSliderValue(event->x()));
          else
            QSlider::mousePressEvent(event);

          event->accept();
        }
    }
  };

  qSlicerLongPETCTJumpSlider* Slider;

};

// --------------------------------------------------------------------------
qSlicerLongPETCTStudySliderWidgetPrivate
::qSlicerLongPETCTStudySliderWidgetPrivate(
  qSlicerLongPETCTStudySliderWidget& object)
  : q_ptr(&object), ReportNode(NULL)
{
}

// --------------------------------------------------------------------------
qSlicerLongPETCTStudySliderWidgetPrivate
::~qSlicerLongPETCTStudySliderWidgetPrivate()
{
}


// --------------------------------------------------------------------------
void qSlicerLongPETCTStudySliderWidgetPrivate
::setupUi(qSlicerLongPETCTStudySliderWidget* widget)
{
  Q_Q(qSlicerLongPETCTStudySliderWidget);

  this->Ui_qSlicerLongPETCTStudySliderWidget::setupUi(widget);

  this->Slider = new qSlicerLongPETCTJumpSlider(widget);

  this->Layout->addWidget(this->Slider);

  QObject::connect( this->Slider, SIGNAL(valueChanged(int)), q, SIGNAL(sliderValueChanged(int)) );

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
::updateView()
{
  Q_D(qSlicerLongPETCTStudySliderWidget);
  Q_ASSERT(d->Slider);
  Q_ASSERT(d->LabelSelectedStudyDateTime);

  disconnect( d->Slider, SIGNAL(valueChanged(int)), this, SIGNAL(sliderValueChanged(int)) );

  d->LabelSelectedStudyDateTime->setText("");

  if(d->ReportNode == NULL)
    return;

  int studiesCount = d->ReportNode->GetSelectedStudiesCount();

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

  vtkMRMLLongPETCTStudyNode* study = d->ReportNode->GetUserSelectedStudy();

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

      d->LabelSelectedStudyDateTime->setText(text.join("   "));
    }

  int sliderVal = d->ReportNode->GetIndexOfSelectedStudy(study);

  if(sliderVal >= 0 && sliderVal <= d->Slider->maximum())
    d->Slider->setValue(sliderVal);

  connect( d->Slider, SIGNAL(valueChanged(int)), this, SIGNAL(sliderValueChanged(int)) );

}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTStudySliderWidget::setReportNode(vtkMRMLLongPETCTReportNode* reportNode)
{
  Q_D(qSlicerLongPETCTStudySliderWidget);

  qvtkReconnect(d->ReportNode.GetPointer(), reportNode, vtkCommand::ModifiedEvent, this, SLOT(updateView()) );
  d->ReportNode = reportNode;

  this->updateView();
}


//-----------------------------------------------------------------------------
vtkMRMLLongPETCTReportNode* qSlicerLongPETCTStudySliderWidget::reportNode()
{
  Q_D(qSlicerLongPETCTStudySliderWidget);

  return d->ReportNode.GetPointer();
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTStudySliderWidget::changeValue(int value)
{
  Q_D(qSlicerLongPETCTStudySliderWidget);
  Q_ASSERT(d->Slider);

  if(value >= d->Slider->minimum() && value <= d->Slider->maximum())
    d->Slider->setValue(value);
}







