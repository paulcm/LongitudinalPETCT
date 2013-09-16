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

// LongitudinalPETCTReportSelection Widgets includes
#include "qMRMLLongitudinalPETCTDialog.h"

#include <vtkMRMLLongitudinalPETCTReportNode.h>


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongitudinalPETCT
class qMRMLLongitudinalPETCTDialogPrivate : public QDialog
{
  Q_DECLARE_PUBLIC(qMRMLLongitudinalPETCTDialog);

protected:
  qMRMLLongitudinalPETCTDialog* const q_ptr;

public:
  qMRMLLongitudinalPETCTDialogPrivate(qMRMLLongitudinalPETCTDialog& object);
  virtual ~qMRMLLongitudinalPETCTDialogPrivate();

  vtkMRMLLongitudinalPETCTReportNode* ReportNode;
};

// --------------------------------------------------------------------------
qMRMLLongitudinalPETCTDialogPrivate
::qMRMLLongitudinalPETCTDialogPrivate(
  qMRMLLongitudinalPETCTDialog& object)
  : q_ptr(&object), ReportNode(NULL)
{
}

// --------------------------------------------------------------------------
qMRMLLongitudinalPETCTDialogPrivate
::~qMRMLLongitudinalPETCTDialogPrivate()
{
}


//-----------------------------------------------------------------------------
// qMRMLLongitudinalPETCTDialog methods

//-----------------------------------------------------------------------------
qMRMLLongitudinalPETCTDialog
::qMRMLLongitudinalPETCTDialog(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qMRMLLongitudinalPETCTDialogPrivate(*this) )
{
}

//-----------------------------------------------------------------------------
qMRMLLongitudinalPETCTDialog
::~qMRMLLongitudinalPETCTDialog()
{
}

//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTDialog::setReportNode(vtkMRMLLongitudinalPETCTReportNode* reportNode)
{
  Q_D(qMRMLLongitudinalPETCTDialog);

  qvtkReconnect(d->ReportNode, reportNode, vtkCommand::ModifiedEvent, this, SLOT(updateDialogFromMRML()) );
  d->ReportNode = reportNode;

  this->updateDialogFromMRML();
}

//-----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTReportNode* qMRMLLongitudinalPETCTDialog::reportNode() const
{
  Q_D(const qMRMLLongitudinalPETCTDialog);
  return d->ReportNode;
}

