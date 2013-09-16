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
#include "qMRMLLongitudinalPETCTReportOverviewWidget.h"
#include "ui_qMRMLLongitudinalPETCTReportOverviewWidget.h"


#include <vtkMRMLLongitudinalPETCTReportNode.h>
#include <vtkMRMLLongitudinalPETCTStudyNode.h>
#include <vtkMRMLLongitudinalPETCTSegmentationNode.h>

#include <vtkMRMLColorTableNode.h>

#include <QBrush>
#include <QColor>
#include <QDate>
#include <QTime>
#include <QHeaderView>
#include <ctkCheckBox.h>



//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongitudinalitudinalPETCT
class qMRMLLongitudinalPETCTReportOverviewWidgetPrivate
  : public Ui_qMRMLLongitudinalPETCTReportOverviewWidget
{
  Q_DECLARE_PUBLIC(qMRMLLongitudinalPETCTReportOverviewWidget);
protected:
  qMRMLLongitudinalPETCTReportOverviewWidget* const q_ptr;

public:
  qMRMLLongitudinalPETCTReportOverviewWidgetPrivate(
    qMRMLLongitudinalPETCTReportOverviewWidget& object);

  virtual ~qMRMLLongitudinalPETCTReportOverviewWidgetPrivate();
  virtual void setupUi(qMRMLLongitudinalPETCTReportOverviewWidget*);

  ctkCheckBox* createConnectedCellWidgetCheckBox();
  void setCellWidgetToolTip(int row, int col, const QString& toolTip);
  QColor getRGBColorFromDoubleValues(double r, double g, double b);

  int optimalColumnWidth();

  void arrangeColumns();

  void prepareHorizontalHeaders();
  void prepareVerticalHeaders();

  void updateHorizontalHeaders();
  void updateVerticalHeaders();


  QIcon IconModelVisibility;
  QIcon IconPlaceholder;

  QBrush TableHeaderSelectedBackgroundRole;

  int MaxHeaderTextLength;

};

// --------------------------------------------------------------------------
qMRMLLongitudinalPETCTReportOverviewWidgetPrivate
::qMRMLLongitudinalPETCTReportOverviewWidgetPrivate(
  qMRMLLongitudinalPETCTReportOverviewWidget& object)
  : q_ptr(&object), MaxHeaderTextLength(15)
{
  this->IconModelVisibility.addFile(":/Icons/VisibleOnSmall.png", QSize(16, 16), QIcon::Normal,
        QIcon::Off);
  this->IconModelVisibility.addFile(":/Icons/VisibleOffSmall.png", QSize(16, 16), QIcon::Normal,
        QIcon::On);

  this->IconPlaceholder.addFile(":/Icons/placeholder.png");

  this->TableHeaderSelectedBackgroundRole.setColor(QColor(98,140,178));
}

// --------------------------------------------------------------------------
qMRMLLongitudinalPETCTReportOverviewWidgetPrivate
::~qMRMLLongitudinalPETCTReportOverviewWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
QColor qMRMLLongitudinalPETCTReportOverviewWidgetPrivate::getRGBColorFromDoubleValues(double r, double g, double b)
{
  return QColor( (r*255+.5), (g*255+.5), (b*255+.5) );
}

