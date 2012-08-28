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

  int ColorIDForAdding;

  bool Applied;

  vtkSmartPointer<vtkMRMLLongPETCTReportNode> ReportNode;

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
  this->ButtonRemove->setMaximumHeight(this->ButtonColor->sizeHint().height());
  this->ButtonRemove->setMaximumWidth(this->ButtonColor->sizeHint().height());

  this->ButtonAddType->setMaximumWidth(this->ButtonColorAddType->sizeHint().height());
  this->ButtonAddType->setMaximumHeight(this->ButtonColorAddType->sizeHint().height());

  QObject::connect( this->ButtonColorAddType, SIGNAL(clicked()), widget, SLOT(colorSelectionButtonClicked()) );
  QObject::connect( this->ButtonColor, SIGNAL(clicked()), widget, SLOT(colorSelectionButtonClicked()) );
  QObject::connect( this->ButtonApply, SIGNAL(clicked()), widget, SLOT(applyClicked()) );
  QObject::connect( this->ComboBoxType, SIGNAL(currentIndexChanged(int)), widget, SLOT(typeSelectionChanged(int)) );
  QObject::connect( this->ButtonAddType, SIGNAL(clicked()), widget, SLOT(addTypeButtonClicked()) );
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
  Q_Q(qSlicerLongPETCTFindingSettingsDialog);
  QObject::disconnect( this->ComboBoxType, SIGNAL(currentIndexChanged(int)), q, SLOT(typeSelectionChanged(int)) );
  this->ComboBoxType->addItem(typeName);
  QObject::connect( this->ComboBoxType, SIGNAL(currentIndexChanged(int)), q, SLOT(typeSelectionChanged(int)) );

  std::cout << "ITEM ADDED: " << typeName.toStdString().c_str() << std::endl;
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
    return d->ReportNode->GetFindingTypeColorID(d->ComboBoxType->currentText().toStdString());

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

void qSlicerLongPETCTFindingSettingsDialog::typeSelectionChanged(int index)
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);
  Q_ASSERT(d->ButtonColor);

  d->ButtonColor->setStyleSheet("");

  if(d->ReportNode == NULL || d->ReportNode->GetScene() == NULL || (index < 0 || index >= d->ReportNode->GetFindingTypesCount()) )
    return;

  const vtkMRMLColorNode* colorNode = d->ReportNode->GetColorNode();


  if(colorNode == NULL)
    return;



  QString selectedTypeName = d->ComboBoxType->currentText();
  int colorID = d->ReportNode->GetFindingTypeColorID(selectedTypeName.toStdString());

  std::cout << "COLOR ID: " << colorID << std::endl;

  double col[3];
  const_cast<vtkMRMLColorNode*>(colorNode)->GetColor(colorID, col);
  QColor color = qSlicerLongPETCTColorSelectionDialog::getRGBColorFromDoubleValues(col[0], col[1], col[2]);

  std::cout << "COLOR: " << color.name().toStdString() << std::endl;
  d->ButtonColor->setStyleSheet("background-color: "+color.name());
  std::cout << "SET COLOR" << std::endl;

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

  std::cout << "HERESETREPORT" << std::endl;

  qvtkReconnect(d->ReportNode.GetPointer(), reportNode, vtkMRMLLongPETCTReportNode::StudiesChangedEvent, this, SLOT(updateView()) );
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

  if(d->ReportNode.GetPointer() == NULL || d->ReportNode->GetScene() == NULL || d->ReportNode->GetUserSelectedFinding() == NULL)
    return;

  std::cout << "HERE" << std::endl;
  this->setFindingName(d->ReportNode->GetUserSelectedFinding()->GetName());
  std::cout << d->ReportNode->GetUserSelectedFinding()->GetName() << std::endl;

  d->ComboBoxType->clear();

  std::cout << "TYPES: "<< d->ReportNode->GetFindingTypesCount() << std::endl;

  for(int i=0; i < d->ReportNode->GetFindingTypesCount(); ++i)
    {
      d->addFindingType(QString(d->ReportNode->GetFindingType(i).first.c_str()));
    }

  std::cout << "HERE END" << std::endl;
  //d->selectFindingType(d->ReportNode->GetUserSelectedFinding()->GetFindingType().first.c_str());

}
//  if (d->ReportNode == NULL || d->ReportNode->GetScene() == NULL || d->ReportNode->GetUserSelectedFinding() == NULL)
//      return;
//
//    this->setFindingName(reportNode->GetUserSelectedFinding()->GetName());
//
//    d->ComboBoxType->clear();
//
//
//    for (int i = 0; i < d->ReportNode->GetFindingTypesCount(); ++i)
//      {
//        d->addFindingType(d->ReportNode->GetFindingType(i).first.c_str());
//      }
//
//
//    d->selectFindingType(d->ReportNode->GetUserSelectedFinding()->GetFindingType().first.c_str());



