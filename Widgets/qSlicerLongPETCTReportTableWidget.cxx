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
#include "qSlicerLongPETCTReportTableWidget.h"


#include <vtkMRMLLongPETCTReportNode.h>
#include <vtkMRMLLongPETCTStudyNode.h>

#include <vtkMRMLColorTableNode.h>

#include <QBrush>
#include <QDate>
#include <QHeaderView>
#include <QLabel>

#include "qSlicerLongPETCTColorSelectionDialog.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongitudinalPETCT
class qSlicerLongPETCTReportTableWidgetPrivate
  : public QTableWidget
{
  Q_DECLARE_PUBLIC(qSlicerLongPETCTReportTableWidget);
protected:
  qSlicerLongPETCTReportTableWidget* const q_ptr;

public:
  qSlicerLongPETCTReportTableWidgetPrivate(
    qSlicerLongPETCTReportTableWidget& object);

  virtual ~qSlicerLongPETCTReportTableWidgetPrivate();
  virtual void setupUi(qSlicerLongPETCTReportTableWidget*);

  vtkSmartPointer<vtkMRMLLongPETCTReportNode> ReportNode;

};

// --------------------------------------------------------------------------
qSlicerLongPETCTReportTableWidgetPrivate
::qSlicerLongPETCTReportTableWidgetPrivate(
  qSlicerLongPETCTReportTableWidget& object)
  : q_ptr(&object), ReportNode(NULL)
{
}

// --------------------------------------------------------------------------
qSlicerLongPETCTReportTableWidgetPrivate
::~qSlicerLongPETCTReportTableWidgetPrivate()
{
}


// --------------------------------------------------------------------------
void qSlicerLongPETCTReportTableWidgetPrivate
::setupUi(qSlicerLongPETCTReportTableWidget* widget)
{
  Q_Q(qSlicerLongPETCTReportTableWidget);

  widget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
  widget->setSelectionMode(QAbstractItemView::NoSelection);

  QObject::connect(q, SIGNAL(cellClicked(int,int)), q,SLOT(segmentationCellClicked(int,int)));

}

//-----------------------------------------------------------------------------
// qSlicerLongPETCTReportTableWidget methods

//-----------------------------------------------------------------------------
qSlicerLongPETCTReportTableWidget
::qSlicerLongPETCTReportTableWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerLongPETCTReportTableWidgetPrivate(*this) )
{
  Q_D(qSlicerLongPETCTReportTableWidget);
  d->setupUi(this);



}

//-----------------------------------------------------------------------------
qSlicerLongPETCTReportTableWidget
::~qSlicerLongPETCTReportTableWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTReportTableWidget::setReportNode(vtkMRMLLongPETCTReportNode* reportNode)
{
  Q_D(qSlicerLongPETCTReportTableWidget);

  qvtkReconnect(d->ReportNode.GetPointer(), reportNode, vtkCommand::ModifiedEvent, this, SLOT(updateView()) );

  d->ReportNode = reportNode;

  this->updateView();
}


//-----------------------------------------------------------------------------
vtkMRMLLongPETCTReportNode* qSlicerLongPETCTReportTableWidget::reportNode()
{
  Q_D(qSlicerLongPETCTReportTableWidget);

  return d->ReportNode.GetPointer();
}


//-----------------------------------------------------------------------------
void
qSlicerLongPETCTReportTableWidget::prepareHorizontalHeaders()
{

  Q_D(qSlicerLongPETCTReportTableWidget);

  if (d->ReportNode == NULL)
    return;

  bool empty = this->columnCount() == 0;

  int diff = d->ReportNode->GetSelectedStudiesCount() - this->columnCount();

  for (int i = 0; i < std::abs(diff); ++i)
    {
      int newColumnID = this->columnCount();

      if (diff > 0)
        this->insertColumn(newColumnID);
      else if (diff < 0)
        this->removeColumn(newColumnID - 1);
    }

  if(empty & this->columnCount() > 0)
    {
      QHeaderView* horizontalHeaderView = this->horizontalHeader();
      if(horizontalHeaderView != NULL)
        {
          QObject::connect(horizontalHeaderView, SIGNAL(sectionClicked(int)), this, SIGNAL(studyClicked(int)));
        }
      }
}

