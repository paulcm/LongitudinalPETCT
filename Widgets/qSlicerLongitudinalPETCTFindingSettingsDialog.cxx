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

// LongitudinalPETCTFindingSelection Widgets includes
#include "qSlicerLongitudinalPETCTFindingSettingsDialog.h"
#include "ui_qSlicerLongitudinalPETCTFindingSettingsDialog.h"

#include <vtkMRMLLongitudinalPETCTReportNode.h>
#include <vtkMRMLColorNode.h>
#include <vtkMRMLColorTableNode.h>
#include <vtkMRMLLongitudinalPETCTFindingNode.h>

#include "qSlicerLongitudinalPETCTColorSelectionDialog.h"

#include <QMessageBox>
#include <vtkSmartPointer.h>
#include <QIcon>
#include <QPixmap>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongitudinalPETCT
class qSlicerLongitudinalPETCTFindingSettingsDialogPrivate : public Ui_qSlicerLongitudinalPETCTFindingSettingsDialog
{
  Q_DECLARE_PUBLIC(qSlicerLongitudinalPETCTFindingSettingsDialog)
  ;
protected:
  qSlicerLongitudinalPETCTFindingSettingsDialog* const q_ptr;

public:
  qSlicerLongitudinalPETCTFindingSettingsDialogPrivate(
      qSlicerLongitudinalPETCTFindingSettingsDialog& object);

  virtual
  ~qSlicerLongitudinalPETCTFindingSettingsDialogPrivate();

  virtual void
  setupUi(qSlicerLongitudinalPETCTFindingSettingsDialog* widget);

  void
  addFindingTypeToComboBox(const QString& typeName, const QColor& color);
  void
  selectFindingType(int index);

  vtkSmartPointer<vtkMRMLLongitudinalPETCTReportNode> ReportNode;
  int ColorIDForAdding;

  QString ButtonColorDefaultText;
  QString ButtonColorDefaultStyleSheet;
  QString LineEditTypeNamePlaceholderText;

  bool UpdatingFindingTypes;
};

// --------------------------------------------------------------------------
qSlicerLongitudinalPETCTFindingSettingsDialogPrivate::qSlicerLongitudinalPETCTFindingSettingsDialogPrivate(
    qSlicerLongitudinalPETCTFindingSettingsDialog& object) :
    q_ptr(&object), ReportNode(NULL), ColorIDForAdding(-1), UpdatingFindingTypes(
        false)
{
}

// --------------------------------------------------------------------------
qSlicerLongitudinalPETCTFindingSettingsDialogPrivate::~qSlicerLongitudinalPETCTFindingSettingsDialogPrivate()
{
}

// --------------------------------------------------------------------------
void
qSlicerLongitudinalPETCTFindingSettingsDialogPrivate::setupUi(
    qSlicerLongitudinalPETCTFindingSettingsDialog* widget)
{
  Q_Q(qSlicerLongitudinalPETCTFindingSettingsDialog);

  this->Ui_qSlicerLongitudinalPETCTFindingSettingsDialog::setupUi(widget);

  this->ButtonColorDefaultStyleSheet = this->ButtonColor->styleSheet();
  this->ButtonColorDefaultText = this->ButtonColor->text();
  this->LineEditTypeNamePlaceholderText =
      this->LineEditTypeName->placeholderText();

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
  this->LineEditTypeName->setPlaceholderText(
      this->LineEditTypeNamePlaceholderText);

  QObject::connect(this->ButtonColor, SIGNAL(clicked()), q,
      SLOT(colorSelectionButtonClicked()));
  QObject::connect(this->ButtonAddType, SIGNAL(clicked()), q,
      SLOT(addTypeButtonClicked()));
  QObject::connect(this->ButtonRemove, SIGNAL(clicked()), q,
      SLOT(removeTypeButtonClicked()));
  QObject::connect(this->ComboBoxType, SIGNAL(currentIndexChanged(int)), q,
      SLOT(selectionChanged(int)));
}

