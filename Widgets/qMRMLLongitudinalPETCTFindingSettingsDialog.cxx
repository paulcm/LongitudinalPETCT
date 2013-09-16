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
#include "qMRMLLongitudinalPETCTFindingSettingsDialog.h"
#include "ui_qMRMLLongitudinalPETCTFindingSettingsDialog.h"

#include <vtkMRMLLongitudinalPETCTReportNode.h>
#include <vtkMRMLLongitudinalPETCTFindingNode.h>


#include <QMessageBox>

#include <QAbstractButton>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongitudinalPETCT
class qMRMLLongitudinalPETCTFindingSettingsDialogPrivate : public Ui_qMRMLLongitudinalPETCTFindingSettingsDialog
{
  Q_DECLARE_PUBLIC(qMRMLLongitudinalPETCTFindingSettingsDialog);

protected:
  qMRMLLongitudinalPETCTFindingSettingsDialog* const q_ptr;

public:
  qMRMLLongitudinalPETCTFindingSettingsDialogPrivate(
      qMRMLLongitudinalPETCTFindingSettingsDialog& object);

  virtual
  ~qMRMLLongitudinalPETCTFindingSettingsDialogPrivate();

  virtual void
  setupUi(qMRMLLongitudinalPETCTFindingSettingsDialog* widget);

  QString findingName;
  int findingColorID;

};

// --------------------------------------------------------------------------
qMRMLLongitudinalPETCTFindingSettingsDialogPrivate::qMRMLLongitudinalPETCTFindingSettingsDialogPrivate(
    qMRMLLongitudinalPETCTFindingSettingsDialog& object) :
    q_ptr(&object), findingName(""), findingColorID(-1)
{
}

// --------------------------------------------------------------------------
qMRMLLongitudinalPETCTFindingSettingsDialogPrivate::~qMRMLLongitudinalPETCTFindingSettingsDialogPrivate()
{
}

// --------------------------------------------------------------------------
void
qMRMLLongitudinalPETCTFindingSettingsDialogPrivate::setupUi(
    qMRMLLongitudinalPETCTFindingSettingsDialog* widget)
{
  Q_Q(qMRMLLongitudinalPETCTFindingSettingsDialog);

  this->Ui_qMRMLLongitudinalPETCTFindingSettingsDialog::setupUi(widget);
  this->ComboBoxColor->setNoneEnabled(false);

  QObject::connect( this->ButtonGroupFindingPresets, SIGNAL(buttonClicked(QAbstractButton*)), q, SLOT(presetButtonClicked(QAbstractButton*)) );
}



//-----------------------------------------------------------------------------
// qMRMLLongitudinalPETCTFindingSettingsDialog methods

//-----------------------------------------------------------------------------
qMRMLLongitudinalPETCTFindingSettingsDialog::qMRMLLongitudinalPETCTFindingSettingsDialog(QWidget* parentWidget) :
    Superclass(parentWidget), d_ptr(
        new qMRMLLongitudinalPETCTFindingSettingsDialogPrivate(*this))
{
  Q_D(qMRMLLongitudinalPETCTFindingSettingsDialog);
  d->setupUi(this);

  this->setWindowTitle("Finding Settings");
  this->setModal(true);

}

//-----------------------------------------------------------------------------
qMRMLLongitudinalPETCTFindingSettingsDialog::~qMRMLLongitudinalPETCTFindingSettingsDialog()
{
}



