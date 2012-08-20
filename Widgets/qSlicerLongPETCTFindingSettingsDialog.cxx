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

// LongPETCTFindingSelection Widgets includes
#include "qSlicerLongPETCTFindingSettingsDialog.h"
#include "ui_qSlicerLongPETCTFindingSettingsDialog.h"

#include <vtkMRMLLongPETCTReportNode.h>
#include <vtkMRMLColorNode.h>
#include <vtkMRMLLongPETCTFindingNode.h>

#include <QCloseEvent>

#include <QMessageBox>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongPETCT
class qSlicerLongPETCTFindingSettingsDialogPrivate :
  public Ui_qSlicerLongPETCTFindingSettingsDialog
{
  Q_DECLARE_PUBLIC(qSlicerLongPETCTFindingSettingsDialog);
protected:
  qSlicerLongPETCTFindingSettingsDialog* const q_ptr;

public:
  qSlicerLongPETCTFindingSettingsDialogPrivate(
    qSlicerLongPETCTFindingSettingsDialog& object);

  virtual ~qSlicerLongPETCTFindingSettingsDialogPrivate();

  virtual void setupUi(qSlicerLongPETCTFindingSettingsDialog* widget);
  void addFindingType(const QString& typeName);

  void selectFindingType(const QString& typeName);


  bool Applied;

  vtkMRMLLongPETCTReportNode* ReportNode;

};

// --------------------------------------------------------------------------
qSlicerLongPETCTFindingSettingsDialogPrivate
::qSlicerLongPETCTFindingSettingsDialogPrivate(
  qSlicerLongPETCTFindingSettingsDialog& object)
  : q_ptr(&object), Applied(false), ReportNode(NULL)
{
}

// --------------------------------------------------------------------------
qSlicerLongPETCTFindingSettingsDialogPrivate
::~qSlicerLongPETCTFindingSettingsDialogPrivate()
{
}

// --------------------------------------------------------------------------
void qSlicerLongPETCTFindingSettingsDialogPrivate
::setupUi(qSlicerLongPETCTFindingSettingsDialog* widget)
{
  Q_Q(qSlicerLongPETCTFindingSettingsDialog);

  this->Ui_qSlicerLongPETCTFindingSettingsDialog::setupUi(widget);

  this->ExpandButton->setOrientation(Qt::Vertical);
  this->ExpandButton->mirrorOnExpand();
  this->WidgetAddType->setVisible(false);

  this->LabelAddType->setMinimumWidth(this->LabelName->sizeHint().width());
  this->ButtonRemove->setMaximumHeight(this->ButtonColor->sizeHint().height());
  this->ButtonRemove->setMaximumWidth(this->ButtonColor->sizeHint().height());

  this->ButtonAddType->setMaximumWidth(this->ButtonColorAddType->sizeHint().height());
  this->ButtonAddType->setMaximumHeight(this->ButtonColorAddType->sizeHint().height());


  QObject::connect( this->ButtonApply, SIGNAL(clicked()), widget, SLOT(applyClicked()) );
  QObject::connect( this->ComboBoxType, SIGNAL(currentIndexChanged(int)), widget, SLOT(typeSelectionChanged(int)) );

}

void qSlicerLongPETCTFindingSettingsDialogPrivate
::selectFindingType(const QString& typeName)
{
  for(int i=0; i < this->ComboBoxType->count(); ++i)
    {
      if(this->ComboBoxType->itemText(i).compare(typeName) == 0)
          {
            this->ComboBoxType->setCurrentIndex(i);
            return;
          }
    }

  this->ComboBoxType->setCurrentIndex(0); // if type was not found in combobox
}


// --------------------------------------------------------------------------
void qSlicerLongPETCTFindingSettingsDialogPrivate::addFindingType(const QString& typeName)
{
  this->ComboBoxType->addItem(typeName);
}
//-----------------------------------------------------------------------------
// qSlicerLongPETCTFindingSettingsDialog methods

//-----------------------------------------------------------------------------
qSlicerLongPETCTFindingSettingsDialog
::qSlicerLongPETCTFindingSettingsDialog(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerLongPETCTFindingSettingsDialogPrivate(*this) )
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);
  d->setupUi(this);

  this->setWindowTitle("Finding Settings");
  this->setModal(true);
}