void
qSlicerLongitudinalPETCTFindingSettingsDialogPrivate::selectFindingType(int index)
{
  if (index >= 0 && index < this->ComboBoxType->count())
    this->ComboBoxType->setCurrentIndex(index);
  else
    this->ComboBoxType->setCurrentIndex(0);
}

// --------------------------------------------------------------------------
void
qSlicerLongitudinalPETCTFindingSettingsDialogPrivate::addFindingTypeToComboBox(
    const QString& typeName, const QColor& color)
{
  Q_ASSERT(this->ComboBoxType);

  QPixmap colorMap(24, 16);
  colorMap.fill(color);
  QIcon itemIcon(colorMap);

  this->ComboBoxType->addItem(itemIcon, typeName);
}
//-----------------------------------------------------------------------------
// qSlicerLongitudinalPETCTFindingSettingsDialog methods

//-----------------------------------------------------------------------------
qSlicerLongitudinalPETCTFindingSettingsDialog::qSlicerLongitudinalPETCTFindingSettingsDialog(
    QWidget* parentWidget) :
    Superclass(parentWidget), d_ptr(
        new qSlicerLongitudinalPETCTFindingSettingsDialogPrivate(*this))
{
  Q_D(qSlicerLongitudinalPETCTFindingSettingsDialog);
  d->setupUi(this);

  this->setWindowTitle("Finding Settings");
  this->setModal(true);
}

//-----------------------------------------------------------------------------
qSlicerLongitudinalPETCTFindingSettingsDialog::~qSlicerLongitudinalPETCTFindingSettingsDialog()
{
}


//-----------------------------------------------------------------------------
void
qSlicerLongitudinalPETCTFindingSettingsDialog::setReportNode(
    vtkMRMLLongitudinalPETCTReportNode* reportNode)
{
  Q_D(qSlicerLongitudinalPETCTFindingSettingsDialog);

  qvtkReconnect(d->ReportNode.GetPointer(), reportNode,
      vtkCommand::ModifiedEvent, this, SLOT(updateView()));
  d->ReportNode = reportNode;

  this->updateView();
}

//-----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTReportNode*
qSlicerLongitudinalPETCTFindingSettingsDialog::reportNode()
{
  Q_D(qSlicerLongitudinalPETCTFindingSettingsDialog);

  return d->ReportNode.GetPointer();
}

//-----------------------------------------------------------------------------
void
qSlicerLongitudinalPETCTFindingSettingsDialog::updateView()
{
  Q_D(qSlicerLongitudinalPETCTFindingSettingsDialog);
  Q_ASSERT(d->ComboBoxType);
  Q_ASSERT(d->LineEditName);


  d->ComboBoxType->clear();
  if (!d->UpdatingFindingTypes)
    d->LineEditName->clear();
  d->LineEditTypeName->clear();
  d->LineEditTypeName->setPlaceholderText(d->LineEditTypeNamePlaceholderText);
  d->ButtonColor->setStyleSheet(d->ButtonColorDefaultStyleSheet);
  d->ButtonColor->setText(d->ButtonColorDefaultText);

  if (d->ReportNode.GetPointer() == NULL
      || d->ReportNode->GetColorNode() == NULL
      || d->ReportNode->GetUserSelectedFinding() == NULL)
    return;

  if (!d->UpdatingFindingTypes)
    d->LineEditName->setText(d->ReportNode->GetUserSelectedFinding()->GetName());

  vtkMRMLColorTableNode* colorTableNode =
      d->ReportNode->GetFindingTypesColorTable();

  double col[3];

  for (int i = 1; i < colorTableNode->GetNumberOfColors(); ++i) // i = 1 because "None" is not selectable
    {
      QString findingType = d->ReportNode->GetFindingTypeName(i);

      colorTableNode->GetColor(i, col);
      QColor color =
          qSlicerLongitudinalPETCTColorSelectionDialog::getRGBColorFromDoubleValues(
              col[0], col[1], col[2]);

      d->addFindingTypeToComboBox(findingType, color);
    }

  bool findingHasSegmentation =
      d->ReportNode->GetUserSelectedFinding()->GetSegmentationsCount() > 0;

  d->ComboBoxType->setDisabled(findingHasSegmentation);

  if (findingHasSegmentation)
    d->ButtonRemove->setDisabled(true);

  if (findingHasSegmentation)
    d->ExpandButton->setChecked(false);

  d->ExpandButton->setDisabled(findingHasSegmentation);

  d->selectFindingType(
      d->ReportNode->GetUserSelectedFinding()->GetColorID() - 1); // -1 because of "None" not in list

}