//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTFindingSettingsDialog::accept()
{
  Q_D(qMRMLLongitudinalPETCTFindingSettingsDialog);
  Q_ASSERT(d->ComboBoxColor);

  QString name = d->LineEditName->text();
  int id = d->ComboBoxColor->currentColor();

  d->findingName = name;
  d->findingColorID = id;


  double c[4];

  vtkMRMLLongitudinalPETCTReportNode* report = this->reportNode();

  if(!report)
    return;

  vtkMRMLColorNode* cn = const_cast<vtkMRMLColorTableNode*>(this->reportNode()->GetColorTableNode());

  if (cn->GetColor(findingColorID(), c))
    {
      if (c[0] == 0. && c[1] == 0. && c[2] == 0.)
        {
          QMessageBox::warning(this, "Finding Settings Warning",
              "The selected color seems to be a background color, which can not be used for Findings. Please select another color");
        }
      else if (report->IsFindingColorInUse(findingColorID()))
        {
          QMessageBox::warning(this, "Finding Settings Warning",
              "The selected color is already assigned to a different Finding. Please select another color.");
        }
      else if (d->findingName.isEmpty())
        {
          QMessageBox::warning(this, "Finding Settings Warning",
              "The Finding name is invalid, please type in a valid name.");
        }
      else if (report->IsFindingNameInUse(d->findingName.toStdString().c_str()))
        {
          QMessageBox::warning(this, "Finding Settings Warning",
              "The Finding name is already in use, please select another name.");
        }
      else
        {
          emit findingSpecified(name, id);
          Superclass::accept();
        }
    }
}

void qMRMLLongitudinalPETCTFindingSettingsDialog::updateDialogFromMRML()
{
  Q_D(qMRMLLongitudinalPETCTFindingSettingsDialog);
  Q_ASSERT(d->ComboBoxColor);
  Q_ASSERT(d->ButtonPresetTumor);


  vtkMRMLLongitudinalPETCTReportNode* report = this->reportNode();

  if(report)
    {
      d->ComboBoxColor->setMRMLColorNode(const_cast<vtkMRMLColorTableNode*>(report->GetColorTableNode()));
      d->ButtonPresetTumor->click();
    }

}


//-----------------------------------------------------------------------------
void
qMRMLLongitudinalPETCTFindingSettingsDialog::presetButtonClicked(
    QAbstractButton* button)
{
  Q_D(qMRMLLongitudinalPETCTFindingSettingsDialog);
  Q_ASSERT(d->LineEditName);
  Q_ASSERT(d->ComboBoxColor);

  QString name = button->accessibleName();

  if (name.compare("Tumor") == 0)
    {
      d->LineEditName->setText("Tumor");
      d->ComboBoxColor->setCurrentColor(7);
    }
  else if (name.compare("Node") == 0)
    {
      d->LineEditName->setText("Lymph Node");
      d->ComboBoxColor->setCurrentColor(23);
    }
  else if (name.compare("Liver") == 0)
    {
      d->LineEditName->setText("Liver");
      d->ComboBoxColor->setCurrentColor(216);
    }
  else if (name.compare("Cerebellum") == 0)
    {
      d->LineEditName->setText("Cerebellum");
      d->ComboBoxColor->setCurrentColor(105);
    }
  else if (name.compare("Aorta") == 0)
    {
      d->LineEditName->setText("Aorta");
      d->ComboBoxColor->setCurrentColor(191);
    }
}


//-----------------------------------------------------------------------------
QString
qMRMLLongitudinalPETCTFindingSettingsDialog::findingName()
{
  Q_D(qMRMLLongitudinalPETCTFindingSettingsDialog);
  return d->findingName;
}

//-----------------------------------------------------------------------------
int
qMRMLLongitudinalPETCTFindingSettingsDialog::findingColorID()
{
  Q_D(qMRMLLongitudinalPETCTFindingSettingsDialog);
  return d->findingColorID;
}

//-----------------------------------------------------------------------------
void
qMRMLLongitudinalPETCTFindingSettingsDialog::setFindingName(const QString& name)
{
  Q_D(qMRMLLongitudinalPETCTFindingSettingsDialog);
  d->findingName = name;
}

//-----------------------------------------------------------------------------
void
qMRMLLongitudinalPETCTFindingSettingsDialog::setFindingColorID(int id)
{
  Q_D(qMRMLLongitudinalPETCTFindingSettingsDialog);
  d->findingColorID = id;
}