// --------------------------------------------------------------------------
void qMRMLLongitudinalPETCTReportOverviewWidgetPrivate
::setupUi(qMRMLLongitudinalPETCTReportOverviewWidget* widget)
{
  Q_Q(qMRMLLongitudinalPETCTReportOverviewWidget);

  this->Ui_qMRMLLongitudinalPETCTReportOverviewWidget::setupUi(widget);

  q->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
  this->TableReport->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
  this->TableReport->setSelectionMode(QAbstractItemView::NoSelection);

  this->LabelInfo->setToolTip("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\"></style></head><body style=\"font-family:\'Lucida Grande\',sans-serif; font-size: 12pt; font-weight: 400; font-style: normal;border: 1px solid black;margin-top:0px;\"><table cellspacing=\"5\"><tbody><tr><td>-</td><td>Click on table cell to select Study and Finding</td></tr><tr><td>-</td><td>Select which statistical value should be shown in table cells</td></tr><tr><td>-</td><td>Move mouse cursor over table cell to display statistical values for the segmentation</td></tr><tr><td>-</td><td>Enable/disable visibility of Models representing segmentations</td></tr></tbody></table></body></html>");

  QObject::connect(this->TableReport, SIGNAL(cellClicked(int,int)), q,SLOT(segmentationCellClicked(int,int)));
  QObject::connect(this->ComboBoxValuesType, SIGNAL(currentIndexChanged(int)), q, SLOT(updateWidgetFromMRML()));

}

// --------------------------------------------------------------------------
int
qMRMLLongitudinalPETCTReportOverviewWidgetPrivate::optimalColumnWidth()
{
  Q_Q(qMRMLLongitudinalPETCTReportOverviewWidget);
  Q_ASSERT(this->TableReport);
  Q_ASSERT(this->LabelFindings);

  int vhwidth = 0;
  if(this->TableReport->verticalHeader() != NULL)
    vhwidth = this->TableReport->verticalHeader()->sizeHint().width();

  int w = q->width() - vhwidth - this->LabelFindings->width() - 5;

  int columns = this->TableReport->columnCount();

  if(q->reportNode())
    {
      int nrOfSelStudies = q->reportNode()->GetNumberOfSelectedStudies();
      if(nrOfSelStudies > columns) // in case table is not updated yet, otherwise nr should be the same
        columns = nrOfSelStudies;
    }

  int colwidth = columns > 0 ? std::max(30,static_cast<int>(w / columns)) : w;

  return colwidth;
}

//-----------------------------------------------------------------------------
void
qMRMLLongitudinalPETCTReportOverviewWidgetPrivate::prepareHorizontalHeaders()
{

  Q_Q(qMRMLLongitudinalPETCTReportOverviewWidget);
  Q_ASSERT(this->TableReport);

  vtkMRMLLongitudinalPETCTReportNode* report = q->reportNode();

  if (report == NULL)
    return;

  bool empty = this->TableReport->columnCount() == 0;

  int diff = report->GetNumberOfSelectedStudies() - this->TableReport->columnCount();

  for (int i = 0; i < std::abs(diff); ++i)
    {
      int newColumnID = this->TableReport->columnCount();

      if (diff > 0)
        {
          this->TableReport->insertColumn(newColumnID);
          for(int j=0; j < this->TableReport->rowCount(); ++j)
            {
              this->TableReport->setCellWidget(j,newColumnID,this->createConnectedCellWidgetCheckBox());
            }
        }
      else if (diff < 0)
        {
          // first remove all cell widgets from column which is about to be removed
          for(int j=0; j < this->TableReport->rowCount(); ++j)
            {
              ctkCheckBox* cellWidget = NULL;
              cellWidget = qobject_cast<ctkCheckBox*>(this->TableReport->cellWidget(j,newColumnID - 1));
              this->TableReport->removeCellWidget(j,newColumnID - 1);
              if(cellWidget)
                cellWidget->deleteLater();
            }

          this->TableReport->removeColumn(newColumnID - 1);
        }

    }

  if(empty && (this->TableReport->columnCount() > 0))
    {
      QHeaderView* horizontalHeaderView = this->TableReport->horizontalHeader();
      if(horizontalHeaderView != NULL)
        {
          QObject::connect(horizontalHeaderView, SIGNAL(sectionClicked(int)), q, SLOT(selectStudyColumn(int)), Qt::UniqueConnection);
        }
      }
}

