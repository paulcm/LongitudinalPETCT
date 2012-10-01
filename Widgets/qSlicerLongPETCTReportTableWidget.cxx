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
#include <vtkMRMLLongPETCTSegmentationNode.h>

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

  void setCellWidgetToolTip(int row, int col, const QString& toolTip);

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

// --------------------------------------------------------------------------
void qSlicerLongPETCTReportTableWidgetPrivate
::setCellWidgetToolTip(int row, int col, const QString& tooltip)
{
  Q_Q(qSlicerLongPETCTReportTableWidget);

  QLabel* cellWidget = qobject_cast<QLabel*>(q->cellWidget(row,col));

  if(cellWidget != NULL)
      cellWidget->setToolTip(tooltip);
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

                  QString cssFontColor = "color: #000";
                  if(findingColor.lightness() < 100)
                    cssFontColor = "color: #FEFEFE";

                  if(label != NULL)
                    {
                      label->setStyleSheet("QWidget{"+cssFontColor +"; background-color:" + findingColor.name()+";} ");
                      std::cout << "STYLESHEET: " << label->styleSheet().toStdString().c_str() << std::endl;
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

  if(d->ReportNode == NULL)
    {
      this->clear();
      return;
    }

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

  for(int i=0; i < this->rowCount(); ++i)
    {
      vtkSmartPointer<vtkMRMLLongPETCTFindingNode> finding = d->ReportNode->GetFinding(i);

      for(int j=0; j < this->columnCount(); ++j)
        {
          QLabel* cellWidget = qobject_cast<QLabel*>(this->cellWidget(i,j));
          if(cellWidget != NULL)
            {
              vtkSmartPointer<vtkMRMLLongPETCTStudyNode> study = d->ReportNode->GetSelectedStudy(j);

              vtkSmartPointer<vtkMRMLLongPETCTSegmentationNode> segmentation = finding->GetSegmentationForStudy(study);

              if (segmentation != NULL)
                {

                  QString tooltip = QString("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\"></style></head><body style=\"font-family:\'Lucida Grande\',sans-serif; font-size: 12pt; font-weight: 400; font-style: normal;border: 1px solid black;margin-top:0px;\"><table style=\"border-collapse: collapse;border-spacing: 2px 10px;margin:0;padding:0\" ><tbody>  <tr><td>SUV<span style=\"vertical-align:sub;\">MAX</span></td><td>%1</td></tr><tr><td>SUV<span style=\"vertical-align:sub;\">MEAN</span></td><td>%2</td></tr><tr><td>SUV<span style=\"vertical-align:sub;\">MIN</span></td><td>%3</td></tr></tbody></table></body></html>").arg(QString().setNum(segmentation->GetSUVMax()),QString().setNum(segmentation->GetSUVMean()),QString().setNum(segmentation->GetSUVMin()));
                  cellWidget->setToolTip(tooltip);

//                  QStringList tooltip;
//                  tooltip << "SUVMax: " << QString().setNum(segmentation->GetSUVMax())
//                      << "\nSUVMean: " << QString().setNum(segmentation->GetSUVMean()) << "\nSUVMin: "
//                      << QString().setNum(segmentation->GetSUVMin());
//
//                  cellWidget->setToolTip(tooltip.join(""));
                }
              else
                cellWidget->setToolTip(NULL);

              if (i == lastSelectedFindingIndex && j == lastSelectedStudyIndex)
                {
                  QString styleSheet = cellWidget->styleSheet();
                  styleSheet.prepend("QLabel{border: 3px solid #DD0000;} ");
                  cellWidget->setStyleSheet(styleSheet);
                  std::cout << "STYLESHEET: " << cellWidget->styleSheet().toStdString().c_str() << std::endl;
                }
            }
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


/*//-----------------------------------------------------------------------------
void qSlicerLongPETCTReportTableWidget::
updateSegmentationSUVs(vtkMRMLLongPETCTStudyNode* study, vtkMRMLLongPETCTFindingNode* finding, double max, double mean, double min)
{
  Q_D(qSlicerLongPETCTReportTableWidget);

  if(d->ReportNode == NULL)
    return;

  int col = d->ReportNode->GetIndexOfSelectedStudy(study);
  int row = d->ReportNode->GetIndexOfFinding(finding);

  if(col != -1 && row != -1)
    {
      QStringList tooltip;
      tooltip << "SUVMax: " << QString().setNum(max) << "\nSUVMean: " << QString().setNum(mean) << "\nSUVMin: " << QString().setNum(min);

      d->setCellWidgetToolTip(row, col, tooltip.join(""));
    }
}

//-----------------------------------------------------------------------------
void
qSlicerLongPETCTReportTableWidget::clearSegmentationSUVs(
    vtkMRMLLongPETCTStudyNode* study, vtkMRMLLongPETCTFindingNode* finding)
{
  Q_D(qSlicerLongPETCTReportTableWidget);

  if (d->ReportNode == NULL)
    return;

  int col = d->ReportNode->GetIndexOfSelectedStudy(study);
  int row = d->ReportNode->GetIndexOfFinding(finding);

  if(col != -1 && row != -1)
    d->setCellWidgetToolTip(row,col,"");
}*/



