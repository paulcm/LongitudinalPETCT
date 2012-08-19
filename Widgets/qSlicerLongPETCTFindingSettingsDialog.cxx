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
  void AddRowToTable(const QString& findingType, const QString& colorName, const QColor& color);

  std::vector<vtkMRMLLongPETCTFindingNode::FindingType> FindingTypes;

  bool Applied;


};

// --------------------------------------------------------------------------
qSlicerLongPETCTFindingSettingsDialogPrivate
::qSlicerLongPETCTFindingSettingsDialogPrivate(
  qSlicerLongPETCTFindingSettingsDialog& object)
  : q_ptr(&object), Applied(false)
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

  QObject::connect( this->ButtonApply, SIGNAL(clicked()), widget, SLOT(applyClicked()) );
}

// --------------------------------------------------------------------------
void qSlicerLongPETCTFindingSettingsDialogPrivate::AddRowToTable(const QString& findingType, const QString& colorName, const QColor& color)
{
  int currentRow = this->Table->rowCount();

  this->Table->insertRow(currentRow);

  QTableWidgetItem* type = new QTableWidgetItem(findingType);
  QTableWidgetItem* name = new QTableWidgetItem(colorName);

  QPushButton* colorButton = new QPushButton(this->Table);
  colorButton->setStyleSheet("background-color: "+color.name());

  this->Table->setItem(currentRow,0,type);
  this->Table->setItem(currentRow,1,name);
  this->Table->setCellWidget(currentRow,2,colorButton);
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
void
qSlicerLongPETCTFindingSettingsDialog::update(vtkMRMLLongPETCTReportNode* node)
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);
  Q_ASSERT(d->LineEditName);
  Q_ASSERT(d->Table);


  vtkMRMLLongPETCTReportNode* selectedReportNode =
      vtkMRMLLongPETCTReportNode::SafeDownCast(node);

  if (selectedReportNode != NULL && selectedReportNode->GetUserSelectedFinding() != NULL)
    {

      while (d->Table->rowCount() > 0)
        {
          d->Table->removeRow(d->Table->rowCount() - 1);
        }

      vtkMRMLColorNode* colorNode = selectedReportNode->GetColorNode();

      d->LineEditName->setText(selectedReportNode->GetUserSelectedFinding()->GetName());

      if (colorNode != NULL)
        {
          d->FindingTypes.clear();
          d->FindingTypes = selectedReportNode->GetFindingTypes();

          for (int i = 0; i < d->FindingTypes.size(); ++i)
            {
              int value = d->FindingTypes.at(i).colorID;
              QString type = d->FindingTypes.at(i).typeName.c_str();
              QString colorName = colorNode->GetColorName(value);
              double col[3];
              colorNode->GetColor(value, col);
              QColor color(col[0] * 255, col[1] * 255, col[2] * 255);
              d->AddRowToTable(type, colorName, color);
            }
        }
    }
}

//-----------------------------------------------------------------------------
void
qSlicerLongPETCTFindingSettingsDialog::execDialog()
{
  this->exec();
}

