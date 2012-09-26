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
#include <vtkMRMLColorTableNode.h>
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

  vtkSmartPointer<vtkMRMLLongPETCTReportNode> ReportNode;
  int ColorIDForAdding;

  QString ButtonColorDefaultText;
  QString ButtonColorDefaultStyleSheet;
  QString LineEditTypeNamePlaceholderText;
};

// --------------------------------------------------------------------------
qSlicerLongPETCTFindingSettingsDialogPrivate
::qSlicerLongPETCTFindingSettingsDialogPrivate(
  qSlicerLongPETCTFindingSettingsDialog& object)
  : q_ptr(&object), ReportNode(NULL), ColorIDForAdding(-1)
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

  this->ButtonColorDefaultStyleSheet = this->ButtonColor->styleSheet();
  this->ButtonColorDefaultText = this->ButtonColor->text();
  this->LineEditTypeNamePlaceholderText = this->LineEditTypeName->placeholderText();

  this->ExpandButton->setOrientation(Qt::Vertical);
  this->ExpandButton->mirrorOnExpand();
  this->ExpandButton->setChecked(false);

  this->LabelAddType->setMinimumWidth(this->LabelName->sizeHint().width());

  int addRemoveMaxSize = this->ButtonColor->sizeHint().height();

  this->ButtonRemove->setMaximumHeight(addRemoveMaxSize);
  this->ButtonRemove->setMaximumWidth(addRemoveMaxSize);

  this->ButtonAddType->setMaximumWidth(addRemoveMaxSize);
  this->ButtonAddType->setMaximumHeight(addRemoveMaxSize);

  this->ButtonColor->setText(this->ButtonColorDefaultText);
  this->ButtonColor->setStyleSheet(this->ButtonColorDefaultStyleSheet);
  this->LineEditTypeName->setPlaceholderText(this->LineEditTypeNamePlaceholderText);

  QObject::connect( this->ButtonColor, SIGNAL(clicked()), q, SLOT(colorSelectionButtonClicked()) );
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
    this->ComboBoxType->setCurrentIndex(-1);
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

  QString empty;

  if(d->ReportNode == NULL)
    return empty;

  int index = d->ComboBoxType->currentIndex() + 1; // +1 because "None" is not in list

  if(index >= 0 && index < d->ReportNode->GetFindingTypesCount())
    return QString(d->ReportNode->GetFindingTypeName(index).c_str());

  return empty;
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

  int index = d->ReportNode->GetFindingTypeColorID(name.toStdString()) -1; // -1 because "None" is not in list

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

  int index = d->ComboBoxType->currentIndex() +1; // +1 because "None" is not in list

  return index;

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

  int index = colorID -1; // -1 because "None" is not in list

  if(index >= 0 && index < d->ComboBoxType->count())
    d->ComboBoxType->setCurrentIndex(index);

  else
    d->ComboBoxType->setCurrentIndex(-1);
}


//-----------------------------------------------------------------------------
void qSlicerLongPETCTFindingSettingsDialog::setReportNode(vtkMRMLLongPETCTReportNode* reportNode)
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);

  qvtkReconnect(d->ReportNode.GetPointer(), reportNode, vtkCommand::ModifiedEvent, this, SLOT(updateView()) );
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

  d->ComboBoxType->clear();
  d->LineEditName->clear();
  d->LineEditTypeName->clear();
  d->LineEditTypeName->setPlaceholderText(d->LineEditTypeNamePlaceholderText);
  d->ButtonColor->setStyleSheet(d->ButtonColorDefaultStyleSheet);
  d->ButtonColor->setText(d->ButtonColorDefaultText);

  if(d->ReportNode.GetPointer() == NULL || d->ReportNode->GetColorNode() == NULL ||  d->ReportNode->GetUserSelectedFinding() == NULL)
    return;

  this->setFindingName(d->ReportNode->GetUserSelectedFinding()->GetName());

  vtkMRMLColorTableNode* colorTableNode = d->ReportNode->GetFindingTypesColorTable();

  double col[3];

  for(int i=1; i < colorTableNode->GetNumberOfColors(); ++i) // i = 1 because "None" is not selectable
    {
      QString findingType = d->ReportNode->GetFindingTypeName(i).c_str();

      colorTableNode->GetColor(i,col);
      QColor color = qSlicerLongPETCTColorSelectionDialog::getRGBColorFromDoubleValues(col[0], col[1], col[2]);

      d->addFindingTypeToComboBox(findingType, color);
    }

  bool findingHasSegmentation = d->ReportNode->GetUserSelectedFinding()->GetSegmentationsCount() > 0;

  d->ComboBoxType->setDisabled(findingHasSegmentation);

  if(findingHasSegmentation)
    d->ButtonRemove->setDisabled(true);

  if(findingHasSegmentation)
    d->ExpandButton->setChecked(false);

  d->ExpandButton->setDisabled(findingHasSegmentation);

  d->selectFindingType(d->ReportNode->GetUserSelectedFinding()->GetColorID()-1); // -1 because of "None" not in list
}



