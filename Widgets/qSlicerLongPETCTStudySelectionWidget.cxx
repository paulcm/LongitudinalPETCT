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

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

#include <QLabel>
#include <QFrame>
#include <QList>
#include <QCheckBox>
#include <QTableWidget>

#include <QDate>
#include <QTime>

#include <ctkExpandButton.h>

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

  void DeselectTableAll();
  void SelectTableRow(int row, bool select);

  QHBoxLayout* Layout;
  QTableWidget* Table;

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

  this->Layout = new QHBoxLayout(widget);
  this->Layout->setSpacing(5);

  this->Table = new QTableWidget(widget);
  this->Table->setRowCount(0);
  this->Table->setColumnCount(4);
  //this->Table->setEditTriggers(QAbstractItemView::NoEditTriggers);

  this->Table->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

  connect(this->Table, SIGNAL(cellClicked(int,int)), widget, SLOT(tableCellClicked(int,int)) );

  this->Layout->addWidget(this->Table);

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

      d->ListStudyCheckBoxes.removeLast();

      lastCheckBoxInList->deleteLater();
    }

  d->Table->clear();

  while ( d->Table->rowCount() > 0)
    {
      d->Table->removeRow(d->Table->rowCount()-1);
    }

  d->Table->setHorizontalHeaderItem(0,new QTableWidgetItem("Name"));
  d->Table->setHorizontalHeaderItem(1,new QTableWidgetItem("Date"));
  d->Table->setHorizontalHeaderItem(2,new QTableWidgetItem("Time"));
  d->Table->setHorizontalHeaderItem(3,new QTableWidgetItem("Instance UID"));

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

  std::cout << "TRYING TO SELECT ROW " << row << std::endl;
  if(row >= 0 && row < d->ListStudyCheckBoxes.size() && row < d->Table->rowCount())
    {
      if(d->ListStudyCheckBoxes.at(row)->isChecked())
        {
          std::cout << "SELECTING ROW " << row << std::endl;
          d->SelectTableRow(row, true);
          emit studySelected(row);
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


