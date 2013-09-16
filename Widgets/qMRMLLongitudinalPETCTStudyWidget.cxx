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

// LongitudinalPETCTStudySelection Widgets includes
#include "qMRMLLongitudinalPETCTStudyWidget.h"
#include "ui_qMRMLLongitudinalPETCTStudyWidget.h"

#include <QCheckBox>
#include <QDate>
#include <QTime>

#include <ctkMessageBox.h>

#include <vtkSmartPointer.h>
#include <vtkMRMLLongitudinalPETCTReportNode.h>
#include <vtkMRMLLongitudinalPETCTStudyNode.h>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongitudinalitudinalPETCT
class qMRMLLongitudinalPETCTStudyWidgetPrivate : public Ui_qMRMLLongitudinalPETCTStudyWidget
{
  Q_DECLARE_PUBLIC (qMRMLLongitudinalPETCTStudyWidget);
protected:
  qMRMLLongitudinalPETCTStudyWidget* const q_ptr;

public:
  qMRMLLongitudinalPETCTStudyWidgetPrivate(
      qMRMLLongitudinalPETCTStudyWidget& object);

  virtual
  ~qMRMLLongitudinalPETCTStudyWidgetPrivate();
  virtual void
  setupUi(qMRMLLongitudinalPETCTStudyWidget*);

};

// --------------------------------------------------------------------------
qMRMLLongitudinalPETCTStudyWidgetPrivate::qMRMLLongitudinalPETCTStudyWidgetPrivate(
    qMRMLLongitudinalPETCTStudyWidget& object) :
    q_ptr(&object)
{
}

// --------------------------------------------------------------------------
qMRMLLongitudinalPETCTStudyWidgetPrivate::~qMRMLLongitudinalPETCTStudyWidgetPrivate()
{
}

// --------------------------------------------------------------------------
void
qMRMLLongitudinalPETCTStudyWidgetPrivate::setupUi(
    qMRMLLongitudinalPETCTStudyWidget* widget)
{
  Q_Q(qMRMLLongitudinalPETCTStudyWidget);

  this->Ui_qMRMLLongitudinalPETCTStudyWidget::setupUi(widget);



  this->TableStudySelection->setTableMode(
      qMRMLLongitudinalPETCTStudyTableWidget::SegmentationSelection);

  this->LabelInfo->setToolTip(
      "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\"></style></head><body style=\"font-family:\'Lucida Grande\',sans-serif; font-size: 12pt; font-weight: 400; font-style: normal;border: 1px solid black;margin-top:0px;\"><table cellspacing=\"5\"><tbody><tr><td>-</td><td>Add/remove Studies to/from the workflow</td></tr></tbody></table></body></html>");

  QObject::connect(this->TableStudySelection,
      SIGNAL(studySelectionChanged(int, bool)), q,
      SLOT(studySelectedInTable(int, bool)));

}

//-----------------------------------------------------------------------------
// qMRMLLongitudinalPETCTStudyWidget methods

//-----------------------------------------------------------------------------
qMRMLLongitudinalPETCTStudyWidget::qMRMLLongitudinalPETCTStudyWidget(
    QWidget* parentWidget) :
    Superclass(parentWidget), d_ptr(
        new qMRMLLongitudinalPETCTStudyWidgetPrivate(*this))
{
  Q_D (qMRMLLongitudinalPETCTStudyWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qMRMLLongitudinalPETCTStudyWidget::~qMRMLLongitudinalPETCTStudyWidget()
{
}

//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTStudyWidget::setReportNode(vtkMRMLLongitudinalPETCTReportNode* reportNode)
{
  Superclass::setReportNode(reportNode);

  Q_D (qMRMLLongitudinalPETCTStudyWidget);

  d->TableStudySelection->setReportNode(reportNode);
}


//-----------------------------------------------------------------------------
void
qMRMLLongitudinalPETCTStudyWidget::studySelectedInTable(int index,
    bool selected)
{
  vtkMRMLLongitudinalPETCTReportNode* report = this->reportNode();

  if (report)
    {
      vtkMRMLLongitudinalPETCTStudyNode* study = report->GetStudy(index);

      if (study)
        emit studySelectedForSegmentation(study, selected);
    }
}