//-----------------------------------------------------------------------------
void qSlicerLongPETCTFindingSettingsDialog::colorSelectionButtonClicked()
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);
  Q_ASSERT(d->ButtonColor);

  if(d->ReportNode == NULL || d->ReportNode->GetColorNode() == NULL)
    return;

  if(QObject::sender() != d->ButtonColor)
    return;

  qSlicerLongPETCTColorSelectionDialog dialog(this);
  dialog.setColorNode(d->ReportNode->GetColorNode());

  dialog.exec();
  int selectedColorID = dialog.selectedColorID();

//  int possibleIndex = d->ReportNode->GetIndexOfFindingColorID(selectedColorID);
//
//  if (possibleIndex != -1)
//    {
//      QString existingColorTypeName = d->ReportNode->GetFindingType(
//          possibleIndex).first.c_str();
//
//      QStringList message;
//      message << "The Finding Type" << existingColorTypeName
//          << "is already associated with this color!\nPlease select another color!";
//      QMessageBox::warning(NULL, "Color Selection", message.join(" "));
//
//      return;
//    }

  if(selectedColorID != -1)
    {
      double c[3];
      if(const_cast<vtkMRMLColorNode*>(d->ReportNode->GetColorNode())->GetColor(selectedColorID,c))
        {
          QColor selectedColor = qSlicerLongPETCTColorSelectionDialog::getRGBColorFromDoubleValues(c[0],c[1],c[2]);
          if(selectedColor.isValid())
            {
              d->ButtonColor->setStyleSheet("background-color: "+selectedColor.name());
              d->ButtonColor->setText("");
            }
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

  int index = d->ComboBoxType->currentIndex() + 1; // +1 because of "None" color

  if(index >= d->ReportNode->GetNumberOfDefaultFindingTypes() && index < d->ReportNode->GetFindingTypesCount())
    {

      if(d->ReportNode->IsFindingTypeInUse(index))
        QMessageBox::information(NULL,"Removing Finding Type","The selected Type has been used for an active Finding and can not be removed!");

      else
        {
          d->ReportNode->RemoveLastFindingType();

          index = index -1; // restore index for combobox

          if(index > 0)
            d->ComboBoxType->setCurrentIndex(index-1); // select new last entry
        }
    }
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTFindingSettingsDialog::addTypeButtonClicked()
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);
  Q_ASSERT(d->LineEditTypeName);
  Q_ASSERT(d->ComboBoxType);
  Q_ASSERT(d->ButtonColor);

  if(d->ReportNode.GetPointer() == NULL)
    return;

  QString typeNameToAdd = d->LineEditTypeName->text().trimmed();
  int indexInFindingTypes = d->ReportNode->GetFindingTypeColorID(typeNameToAdd.toStdString());

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
      double color[4];
      const_cast<vtkMRMLColorNode*>(d->ReportNode->GetColorNode())->GetColor(d->ColorIDForAdding,color);
      d->ReportNode->AddFindingType(typeNameToAdd.toStdString(),color);

      d->LineEditTypeName->clear();
      d->LineEditTypeName->setPlaceholderText(d->LineEditTypeNamePlaceholderText);
      d->ButtonColor->setStyleSheet("");
      d->ButtonColor->setText(d->ButtonColorDefaultText);

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

  if(index >= d->ReportNode->GetNumberOfDefaultFindingTypes()-1 && index == d->ComboBoxType->count()-1)
    d->ButtonRemove->setEnabled(true);
  else
    d->ButtonRemove->setEnabled(false);
}




