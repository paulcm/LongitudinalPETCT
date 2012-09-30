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

// LongPETCTAnalysisSettings Widgets includes
#include "qSlicerLongPETCTAnalysisSettingsWidget.h"
#include "ui_qSlicerLongPETCTAnalysisSettingsWidget.h"


#include <QList>
#include <QCheckBox>
#include <QDate>
#include <QTime>

#include <vtkSmartPointer.h>
#include <vtkMRMLLongPETCTReportNode.h>
#include <vtkMRMLLongPETCTStudyNode.h>
#include <vtkCommand.h>


#include <qMRMLNodeComboBox.h>


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongPETCT
class qSlicerLongPETCTAnalysisSettingsWidgetPrivate :
  public Ui_qSlicerLongPETCTAnalysisSettingsWidget
//  : public QWidget
{
  Q_DECLARE_PUBLIC(qSlicerLongPETCTAnalysisSettingsWidget);
protected:
  qSlicerLongPETCTAnalysisSettingsWidget* const q_ptr;

public:
  qSlicerLongPETCTAnalysisSettingsWidgetPrivate(
    qSlicerLongPETCTAnalysisSettingsWidget& object);

  virtual ~qSlicerLongPETCTAnalysisSettingsWidgetPrivate();
  virtual void setupUi(qSlicerLongPETCTAnalysisSettingsWidget* widget);

  vtkSmartPointer<vtkMRMLLongPETCTReportNode> ReportNode;

  int numberOfCheckedItems();

  //void prepareCheckBoxes();
  //void removeLastCheckBoxes(int amount);

  //QList<QCheckBox*> ListSelectedStudiesForAnalysis;
};

// --------------------------------------------------------------------------
qSlicerLongPETCTAnalysisSettingsWidgetPrivate
::qSlicerLongPETCTAnalysisSettingsWidgetPrivate(
  qSlicerLongPETCTAnalysisSettingsWidget& object)
  : q_ptr(&object), ReportNode(NULL)
{
}

// --------------------------------------------------------------------------
qSlicerLongPETCTAnalysisSettingsWidgetPrivate
::~qSlicerLongPETCTAnalysisSettingsWidgetPrivate()
{
}

// --------------------------------------------------------------------------
void qSlicerLongPETCTAnalysisSettingsWidgetPrivate
::setupUi(qSlicerLongPETCTAnalysisSettingsWidget* widget)
{
  Q_Q(qSlicerLongPETCTAnalysisSettingsWidget);

  this->Ui_qSlicerLongPETCTAnalysisSettingsWidget::setupUi(widget);

  this->WidgetButtonsPanel->setVisible(false);

  QObject::connect(this->ButtonQualitativeAnalysis, SIGNAL(clicked()), q, SIGNAL(qualitativeAnalysisClicked()) );
  QObject::connect(this->ButtonQuantitativeAnalysis, SIGNAL(clicked()), q, SIGNAL(quantitativeAnalysisClicked()) );
  QObject::connect(this->ListSelectedObjects, SIGNAL(itemChanged(QListWidgetItem*)), q, SLOT(listItemChanged(QListWidgetItem*)) );
  QObject::connect(this->ButtonVolumeRendering, SIGNAL(toggled(bool)), q, SIGNAL(volumeRenderingToggled(bool)) );
  QObject::connect(this->ButtonSpinView, SIGNAL(toggled(bool)), q, SIGNAL(spinViewToggled(bool)) );
}

// --------------------------------------------------------------------------
int qSlicerLongPETCTAnalysisSettingsWidgetPrivate
::numberOfCheckedItems()
{
  Q_ASSERT(this->ListSelectedObjects);

  int nr = 0;

  for(int i=0; i < this->ListSelectedObjects->count(); ++i)
    {
      if(this->ListSelectedObjects->item(i)->checkState() == Qt::Checked)
        nr++;
    }


  return nr;
}

//// --------------------------------------------------------------------------
//void qSlicerLongPETCTAnalysisSettingsWidgetPrivate
//::prepareCheckBoxes()
//{
//  Q_Q(qSlicerLongPETCTAnalysisSettingsWidget);
//
//  if(this->ReportNode.GetPointer() == NULL)
//    {
//      this->removeLastCheckBoxes(this->ListSelectedStudiesForAnalysis.size());
//      return;
//    }
//
//  int diff = this->ReportNode->GetSelectedStudiesCount() - this->ListSelectedStudiesForAnalysis.size();
//
//  for (int i = 0; i < std::abs(diff); ++i)
//    {
//      if (diff > 0)
//        {
//          QCheckBox* cb = new QCheckBox(q);
//          this->ListSelectedStudiesForAnalysis.append(cb);
//          this->LayoutForSelectionObjects->addWidget(cb);
//        }
//      else if(diff < 0)
//        {
//          removeLastCheckBoxes(1);
//        }
//    }
//
//
//}

// --------------------------------------------------------------------------
//void
//qSlicerLongPETCTAnalysisSettingsWidgetPrivate::removeLastCheckBoxes(int amount)
//{
//  while (this->ListSelectedStudiesForAnalysis.size() > 0 && amount > 0)
//    {
//      QCheckBox* toRemove = this->ListSelectedStudiesForAnalysis.last();
//      this->LayoutForSelectionObjects->removeWidget(toRemove);
//      this->ListSelectedStudiesForAnalysis.removeLast();
//      toRemove->deleteLater();
//
//      --amount;
//    }
//}


//-----------------------------------------------------------------------------
// qSlicerLongPETCTAnalysisSettingsWidget methods

