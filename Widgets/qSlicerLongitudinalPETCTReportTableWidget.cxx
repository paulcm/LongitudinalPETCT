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
#include "qSlicerLongitudinalPETCTReportTableWidget.h"
#include "ui_qSlicerLongitudinalPETCTReportTableWidget.h"


#include <vtkMRMLLongitudinalPETCTReportNode.h>
#include <vtkMRMLLongitudinalPETCTStudyNode.h>
#include <vtkMRMLLongitudinalPETCTSegmentationNode.h>

#include <vtkMRMLColorTableNode.h>

#include <QBrush>
#include <QDate>
#include <QTime>
#include <QHeaderView>
#include <ctkCheckBox.h>


#include "qSlicerLongitudinalPETCTColorSelectionDialog.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongitudinalitudinalPETCT
class qSlicerLongitudinalPETCTReportTableWidgetPrivate
  : public Ui_qSlicerLongitudinalPETCTReportTableWidget
{
  Q_DECLARE_PUBLIC(qSlicerLongitudinalPETCTReportTableWidget);
protected:
  qSlicerLongitudinalPETCTReportTableWidget* const q_ptr;

public:
  qSlicerLongitudinalPETCTReportTableWidgetPrivate(
    qSlicerLongitudinalPETCTReportTableWidget& object);

  virtual ~qSlicerLongitudinalPETCTReportTableWidgetPrivate();
  virtual void setupUi(qSlicerLongitudinalPETCTReportTableWidget*);

  vtkSmartPointer<vtkMRMLLongitudinalPETCTReportNode> ReportNode;

  ctkCheckBox* createConnectedCellWidgetCheckBox();
  void setCellWidgetToolTip(int row, int col, const QString& toolTip);

  int optimalColumnWidth();

  QIcon IconModelVisibility;
  QIcon IconPlaceholder;

  QBrush TableHeaderSelectedBackgroundRole;

};

// --------------------------------------------------------------------------
qSlicerLongitudinalPETCTReportTableWidgetPrivate
::qSlicerLongitudinalPETCTReportTableWidgetPrivate(
  qSlicerLongitudinalPETCTReportTableWidget& object)
  : q_ptr(&object), ReportNode(NULL)
{
  this->IconModelVisibility.addFile(":/Icons/VisibleOnSmall.png", QSize(16, 16), QIcon::Normal,
        QIcon::Off);
  this->IconModelVisibility.addFile(":/Icons/VisibleOffSmall.png", QSize(16, 16), QIcon::Normal,
        QIcon::On);

  this->IconPlaceholder.addFile(":/Icons/placeholder.png");

  this->TableHeaderSelectedBackgroundRole.setColor(QColor(98,140,178));
}

// --------------------------------------------------------------------------
qSlicerLongitudinalPETCTReportTableWidgetPrivate
::~qSlicerLongitudinalPETCTReportTableWidgetPrivate()
{
}


// --------------------------------------------------------------------------
void qSlicerLongitudinalPETCTReportTableWidgetPrivate
::setupUi(qSlicerLongitudinalPETCTReportTableWidget* widget)
{
  Q_Q(qSlicerLongitudinalPETCTReportTableWidget);

  this->Ui_qSlicerLongitudinalPETCTReportTableWidget::setupUi(widget);

  q->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
  this->TableReport->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
  this->TableReport->setSelectionMode(QAbstractItemView::NoSelection);

  this->LabelInfo->setToolTip("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\"></style></head><body style=\"font-family:\'Lucida Grande\',sans-serif; font-size: 12pt; font-weight: 400; font-style: normal;border: 1px solid black;margin-top:0px;\"><table cellspacing=\"5\"><tbody><tr><td>-</td><td>Click on table cell to select Study and Finding</td></tr><tr><td>-</td><td>Mouse over table cell to display SUV values for a segmentation</td></tr><tr><td>-</td><td>Enable/disable visibility of Models representing segmentations</td></tr></tbody></table></body></html>");

  QObject::connect(this->TableReport, SIGNAL(cellClicked(int,int)), q,SLOT(segmentationCellClicked(int,int)));

}