//-----------------------------------------------------------------------------
void
qSlicerLongPETCTReportTableWidget::prepareVerticalHeaders()
{

  Q_D(qSlicerLongPETCTReportTableWidget);

  if (d->ReportNode == NULL)
    return;

  bool empty = this->rowCount() == 0;

  int diff = d->ReportNode->GetFindingsCount() - this->rowCount();

  for (int i = 0; i < std::abs(diff); ++i)
    {
      int newRowID = this->rowCount();

      if (diff > 0)
        {
          this->insertRow(newRowID);
          for(int j=0; j < this->columnCount();++j)
            {
              QLabel* label = new QLabel(this);
              this->setCellWidget(newRowID,j,label);
            }
        }
      else if (diff < 0)
        this->removeRow(newRowID - 1);
    }

  if(empty & this->rowCount() > 0)
     {
       QHeaderView* verticalHeaderView = this->verticalHeader();
       if(verticalHeaderView != NULL)
         {
           QObject::connect(verticalHeaderView, SIGNAL(sectionClicked(int)), this, SIGNAL(findingClicked(int)));
         }
     }
}

//-----------------------------------------------------------------------------
void
qSlicerLongPETCTReportTableWidget::updateHorizontalHeaders()
{
  Q_D(qSlicerLongPETCTReportTableWidget);

  if (d->ReportNode == NULL || this->columnCount() != d->ReportNode->GetSelectedStudiesCount())
    return;

  int vhwidth = 0;
  if(this->verticalHeader() != NULL)
    vhwidth = this->verticalHeader()->width();

  int w = this->width() - vhwidth - 5;

  int colwidth = 0;

  if(d->ReportNode->GetSelectedStudiesCount() > 0)
    colwidth = std::max(30,static_cast<int>(w / d->ReportNode->GetSelectedStudiesCount()));

  for (int i = 0; i < d->ReportNode->GetSelectedStudiesCount(); ++i)
    {
      vtkSmartPointer<vtkMRMLLongPETCTStudyNode> study =
          d->ReportNode->GetSelectedStudy(i);

      if (study == NULL)
        continue;

      QDate date = QDate::fromString(
          QString(study->GetAttribute("DICOM.StudyDate")).trimmed(),
          "yyyyMMdd");
      QString itemText = date.toString(Qt::SystemLocaleShortDate);
      QTableWidgetItem* item = new QTableWidgetItem(itemText);

      QFont font = item->font();
      font.setBold(true);
      item->setFont(font);

      item->setToolTip(itemText);

      this->setHorizontalHeaderItem(i, item);
      this->setColumnWidth(i, colwidth);
    }

}

