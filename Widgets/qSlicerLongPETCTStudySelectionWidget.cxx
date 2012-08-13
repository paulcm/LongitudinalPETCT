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
#include <QFrame>
#include <QList>
#include <QCheckBox>

#include <QDate>
#include <QTime>

#include <vtkMRMLLongPETCTReportNode.h>
#include <vtkMRMLLongPETCTStudyNode.h>


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

  virtual ~qSlicerLongPETCTStudySelectionWidgetPrivate();
  virtual void setupUi(qSlicerLongPETCTStudySelectionWidget*);

  QVBoxLayout* Layout;

  QList<QCheckBox*> ListStudyCheckBoxes;

};

// --------------------------------------------------------------------------
qSlicerLongPETCTStudySelectionWidgetPrivate
::qSlicerLongPETCTStudySelectionWidgetPrivate(
  qSlicerLongPETCTStudySelectionWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
qSlicerLongPETCTStudySelectionWidgetPrivate
::~qSlicerLongPETCTStudySelectionWidgetPrivate()
{
  Q_Q(qSlicerLongPETCTStudySelectionWidget);
  for(int i=0; i < this->ListStudyCheckBoxes.size(); ++i)
    {
      this->ListStudyCheckBoxes.value(i)->deleteLater();
    }
}


// --------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidgetPrivate
::setupUi(qSlicerLongPETCTStudySelectionWidget* widget)
{
  Q_Q(qSlicerLongPETCTStudySelectionWidget);

  this->Layout = new QVBoxLayout(widget);
  this->Layout->setSpacing(8);
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
void qSlicerLongPETCTStudySelectionWidget
::update(vtkMRMLNode* node)
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  Q_ASSERT(d->ListStudyCheckBoxes);
  Q_ASSERT(d->Layout);

  vtkMRMLLongPETCTReportNode* selectedReportNode = vtkMRMLLongPETCTReportNode::SafeDownCast(node);

  if( ! selectedReportNode)
    return;

  // first clear current set comboboxes from list and remove them from layout
  while ( ! d->ListStudyCheckBoxes.isEmpty())
    {
      std::cout << "Deleting Checkbox" << std::endl;
      QCheckBox* lastCheckBoxInList = d->ListStudyCheckBoxes.last();

      disconnect(lastCheckBoxInList,SIGNAL(toggled(bool)), this, SLOT(studyCheckBoxClicked(bool)));

      d->Layout->removeWidget(lastCheckBoxInList);
      d->ListStudyCheckBoxes.removeLast();

      lastCheckBoxInList->deleteLater();
    }


  for(int i=0; i < selectedReportNode->GetStudiesCount(); ++i)
    {
      vtkMRMLLongPETCTStudyNode* study = selectedReportNode->GetStudy(i);
      QCheckBox* cB = new QCheckBox(); // has no parent but will be deleted by deconstructor
      cB->setChecked(study->IsSelected());

      QDate date = QDate::fromString(QString(study->GetAttribute("DICOM.StudyDate")).trimmed(),"yyyyMMdd");
      QTime time = QTime::fromString(QString(study->GetAttribute("DICOM.StudyTime")).trimmed().left(6),"hhmmss");

      QStringList cBText;
      cBText << "Study Date:" << date.toString(Qt::SystemLocaleLongDate) << "\t";
      cBText << "Time:" << time.toString(Qt::ISODate);

      cB->setText(cBText.join(" "));

      connect(cB,SIGNAL(toggled(bool)), this, SLOT(studyCheckBoxClicked(bool)));

      d->ListStudyCheckBoxes.append(cB);
      d->Layout->addWidget(cB);
    }
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidget::studyCheckBoxClicked(bool selected)
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  Q_ASSERT(d->ListStudyCheckBoxes);

  QCheckBox* sender =  qobject_cast<QCheckBox*>(QObject::sender());

  for(int i=0; i < d->ListStudyCheckBoxes.size(); ++i)
    {
      if(sender == d->ListStudyCheckBoxes.value(i))
        {
          if(selected)
            emit studySelected(i);
          else
            emit studyDeselected(i);

          break;
        }
    }
}


