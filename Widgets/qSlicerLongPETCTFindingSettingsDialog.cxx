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

#include "qSlicerLongPETCTColorSelectionDialog.h"

#include <QMessageBox>
#include <vtkSmartPointer.h>
#include <QIcon>
#include <QPixmap>

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

  void addFindingTypeToComboBox(const QString& typeName, const QColor& color);
  void selectFindingType(int index);

  bool Applied;
  vtkSmartPointer<vtkMRMLLongPETCTReportNode> ReportNode;
  int ColorIDForAdding;
};

// --------------------------------------------------------------------------
qSlicerLongPETCTFindingSettingsDialogPrivate
::qSlicerLongPETCTFindingSettingsDialogPrivate(
  qSlicerLongPETCTFindingSettingsDialog& object)
  : q_ptr(&object), Applied(false), ReportNode(NULL), ColorIDForAdding(-1)
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

  int addRemoveMaxSize = this->ButtonColorAddType->sizeHint().height();

  this->ButtonRemove->setMaximumHeight(addRemoveMaxSize);
  this->ButtonRemove->setMaximumWidth(addRemoveMaxSize);

  this->ButtonAddType->setMaximumWidth(addRemoveMaxSize);
  this->ButtonAddType->setMaximumHeight(addRemoveMaxSize);


  QObject::connect( this->ButtonColorAddType, SIGNAL(clicked()), q, SLOT(colorSelectionButtonClicked()) );
  QObject::connect( this->ButtonApply, SIGNAL(clicked()), q, SLOT(applyClicked()) );
  QObject::connect( this->ButtonAddType, SIGNAL(clicked()), q, SLOT(addTypeButtonClicked()) );
  QObject::connect( this->ButtonRemove, SIGNAL(clicked()), q, SLOT(removeTypeButtonClicked()) );
  QObject::connect( this->ComboBoxType, SIGNAL(currentIndexChanged(int)), q, SLOT(selectionChanged(int)) );

}

void qSlicerLongPETCTFindingSettingsDialogPrivate
::selectFindingType(int index)
{
  if(index >= 0 && index < this->ComboBoxType->count())
    this->ComboBoxType->setCurrentIndex(index);
  else
    this->ComboBoxType->setCurrentIndex(0);
}


// --------------------------------------------------------------------------
void qSlicerLongPETCTFindingSettingsDialogPrivate::addFindingTypeToComboBox(const QString& typeName, const QColor& color)
{
  Q_ASSERT(this->ComboBoxType);

  QPixmap colorMap(24,16);
  colorMap.fill(color);
  QIcon itemIcon(colorMap);

  this->ComboBoxType->addItem(itemIcon, typeName);
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

  if(d->ReportNode == NULL)
    {
      d->ComboBoxType->setCurrentIndex(-1);
      return;
    }

  int index = d->ReportNode->GetIndexOfFindingTypeName(name.toStdString());

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

  if(d->ReportNode == NULL)
    return -1;

  int index = d->ComboBoxType->currentIndex();

  if(index >= 0 && index < d->ReportNode->GetFindingTypesCount())
    return d->ReportNode->GetFindingType(index).second;

  return -1;
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTFindingSettingsDialog::setColorID(int colorID)
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);
  Q_ASSERT(d->ComboBoxType);

  if(d->ReportNode == NULL)
    {
      d->ComboBoxType->setCurrentIndex(-1);
      return;
    }

  int index = d->ReportNode->GetIndexOfFindingColorID(colorID);

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

  qvtkReconnect(d->ReportNode.GetPointer(), reportNode, vtkMRMLLongPETCTReportNode::FindingTypesChangedEvent, this, SLOT(updateView()) );
  d->ReportNode = reportNode;

  this->updateView();
}

//-----------------------------------------------------------------------------
vtkMRMLLongPETCTReportNode* qSlicerLongPETCTFindingSettingsDialog::reportNode()
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);

  return d->ReportNode.GetPointer();
}


//-----------------------------------------------------------------------------
void qSlicerLongPETCTFindingSettingsDialog
::updateView()
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);
  Q_ASSERT(d->ComboBoxType);


  if(d->ReportNode.GetPointer() == NULL || d->ReportNode->GetColorNode() == NULL ||  d->ReportNode->GetUserSelectedFinding() == NULL)
    return;

  this->setFindingName(d->ReportNode->GetUserSelectedFinding()->GetName());

  d->ComboBoxType->clear();


  vtkMRMLColorNode* colorNode = const_cast<vtkMRMLColorNode*>(d->ReportNode->GetColorNode());

  double col[3];

  for(int i=0; i < d->ReportNode->GetFindingTypesCount(); ++i)
    {
      QString findingType = d->ReportNode->GetFindingType(i).first.c_str();
      int colorID = d->ReportNode->GetFindingType(i).second;

      colorNode->GetColor(colorID,col);

      QColor color = qSlicerLongPETCTColorSelectionDialog::getRGBColorFromDoubleValues(col[0], col[1], col[2]);

      d->addFindingTypeToComboBox(findingType, color);
    }

  d->selectFindingType(d->ReportNode->GetIndexOfFindingTypeName(d->ReportNode->GetUserSelectedFinding()->GetFindingType().first.c_str()));


}



