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
#include "ui_qSlicerLongPETCTStudySelectionWidget.h"

#include <QCheckBox>
#include <QDate>
#include <QTime>

#include <QMessageBox>

#include <vtkMRMLLongPETCTReportNode.h>
#include <vtkMRMLLongPETCTStudyNode.h>


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongitudinalPETCT
class qSlicerLongPETCTStudySelectionWidgetPrivate
  : public Ui_qSlicerLongPETCTStudySelectionWidget
{
  Q_DECLARE_PUBLIC(qSlicerLongPETCTStudySelectionWidget);
protected:
  qSlicerLongPETCTStudySelectionWidget* const q_ptr;

public:
  qSlicerLongPETCTStudySelectionWidgetPrivate(
    qSlicerLongPETCTStudySelectionWidget& object);

  virtual ~qSlicerLongPETCTStudySelectionWidgetPrivate();
  virtual void setupUi(qSlicerLongPETCTStudySelectionWidget*);

  void DeselectTableAll();
  void SelectTableRow(int row, bool select);

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
::DeselectTableAll()
{
  Q_Q(qSlicerLongPETCTStudySelectionWidget);
  QTableWidgetSelectionRange range(0,0,this->Table->rowCount()-1,this->Table->columnCount()-1);
  this->Table->setRangeSelected(range,false);
}

// --------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidgetPrivate
::SelectTableRow(int row, bool select)
{
  Q_Q(qSlicerLongPETCTStudySelectionWidget);

  this->DeselectTableAll();

  if(row >= 0 && row < this->Table->rowCount())
    {
      QTableWidgetSelectionRange range(row,0,row,this->Table->columnCount()-1);
      this->Table->setRangeSelected(range,select);
    }

}

// --------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidgetPrivate
::setupUi(qSlicerLongPETCTStudySelectionWidget* widget)
{
  Q_Q(qSlicerLongPETCTStudySelectionWidget);

  this->Ui_qSlicerLongPETCTStudySelectionWidget::setupUi(widget);

  this->WidgetButtonsPanel->setVisible(false);

  QObject::connect(this->Table, SIGNAL(cellClicked(int,int)), widget, SLOT(tableCellClicked(int,int)) );

  QObject::connect(this->ButtonVolumeRendering, SIGNAL(toggled(bool)), widget, SIGNAL(volumeRenderingToggled(bool)) );
  //QObject::connect(this->ButtonGPURendering, SIGNAL(toggled(bool)), widget, SIGNAL(gpuRenderingToggled(bool)) );
  QObject::connect(this->ButtonRockView, SIGNAL(toggled(bool)), widget, SIGNAL(rockViewToggled(bool)) );
  QObject::connect(this->SpinBoxOpacityPow, SIGNAL(valueChanged(double)), widget, SIGNAL(opacityPowChanged(double)) );

  this->ButtonGPURendering->setVisible(false);
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

  vtkMRMLLongPETCTReportNode* selectedReportNode = vtkMRMLLongPETCTReportNode::SafeDownCast(node);

  if( ! selectedReportNode)
    return;

  // first clear current set comboboxes from list and remove them from layout
  while ( ! d->ListStudyCheckBoxes.isEmpty())
    {
      std::cout << "Deleting Checkbox" << std::endl;
      QCheckBox* lastCheckBoxInList = d->ListStudyCheckBoxes.last();

      disconnect(lastCheckBoxInList,SIGNAL(toggled(bool)), this, SLOT(studyCheckBoxClicked(bool)));

      d->ListStudyCheckBoxes.removeLast();

      lastCheckBoxInList->deleteLater();
    }

  while ( d->Table->rowCount() > 0)
    {
      d->Table->removeRow(d->Table->rowCount()-1);
    }


  for(int i=0; i < selectedReportNode->GetStudiesCount(); ++i)
    {
      vtkMRMLLongPETCTStudyNode* study = selectedReportNode->GetStudy(i);
      QCheckBox* cB = new QCheckBox(); // has no parent but will be deleted by deconstructor
      cB->setChecked(study->IsSelected());

      QString studyID = study->GetAttribute("DICOM.StudyID");
      cB->setText(studyID);

      connect(cB,SIGNAL(toggled(bool)), this, SLOT(studyCheckBoxClicked(bool)));

      d->ListStudyCheckBoxes.append(cB);

      QDate date = QDate::fromString(QString(study->GetAttribute("DICOM.StudyDate")).trimmed(),"yyyyMMdd");
      QTime time = QTime::fromString(QString(study->GetAttribute("DICOM.StudyTime")).trimmed().left(6),"hhmmss");

      QString dateStr = date.toString(Qt::SystemLocaleLongDate);
      QString timeStr = time.toString(Qt::ISODate);

      int currRow = d->Table->rowCount();

      QTableWidgetItem* dateItem = new QTableWidgetItem(dateStr);
      dateItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      QTableWidgetItem* timeItem = new QTableWidgetItem(timeStr);
      timeItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      QTableWidgetItem* uidItem = new QTableWidgetItem(study->GetAttribute("DICOM.StudyInstanceUID"));
      uidItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);


      d->Table->insertRow(currRow);
      d->Table->setCellWidget(currRow,0,cB);
      d->Table->setItem(currRow,1, dateItem);
      d->Table->setItem(currRow,2, timeItem);
      d->Table->setItem(currRow,3, uidItem);
    }
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidget::tableCellClicked(int row, int column)
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  Q_ASSERT(d->ListStudyCheckBoxes);
  Q_ASSERT(d->Table);

  d->DeselectTableAll();

  if(row >= 0 && row < d->ListStudyCheckBoxes.size() && row < d->Table->rowCount())
    {
      if(d->ListStudyCheckBoxes.at(row)->isChecked())
        {
          d->SelectTableRow(row, true);
          emit studySelected(row);
        }
      else
        {
          static bool unseen = true;
          if(unseen)
            QMessageBox::information(NULL,"Longitudinal PET/CT Analysis","By clicking into a row the study is loaded into the view. This is only possible if the study has been checked into the workflow first.");
        }
    }
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidget::studyCheckBoxClicked(bool selected)
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  Q_ASSERT(d->ListStudyCheckBoxes);
  Q_ASSERT(d->Table);

  QCheckBox* sender =  qobject_cast<QCheckBox*>(QObject::sender());

  d->DeselectTableAll();

  for(int i=0; i < d->ListStudyCheckBoxes.size(); ++i)
    {
      if(sender == d->ListStudyCheckBoxes.value(i))
        {
          if(selected)
            {
              d->SelectTableRow(i, true);
              emit studySelected(i);
            }
          else
            {
              d->SelectTableRow(i, false);
              emit studyDeselected(i);
            }
          break;
        }
    }
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidget::selectStudyInRow(int row)
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  d->SelectTableRow(row, true);
}