//-----------------------------------------------------------------------------
void
qMRMLLongitudinalPETCTReportOverviewWidgetPrivate::prepareVerticalHeaders()
{

  Q_Q(qMRMLLongitudinalPETCTReportOverviewWidget);

  vtkMRMLLongitudinalPETCTReportNode* report = q->reportNode();

  if (report == NULL)
    return;

  bool empty = this->TableReport->rowCount() == 0;

  int diff = report->GetNumberOfFindingNodeIDs() - this->TableReport->rowCount();

  for (int i = 0; i < std::abs(diff); ++i)
    {
      int newRowID = this->TableReport->rowCount();

      if (diff > 0)
        {
          this->TableReport->insertRow(newRowID);

          for(int j=0; j < this->TableReport->columnCount(); ++j)
            {
              this->TableReport->setCellWidget(newRowID,j,this->createConnectedCellWidgetCheckBox());
            }
        }

      else if (diff < 0)
        {
          // first remove all cell widgets from column which is about to be removed
          for (int j = 0; j < this->TableReport->rowCount(); ++j)
            {
              ctkCheckBox* cellWidget = NULL;
              cellWidget = qobject_cast<ctkCheckBox*>(this->TableReport->cellWidget(newRowID-1,j));
              this->TableReport->removeCellWidget(newRowID - 1, j);
              if (cellWidget)
                cellWidget->deleteLater();
            }

          this->TableReport->removeRow(newRowID - 1);
        }

    }

  if(empty && (this->TableReport->rowCount() > 0))
     {
       QHeaderView* verticalHeaderView = this->TableReport->verticalHeader();
       if(verticalHeaderView != NULL)
         {
           QObject::connect(verticalHeaderView, SIGNAL(sectionClicked(int)), q, SLOT(selectFindingRow(int)),Qt::UniqueConnection);
         }
     }
}

//-----------------------------------------------------------------------------
void
qMRMLLongitudinalPETCTReportOverviewWidgetPrivate::updateHorizontalHeaders()
{
  Q_Q(qMRMLLongitudinalPETCTReportOverviewWidget);

  vtkMRMLLongitudinalPETCTReportNode* report = q->reportNode();

  if (report == NULL || this->TableReport->columnCount() != report->GetNumberOfSelectedStudies())
    return;


  for (int i = 0; i < report->GetNumberOfSelectedStudies(); ++i)
    {
      vtkSmartPointer<vtkMRMLLongitudinalPETCTStudyNode> study =
          report->GetSelectedStudy(i);

      if (study == NULL)
        continue;

      QDate date = QDate::fromString(
          QString(study->GetAttribute("DICOM.StudyDate")).trimmed(),
          "yyyyMMdd");
      QString itemText = date.toString(Qt::DefaultLocaleShortDate);

      if (itemText.length() > this->MaxHeaderTextLength)
        {
          int presize = (this->MaxHeaderTextLength / 2);
          int postsize = presize - 3;
          itemText = itemText.left(presize) + "..." + itemText.right(postsize);
        }


      QTableWidgetItem* hhItem = this->TableReport->horizontalHeaderItem(i);

      if (!hhItem)
        {
          hhItem = new QTableWidgetItem(itemText);
          this->TableReport->setHorizontalHeaderItem(i, hhItem);
        }
      else
        hhItem->setText(itemText);

      QFont font = hhItem->font();
      font.setBold(true);
      hhItem->setFont(font);
      hhItem->setToolTip(date.toString(Qt::DefaultLocaleLongDate));


      this->TableReport->setColumnWidth(i, this->optimalColumnWidth());
    }

}