//-----------------------------------------------------------------------------
void
qSlicerLongitudinalPETCTFindingSettingsDialog::colorSelectionButtonClicked()
{
  Q_D(qSlicerLongitudinalPETCTFindingSettingsDialog);
  Q_ASSERT(d->ButtonColor);

  if (d->ReportNode == NULL || d->ReportNode->GetColorNode() == NULL)
    return;

  if (QObject::sender() != d->ButtonColor)
    return;

  qSlicerLongitudinalPETCTColorSelectionDialog dialog(this);
  dialog.setColorNode(d->ReportNode->GetColorNode());

  dialog.exec();
  int selectedColorID = dialog.selectedColorID();

  if (selectedColorID != -1)
    {
      double c[3];
      if (const_cast<vtkMRMLColorNode*>(d->ReportNode->GetColorNode())->GetColor(
          selectedColorID, c))
        {
          QColor selectedColor =
              qSlicerLongitudinalPETCTColorSelectionDialog::getRGBColorFromDoubleValues(
                  c[0], c[1], c[2]);
          if (selectedColor.isValid())
            {
              d->ButtonColor->setStyleSheet(
                  "background-color: " + selectedColor.name());
              d->ButtonColor->setText("");
            }
        }

    }

  d->ColorIDForAdding = selectedColorID;

}

//-----------------------------------------------------------------------------
void
qSlicerLongitudinalPETCTFindingSettingsDialog::removeTypeButtonClicked()
{
  Q_D(qSlicerLongitudinalPETCTFindingSettingsDialog);
  Q_ASSERT(d->ComboBoxType);

  if (d->ReportNode == NULL)
    return;

  int index = d->ComboBoxType->currentIndex() + 1; // +1 because of "None" color

  if (index >= d->ReportNode->GetNumberOfDefaultFindingTypes()
      && index < d->ReportNode->GetFindingTypesCount())
    {

      if (d->ReportNode->IsFindingTypeInUse(index))
        QMessageBox::information(NULL, "Removing Finding Type",
            "The selected Type has been used for an active Finding and can not be removed!");

      else
        {
          d->ReportNode->RemoveLastFindingType();

          index = index - 1; // restore index for combobox

          if (index > 0)
            d->ComboBoxType->setCurrentIndex(index - 1); // select new last entry
        }
    }
}

//-----------------------------------------------------------------------------
void
qSlicerLongitudinalPETCTFindingSettingsDialog::addTypeButtonClicked()
{
  Q_D(qSlicerLongitudinalPETCTFindingSettingsDialog);
  Q_ASSERT(d->LineEditTypeName);
  Q_ASSERT(d->ComboBoxType);
  Q_ASSERT(d->ButtonColor);

  if (d->ReportNode.GetPointer() == NULL)
    return;

  d->UpdatingFindingTypes = true;

  QString typeNameToAdd = d->LineEditTypeName->text().trimmed();
  int indexInFindingTypes = d->ReportNode->GetFindingTypeColorID(
      typeNameToAdd.toStdString());

  if (d->ColorIDForAdding == -1)
    {
      QMessageBox::warning(NULL, "Adding Finding Type",
          "The the selected color is invalid.\nPlease select a valid color before adding!");
      return;
    }
  else if (typeNameToAdd.isEmpty())
    {
      QMessageBox::warning(NULL, "Adding Finding Type",
          "The Finding Type name is invalid. Please enter a valid name!");
      return;
    }
  else if (indexInFindingTypes != -1)
    {
      QMessageBox::warning(NULL, "Adding Finding Type",
          "A Finding Type with the given name is already existing. Please enter a valid name!");
    }
  else
    {
      double color[4];
      const_cast<vtkMRMLColorNode*>(d->ReportNode->GetColorNode())->GetColor(
          d->ColorIDForAdding, color);

      d->ReportNode->AddFindingType(typeNameToAdd.toStdString(), color);

      d->LineEditTypeName->clear();
      d->LineEditTypeName->setPlaceholderText(
          d->LineEditTypeNamePlaceholderText);
      d->ButtonColor->setStyleSheet("");
      d->ButtonColor->setText(d->ButtonColorDefaultText);

      //this->typeSelectionChanged(indexInFindingTypes);
      d->ComboBoxType->setCurrentIndex(d->ComboBoxType->count() - 1);
    }

  d->UpdatingFindingTypes = false;
}