//-----------------------------------------------------------------------------
bool qSlicerLongPETCTStudySelectionWidget::volumeRendering()
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  Q_ASSERT(d->ButtonVolumeRendering);

  return d->ButtonVolumeRendering->isChecked();
}

//-----------------------------------------------------------------------------
bool qSlicerLongPETCTStudySelectionWidget::gpuRendering()
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  Q_ASSERT(d->ButtonGPURendering);

  return d->ButtonGPURendering->isChecked();
}

//-----------------------------------------------------------------------------
bool qSlicerLongPETCTStudySelectionWidget::rockView()
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  Q_ASSERT(d->ButtonRockView);

  return d->ButtonRockView->isChecked();
}

//-----------------------------------------------------------------------------
double qSlicerLongPETCTStudySelectionWidget::opacityPow()
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  Q_ASSERT(d->SpinBoxOpacityPow);

  return d->SpinBoxOpacityPow->value();
}


//-----------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidget::setVolumeRendering(bool checked)
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  Q_ASSERT(d->ButtonVolumeRendering);

  d->ButtonVolumeRendering->setChecked(checked);
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidget::setGPURendering(bool checked)
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  Q_ASSERT(d->ButtonGPURendering);

  d->ButtonGPURendering->setChecked(checked);
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidget::setRockView(bool checked)
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  Q_ASSERT(d->ButtonRockView);

  d->ButtonRockView->setChecked(checked);
}


//-----------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidget::setOpacityPow(double opacityPow)
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  Q_ASSERT(d->SpinBoxOpacityPow);

  if(opacityPow >= d->SpinBoxOpacityPow->minimum() && opacityPow <= d->SpinBoxOpacityPow->maximum())
    d->SpinBoxOpacityPow->setValue(opacityPow);
}



