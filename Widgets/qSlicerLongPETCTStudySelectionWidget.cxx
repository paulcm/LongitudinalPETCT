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


#include <ctkMessageBox.h>



#include <vtkSmartPointer.h>
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

  void deselectTableAll();
  void selectTableRow(int row, bool select);

  void clearTableAndCheckBoxes();

  void addRowToTableForStudy(vtkSmartPointer<vtkMRMLLongPETCTStudyNode> study);

  QList<QCheckBox*> ListStudyCheckBoxes;
  vtkSmartPointer<vtkMRMLLongPETCTReportNode> ReportNode;

  QTableWidgetItem* createEnabledSelectableTableWidgetItem(const QString& text);


};

// --------------------------------------------------------------------------
qSlicerLongPETCTStudySelectionWidgetPrivate
::qSlicerLongPETCTStudySelectionWidgetPrivate(
  qSlicerLongPETCTStudySelectionWidget& object)
  : q_ptr(&object), ReportNode(NULL)
{
}

// --------------------------------------------------------------------------
qSlicerLongPETCTStudySelectionWidgetPrivate
::~qSlicerLongPETCTStudySelectionWidgetPrivate()
{
}

// --------------------------------------------------------------------------
QTableWidgetItem* qSlicerLongPETCTStudySelectionWidgetPrivate::createEnabledSelectableTableWidgetItem(const QString& text)
{
  QTableWidgetItem* item = new QTableWidgetItem(text);
  item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

  return item;
}

// --------------------------------------------------------------------------
void
qSlicerLongPETCTStudySelectionWidgetPrivate::addRowToTableForStudy(vtkSmartPointer<vtkMRMLLongPETCTStudyNode> study)
{
  Q_Q(qSlicerLongPETCTStudySelectionWidget);
  Q_ASSERT(this->Table);

  QCheckBox* studyIDCheckBox = new QCheckBox(q);
  studyIDCheckBox->setChecked(study->GetSelected());

  QString studyID = study->GetAttribute("DICOM.StudyID");
  studyIDCheckBox->setText(studyID);

  QObject::connect(studyIDCheckBox,SIGNAL(toggled(bool)), q, SLOT(studyCheckBoxClicked(bool)));

  this->ListStudyCheckBoxes.append(studyIDCheckBox);

  QDate date = QDate::fromString(QString(study->GetAttribute("DICOM.StudyDate")).trimmed(),"yyyyMMdd");
  QTime time = QTime::fromString(QString(study->GetAttribute("DICOM.StudyTime")).trimmed().left(6),"hhmmss");

  QString dateStr = date.toString(Qt::SystemLocaleLongDate);
  QString timeStr = time.toString(Qt::ISODate);
  QString studyUID = study->GetAttribute("DICOM.StudyInstanceUID");

  int row = this->Table->rowCount();
  int col = 0;

  this->Table->insertRow(row);
  this->Table->setCellWidget(row, col++, studyIDCheckBox);
  this->Table->setItem(row, col++, this->createEnabledSelectableTableWidgetItem(dateStr));
  this->Table->setItem(row, col++, this->createEnabledSelectableTableWidgetItem(timeStr));
  this->Table->setItem(row, col++, this->createEnabledSelectableTableWidgetItem(studyUID));

}
// --------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidgetPrivate
::clearTableAndCheckBoxes()
{
  Q_Q(qSlicerLongPETCTStudySelectionWidget);
  Q_ASSERT(this->Table);

  // remove all checkboxes first, disconnect them and mark them for deletion
  while ( ! this->ListStudyCheckBoxes.isEmpty())
    {
      QCheckBox* lastCheckBoxInList = this->ListStudyCheckBoxes.last();
      QObject::disconnect(lastCheckBoxInList,SIGNAL(toggled(bool)), q, SLOT(studyCheckBoxClicked(bool)));
      this->ListStudyCheckBoxes.removeLast();
      lastCheckBoxInList->deleteLater();
    }

  // remove all rows from table
  while ( this->Table->rowCount() > 0)
    {
      this->Table->removeRow(this->Table->rowCount()-1);
    }
}


// --------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidgetPrivate
::deselectTableAll()
{
  Q_ASSERT(this->Table);

  QTableWidgetSelectionRange range(0,0,this->Table->rowCount()-1,this->Table->columnCount()-1);
  this->Table->setRangeSelected(range,false);
}

// --------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidgetPrivate
::selectTableRow(int row, bool select)
{
  Q_ASSERT(this->Table);

  this->deselectTableAll();

  QTableWidgetSelectionRange range(row,0,row,this->Table->columnCount()-1);
    this->Table->setRangeSelected(range,select);

}