//-----------------------------------------------------------------------------
void
qSlicerLongitudinalPETCTFindingSettingsDialog::selectionChanged(int index)
{
  Q_D(qSlicerLongitudinalPETCTFindingSettingsDialog);
  Q_ASSERT(d->ButtonRemove);
  Q_ASSERT(d->ButtonBox);
  Q_ASSERT(d->LineEditName);
  Q_ASSERT(d->ComboBoxType);

  if (d->ReportNode.GetPointer() == NULL)
    return;

  if (index >= d->ReportNode->GetNumberOfDefaultFindingTypes() - 1
      && index == d->ComboBoxType->count() - 1)
    d->ButtonRemove->setEnabled(true);
  else
    d->ButtonRemove->setEnabled(false);

  vtkSmartPointer<vtkMRMLLongitudinalPETCTFindingNode> finding =
      d->ReportNode->GetUserSelectedFinding();

  QPushButton* okBtn = d->ButtonBox->button(QDialogButtonBox::Ok);

  int currentSelectedColorID = d->ComboBoxType->currentIndex() + 1;

  if (d->ReportNode->IsFindingTypeInUse(currentSelectedColorID) && finding
      && finding->GetColorID() != currentSelectedColorID)
    {
      if (okBtn)
        {
          okBtn->setDisabled(true);
          okBtn->setToolTip(
              "A Finding with the currently selected Type already exists in the workflow. Please select/create an other type.");
        }
    }
  else
    {
      if (okBtn)
        {
          okBtn->setEnabled(true);
          okBtn->setToolTip(NULL);
        }
    }

  if (finding->GetColorID() == -1)
    d->LineEditName->setText(d->ComboBoxType->currentText());
}

//-----------------------------------------------------------------------------
void
qSlicerLongitudinalPETCTFindingSettingsDialog::accept()
{
  Q_D(qSlicerLongitudinalPETCTFindingSettingsDialog);
  Q_ASSERT(d->LineEditName);
  Q_ASSERT(d->ComboBoxType);

  if(d->ReportNode)
    {
      vtkSmartPointer<vtkMRMLLongitudinalPETCTFindingNode> finding =
            d->ReportNode->GetUserSelectedFinding();

      if(finding)
        {
          if( ! d->LineEditName->text().isEmpty() )
            finding->SetName(d->LineEditName->text().toStdString().c_str());
          finding->SetTypeName(d->ReportNode->GetFindingTypeName(d->ComboBoxType->currentIndex()+1));
          finding->SetColorID(d->ComboBoxType->currentIndex()+1);
        }
      }

  Superclass::accept();
}

//-----------------------------------------------------------------------------
void
qSlicerLongitudinalPETCTFindingSettingsDialog::hide()
{
  Q_D(qSlicerLongitudinalPETCTFindingSettingsDialog);

  if(d->ReportNode)
    qvtkDisconnect(d->ReportNode.GetPointer(), vtkCommand::ModifiedEvent, this, SLOT(updateView()));

  Superclass::hide();
}