// --------------------------------------------------------------------------
int
qSlicerLongitudinalPETCTReportTableWidgetPrivate::optimalColumnWidth()
{
  Q_Q(qSlicerLongitudinalPETCTReportTableWidget);
  Q_ASSERT(this->TableReport);
  Q_ASSERT(this->LabelFindings);

  int vhwidth = 0;
  if(this->TableReport->verticalHeader() != NULL)
    vhwidth = this->TableReport->verticalHeader()->sizeHint().width();

  int w = q->width() - vhwidth - this->LabelFindings->width() - 5;

  int columns = this->TableReport->columnCount();

  if(this->ReportNode)
    {
      int nrOfSelStudies = this->ReportNode->GetNumberOfSelectedStudies();
      if(nrOfSelStudies > columns) // in case table is not updated yet, otherwise nr should be the same
        columns = nrOfSelStudies;
    }

  int colwidth = columns > 0 ? std::max(30,static_cast<int>(w / columns)) : w;

  return colwidth;
}

// --------------------------------------------------------------------------
ctkCheckBox*
qSlicerLongitudinalPETCTReportTableWidgetPrivate::createConnectedCellWidgetCheckBox()
{
  Q_Q(qSlicerLongitudinalPETCTReportTableWidget);

  ctkCheckBox* cellWidgetCheckBox = new ctkCheckBox(this->TableReport);
  cellWidgetCheckBox->setIconSize(QSize(16,16));
  cellWidgetCheckBox->setIndicatorIcon(this->IconPlaceholder);
  cellWidgetCheckBox->setChecked(true);

  QObject::connect(cellWidgetCheckBox, SIGNAL(clicked(bool)), q, SLOT(segmentationModelVisibilityChecked(bool)) );

  return cellWidgetCheckBox;
}

// --------------------------------------------------------------------------
void qSlicerLongitudinalPETCTReportTableWidgetPrivate
::setCellWidgetToolTip(int row, int col, const QString& tooltip)
{
  ctkCheckBox* cellWidget = qobject_cast<ctkCheckBox*>(this->TableReport->cellWidget(row,col));
  if(cellWidget != NULL)
      cellWidget->setToolTip(tooltip);
}

//-----------------------------------------------------------------------------
// qSlicerLongitudinalPETCTReportTableWidget methods

//-----------------------------------------------------------------------------
qSlicerLongitudinalPETCTReportTableWidget
::qSlicerLongitudinalPETCTReportTableWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerLongitudinalPETCTReportTableWidgetPrivate(*this) ), SelMode(ColumnSelectable)
{
  Q_D(qSlicerLongitudinalPETCTReportTableWidget);
  d->setupUi(this);



}

//-----------------------------------------------------------------------------
qSlicerLongitudinalPETCTReportTableWidget
::~qSlicerLongitudinalPETCTReportTableWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTReportTableWidget::setReportNode(vtkMRMLLongitudinalPETCTReportNode* reportNode)
{
  Q_D(qSlicerLongitudinalPETCTReportTableWidget);

  qvtkReconnect(d->ReportNode.GetPointer(), reportNode, vtkCommand::ModifiedEvent, this, SLOT(updateView()) );

  d->ReportNode = reportNode;

  this->updateView();
}


//-----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTReportNode* qSlicerLongitudinalPETCTReportTableWidget::reportNode()
{
  Q_D(qSlicerLongitudinalPETCTReportTableWidget);

  return d->ReportNode.GetPointer();
}