//-----------------------------------------------------------------------------
void
qMRMLLongitudinalPETCTReportOverviewWidgetPrivate::updateVerticalHeaders()
{

  Q_Q(qMRMLLongitudinalPETCTReportOverviewWidget);

  vtkMRMLLongitudinalPETCTReportNode* report = q->reportNode();

  if (report == NULL
      || this->TableReport->rowCount() != report->GetNumberOfFindingNodeIDs()
      || this->TableReport->columnCount() != report->GetNumberOfSelectedStudies())
    return;


  for (int i = 0; i < report->GetNumberOfFindingNodeIDs(); ++i)
    {
      vtkSmartPointer<vtkMRMLLongitudinalPETCTFindingNode> finding =
          report->GetFinding(i);

      if (finding.GetPointer() == NULL)
        continue;

      for (int j = 0; j < report->GetNumberOfSelectedStudies(); ++j)
        {
          //qSlicerLongitudinalPETCTSegmentationTableCellWidget* cellWidget = qobject_cast<qSlicerLongitudinalPETCTSegmentationTableCellWidget*>(this->cellWidget(i,j));
          ctkCheckBox* cellWidget = qobject_cast<ctkCheckBox*>(this->TableReport->cellWidget(i,j));

          if (finding->GetSegmentationMappedByStudyNodeID(
              report->GetSelectedStudy(j)->GetID()))
            {

              int colorID = finding->GetColorID();
              vtkMRMLColorTableNode* colorTable =
                  const_cast<vtkMRMLColorTableNode*>(report->GetColorTableNode());
              if (colorTable)
                {
                  double color[4];
                  colorTable->GetColor(colorID, color);
                  QColor findingColor =
                      this->getRGBColorFromDoubleValues(
                          color[0], color[1], color[2]);

                  QString cssFontColor = "color: #000";


                  if(findingColor.lightness() < 110)
                    cssFontColor = "color: #FEFEFE";

                  if(cellWidget)
                    {
                      cellWidget->setStyleSheet("QCheckBox QToolTip {"+cssFontColor +"; background-color:" + findingColor.name()+";} QCheckBox {background-color:" + findingColor.name()+";}");
                    }
                }
            }
          else
            {
              if(cellWidget)
                {
                  cellWidget->setIndicatorIcon(IconPlaceholder);
                  cellWidget->setToolTip(NULL);
                  cellWidget->setStyleSheet(NULL);
                }
            }
        }

      QString itemText = finding->GetName();
      if (itemText.length() > this->MaxHeaderTextLength)
        {
          int presize = (this->MaxHeaderTextLength / 2);
          int postsize = presize - 3;
          itemText = itemText.left(presize) + "..." + itemText.right(postsize);
        }

      QTableWidgetItem* vhItem = this->TableReport->verticalHeaderItem(i);

      if ( ! vhItem)
        {
          vhItem = new QTableWidgetItem(itemText);
          this->TableReport->setVerticalHeaderItem(i, vhItem);
        }
      else
        vhItem->setText(itemText);

      QFont font = vhItem->font();
      font.setBold(true);
      vhItem->setFont(font);
      vhItem->setToolTip(finding->GetName());
    }
}

//-----------------------------------------------------------------------------
void
qMRMLLongitudinalPETCTReportOverviewWidgetPrivate::arrangeColumns()
{
  for (int i = 0; i < this->TableReport->columnCount(); ++i)
    {
      this->TableReport->setColumnWidth(i, this->optimalColumnWidth());
    }
}

// --------------------------------------------------------------------------
ctkCheckBox*
qMRMLLongitudinalPETCTReportOverviewWidgetPrivate::createConnectedCellWidgetCheckBox()
{
  Q_Q(qMRMLLongitudinalPETCTReportOverviewWidget);

  ctkCheckBox* cellWidgetCheckBox = new ctkCheckBox(this->TableReport);
  cellWidgetCheckBox->setIconSize(QSize(16,16));
  cellWidgetCheckBox->setIndicatorIcon(this->IconPlaceholder);
  cellWidgetCheckBox->setChecked(true);
  cellWidgetCheckBox->setStyleSheet("text-align: center");

  QObject::connect(cellWidgetCheckBox, SIGNAL(clicked(bool)), q, SLOT(segmentationModelVisibilityChecked(bool)) );

  return cellWidgetCheckBox;
}