//-----------------------------------------------------------------------------
void qSlicerLongPETCTFindingSettingsDialog::colorSelectionButtonClicked()
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);
  Q_ASSERT(d->ButtonColor);
  Q_ASSERT(d->ButtonColorAddType);

  if(d->ReportNode == NULL || d->ReportNode->GetColorNode() == NULL)
    return;

  QPushButton* senderButton = qobject_cast<QPushButton*>(QObject::sender());
  Q_ASSERT(senderButton);

  if(senderButton != d->ButtonColor && senderButton != d->ButtonColorAddType)
    return;

  int selectedColorID = qSlicerLongPETCTColorSelectionDialog::colorIDSelectionForNode(this, d->ReportNode->GetColorNode());

  int possibleIndex = d->ReportNode->GetIndexOfFindingColorID(selectedColorID);
  if(possibleIndex != -1)
    {
      QString existingColorTypeName = d->ReportNode->GetFindingType(possibleIndex).first.c_str();
      if(existingColorTypeName.compare(d->ComboBoxType->currentText()) != 0 || senderButton == d->ButtonColorAddType)
        {
          QStringList message;
          message << "The Finding Type"<<existingColorTypeName<<"is already associated with this color!\nPlease select another color or change the color for"<<existingColorTypeName;
          QMessageBox::warning(NULL,"Color Selection",message.join(" "));
        }

      return;
    }

  if(selectedColorID != -1)
    {
      double c[3];
      if(const_cast<vtkMRMLColorNode*>(d->ReportNode->GetColorNode())->GetColor(selectedColorID,c))
        {
          QColor selectedColor = qSlicerLongPETCTColorSelectionDialog::getRGBColorFromDoubleValues(c[0],c[1],c[2]);
          if(selectedColor.isValid())
            {
              senderButton->setStyleSheet("background-color: "+selectedColor.name());
              d->ReportNode->SetFindingTypeColorID(d->ComboBoxType->currentText().toStdString(),selectedColorID);
            }
        }
    }

  if(senderButton == d->ButtonColorAddType)
    {
      std::cout << "SETTING COLORIDFORADDING: "<<selectedColorID << std::endl;
      d->ColorIDForAdding = selectedColorID;
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

  if(d->ReportNode == NULL)
    return;

  QString typeNameToAdd = d->LineEditNameAddType->text().trimmed();
  int indexInFindingTypes = d->ReportNode->GetIndexOfFindingTypeName(typeNameToAdd.toStdString());


  if(d->ColorIDForAdding == -1)
    {
       QMessageBox::warning(NULL,"Adding Finding Type","The the selected color is invalid.\nPlease select a valid color before adding!");
       return;
    }
  if(typeNameToAdd.isEmpty() || d->ColorIDForAdding == -1)
    {
      QMessageBox::warning(NULL,"Adding Finding Type","The Finding Type name and/or the selected color are invalid. Please enter a valid name and select a color!");
    }
  else if(indexInFindingTypes != -1 && d->ColorIDForAdding != -1)
    {
      QMessageBox::warning(NULL,"Adding Finding Type","A Finding Type with the given name is already existing. Please enter a valid name!");
    }
  else
    {
      d->ReportNode->AddFindingType(std::make_pair(typeNameToAdd.toStdString(), d->ColorIDForAdding));
      d->ComboBoxType->addItem(typeNameToAdd);
      d->LineEditNameAddType->clear();
      d->ButtonColorAddType->setStyleSheet("");
      this->typeSelectionChanged(indexInFindingTypes);
      d->ComboBoxType->setCurrentIndex(d->ComboBoxType->count()-1);
    }

}