//-----------------------------------------------------------------------------
void
qSlicerLongitudinalPETCTReportTableWidget::prepareHorizontalHeaders()
{

  Q_D(qSlicerLongitudinalPETCTReportTableWidget);
  Q_ASSERT(d->TableReport);

  if (d->ReportNode == NULL)
    return;

  bool empty = d->TableReport->columnCount() == 0;

  int diff = d->ReportNode->GetNumberOfSelectedStudies() - d->TableReport->columnCount();

  for (int i = 0; i < std::abs(diff); ++i)
    {
      int newColumnID = d->TableReport->columnCount();

      if (diff > 0)
        {
          d->TableReport->insertColumn(newColumnID);
          for(int j=0; j < d->TableReport->rowCount(); ++j)
            {
              d->TableReport->setCellWidget(j,newColumnID,d->createConnectedCellWidgetCheckBox());
            }
        }
      else if (diff < 0)
        {
          // first remove all cell widgets from column which is about to be removed
          for(int j=0; j < d->TableReport->rowCount(); ++j)
            {
              ctkCheckBox* cellWidget = NULL;
              cellWidget = qobject_cast<ctkCheckBox*>(d->TableReport->cellWidget(j,newColumnID - 1));
              d->TableReport->removeCellWidget(j,newColumnID - 1);
              if(cellWidget)
                cellWidget->deleteLater();
            }

          d->TableReport->removeColumn(newColumnID - 1);
        }

    }

  if(empty && (d->TableReport->columnCount() > 0))
    {
      QHeaderView* horizontalHeaderView = d->TableReport->horizontalHeader();
      if(horizontalHeaderView != NULL)
        {
          QObject::connect(horizontalHeaderView, SIGNAL(sectionClicked(int)), this, SIGNAL(studyClicked(int)), Qt::UniqueConnection);
        }
      }
}

//-----------------------------------------------------------------------------
void
qSlicerLongitudinalPETCTReportTableWidget::prepareVerticalHeaders()
{

  Q_D(qSlicerLongitudinalPETCTReportTableWidget);

  if (d->ReportNode == NULL)
    return;

  bool empty = d->TableReport->rowCount() == 0;

  int diff = d->ReportNode->GetNumberOfFindingNodeIDs() - d->TableReport->rowCount();

  for (int i = 0; i < std::abs(diff); ++i)
    {
      int newRowID = d->TableReport->rowCount();

      if (diff > 0)
        {
          d->TableReport->insertRow(newRowID);

          for(int j=0; j < d->TableReport->columnCount(); ++j)
            {
              d->TableReport->setCellWidget(newRowID,j,d->createConnectedCellWidgetCheckBox());
            }
        }

      else if (diff < 0)
        {
          // first remove all cell widgets from column which is about to be removed
          for (int j = 0; j < d->TableReport->rowCount(); ++j)
            {
              ctkCheckBox* cellWidget = NULL;
              cellWidget = qobject_cast<ctkCheckBox*>(d->TableReport->cellWidget(newRowID-1,j));
              d->TableReport->removeCellWidget(newRowID - 1, j);
              if (cellWidget)
                cellWidget->deleteLater();
            }

          d->TableReport->removeRow(newRowID - 1);
        }

    }

  if(empty && (d->TableReport->rowCount() > 0))
     {
       QHeaderView* verticalHeaderView = d->TableReport->verticalHeader();
       if(verticalHeaderView != NULL)
         {
           QObject::connect(verticalHeaderView, SIGNAL(sectionClicked(int)), this, SIGNAL(findingClicked(int)),Qt::UniqueConnection);
         }
     }
}