//-----------------------------------------------------------------------------
void qSlicerLongPETCTFindingSettingsDialog::colorSelectionButtonClicked()
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);
  Q_ASSERT(d->ButtonColorAddType);

  if(d->ReportNode == NULL || d->ReportNode->GetColorNode() == NULL)
    return;

  if(QObject::sender() != d->ButtonColorAddType)
    return;

  qSlicerLongPETCTColorSelectionDialog dialog(this);
  dialog.setColorNode(d->ReportNode->GetColorNode());

  dialog.exec();
  int selectedColorID = dialog.selectedColorID();

  int possibleIndex = d->ReportNode->GetIndexOfFindingColorID(selectedColorID);

  if (possibleIndex != -1)
    {
      QString existingColorTypeName = d->ReportNode->GetFindingType(
          possibleIndex).first.c_str();

      QStringList message;
      message << "The Finding Type" << existingColorTypeName
          << "is already associated with this color!\nPlease select another color!";
      QMessageBox::warning(NULL, "Color Selection", message.join(" "));

      return;
    }

  if(selectedColorID != -1)
    {
      double c[3];
      if(const_cast<vtkMRMLColorNode*>(d->ReportNode->GetColorNode())->GetColor(selectedColorID,c))
        {
          QColor selectedColor = qSlicerLongPETCTColorSelectionDialog::getRGBColorFromDoubleValues(c[0],c[1],c[2]);
          if(selectedColor.isValid())
              d->ButtonColorAddType->setStyleSheet("background-color: "+selectedColor.name());
        }
    }


    d->ColorIDForAdding = selectedColorID;

}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTFindingSettingsDialog::removeTypeButtonClicked()
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);
  Q_ASSERT(d->ComboBoxType);

  if(d->ReportNode == NULL)
    return;

  int index = d->ComboBoxType->currentIndex();

  if(index >= vtkMRMLLongPETCTFindingNode::DefaultFindingTypes.size() && index < d->ReportNode->GetFindingTypesCount())
    {
      d->ReportNode->RemoveFindingType(index);

      if(index > 0)
        d->ComboBoxType->setCurrentIndex(index-1);
    }
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTFindingSettingsDialog::addTypeButtonClicked()
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);
  Q_ASSERT(d->LineEditNameAddType);
  Q_ASSERT(d->ComboBoxType);
  Q_ASSERT(d->ButtonColorAddType);
  Q_ASSERT(d->LineEditNameAddType);

  if(d->ReportNode.GetPointer() == NULL)
    return;

  QString typeNameToAdd = d->LineEditNameAddType->text().trimmed();
  int indexInFindingTypes = d->ReportNode->GetIndexOfFindingTypeName(typeNameToAdd.toStdString());

  if(d->ColorIDForAdding == -1)
    {
       QMessageBox::warning(NULL,"Adding Finding Type","The the selected color is invalid.\nPlease select a valid color before adding!");
       return;
    }
  else if(typeNameToAdd.isEmpty())
    {
      QMessageBox::warning(NULL,"Adding Finding Type","The Finding Type name is invalid. Please enter a valid name!");
      return;
    }
  else if(indexInFindingTypes != -1)
    {
      QMessageBox::warning(NULL,"Adding Finding Type","A Finding Type with the given name is already existing. Please enter a valid name!");
    }
  else
    {
      d->ReportNode->AddFindingType(std::make_pair(typeNameToAdd.toStdString(), d->ColorIDForAdding));
      d->LineEditNameAddType->clear();
      d->ButtonColorAddType->setStyleSheet("");

      //this->typeSelectionChanged(indexInFindingTypes);
      d->ComboBoxType->setCurrentIndex(d->ComboBoxType->count()-1);
    }

}


//-----------------------------------------------------------------------------
void qSlicerLongPETCTFindingSettingsDialog::selectionChanged(int index)
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);
  Q_ASSERT(d->ButtonRemove);

  if(d->ReportNode.GetPointer() == NULL)
    return;

  vtkMRMLLongPETCTFindingNode::FindingType type = d->ReportNode->GetFindingType(index);

  d->ButtonRemove->setDisabled(vtkMRMLLongPETCTFindingNode::IsDefaultFindingType(type));
}


