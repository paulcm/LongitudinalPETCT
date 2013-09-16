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

// LongitudinalPETCTReportTable Widgets includes
#include "qMRMLLongitudinalPETCTStudyTableWidget.h"
#include "ui_qMRMLLongitudinalPETCTStudyTableWidget.h"


#include <vtkMRMLLongitudinalPETCTReportNode.h>
#include <vtkMRMLLongitudinalPETCTStudyNode.h>
#include <vtkMRMLLongitudinalPETCTSegmentationNode.h>

#include <vtkMRMLColorTableNode.h>

#include <QBrush>
#include <QDate>
#include <QTime>
#include <QHeaderView>
#include <ctkCheckBox.h>


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongitudinalitudinalPETCT
class qMRMLLongitudinalPETCTStudyTableWidgetPrivate
  : public Ui_qMRMLLongitudinalPETCTStudyTableWidget
{
  Q_DECLARE_PUBLIC(qMRMLLongitudinalPETCTStudyTableWidget);
protected:
  qMRMLLongitudinalPETCTStudyTableWidget* const q_ptr;

public:
  qMRMLLongitudinalPETCTStudyTableWidgetPrivate(
    qMRMLLongitudinalPETCTStudyTableWidget& object);

  virtual ~qMRMLLongitudinalPETCTStudyTableWidgetPrivate();
  virtual void setupUi(qMRMLLongitudinalPETCTStudyTableWidget*);

  QTableWidgetItem* createEnabledTableWidgetItem(const QString& text);

  qMRMLLongitudinalPETCTStudyTableWidget::TableMode Mode;

  QDate dateFromDICOMStr(const QString& dicomDateStr);
  QTime timeFromDICOMStr(const QString& dicomTimeStr);

  void addStudyToTable(vtkMRMLLongitudinalPETCTStudyNode* study, bool disabled);
  void clearTableRows();


};

// --------------------------------------------------------------------------
qMRMLLongitudinalPETCTStudyTableWidgetPrivate
::qMRMLLongitudinalPETCTStudyTableWidgetPrivate(
  qMRMLLongitudinalPETCTStudyTableWidget& object)
  : q_ptr(&object), Mode(qMRMLLongitudinalPETCTStudyTableWidget::SegmentationSelection)
{
}

// --------------------------------------------------------------------------
qMRMLLongitudinalPETCTStudyTableWidgetPrivate
::~qMRMLLongitudinalPETCTStudyTableWidgetPrivate()
{
}

// --------------------------------------------------------------------------
QDate qMRMLLongitudinalPETCTStudyTableWidgetPrivate
::dateFromDICOMStr(const QString& dicomDateStr)
{
  return QDate::fromString(dicomDateStr.trimmed(),"yyyyMMdd");
}

// --------------------------------------------------------------------------
QTime qMRMLLongitudinalPETCTStudyTableWidgetPrivate
::timeFromDICOMStr(const QString& dicomTimeStr)
{
  return QTime::fromString(dicomTimeStr.trimmed().left(6),"hhmmss");
}


// --------------------------------------------------------------------------
void qMRMLLongitudinalPETCTStudyTableWidgetPrivate
::setupUi(qMRMLLongitudinalPETCTStudyTableWidget* widget)
{
  Q_Q(qMRMLLongitudinalPETCTStudyTableWidget);

  this->Ui_qMRMLLongitudinalPETCTStudyTableWidget::setupUi(widget);


  for(int i=0; i < this->Table->columnCount(); ++i)
    {
      QTableWidgetItem* tempHorizontalHeader = this->Table->horizontalHeaderItem(i);
      if(tempHorizontalHeader)
        tempHorizontalHeader->setToolTip(tempHorizontalHeader->text());
    }
  QObject::connect(this->Table, SIGNAL(itemChanged(QTableWidgetItem*)), q, SLOT(tableItemChanged(QTableWidgetItem*)) );

}