//-----------------------------------------------------------------------------
void
qSlicerLongitudinalPETCTReportTableWidget::updateHorizontalHeaders()
{
  Q_D(qSlicerLongitudinalPETCTReportTableWidget);
  Q_ASSERT(d->TableReport);
  Q_ASSERT(d->LabelFindings);

  if (d->ReportNode == NULL || d->TableReport->columnCount() != d->ReportNode->GetNumberOfSelectedStudies())
    return;


  for (int i = 0; i < d->ReportNode->GetNumberOfSelectedStudies(); ++i)
    {
      vtkSmartPointer<vtkMRMLLongitudinalPETCTStudyNode> study =
          d->ReportNode->GetSelectedStudy(i);

      if (study == NULL)
        continue;

      QDate date = QDate::fromString(
          QString(study->GetAttribute("DICOM.StudyDate")).trimmed(),
          "yyyyMMdd");
      QString itemText = date.toString(Qt::DefaultLocaleShortDate);


      QTableWidgetItem* hhItem = d->TableReport->horizontalHeaderItem(i);

      if (!hhItem)
        {
          hhItem = new QTableWidgetItem(itemText);
          d->TableReport->setHorizontalHeaderItem(i, hhItem);
        }
      else
        hhItem->setText(itemText);

      QFont font = hhItem->font();
      font.setBold(true);
      hhItem->setFont(font);
      hhItem->setToolTip(itemText);


      d->TableReport->setColumnWidth(i, d->optimalColumnWidth());
    }

}

//-----------------------------------------------------------------------------
void
qSlicerLongitudinalPETCTReportTableWidget::updateVerticalHeaders()
{

  Q_D(qSlicerLongitudinalPETCTReportTableWidget);



  if (d->ReportNode == NULL
      || d->TableReport->rowCount() != d->ReportNode->GetNumberOfFindingNodeIDs()
      || d->TableReport->columnCount() != d->ReportNode->GetNumberOfSelectedStudies())
    return;


  for (int i = 0; i < d->ReportNode->GetNumberOfFindingNodeIDs(); ++i)
    {
      vtkSmartPointer<vtkMRMLLongitudinalPETCTFindingNode> finding =
          d->ReportNode->GetFinding(i);

      if (finding.GetPointer() == NULL)
        continue;

      for (int j = 0; j < d->ReportNode->GetNumberOfSelectedStudies(); ++j)
        {
          //qSlicerLongitudinalPETCTSegmentationTableCellWidget* cellWidget = qobject_cast<qSlicerLongitudinalPETCTSegmentationTableCellWidget*>(this->cellWidget(i,j));
          ctkCheckBox* cellWidget = qobject_cast<ctkCheckBox*>(d->TableReport->cellWidget(i,j));

          if (finding->GetSegmentationMappedByStudyNodeID(
              d->ReportNode->GetSelectedStudy(j)->GetID()))
            {
//              if (!cellWidget)
//                {
//                  cellWidget = d->createConnectedCellWidgetCheckBox();
//                  QObject::connect(cellWidget, SIGNAL(clicked(bool)), this, SLOT(segmentationModelVisibilityChecked(bool)) );
//                  d->TableReport->setCellWidget(i, j, cellWidget);
//                }

              int colorID = finding->GetColorID();
              vtkSmartPointer<vtkMRMLColorTableNode> colorTable =
                  d->ReportNode->GetFindingTypesColorTableNode();
              if (colorTable)
                {
                  double color[4];
                  colorTable->GetColor(colorID, color);
                  QColor findingColor =
                      qSlicerLongitudinalPETCTColorSelectionDialog::getRGBColorFromDoubleValues(
                          color[0], color[1], color[2]);

                  QString cssFontColor = "color: #000";

                  if(findingColor.lightness() < 125)
                    cssFontColor = "color: #FEFEFE";

                  if(cellWidget)
                    {
                      cellWidget->setStyleSheet("QCheckBox QToolTip {background-color:" + findingColor.name()+";} QCheckBox {"+cssFontColor +"; background-color:" + findingColor.name()+";}");
                    }
                }
            }
          else
            {
              if(cellWidget)
                {
                  cellWidget->setIndicatorIcon(d->IconPlaceholder);
                  cellWidget->setToolTip(NULL);
                  cellWidget->setStyleSheet(NULL);

                  //d->TableReport->removeCellWidget(i,j);
                  //cellWidget->deleteLater();
                }
            }
        }

      QString itemText = finding->GetName();

      QTableWidgetItem* vhItem = d->TableReport->verticalHeaderItem(i);

      if ( ! vhItem)
        {
          vhItem = new QTableWidgetItem(itemText);
          d->TableReport->setVerticalHeaderItem(i, vhItem);
        }
      else
        vhItem->setText(itemText);

      QFont font = vhItem->font();
      font.setBold(true);
      vhItem->setFont(font);
      vhItem->setToolTip(itemText);
    }
}