// --------------------------------------------------------------------------
void qMRMLLongitudinalPETCTReportOverviewWidgetPrivate
::setCellWidgetToolTip(int row, int col, const QString& tooltip)
{
  ctkCheckBox* cellWidget = qobject_cast<ctkCheckBox*>(this->TableReport->cellWidget(row,col));
  if(cellWidget != NULL)
      cellWidget->setToolTip(tooltip);
}

//-----------------------------------------------------------------------------
// qMRMLLongitudinalPETCTReportOverviewWidget methods

//-----------------------------------------------------------------------------
qMRMLLongitudinalPETCTReportOverviewWidget
::qMRMLLongitudinalPETCTReportOverviewWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qMRMLLongitudinalPETCTReportOverviewWidgetPrivate(*this) ), SelMode(ColumnSelectable)
{
  Q_D(qMRMLLongitudinalPETCTReportOverviewWidget);
  d->setupUi(this);



}

//-----------------------------------------------------------------------------
qMRMLLongitudinalPETCTReportOverviewWidget
::~qMRMLLongitudinalPETCTReportOverviewWidget()
{
}



//-----------------------------------------------------------------------------
void
qMRMLLongitudinalPETCTReportOverviewWidget::updateWidgetFromMRML()
{
  Q_D(qMRMLLongitudinalPETCTReportOverviewWidget);
  Q_ASSERT(d->TableReport);
  Q_ASSERT(d->LabelSelectedValue);
  Q_ASSERT(d->ComboBoxValuesType);


  vtkMRMLLongitudinalPETCTReportNode* report = this->reportNode();

  if ( ! report)
    {
      while (d->TableReport->rowCount() > 0)
        {
          d->TableReport->removeRow(d->TableReport->rowCount() - 1);
        }

      while (d->TableReport->columnCount() > 0)
        {
          d->TableReport->removeColumn(d->TableReport->columnCount() - 1);
        }

      d->LabelSelectedValue->setText(NULL);

      return;
    }

  d->prepareHorizontalHeaders();
  d->prepareVerticalHeaders();
  d->updateVerticalHeaders();
  d->updateHorizontalHeaders();



  for (int i = 0; i < d->TableReport->rowCount(); ++i)
    {
      vtkSmartPointer<vtkMRMLLongitudinalPETCTFindingNode> finding =
          report->GetFinding(i);

      for (int j = 0; j < d->TableReport->columnCount(); ++j)
        {
          //qSlicerLongitudinalPETCTSegmentationTableCellWidget* cellWidget = qobject_cast<qSlicerLongitudinalPETCTSegmentationTableCellWidget*>(this->cellWidget(i,j));
          ctkCheckBox* cellWidget = qobject_cast<ctkCheckBox*>(
              d->TableReport->cellWidget(i, j));
          if (cellWidget != NULL)
            {

              vtkSmartPointer<vtkMRMLLongitudinalPETCTStudyNode> study =
                  report->GetSelectedStudy(j);

              vtkSmartPointer<vtkMRMLLongitudinalPETCTSegmentationNode> segmentation = NULL;
              if(study)
                segmentation = finding->GetSegmentationMappedByStudyNodeID(study->GetID());

              switch (this->SelMode)
                {
              case qMRMLLongitudinalPETCTReportOverviewWidget::AllSelectable:
                cellWidget->setIndicatorIcon(d->IconModelVisibility);
                break;
              case qMRMLLongitudinalPETCTReportOverviewWidget::RowSelectable:
                if(finding == report->GetActiveFindingNode())
                  cellWidget->setIndicatorIcon(d->IconModelVisibility);
                else
                  cellWidget->setIndicatorIcon(d->IconPlaceholder);
                break;
              case qMRMLLongitudinalPETCTReportOverviewWidget::ColumnSelectable:
                if(study == report->GetActiveStudyNode())
                  cellWidget->setIndicatorIcon(d->IconModelVisibility);
                else
                  cellWidget->setIndicatorIcon(d->IconPlaceholder);
                break;
              default:
                break;
                }

              if (segmentation != NULL)
                {
                  cellWidget->setChecked(segmentation->GetModelVisible());
                  QString tooltip =
                      QString(
                          "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">hr { margin: 0; padding: 2px } </style></head><body style=\"font-family:\'Lucida Grande\',sans-serif; font-size: 12pt; font-weight: 400; font-style: normal;border: 1px solid black;margin-top:0px;\"><table style=\"border-collapse: collapse;border-spacing: 2px 10px;margin:0;padding:0\" ><tbody><tr><td>SUV<span style=\"vertical-align:sub;\">MAX</span></td><td>%1</td></tr><tr><td>SUV<span style=\"vertical-align:sub;\">MEAN</span></td><td>%2</td></tr><tr><td>SUV<span style=\"vertical-align:sub;\">MIN</span></td><td>%3</td></tr><tr><td colspan=\"2\"><hr /></td></tr><tr><td>Std.Dev.</td><td>%4</td></tr><tr><td>Vol.</td><td>%5 cc</td></tr><tr><td>Count</td><td>%6 vx</td></tr></tbody></table></body></html>").arg(
                          QString().setNum(segmentation->GetSUVMax()),
                          QString().setNum(segmentation->GetSUVMean()),
                          QString().setNum(segmentation->GetSUVMin()),
                          QString().setNum(segmentation->GetStdDev()),
                  	  	  QString().setNum(segmentation->GetVolcc()),
                          QString().setNum(segmentation->GetCount()));


                          cellWidget->setToolTip(tooltip);


                  switch (d->ComboBoxValuesType->currentIndex())
                  {
                  case 0:
                	  cellWidget->setText(QString().setNum(segmentation->GetSUVMax()));
                	  break;
                  case 1:
                	  cellWidget->setText(QString().setNum(segmentation->GetSUVMean()));
                	  break;
                  case 2:
                   	  cellWidget->setText(QString().setNum(segmentation->GetSUVMin()));
                   	break;
                  case 3:
                	  cellWidget->setText(QString().setNum(segmentation->GetStdDev()));
                	  break;
                  case 4:
                	  cellWidget->setText(QString().setNum(segmentation->GetVolcc())+" cc");
                	  break;
                  case 5:
                      cellWidget->setText(QString().setNum(segmentation->GetVolmm3())+" mm^3");
                      break;
                  case 6:
                	  cellWidget->setText(QString().setNum(segmentation->GetCount())+" voxels");
                	  break;
                  default:
                	  cellWidget->setText(NULL);
                  }

                }
              else
                {
                  cellWidget->setToolTip(NULL);
                  cellWidget->setIndicatorIcon(d->IconPlaceholder);
                  cellWidget->setText(NULL);
                }

            }
        }
    }

  vtkSmartPointer<vtkMRMLLongitudinalPETCTStudyNode> study =
      report->GetActiveStudyNode();

  if (study)
    {
      QDate date = QDate::fromString(
          QString(study->GetAttribute("DICOM.StudyDate")).trimmed(),
          "yyyyMMdd");
      QTime time = QTime::fromString(
          QString(study->GetAttribute("DICOM.StudyTime")).trimmed().left(6),
          "hhmmss");

      QStringList text;
      text << date.toString(Qt::DefaultLocaleLongDate);
      text << time.toString(Qt::ISODate);

      d->LabelSelectedValue->setText(text.join("   "));
    }
  else
    d->LabelSelectedValue->setText(NULL);

  d->arrangeColumns();
}


