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
#include "qSlicerLongPETCTColorSelectionDialog.h"
#include "ui_qSlicerLongPETCTColorSelectionDialog.h"

#include <vtkMRMLColorNode.h>

#include <QIcon>
#include <QPixmap>
#include <QListWidgetItem>


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongPETCT
class qSlicerLongPETCTColorSelectionDialogPrivate :
  public Ui_qSlicerLongPETCTColorSelectionDialog
{
  Q_DECLARE_PUBLIC(qSlicerLongPETCTColorSelectionDialog);
protected:
  qSlicerLongPETCTColorSelectionDialog* const q_ptr;

public:
  qSlicerLongPETCTColorSelectionDialogPrivate(
    qSlicerLongPETCTColorSelectionDialog& object);

  virtual ~qSlicerLongPETCTColorSelectionDialogPrivate();
  virtual void setupUi(qSlicerLongPETCTColorSelectionDialog* widget);

  vtkMRMLColorNode* ColorNode;
};

// --------------------------------------------------------------------------
qSlicerLongPETCTColorSelectionDialogPrivate
::qSlicerLongPETCTColorSelectionDialogPrivate(
  qSlicerLongPETCTColorSelectionDialog& object)
  : q_ptr(&object), ColorNode(NULL)
{
}

// --------------------------------------------------------------------------
qSlicerLongPETCTColorSelectionDialogPrivate
::~qSlicerLongPETCTColorSelectionDialogPrivate()
{
}

// --------------------------------------------------------------------------
void qSlicerLongPETCTColorSelectionDialogPrivate
::setupUi(qSlicerLongPETCTColorSelectionDialog* widget)
{
  Q_Q(qSlicerLongPETCTColorSelectionDialog);

  this->Ui_qSlicerLongPETCTColorSelectionDialog::setupUi(widget);

  QObject::connect(this->LineEditSearch, SIGNAL( textChanged(QString)), q, SLOT(populateColorsList(QString)) );
  QObject::connect(this->ListWidgetColors, SIGNAL(itemSelectionChanged()), q, SLOT(colorSelectionChanged()) );
}


//-----------------------------------------------------------------------------
// qSlicerLongPETCTColorSelectionDialog methods

//-----------------------------------------------------------------------------
qSlicerLongPETCTColorSelectionDialog
::qSlicerLongPETCTColorSelectionDialog(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerLongPETCTColorSelectionDialogPrivate(*this) )
{
  Q_D(qSlicerLongPETCTColorSelectionDialog);
  d->setupUi(this);

  this->setWindowTitle("Color Selection");
  this->setModal(true);
}

//-----------------------------------------------------------------------------
qSlicerLongPETCTColorSelectionDialog
::~qSlicerLongPETCTColorSelectionDialog()
{
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTColorSelectionDialog
::populateColorsList(const QString& filter)
{
  Q_D(qSlicerLongPETCTColorSelectionDialog);
  Q_ASSERT(d->ListWidgetColors);

  d->ListWidgetColors->clear();
  if(d->ColorNode == NULL)
    return;

  int numberOfColors = d->ColorNode->GetNumberOfColors();

  for(int i=0; i < numberOfColors; ++i)
    {

      if( !filter.isEmpty() && !QString(d->ColorNode->GetColorName(i)).contains(filter, Qt::CaseInsensitive))
        continue;


      double c[3];
      d->ColorNode->GetColor(i,c);

      QColor color = qSlicerLongPETCTColorSelectionDialog::getRGBColorFromDoubleValues(c[0],c[1],c[2]);
      QPixmap pixmap(24,16);

      if(color.isValid())
        pixmap.fill(color);

      QIcon colorIcon(pixmap);

      QListWidgetItem* item = new QListWidgetItem(colorIcon, d->ColorNode->GetColorName(i));

      d->ListWidgetColors->addItem(item);
    }

}

//-----------------------------------------------------------------------------
QColor qSlicerLongPETCTColorSelectionDialog::getRGBColorFromDoubleValues(double r, double g, double b)
{
  return QColor( (r*255+.5), (g*255+.5), (b*255+.5) );
}

//-----------------------------------------------------------------------------
int
qSlicerLongPETCTColorSelectionDialog::getColorIDByListName(const QString& name)
{
  Q_D(qSlicerLongPETCTColorSelectionDialog);

  if (d->ColorNode == NULL)
    return -1;

  int numberOfColors = d->ColorNode->GetNumberOfColors();

  for (int i = 0; i < numberOfColors; ++i)
    {
      QString tempColorName = d->ColorNode->GetColorName(i);
      if (tempColorName.compare(name) == 0)
          return i;
    }

  return -1;
}

//-----------------------------------------------------------------------------
int qSlicerLongPETCTColorSelectionDialog::colorIDSelectionForNode(QWidget* parent, const vtkMRMLColorNode* colorNode)
{
  qSlicerLongPETCTColorSelectionDialog dialog(parent);
  dialog.setColorNode(colorNode);
  dialog.populateColorsList();
  dialog.exec();
  return dialog.selectedColorID();
}


//-----------------------------------------------------------------------------
int qSlicerLongPETCTColorSelectionDialog::selectedColorID()
{
  Q_D(qSlicerLongPETCTColorSelectionDialog);
  Q_ASSERT(d->ListWidgetColors);

  if (d->ColorNode == NULL || d->ListWidgetColors->selectedItems().size() == 0)
    return -1;

  QListWidgetItem* firstSelectedItem = d->ListWidgetColors->selectedItems().value(0);

  return this->getColorIDByListName(firstSelectedItem->text());
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTColorSelectionDialog::setColorNode(const vtkMRMLColorNode* colorNode)
{
  Q_D(qSlicerLongPETCTColorSelectionDialog);

  d->ColorNode = const_cast<vtkMRMLColorNode*>(colorNode);
  this->populateColorsList();
}

//-----------------------------------------------------------------------------
const vtkMRMLColorNode* qSlicerLongPETCTColorSelectionDialog::colorNode()
{
  Q_D(qSlicerLongPETCTColorSelectionDialog);

  return d->ColorNode;
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTColorSelectionDialog::colorSelectionChanged()
{
  Q_D(qSlicerLongPETCTColorSelectionDialog);
  Q_ASSERT(d->ListWidgetColors);
  Q_ASSERT(d->ButtonSelect);

  if(d->ListWidgetColors->selectedItems().size() > 0)
    d->ButtonSelect->setEnabled(true);
  else
    d->ButtonSelect->setEnabled(false);
}