//-----------------------------------------------------------------------------
qSlicerLongPETCTAnalysisSettingsWidget
::qSlicerLongPETCTAnalysisSettingsWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerLongPETCTAnalysisSettingsWidgetPrivate(*this) )
{
  Q_D(qSlicerLongPETCTAnalysisSettingsWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerLongPETCTAnalysisSettingsWidget
::~qSlicerLongPETCTAnalysisSettingsWidget()
{
}


//-----------------------------------------------------------------------------
void qSlicerLongPETCTAnalysisSettingsWidget::setReportNode(vtkMRMLLongPETCTReportNode* reportNode)
{
  Q_D(qSlicerLongPETCTAnalysisSettingsWidget);

  qvtkReconnect(d->ReportNode.GetPointer(), reportNode, vtkCommand::ModifiedEvent, this, SLOT(updateView()) );
  d->ReportNode = reportNode;

  this->updateView();

}


//-----------------------------------------------------------------------------
vtkMRMLLongPETCTReportNode* qSlicerLongPETCTAnalysisSettingsWidget::reportNode()
{
  Q_D(qSlicerLongPETCTAnalysisSettingsWidget);

  return d->ReportNode.GetPointer();
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTAnalysisSettingsWidget
::updateView()
{
  Q_D(qSlicerLongPETCTAnalysisSettingsWidget);
  Q_ASSERT(d->ListSelectedObjects);

  //d->prepareCheckBoxes();
  d->ListSelectedObjects->clear();

  if(d->ReportNode == NULL)
    return;

//  if(d->ReportNode->GetSelectedStudiesCount() != d->ListSelectedStudiesForAnalysis.size())
//    {
//      d->removeLastCheckBoxes(d->ListSelectedStudiesForAnalysis.size());
//      return;
//    }
  QObject::disconnect(d->ListSelectedObjects, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(listItemChanged(QListWidgetItem*)) );

  for(int i=0; i < d->ReportNode->GetSelectedStudiesCount(); ++i)
    {
      vtkSmartPointer<vtkMRMLLongPETCTStudyNode> study = d->ReportNode->GetSelectedStudy(i);

      QDate date = QDate::fromString(QString(study->GetAttribute("DICOM.StudyDate")).trimmed(),"yyyyMMdd");
      QTime time = QTime::fromString(QString(study->GetAttribute("DICOM.StudyTime")).trimmed().left(6),"hhmmss");

      QString studyID = study->GetAttribute("DICOM.StudyID");
      QString dateStr = date.toString(Qt::SystemLocaleShortDate);
      QString timeStr = time.toString(Qt::ISODate);

      QListWidgetItem* item = new QListWidgetItem(d->ListSelectedObjects);
      item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);

      if(study->GetSelectedForAnalysis())
        item->setCheckState(Qt::Checked);
      else
        item->setCheckState(Qt::Unchecked);

      item->setText("Study ID: "+studyID+"\t| Date: "+dateStr+"\t| Time:"+timeStr);

      d->ListSelectedObjects->addItem(item);
    }

  bool minOneSel = d->numberOfCheckedItems() > 0;
  d->ListSelectedObjects->setEnabled(minOneSel);

  QObject::connect(d->ListSelectedObjects, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(listItemChanged(QListWidgetItem*)) );

}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTAnalysisSettingsWidget
::listItemChanged(QListWidgetItem* item)
{
  Q_D(qSlicerLongPETCTAnalysisSettingsWidget);
  Q_ASSERT(d->ListSelectedObjects);

  int index = 0;

  for(;index < d->ListSelectedObjects->count(); ++index)
    {
      if(d->ListSelectedObjects->item(index) == item)
        break;
    }

  bool checked = item->checkState() == Qt::Checked;

  if(d->ReportNode != NULL)
    {

      bool noneSelected = d->numberOfCheckedItems() == 0;

      if(noneSelected)
        {
          QObject::disconnect(d->ListSelectedObjects, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(listItemChanged(QListWidgetItem*)) );
          item->setCheckState(Qt::Checked);
          QObject::connect(d->ListSelectedObjects, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(listItemChanged(QListWidgetItem*)) );
        }
      else
        {
          vtkSmartPointer<vtkMRMLLongPETCTStudyNode> study = d->ReportNode->GetSelectedStudy(index);

          if(study != NULL)
            {
              study->SetSelectedForAnalysis(checked);
              emit studySelectedForAnalysis(index,checked);
            }

        }

    }
}


//-----------------------------------------------------------------------------
bool qSlicerLongPETCTAnalysisSettingsWidget::volumeRendering()
{
  Q_D(qSlicerLongPETCTAnalysisSettingsWidget);
  Q_ASSERT(d->ButtonVolumeRendering);

  return d->ButtonVolumeRendering->isChecked();
}

//-----------------------------------------------------------------------------
bool qSlicerLongPETCTAnalysisSettingsWidget::spinView()
{
  Q_D(qSlicerLongPETCTAnalysisSettingsWidget);
  Q_ASSERT(d->ButtonSpinView);

  return d->ButtonSpinView->isChecked();
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTAnalysisSettingsWidget::setVolumeRendering(bool checked)
{
  Q_D(qSlicerLongPETCTAnalysisSettingsWidget);
  Q_ASSERT(d->ButtonVolumeRendering);

  d->ButtonVolumeRendering->setChecked(checked);
}


//-----------------------------------------------------------------------------
void qSlicerLongPETCTAnalysisSettingsWidget::setSpinView(bool checked)
{
  Q_D(qSlicerLongPETCTAnalysisSettingsWidget);
  Q_ASSERT(d->ButtonSpinView);

  d->ButtonSpinView->setChecked(checked);
}