//-----------------------------------------------------------------------------
void
qMRMLLongitudinalPETCTReportOverviewWidget::selectStudyColumn(int index)
{
  Q_D (qMRMLLongitudinalPETCTReportOverviewWidget);
  Q_ASSERT(d->TableReport);

  for (int i = 0; i < d->TableReport->columnCount(); ++i)
    {
      if (i != index)
        {
          d->TableReport->horizontalHeaderItem(i)->setForeground(Qt::black);
          ;
          d->TableReport->horizontalHeaderItem(i)->setBackground(Qt::white);
        }
      else
        {
          d->TableReport->horizontalHeaderItem(i)->setForeground(Qt::white);
          d->TableReport->horizontalHeaderItem(i)->setBackground(
              d->TableHeaderSelectedBackgroundRole);

          if (this->reportNode())
            {
              vtkMRMLLongitudinalPETCTStudyNode* study =
                  this->reportNode()->GetSelectedStudy(index);

              if (study)
                emit studySelected(study);
            }
        }
    }
  if(d->TableReport->columnCount() == 0)
    {
      emit studySelected(NULL);
    }
}

//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTReportOverviewWidget
::selectFindingRow(int index)
{
  Q_D(qMRMLLongitudinalPETCTReportOverviewWidget);
  Q_ASSERT(d->TableReport);

  for (int i = 0; i < d->TableReport->rowCount(); ++i)
    {
      if (i != index)
        {
          d->TableReport->verticalHeaderItem(i)->setForeground(Qt::black);
          d->TableReport->verticalHeaderItem(i)->setBackground(Qt::white);
        }
      else
        {
          d->TableReport->verticalHeaderItem(i)->setForeground(Qt::white);
          d->TableReport->verticalHeaderItem(i)->setBackground(
              d->TableHeaderSelectedBackgroundRole);

          if (this->reportNode())
            {
              vtkMRMLLongitudinalPETCTFindingNode* finding =
                  this->reportNode()->GetFinding(index);

              if (finding)
                emit findingSelected(finding);
            }
        }
    }
}



