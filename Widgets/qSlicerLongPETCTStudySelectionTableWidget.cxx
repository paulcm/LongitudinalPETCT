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

// LongPETCTReportTable Widgets includes
#include "qSlicerLongPETCTStudySelectionTableWidget.h"
#include "ui_qSlicerLongPETCTStudySelectionTableWidget.h"


#include <vtkMRMLLongPETCTReportNode.h>
#include <vtkMRMLLongPETCTStudyNode.h>
#include <vtkMRMLLongPETCTSegmentationNode.h>

#include <vtkMRMLColorTableNode.h>

#include <QBrush>
#include <QDate>
#include <QTime>
#include <QHeaderView>
#include <ctkCheckBox.h>


#include "qSlicerLongPETCTColorSelectionDialog.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongitudinalPETCT
class qSlicerLongPETCTStudySelectionTableWidgetPrivate
  : public Ui_qSlicerLongPETCTStudySelectionTableWidget
{
  Q_DECLARE_PUBLIC(qSlicerLongPETCTStudySelectionTableWidget);
protected:
  qSlicerLongPETCTStudySelectionTableWidget* const q_ptr;

public:
  qSlicerLongPETCTStudySelectionTableWidgetPrivate(
    qSlicerLongPETCTStudySelectionTableWidget& object);

  virtual ~qSlicerLongPETCTStudySelectionTableWidgetPrivate();
  virtual void setupUi(qSlicerLongPETCTStudySelectionTableWidget*);

  QTableWidgetItem* createEnabledSelectableTableWidgetItem(const QString& text);

  qSlicerLongPETCTStudySelectionTableWidget::TableMode Mode;

  QDate dateFromDICOMStr(const QString& dicomDateStr);
  QTime timeFromDICOMStr(const QString& dicomTimeStr);

};

// --------------------------------------------------------------------------
qSlicerLongPETCTStudySelectionTableWidgetPrivate
::qSlicerLongPETCTStudySelectionTableWidgetPrivate(
  qSlicerLongPETCTStudySelectionTableWidget& object)
  : q_ptr(&object), Mode(qSlicerLongPETCTStudySelectionTableWidget::WorkflowSelection)
{
}

// --------------------------------------------------------------------------
qSlicerLongPETCTStudySelectionTableWidgetPrivate
::~qSlicerLongPETCTStudySelectionTableWidgetPrivate()
{
}

// --------------------------------------------------------------------------
QDate qSlicerLongPETCTStudySelectionTableWidgetPrivate
::dateFromDICOMStr(const QString& dicomDateStr)
{
  return QDate::fromString(dicomDateStr.trimmed(),"yyyyMMdd");
}

// --------------------------------------------------------------------------
QTime qSlicerLongPETCTStudySelectionTableWidgetPrivate
::timeFromDICOMStr(const QString& dicomTimeStr)
{
  return QTime::fromString(dicomTimeStr.trimmed().left(6),"hhmmss");
}


// --------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionTableWidgetPrivate
::setupUi(qSlicerLongPETCTStudySelectionTableWidget* widget)
{
  Q_Q(qSlicerLongPETCTStudySelectionTableWidget);

  this->Ui_qSlicerLongPETCTStudySelectionTableWidget::setupUi(widget);


  for(int i=0; i < this->Table->columnCount(); ++i)
    {
      QTableWidgetItem* tempHorizontalHeader = this->Table->horizontalHeaderItem(i);
      if(tempHorizontalHeader)
        tempHorizontalHeader->setToolTip(tempHorizontalHeader->text());
    }
  QObject::connect(this->Table, SIGNAL(itemChanged(QTableWidgetItem*)), q, SLOT(tableItemChanged(QTableWidgetItem*)) );
  QObject::connect(this->Table, SIGNAL(cellClicked(int,int)), q, SLOT(tableCellClicked(int,int)) );

}