// --------------------------------------------------------------------------
QTableWidgetItem* qMRMLLongitudinalPETCTStudyTableWidgetPrivate::createEnabledTableWidgetItem(const QString& text)
{
  QTableWidgetItem* item = new QTableWidgetItem(text);
  item->setFlags(Qt::ItemIsEnabled);
  item->setToolTip(text);

  return item;
}

// --------------------------------------------------------------------------
void qMRMLLongitudinalPETCTStudyTableWidgetPrivate
::clearTableRows()
{
  Q_Q(qMRMLLongitudinalPETCTStudyTableWidget);

  QObject::disconnect(this->Table, SIGNAL(itemChanged(QTableWidgetItem*)), q, SLOT(tableItemChanged(QTableWidgetItem*)) );

  // remove all rows from table
  while ( this->Table->rowCount() > 0)
    {
      this->Table->removeRow(this->Table->rowCount()-1);
    }

  QObject::connect(this->Table, SIGNAL(itemChanged(QTableWidgetItem*)), q, SLOT(tableItemChanged(QTableWidgetItem*)) );

}


// --------------------------------------------------------------------------
void
qMRMLLongitudinalPETCTStudyTableWidgetPrivate::addStudyToTable(vtkMRMLLongitudinalPETCTStudyNode* study, bool disabled)
{
  Q_Q(qMRMLLongitudinalPETCTStudyTableWidget);

  QObject::disconnect(this->Table, SIGNAL(itemChanged(QTableWidgetItem*)), q, SLOT(tableItemChanged(QTableWidgetItem*)) );

  QString studyID = study->GetAttribute("DICOM.StudyID");
  QTableWidgetItem* checkboxItem = new QTableWidgetItem(studyID);

  if(disabled)
    {
      checkboxItem->setFlags(Qt::NoItemFlags);

      if(this->Mode == qMRMLLongitudinalPETCTStudyTableWidget::SegmentationSelection)
        checkboxItem->setToolTip("This study can not be deselected while it is used for segmentation by one of the Findings.");
      if(this->Mode == qMRMLLongitudinalPETCTStudyTableWidget::AnalysisSelection)
        checkboxItem->setToolTip("This study can not be deselected because at least one study must be selected for the analysis view.");
    }
  else
    {
      checkboxItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
      checkboxItem->setToolTip(NULL);
    }

  if( (this->Mode == qMRMLLongitudinalPETCTStudyTableWidget::SegmentationSelection && study->GetSelectedForSegmentation()) || (this->Mode == qMRMLLongitudinalPETCTStudyTableWidget::AnalysisSelection && study->GetSelectedForAnalysis()))
    checkboxItem->setCheckState(Qt::Checked);
  else
    checkboxItem->setCheckState(Qt::Unchecked);



  QString dateStr = this->dateFromDICOMStr(study->GetAttribute("DICOM.StudyDate")).toString(Qt::DefaultLocaleLongDate);
  QString timeStr = this->timeFromDICOMStr(study->GetAttribute("DICOM.StudyTime")).toString(Qt::ISODate);
  QString patientWeightStr = QString(study->GetAttribute("DICOM.PatientWeight")).trimmed();
  if( ! patientWeightStr.isEmpty())
    patientWeightStr.append(" kg");
  QString radiopharmaconStartTimeStr = this->timeFromDICOMStr(study->GetAttribute("DICOM.RadioPharmaconStartTime")).toString(Qt::ISODate);
  QString petSeriesTimeStr = this->timeFromDICOMStr(study->GetAttribute("DICOM.PETSeriesTime")).toString(Qt::ISODate);
  QString ctSeriesTimeStr = this->timeFromDICOMStr(study->GetAttribute("DICOM.CTSeriesTime")).toString(Qt::ISODate);
  QString radionuclideHalfLifeStr = QString(study->GetAttribute("DICOM.RadionuclideHalfLife")).trimmed();
  if( ! radionuclideHalfLifeStr.isEmpty())
    radionuclideHalfLifeStr.append(" s");
  QString decayCorrectionStr = study->GetAttribute("DICOM.DecayCorrection");
  QString decayFactorStr = study->GetAttribute("DICOM.DecayFactor");
  QString studyUIDStr = study->GetAttribute("DICOM.StudyInstanceUID");


  int row = this->Table->rowCount();
  int col = 0;

  this->Table->insertRow(row);
  this->Table->setItem(row, col++, checkboxItem);
  this->Table->setItem(row, col++, this->createEnabledTableWidgetItem(dateStr));
  this->Table->setItem(row, col++, this->createEnabledTableWidgetItem(timeStr));
  this->Table->setItem(row, col++, this->createEnabledTableWidgetItem(patientWeightStr.trimmed()));
  this->Table->setItem(row, col++, this->createEnabledTableWidgetItem(radiopharmaconStartTimeStr));
  this->Table->setItem(row, col++, this->createEnabledTableWidgetItem(ctSeriesTimeStr));
  this->Table->setItem(row, col++, this->createEnabledTableWidgetItem(petSeriesTimeStr));
  this->Table->setItem(row, col++, this->createEnabledTableWidgetItem(radionuclideHalfLifeStr.trimmed()));
  this->Table->setItem(row, col++, this->createEnabledTableWidgetItem(decayCorrectionStr.trimmed()));
  this->Table->setItem(row, col++, this->createEnabledTableWidgetItem(decayFactorStr.trimmed()));
  this->Table->setItem(row, col++, this->createEnabledTableWidgetItem(studyUIDStr.trimmed()));

  QObject::connect(this->Table, SIGNAL(itemChanged(QTableWidgetItem*)), q, SLOT(tableItemChanged(QTableWidgetItem*)) );
}


