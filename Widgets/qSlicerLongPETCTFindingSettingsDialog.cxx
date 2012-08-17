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

  bool applied;

};

// --------------------------------------------------------------------------
qSlicerLongPETCTFindingSettingsDialogPrivate
::qSlicerLongPETCTFindingSettingsDialogPrivate(
  qSlicerLongPETCTFindingSettingsDialog& object)
  : q_ptr(&object), applied(false)
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
  this->ColorTableComboBox->setNodeTypes(QStringList("vtkMRMLColorNode"));

  QObject::connect( this->ButtonApply, SIGNAL(clicked()), widget, SLOT(applyClicked()) );

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
void qSlicerLongPETCTFindingSettingsDialog::setMRMLScene(vtkMRMLScene* mrmlScene)
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);
  Q_ASSERT(d->ColorTableComboBox);

  d->ColorTableComboBox->setAddEnabled(false);
  d->ColorTableComboBox->setNoneEnabled(false);
  d->ColorTableComboBox->setSelectNodeUponCreation(true);
  d->ColorTableComboBox->setShowHidden(true);
  d->ColorTableComboBox->showChildNodeTypes();
  QStringList list;


  d->ColorTableComboBox->setMRMLScene(mrmlScene);
}

//-----------------------------------------------------------------------------
vtkMRMLScene* qSlicerLongPETCTFindingSettingsDialog::mrmlScene()
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);

  return NULL; //d->MRMLNodeComboBoxFinding->mrmlScene();
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

  return d->applied;
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTFindingSettingsDialog::setApplied(bool applied)
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);

  d->applied = applied;
}
//-----------------------------------------------------------------------------
void qSlicerLongPETCTFindingSettingsDialog::applyClicked()
{
  Q_D(qSlicerLongPETCTFindingSettingsDialog);

  d->applied = true;

  this->close();
}