//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTReportOverviewWidget
::resizeEvent(QResizeEvent* event)
{
  Superclass::resizeEvent(event);

  Q_D (qMRMLLongitudinalPETCTReportOverviewWidget);
  d->arrangeColumns();
}


//-----------------------------------------------------------------------------
void
qMRMLLongitudinalPETCTReportOverviewWidget::segmentationCellClicked(int row,
    int column)
{
  this->selectStudyColumn(column);
  this->selectFindingRow(row);
}





//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTReportOverviewWidget
::segmentationModelVisibilityChecked(bool toggled)
{
  Q_D(qMRMLLongitudinalPETCTReportOverviewWidget);

  ctkCheckBox* sender = qobject_cast<ctkCheckBox*>(QObject::sender());

  if (sender)
    {
      for(int i=0; i < d->TableReport->rowCount(); ++i)
        {
          for(int j=0; j < d->TableReport->columnCount(); ++j)
            {
              if(d->TableReport->cellWidget(i,j) == sender)
                {
                  bool changed = false;

                  vtkMRMLLongitudinalPETCTReportNode* report = this->reportNode();

                  if(report)
                    {
                      vtkMRMLLongitudinalPETCTFindingNode* finding = report->GetFinding(i);
                      if(finding)
                        {
                          vtkMRMLLongitudinalPETCTStudyNode* study = report->GetSelectedStudy(j);
                          vtkMRMLLongitudinalPETCTSegmentationNode* seg = NULL;

                          bool changeNecessary = false;

                          if(this->SelMode == RowSelectable && finding != report->GetActiveFindingNode())
                            changeNecessary = true;
                          else if(this->SelMode == ColumnSelectable && study != report->GetActiveStudyNode())
                            changeNecessary = true;

                          if(changeNecessary)
                            this->segmentationCellClicked(i,j);

                          if(study)
                              seg = finding->GetSegmentationMappedByStudyNodeID(study->GetID());

                          if(seg)
                            {
                              seg->SetModelVisible(toggled);
                              changed = true;
                            }

                        }
                    }
                  if(changed)
                    emit segmentationModelVisiblityChecked(i,j,toggled);

                  break;
                }
            }
        }
    }
}