// --------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidgetPrivate
::setupUi(qSlicerLongPETCTStudySelectionWidget* widget)
{
  Q_Q(qSlicerLongPETCTStudySelectionWidget);

  this->Ui_qSlicerLongPETCTStudySelectionWidget::setupUi(widget);

  this->WidgetButtonsPanel->setVisible(false);

  //this->SliderOpacityPow->setOrientation(Qt::Vertical);
  double opcValue = this->SpinBoxOpacityPow->value();
  double min = this->SpinBoxOpacityPow->minimum();
  double max = this->SpinBoxOpacityPow->maximum();
  double step = this->SpinBoxOpacityPow->singleStep();

  this->SliderOpacityPow->setValue(opcValue); //this->SpinBoxOpacityPow->value());
  this->SliderOpacityPow->setMinimum(min); //this->SpinBoxOpacityPow->minimum());
  this->SliderOpacityPow->setMaximum(max); //this->SpinBoxOpacityPow->minimum());
  this->SliderOpacityPow->setSingleStep(step); //this->SpinBoxOpacityPow->singleStep());

  QObject::connect(this->Table, SIGNAL(cellClicked(int,int)), q, SLOT(tableCellClicked(int)) );

  QObject::connect(this->ButtonVolumeRendering, SIGNAL(toggled(bool)), q, SIGNAL(volumeRenderingToggled(bool)) );
  //QObject::connect(this->ButtonGPURendering, SIGNAL(toggled(bool)), q, SIGNAL(gpuRenderingToggled(bool)) );
  QObject::connect(this->ButtonRockView, SIGNAL(toggled(bool)), q, SIGNAL(rockViewToggled(bool)) );

  QObject::connect(this->SpinBoxOpacityPow, SIGNAL(valueChanged(double)), this->SliderOpacityPow, SLOT(setValue(double)) );
  QObject::connect(this->SliderOpacityPow, SIGNAL(valueChanged(double)), this->SpinBoxOpacityPow, SLOT(setValue(double)) );
  QObject::connect(this->SpinBoxOpacityPow, SIGNAL(valueChanged(double)), q, SIGNAL(opacityPowChanged(double)) );

  QObject::connect(this->CheckBoxStudiesCentered, SIGNAL(toggled(bool)), q, SIGNAL(showStudiesCentered(bool)) );

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
::updateView()
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);

  d->clearTableAndCheckBoxes(); // clear table first

  if(d->ReportNode.GetPointer() == NULL)
    return;

  d->deselectTableAll();

  for(int i=0; i < d->ReportNode->GetStudiesCount(); ++i)
    {
      vtkSmartPointer<vtkMRMLLongPETCTStudyNode> study = d->ReportNode->GetStudy(i);
      d->addRowToTableForStudy(study);
    }
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidget::tableCellClicked(int row)
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  Q_ASSERT(d->Table);

  if(row >= 0 && row < d->ListStudyCheckBoxes.size() && row < d->Table->rowCount())
    {
      if(d->ListStudyCheckBoxes.at(row)->isChecked())
        {
          d->selectTableRow(row, true);
          emit studySelected(row);
        }
      else
        {
          static bool unseen = true;
          if(unseen)
            QMessageBox::information(NULL,"Longitudinal PET/CT Analysis","By clicking into a row the study is loaded into the view. This is only possible if the study has been checked into the workflow first.");

          d->deselectTableAll();
        }
    }
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidget::studyCheckBoxClicked(bool selected)
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  Q_ASSERT(d->Table);

  QCheckBox* sender =  qobject_cast<QCheckBox*>(QObject::sender());

  d->deselectTableAll();


  for(int i=0; i < d->ListStudyCheckBoxes.size(); ++i)
    {
      if(sender == d->ListStudyCheckBoxes.value(i))
        {
          if(selected)
            {
              //d->selectTableRow(i, true);
              emit studySelected(i);
            }
          else
            {
              //d->selectTableRow(i, false);
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
  d->selectTableRow(row, true);
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


//-----------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidget::setReportNode(vtkMRMLLongPETCTReportNode* reportNode)
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);

  qvtkReconnect(d->ReportNode.GetPointer(), reportNode, vtkCommand::ModifiedEvent, this, SLOT(updateView()) );
  d->ReportNode = reportNode;

  this->updateView();
}

//-----------------------------------------------------------------------------
vtkMRMLLongPETCTReportNode* qSlicerLongPETCTStudySelectionWidget::reportNode()
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);

  return d->ReportNode.GetPointer();
}

//-----------------------------------------------------------------------------
bool qSlicerLongPETCTStudySelectionWidget::centeredSelected()
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  Q_ASSERT(d->CheckBoxStudiesCentered);

  return d->CheckBoxStudiesCentered->isChecked();
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionWidget::setCenteredSelected(bool selected)
{
  Q_D(qSlicerLongPETCTStudySelectionWidget);
  Q_ASSERT(d->CheckBoxStudiesCentered);

  return d->CheckBoxStudiesCentered->setChecked(selected);
}