//-----------------------------------------------------------------------------
void
qSlicerLongPETCTReportTableWidget::updateVerticalHeaders()
{

  Q_D(qSlicerLongPETCTReportTableWidget);

  if (d->ReportNode == NULL
      || this->rowCount() != d->ReportNode->GetFindingsCount()
      || this->columnCount() != d->ReportNode->GetSelectedStudiesCount())
    return;

  for (int i = 0; i < d->ReportNode->GetFindingsCount(); ++i)
    {
      vtkSmartPointer<vtkMRMLLongPETCTFindingNode> finding =
          d->ReportNode->GetFinding(i);

      if (finding.GetPointer() == NULL)
        continue;

      for (int j = 0; j < d->ReportNode->GetSelectedStudiesCount(); ++j)
        {
          QLabel* label = qobject_cast<QLabel*>(this->cellWidget(i,j));

          if (finding->GetSegmentationForStudy(
              d->ReportNode->GetSelectedStudy(j)) != NULL)
            {
              int colorID = finding->GetColorID();
              vtkMRMLColorTableNode* colorTable =
                  d->ReportNode->GetFindingTypesColorTable();
              if (colorTable)
                {
                  double color[3];
                  colorTable->GetColor(colorID, color);
                  QColor findingColor =
                      qSlicerLongPETCTColorSelectionDialog::getRGBColorFromDoubleValues(
                          color[0], color[1], color[2]);

                  if(label != NULL)
                    {
                      label->setStyleSheet("background-color:" + findingColor.name());
                    }

                }
            }
          else
            {
              if(label != NULL)
                label->setStyleSheet(NULL);
            }
        }

      QString itemText = finding->GetName();
      QTableWidgetItem* item = new QTableWidgetItem(itemText);

      QFont font = item->font();
      font.setBold(true);
      item->setFont(font);

      item->setToolTip(itemText);
      this->setVerticalHeaderItem(i, item);

    }
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTReportTableWidget::updateView()
{
  Q_D(qSlicerLongPETCTReportTableWidget);

  this->prepareHorizontalHeaders();
  this->prepareVerticalHeaders();
  this->updateVerticalHeaders();
  this->updateHorizontalHeaders();

  int lastSelectedStudyIndex = d->ReportNode->GetIndexOfSelectedStudy(d->ReportNode->GetUserSelectedStudy());

  if(lastSelectedStudyIndex >= 0 && lastSelectedStudyIndex < d->ReportNode->GetSelectedStudiesCount())
    this->selectStudyColumn(lastSelectedStudyIndex);

  int lastSelectedFindingIndex = d->ReportNode->GetIndexOfFinding(d->ReportNode->GetUserSelectedFinding());

  if(lastSelectedFindingIndex >= 0 && lastSelectedFindingIndex < d->ReportNode->GetFindingsCount())
    this->selectFindingRow(lastSelectedFindingIndex);

  if(lastSelectedStudyIndex != -1 && lastSelectedFindingIndex != -1)
    {
      QLabel* selCellWidget = qobject_cast<QLabel*>(this->cellWidget(lastSelectedFindingIndex,lastSelectedStudyIndex));
      if(selCellWidget != NULL)
        {
          QString styleSheet = selCellWidget->styleSheet();
          styleSheet = styleSheet + "; border: 3px solid #FF0000";
          selCellWidget->setStyleSheet(styleSheet);
        }
    }
}


//-----------------------------------------------------------------------------
void qSlicerLongPETCTReportTableWidget
::selectStudyColumn(int index)
{

  if(index >= 0 && index < this->columnCount())
    {
      QBrush background(QColor(98,140,178));
      QBrush foreground(QColor(255,255,255));
      this->horizontalHeaderItem(index)->setBackground(background);
      this->horizontalHeaderItem(index)->setForeground(foreground);
    }
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTReportTableWidget
::selectFindingRow(int index)
{

  if(index >= 0 && index < this->rowCount())
    {
      QBrush background(QColor(98,140,178));
      QBrush foreground(QColor(255,255,255));
      this->verticalHeaderItem(index)->setBackground(background);
      this->verticalHeaderItem(index)->setForeground(foreground);
    }
}

//-----------------------------------------------------------------------------
void
qSlicerLongPETCTReportTableWidget::arrangeColumns()
{
  int width = this->width() - this->verticalHeader()->width() - 5;
  int columns = this->columnCount();

  for (int i = 0; i < columns; ++i)
    {
      int colWidth = static_cast<int>(width / columns);
      if (colWidth < 30)
        colWidth = 30;

      this->setColumnWidth(i, colWidth);
    }
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTReportTableWidget
::resizeEvent(QResizeEvent* event)
{
  Superclass::resizeEvent(event);
  this->arrangeColumns();
}


//-----------------------------------------------------------------------------
void qSlicerLongPETCTReportTableWidget
::segmentationCellClicked(int row, int column)
{
  emit studyClicked(column);
  emit findingClicked(row);
}


//-----------------------------------------------------------------------------
void qSlicerLongPETCTReportTableWidget::
updateSegmentationSUVs(vtkMRMLLongPETCTStudyNode* study, vtkMRMLLongPETCTFindingNode* finding, double max, double mean, double min)
{
  Q_D(qSlicerLongPETCTReportTableWidget);

  if(d->ReportNode == NULL)
    return;

  int col = d->ReportNode->GetIndexOfSelectedStudy(study);
  int row = d->ReportNode->GetIndexOfFinding(finding);

  QLabel* cellWidget = qobject_cast<QLabel*>(this->cellWidget(row,col));

  if(cellWidget != NULL)
    {
      QStringList tooltip;
      tooltip << "SUVMax: " << QString().setNum(max) << "\nSUVMean: " << QString().setNum(mean) << "\nSUVMin: " << QString().setNum(min);
      cellWidget->setToolTip(tooltip.join(""));
    }

}