//-----------------------------------------------------------------------------
void
qSlicerLongitudinalPETCTReportTableWidget::updateView()
{
  Q_D(qSlicerLongitudinalPETCTReportTableWidget);
  Q_ASSERT(d->TableReport);
  Q_ASSERT(d->LabelSelectedValue);

  std::cout << "UPDATING REPORT TABLE WIDGET" << std::endl;

  if ( ! d->ReportNode)
    {
      d->TableReport->clear();
      return;
    }

  this->prepareHorizontalHeaders();
  this->prepareVerticalHeaders();
  this->updateVerticalHeaders();
  this->updateHorizontalHeaders();


  int lastSelectedStudyIndex = d->ReportNode->GetIndexOfSelectedStudy(
      d->ReportNode->GetUserSelectedStudyNode());

  if (lastSelectedStudyIndex >= 0
      && lastSelectedStudyIndex < d->ReportNode->GetNumberOfSelectedStudies())
    this->selectStudyColumn(lastSelectedStudyIndex);

  int lastSelectedFindingIndex = -1;
  if(d->ReportNode->GetUserSelectedFindingNode())
    lastSelectedFindingIndex = d->ReportNode->GetIndexOfFindingNodeID(d->ReportNode->GetUserSelectedFindingNode()->GetID());

  if (lastSelectedFindingIndex >= 0
      && lastSelectedFindingIndex < d->ReportNode->GetNumberOfFindingNodeIDs())
    this->selectFindingRow(lastSelectedFindingIndex);

  for (int i = 0; i < d->TableReport->rowCount(); ++i)
    {
      vtkSmartPointer<vtkMRMLLongitudinalPETCTFindingNode> finding =
          d->ReportNode->GetFinding(i);

      for (int j = 0; j < d->TableReport->columnCount(); ++j)
        {
          //qSlicerLongitudinalPETCTSegmentationTableCellWidget* cellWidget = qobject_cast<qSlicerLongitudinalPETCTSegmentationTableCellWidget*>(this->cellWidget(i,j));
          ctkCheckBox* cellWidget = qobject_cast<ctkCheckBox*>(
              d->TableReport->cellWidget(i, j));
          if (cellWidget != NULL)
            {

              vtkSmartPointer<vtkMRMLLongitudinalPETCTStudyNode> study =
                  d->ReportNode->GetSelectedStudy(j);

              vtkSmartPointer<vtkMRMLLongitudinalPETCTSegmentationNode> segmentation = NULL;
              if(study)
                segmentation = finding->GetSegmentationMappedByStudyNodeID(study->GetID());

              switch (this->SelMode)
                {
              case qSlicerLongitudinalPETCTReportTableWidget::AllSelectable:
                cellWidget->setIndicatorIcon(d->IconModelVisibility);
                break;
              case qSlicerLongitudinalPETCTReportTableWidget::RowSelectable:
                if(finding == d->ReportNode->GetUserSelectedFindingNode())
                  cellWidget->setIndicatorIcon(d->IconModelVisibility);
                else
                  cellWidget->setIndicatorIcon(d->IconPlaceholder);
                break;
              case qSlicerLongitudinalPETCTReportTableWidget::ColumnSelectable:
                if(study == d->ReportNode->GetUserSelectedStudyNode())
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
                          "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\"></style></head><body style=\"font-family:\'Lucida Grande\',sans-serif; font-size: 12pt; font-weight: 400; font-style: normal;border: 1px solid black;margin-top:0px;\"><table style=\"border-collapse: collapse;border-spacing: 2px 10px;margin:0;padding:0\" ><tbody>  <tr><td>SUV<span style=\"vertical-align:sub;\">MAX</span></td><td>%1</td></tr><tr><td>SUV<span style=\"vertical-align:sub;\">MEAN</span></td><td>%2</td></tr><tr><td>SUV<span style=\"vertical-align:sub;\">MIN</span></td><td>%3</td></tr></tbody></table></body></html>").arg(
                          QString().setNum(segmentation->GetSUVMax()),
                          QString().setNum(segmentation->GetSUVMean()),
                          QString().setNum(segmentation->GetSUVMin()));
                  cellWidget->setToolTip(tooltip);
                }
              else
                {
                  cellWidget->setToolTip(NULL);
                  cellWidget->setIndicatorIcon(d->IconPlaceholder);
                }

              if (i == lastSelectedFindingIndex && j == lastSelectedStudyIndex)
                {
                  QString styleSheet = cellWidget->styleSheet();

                  int insertPos = 0;

                  if( ! styleSheet.isEmpty())
                    insertPos = styleSheet.length() - 1;

                  styleSheet.insert(insertPos,
                      "border: 3px solid #DD0000;");
                  cellWidget->setStyleSheet(styleSheet);
                }
            }
        }
    }

  vtkSmartPointer<vtkMRMLLongitudinalPETCTStudyNode> study =
      d->ReportNode->GetUserSelectedStudyNode();

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

  this->arrangeColumns();

  std::cout << "END UPDATING REPORT TABLE WIDGET" << std::endl;
}


