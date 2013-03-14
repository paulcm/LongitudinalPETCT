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
#include "qSlicerLongitudinalPETCTModuleSettingsDialog.h"
#include "ui_qSlicerLongitudinalPETCTModuleSettingsDialog.h"


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongitudinalPETCT
class qSlicerLongitudinalPETCTModuleSettingsDialogPrivate : public Ui_qSlicerLongitudinalPETCTModuleSettingsDialog
{
  Q_DECLARE_PUBLIC(qSlicerLongitudinalPETCTModuleSettingsDialog)
  ;
protected:
  qSlicerLongitudinalPETCTModuleSettingsDialog* const q_ptr;

public:
  qSlicerLongitudinalPETCTModuleSettingsDialogPrivate(
      qSlicerLongitudinalPETCTModuleSettingsDialog& object);

  virtual
  ~qSlicerLongitudinalPETCTModuleSettingsDialogPrivate();

  virtual void
  setupUi(qSlicerLongitudinalPETCTModuleSettingsDialog* widget);

};

// --------------------------------------------------------------------------
qSlicerLongitudinalPETCTModuleSettingsDialogPrivate::qSlicerLongitudinalPETCTModuleSettingsDialogPrivate(
    qSlicerLongitudinalPETCTModuleSettingsDialog& object) :
    q_ptr(&object)
{
}

// --------------------------------------------------------------------------
qSlicerLongitudinalPETCTModuleSettingsDialogPrivate::~qSlicerLongitudinalPETCTModuleSettingsDialogPrivate()
{
}

// --------------------------------------------------------------------------
void
qSlicerLongitudinalPETCTModuleSettingsDialogPrivate::setupUi(
    qSlicerLongitudinalPETCTModuleSettingsDialog* widget)
{
  //Q_Q(qSlicerLongitudinalPETCTModuleSettingsDialog);

  this->Ui_qSlicerLongitudinalPETCTModuleSettingsDialog::setupUi(widget);


  //QObject::connect(this->ButtonColor, SIGNAL(clicked()), q,
      //SLOT(colorSelectionButtonClicked()));

}


//-----------------------------------------------------------------------------
// qSlicerLongitudinalPETCTModuleSettingsDialog methods

//-----------------------------------------------------------------------------
qSlicerLongitudinalPETCTModuleSettingsDialog::qSlicerLongitudinalPETCTModuleSettingsDialog(
    QWidget* parentWidget) :
    Superclass(parentWidget), d_ptr(
        new qSlicerLongitudinalPETCTModuleSettingsDialogPrivate(*this))
{
  Q_D(qSlicerLongitudinalPETCTModuleSettingsDialog);
  d->setupUi(this);

  this->setWindowTitle("Longitudinal PET/CT Analysis Module Settings");
  this->setModal(true);
}

//-----------------------------------------------------------------------------
qSlicerLongitudinalPETCTModuleSettingsDialog::~qSlicerLongitudinalPETCTModuleSettingsDialog()
{
}


//-----------------------------------------------------------------------------
bool qSlicerLongitudinalPETCTModuleSettingsDialog::registration()
{
  Q_D(qSlicerLongitudinalPETCTModuleSettingsDialog);
  Q_ASSERT(d->CheckBoxRegistration);

  return d->CheckBoxRegistration->isChecked();
}

//-----------------------------------------------------------------------------
bool qSlicerLongitudinalPETCTModuleSettingsDialog::volumeRendering()
{
  Q_D(qSlicerLongitudinalPETCTModuleSettingsDialog);
  Q_ASSERT(d->CheckBoxVolumeRendering);

  return d->CheckBoxVolumeRendering->isChecked();
}

//-----------------------------------------------------------------------------
bool qSlicerLongitudinalPETCTModuleSettingsDialog::spinning()
{
  Q_D(qSlicerLongitudinalPETCTModuleSettingsDialog);
  Q_ASSERT(d->CheckBoxSpinning);

  return d->CheckBoxSpinning->isChecked();
}

//-----------------------------------------------------------------------------
bool qSlicerLongitudinalPETCTModuleSettingsDialog::makeModels()
{
  Q_D(qSlicerLongitudinalPETCTModuleSettingsDialog);
  Q_ASSERT(d->CheckBoxMakeModels);

  return d->CheckBoxMakeModels->isChecked();
}

//-----------------------------------------------------------------------------
bool qSlicerLongitudinalPETCTModuleSettingsDialog::outlineSegmentations()
{
  Q_D(qSlicerLongitudinalPETCTModuleSettingsDialog);
  Q_ASSERT(d->CheckBoxOutlineSegmentations);

  return d->CheckBoxOutlineSegmentations->isChecked();
}

//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTModuleSettingsDialog::setRegistration(bool checked)
{
  Q_D(qSlicerLongitudinalPETCTModuleSettingsDialog);
  Q_ASSERT(d->CheckBoxRegistration);

  d->CheckBoxRegistration->setChecked(checked);
}

//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTModuleSettingsDialog::setVolumeRendering(bool checked)
{
  Q_D(qSlicerLongitudinalPETCTModuleSettingsDialog);
  Q_ASSERT(d->CheckBoxVolumeRendering);

  d->CheckBoxVolumeRendering->setChecked(checked);
}

//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTModuleSettingsDialog::setSpinning(bool checked)
{
  Q_D(qSlicerLongitudinalPETCTModuleSettingsDialog);
  Q_ASSERT(d->CheckBoxSpinning);

  d->CheckBoxSpinning->setChecked(checked);
}

//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTModuleSettingsDialog::setMakeModels(bool checked)
{
  Q_D(qSlicerLongitudinalPETCTModuleSettingsDialog);
  Q_ASSERT(d->CheckBoxMakeModels);

  d->CheckBoxMakeModels->setChecked(checked);
}

//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTModuleSettingsDialog::setOutlineSegmentations(bool checked)
{
  Q_D(qSlicerLongitudinalPETCTModuleSettingsDialog);
  Q_ASSERT(d->CheckBoxOutlineSegmentations);

  d->CheckBoxOutlineSegmentations->setChecked(checked);
}