//-----------------------------------------------------------------------------
// qMRMLLongitudinalPETCTStudyTableWidget methods

//-----------------------------------------------------------------------------
qMRMLLongitudinalPETCTStudyTableWidget
::qMRMLLongitudinalPETCTStudyTableWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qMRMLLongitudinalPETCTStudyTableWidgetPrivate(*this) )
{
  Q_D(qMRMLLongitudinalPETCTStudyTableWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qMRMLLongitudinalPETCTStudyTableWidget
::~qMRMLLongitudinalPETCTStudyTableWidget()
{
}



void qMRMLLongitudinalPETCTStudyTableWidget::setTableMode(TableMode mode)
{
  Q_D(qMRMLLongitudinalPETCTStudyTableWidget);

  if(mode)
    d->Mode = mode;
}

qMRMLLongitudinalPETCTStudyTableWidget::TableMode qMRMLLongitudinalPETCTStudyTableWidget::tableMode()
{
  Q_D(qMRMLLongitudinalPETCTStudyTableWidget);

  return d->Mode;
}


//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTStudyTableWidget
::tableItemChanged(QTableWidgetItem* item)
{
  Q_D(qMRMLLongitudinalPETCTStudyTableWidget);
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
void
qMRMLLongitudinalPETCTStudyTableWidget::updateWidgetFromMRML()
{
  Q_D (qMRMLLongitudinalPETCTStudyTableWidget);
  Q_ASSERT(d->Table);

  d->clearTableRows();

  vtkMRMLLongitudinalPETCTReportNode* report = this->reportNode();

  if (!report)
    return;

  bool disabled = false;
  vtkMRMLLongitudinalPETCTStudyNode* study = NULL;

  if (d->Mode == SegmentationSelection)
    {
      for (int i = 0; i < report->GetNumberOfStudyNodeIDs(); ++i)
        {
          study = report->GetStudy(i);
          if (!study)
            continue;
          disabled = report->IsStudyInUse(study);
          d->addStudyToTable(study, disabled);

        }
    }
  else if (d->Mode == AnalysisSelection)
    {
      bool lastSelected =
          report->GetNumberOfSelectedStudiesSelectedForAnalysis() == 1;

      for (int i = 0; i < report->GetNumberOfStudyNodeIDs(); ++i)
        {
          study = report->GetSelectedStudy(i);
          if (!study)
            continue;
          disabled = study->GetSelectedForAnalysis() && lastSelected;
          d->addStudyToTable(study, disabled);
        }
    }
}