//-----------------------------------------------------------------------------
void
qSlicerLongitudinalPETCTReportTableWidget::selectStudyColumn(int index)
{
  Q_D(qSlicerLongitudinalPETCTReportTableWidget);
  Q_ASSERT(d->TableReport);

  for (int i = 0; i < d->TableReport->columnCount(); ++i)
    {
      if (i != index)
        {
          d->TableReport->horizontalHeaderItem(i)->setForeground(Qt::black);
          d->TableReport->horizontalHeaderItem(i)->setBackground(Qt::white);
        }
      else
        {
          d->TableReport->horizontalHeaderItem(i)->setForeground(Qt::white);
          d->TableReport->horizontalHeaderItem(i)->setBackground(
              d->TableHeaderSelectedBackgroundRole);
        }
    }
}

//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTReportTableWidget
::selectFindingRow(int index)
{
  Q_D(qSlicerLongitudinalPETCTReportTableWidget);
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
        }
    }
}

//-----------------------------------------------------------------------------
void
qSlicerLongitudinalPETCTReportTableWidget::arrangeColumns()
{
  Q_D(qSlicerLongitudinalPETCTReportTableWidget);
  Q_ASSERT(d->TableReport);

  for (int i = 0; i < d->TableReport->columnCount(); ++i)
    {
      d->TableReport->setColumnWidth(i, d->optimalColumnWidth());
    }
}

//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTReportTableWidget
::resizeEvent(QResizeEvent* event)
{
  Superclass::resizeEvent(event);
  this->arrangeColumns();
}


//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTReportTableWidget
::segmentationCellClicked(int row, int column)
{

  emit studyClicked(column);
  emit findingClicked(row);
}


//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTReportTableWidget
::segmentationModelVisibilityChecked(bool toggled)
{
  Q_D(qSlicerLongitudinalPETCTReportTableWidget);

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

                  if(d->ReportNode)
                    {
                      vtkMRMLLongitudinalPETCTFindingNode* finding = d->ReportNode->GetFinding(i);
                      if(finding)
                        {
                          vtkMRMLLongitudinalPETCTStudyNode* study = d->ReportNode->GetSelectedStudy(j);
                          vtkMRMLLongitudinalPETCTSegmentationNode* seg = NULL;

                          bool changeNecessary = false;

                          if(this->SelMode == RowSelectable && finding != d->ReportNode->GetUserSelectedFindingNode())
                            changeNecessary = true;
                          else if(this->SelMode == ColumnSelectable && study != d->ReportNode->GetUserSelectedStudyNode())
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



