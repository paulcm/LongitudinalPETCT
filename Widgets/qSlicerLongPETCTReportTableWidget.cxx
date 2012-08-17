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

#include <QBrush>
#include <QDate>

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


};

// --------------------------------------------------------------------------
qSlicerLongPETCTReportTableWidgetPrivate
::qSlicerLongPETCTReportTableWidgetPrivate(
  qSlicerLongPETCTReportTableWidget& object)
  : q_ptr(&object)
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
void qSlicerLongPETCTReportTableWidget
::update(vtkMRMLNode* node)
{
  Q_D(qSlicerLongPETCTReportTableWidget);

  vtkSmartPointer<vtkMRMLLongPETCTReportNode> selectedReportNode = vtkMRMLLongPETCTReportNode::SafeDownCast(node);

  if(selectedReportNode.GetPointer() == NULL)
    return;

  this->clear();
  while (this->columnCount() > 0)
    this->removeColumn(this->columnCount()-1);
  while (this->rowCount() > 0)
    this->removeRow(this->rowCount()-1);

  this->insertColumn(0);
  this->setHorizontalHeaderItem(0, new QTableWidgetItem(""));

  int selectedStudies = selectedReportNode->GetSelectedStudiesCount();

  for(int i=0; i < selectedStudies; ++i)
    {
      vtkSmartPointer<vtkMRMLLongPETCTStudyNode> study = selectedReportNode->GetSelectedStudy(i);

      if(study.GetPointer() == NULL)
        return;

      int newColumnID = this->columnCount();

      this->insertColumn(newColumnID);

      QDate date = QDate::fromString(QString(study->GetAttribute("DICOM.StudyDate")).trimmed(),"yyyyMMdd");
      QString itemText = date.toString(Qt::SystemLocaleShortDate);
      QTableWidgetItem* item = new QTableWidgetItem(itemText);

      QFont font = item->font();
      font.setBold(true);
      item->setFont(font);

      item->setToolTip(itemText);
      this->setHorizontalHeaderItem(newColumnID, item);
    }

  int lastSelectedIndex = selectedReportNode->GetIndexOfSelectedStudy(selectedReportNode->GetUserSelectedStudy());

  if(lastSelectedIndex >= 0 && lastSelectedIndex < selectedReportNode->GetSelectedStudiesCount())
    this->selectStudyColumn(lastSelectedIndex);

  this->arrangeColumns();
}


//-----------------------------------------------------------------------------
void qSlicerLongPETCTReportTableWidget
::selectStudyColumn(int index)
{

  if(index >= 0 && index < this->columnCount()-1)
    {
      QBrush background(QColor(98,140,178));
      QBrush foreground(QColor(255,255,255));
      this->horizontalHeaderItem(index+1)->setBackground(background);
      this->horizontalHeaderItem(index+1)->setForeground(foreground);
    }

}

//-----------------------------------------------------------------------------
void
qSlicerLongPETCTReportTableWidget::arrangeColumns()
{
  int width = this->width() - 4;
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