//-----------------------------------------------------------------------------
qSlicerLongPETCTFindingSettingsDialog
::~qSlicerLongPETCTFindingSettingsDialog()
{
}

//-----------------------------------------------------------------------------
QString qSlicerLongPETCTFindingSettingsDialog::findingName()
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);
  Q_ASSERT(d->LineEditName);

  QString tempName = d->LineEditName->text();

  return d->LineEditName->text();
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTFindingSettingsDialog::setFindingName(const QString& name)
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);
  Q_ASSERT(d->LineEditName);

  return d->LineEditName->setText(name);
}

//-----------------------------------------------------------------------------
QString qSlicerLongPETCTFindingSettingsDialog::typeName()
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);
  Q_ASSERT(d->ComboBoxType);

  return d->ComboBoxType->currentText();
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTFindingSettingsDialog::setTypeName(const QString& name)
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);
  Q_ASSERT(d->ComboBoxType);

  int index = vtkMRMLLongPETCTReportNode::GetIndexFindingTypeName(name.toStdString());

  if(index >= 0 && index < d->ComboBoxType->count())
    d->ComboBoxType->setCurrentIndex(index);

  else
    d->ComboBoxType->setCurrentIndex(-1);
}

//-----------------------------------------------------------------------------
int qSlicerLongPETCTFindingSettingsDialog::colorID()
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);
  Q_ASSERT(d->ComboBoxType);

  int index = d->ComboBoxType->currentIndex();

  if(index >= 0 && index < vtkMRMLLongPETCTReportNode::GetFindingTypes().size())
    return vtkMRMLLongPETCTReportNode::GetFindingTypes().at(index).first;

  return -1;
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTFindingSettingsDialog::setColorID(int colorID)
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);
  Q_ASSERT(d->ComboBoxType);

  int index = vtkMRMLLongPETCTReportNode::GetIndexFindingColorID(colorID);

  if(index >= 0 && index < d->ComboBoxType->count())
    d->ComboBoxType->setCurrentIndex(index);

  else
    d->ComboBoxType->setCurrentIndex(-1);
}



//-----------------------------------------------------------------------------
bool qSlicerLongPETCTFindingSettingsDialog::applied()
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);

  return d->Applied;
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTFindingSettingsDialog::setApplied(bool applied)
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);

  d->Applied = applied;
}
//-----------------------------------------------------------------------------
void qSlicerLongPETCTFindingSettingsDialog::applyClicked()
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);

  d->Applied = true;
  this->close();
}

void qSlicerLongPETCTFindingSettingsDialog::typeSelectionChanged(int index)
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);
  Q_ASSERT(d->ButtonColor);

  d->ButtonColor->setStyleSheet("");

  if(d->ReportNode == NULL || d->ReportNode->GetScene() == NULL || (index < 0 || index >= vtkMRMLLongPETCTReportNode::GetFindingTypes().size()) )
    return;

  const vtkMRMLColorNode* colorNode = d->ReportNode->GetColorNode();

  if(colorNode == NULL)
    return;

  int colorID = vtkMRMLLongPETCTReportNode::GetFindingTypes().at(index).first;

  QString colorName = const_cast<vtkMRMLColorNode*>(colorNode)->GetColorName(colorID);
  double col[3];
  const_cast<vtkMRMLColorNode*>(colorNode)->GetColor(colorID, col);
  QColor color(col[0] * 255, col[1] * 255, col[2] * 255);

  d->ButtonColor->setStyleSheet("background-color: "+color.name());
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTFindingSettingsDialog::show()
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);
  d->Applied = false;

  Superclass::show();
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTFindingSettingsDialog::setReportNode(vtkMRMLLongPETCTReportNode* reportNode)
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);

  d->ReportNode = reportNode;

  if (d->ReportNode == NULL || d->ReportNode->GetScene() == NULL || d->ReportNode->GetUserSelectedFinding() == NULL)
      return;

    this->setFindingName(reportNode->GetUserSelectedFinding()->GetName());

    d->ComboBoxType->clear();


    for (int i = 0; i < vtkMRMLLongPETCTReportNode::GetFindingTypes().size(); ++i)
      {
        d->addFindingType(vtkMRMLLongPETCTReportNode::GetFindingTypes().at(i).second.c_str());
      }

    d->selectFindingType(d->ReportNode->GetUserSelectedFinding()->GetFindingType().second.c_str());

}