QTableWidgetItem* qSlicerLongPETCTStudySelectionTableWidgetPrivate::createEnabledSelectableTableWidgetItem(const QString& text)
{
  QTableWidgetItem* item = new QTableWidgetItem(text);
  item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  item->setToolTip(text);

  return item;
}


//-----------------------------------------------------------------------------
// qSlicerLongPETCTStudySelectionTableWidget methods

//-----------------------------------------------------------------------------
qSlicerLongPETCTStudySelectionTableWidget
::qSlicerLongPETCTStudySelectionTableWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerLongPETCTStudySelectionTableWidgetPrivate(*this) )
{
  Q_D(qSlicerLongPETCTStudySelectionTableWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerLongPETCTStudySelectionTableWidget
::~qSlicerLongPETCTStudySelectionTableWidget()
{
}

//void deselectTableAll();
//void selectTableRow(int row, bool select);
//void clearTableAndCheckBoxes(

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
void
qSlicerLongPETCTStudySelectionTableWidget::addStudyToTable(vtkMRMLLongPETCTStudyNode* study, bool disabled)
{
  Q_D(qSlicerLongPETCTStudySelectionTableWidget);
  Q_ASSERT(d->Table);

  QObject::disconnect(d->Table, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(tableItemChanged(QTableWidgetItem*)) );

  QString studyID = study->GetAttribute("DICOM.StudyID");
  QTableWidgetItem* checkboxItem = new QTableWidgetItem(studyID);

  if(disabled)
    {
      checkboxItem->setFlags(Qt::NoItemFlags);

      if(d->Mode == WorkflowSelection)
        checkboxItem->setToolTip("This study can not be deselected while it is used for segmentation by one of the Findings.");
      if(d->Mode == AnalysisSelection)
        checkboxItem->setToolTip("This study can not be deselected because at least one study must be selected for the analysis view.");
    }
  else
    {
      checkboxItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      checkboxItem->setToolTip(NULL);
    }

  if( (d->Mode == WorkflowSelection && study->GetSelectedForSegmentation()) || (d->Mode == AnalysisSelection && study->GetSelectedForAnalysis()))
    checkboxItem->setCheckState(Qt::Checked);
  else
    checkboxItem->setCheckState(Qt::Unchecked);


  QString dateStr = d->dateFromDICOMStr(study->GetAttribute("DICOM.StudyDate")).toString(Qt::SystemLocaleLongDate);
  QString timeStr = d->timeFromDICOMStr(study->GetAttribute("DICOM.StudyTime")).toString(Qt::ISODate);
  QString patientWeightStr = QString(study->GetAttribute("DICOM.PatientWeight")).trimmed();
  if( ! patientWeightStr.isEmpty())
    patientWeightStr.append(" kg");
  QString radiopharmaconStartTimeStr = d->timeFromDICOMStr(study->GetAttribute("DICOM.RadioPharmaconStartTime")).toString(Qt::ISODate);
  QString seriesTimeStr = d->timeFromDICOMStr(study->GetAttribute("DICOM.SeriesTime")).toString(Qt::ISODate);
  QString radionuclideHalfLifeStr = QString(study->GetAttribute("DICOM.RadionuclideHalfLife")).trimmed();
  if( ! radionuclideHalfLifeStr.isEmpty())
    radionuclideHalfLifeStr.append(" s");
  QString decayCorrectionStr = study->GetAttribute("DICOM.DecayCorrection");
  QString decayFactorStr = study->GetAttribute("DICOM.DecayFactor");
  QString studyUIDStr = study->GetAttribute("DICOM.StudyInstanceUID");


  int row = d->Table->rowCount();
  int col = 0;

  d->Table->insertRow(row);
  d->Table->setItem(row, col++, checkboxItem);
  d->Table->setItem(row, col++, d->createEnabledSelectableTableWidgetItem(dateStr));
  d->Table->setItem(row, col++, d->createEnabledSelectableTableWidgetItem(timeStr));
  d->Table->setItem(row, col++, d->createEnabledSelectableTableWidgetItem(patientWeightStr.trimmed()));
  d->Table->setItem(row, col++, d->createEnabledSelectableTableWidgetItem(radiopharmaconStartTimeStr));
  d->Table->setItem(row, col++, d->createEnabledSelectableTableWidgetItem(seriesTimeStr));
  d->Table->setItem(row, col++, d->createEnabledSelectableTableWidgetItem(radionuclideHalfLifeStr.trimmed()));
  d->Table->setItem(row, col++, d->createEnabledSelectableTableWidgetItem(decayCorrectionStr.trimmed()));
  d->Table->setItem(row, col++, d->createEnabledSelectableTableWidgetItem(decayFactorStr.trimmed()));
  d->Table->setItem(row, col++, d->createEnabledSelectableTableWidgetItem(studyUIDStr.trimmed()));

  QObject::connect(d->Table, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(tableItemChanged(QTableWidgetItem*)) );
}

void qSlicerLongPETCTStudySelectionTableWidget::setTableMode(TableMode mode)
{
  Q_D(qSlicerLongPETCTStudySelectionTableWidget);

  if(mode)
    d->Mode = mode;
}

qSlicerLongPETCTStudySelectionTableWidget::TableMode qSlicerLongPETCTStudySelectionTableWidget::tableMode()
{
  Q_D(qSlicerLongPETCTStudySelectionTableWidget);

  return d->Mode;
}

// --------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionTableWidget
::deselectTableAll()
{
  Q_D(qSlicerLongPETCTStudySelectionTableWidget);
  Q_ASSERT(d->Table);

  QTableWidgetSelectionRange range(0,0,d->Table->rowCount()-1,d->Table->columnCount()-1);
  d->Table->setRangeSelected(range,false);

}

// --------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionTableWidget
::selectTableRow(int row, bool select)
{
  Q_D(qSlicerLongPETCTStudySelectionTableWidget);
  Q_ASSERT(d->Table);

  this->deselectTableAll();

  QTableWidgetSelectionRange range(row,0,row,d->Table->columnCount()-1);

  d->Table->setRangeSelected(range,select);

}

// --------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionTableWidget
::clearRows()
{
  Q_D(qSlicerLongPETCTStudySelectionTableWidget);
  Q_ASSERT(d->Table);

  QObject::disconnect(d->Table, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(tableItemChanged(QTableWidgetItem*)) );

  // remove all rows from table
  while ( d->Table->rowCount() > 0)
    {
      d->Table->removeRow(d->Table->rowCount()-1);
    }

  QObject::connect(d->Table, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(tableItemChanged(QTableWidgetItem*)) );

}


//-----------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionTableWidget
::tableItemChanged(QTableWidgetItem* item)
{
  Q_D(qSlicerLongPETCTStudySelectionTableWidget);
  Q_ASSERT(d->Table);

  int index = 0;

  for(; index < d->Table->rowCount(); ++index)
    {
      if(d->Table->item(index,0) == item)
        break;
    }

  if(item->checkState() == Qt::Checked)
    emit studySelectionChanged(index, true);

  else if (item->checkState() == Qt::Unchecked)
    emit studySelectionChanged(index, false);

}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTStudySelectionTableWidget::tableCellClicked(int row, int col)
{
  Q_D(qSlicerLongPETCTStudySelectionTableWidget);
  Q_ASSERT(d->Table);

  if(row >= 0 && row < d->Table->rowCount())
    {
      if(d->Table->item(row,0)->checkState() == Qt::Checked)
        {
          this->selectTableRow(row, true);

          if(col > 0)
            emit studySelectionChanged(row, true);
        }
      else
        {
          //static bool unseen = true;
          //if(unseen)
            //QMessageBox::information(NULL,"Longitudinal PET/CT Analysis","By clicking into a row the study is loaded into the view. This is only possible if the study has been checked into the workflow first.");

          this->deselectTableAll();
        }
    }
}


