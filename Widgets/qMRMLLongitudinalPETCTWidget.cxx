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
#include "qMRMLLongitudinalPETCTWidget.h"

#include <vtkMRMLLongitudinalPETCTReportNode.h>


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongitudinalPETCT
class qMRMLLongitudinalPETCTWidgetPrivate : public qMRMLWidget
{
  Q_DECLARE_PUBLIC(qMRMLLongitudinalPETCTWidget);

protected:
  qMRMLLongitudinalPETCTWidget* const q_ptr;

public:
  qMRMLLongitudinalPETCTWidgetPrivate(qMRMLLongitudinalPETCTWidget& object);
  virtual ~qMRMLLongitudinalPETCTWidgetPrivate();

  vtkMRMLLongitudinalPETCTReportNode* ReportNode;
};

// --------------------------------------------------------------------------
qMRMLLongitudinalPETCTWidgetPrivate
::qMRMLLongitudinalPETCTWidgetPrivate(
  qMRMLLongitudinalPETCTWidget& object)
  : q_ptr(&object), ReportNode(NULL)
{
}

// --------------------------------------------------------------------------
qMRMLLongitudinalPETCTWidgetPrivate
::~qMRMLLongitudinalPETCTWidgetPrivate()
{
}


//-----------------------------------------------------------------------------
// qMRMLLongitudinalPETCTWidget methods

//-----------------------------------------------------------------------------
qMRMLLongitudinalPETCTWidget
::qMRMLLongitudinalPETCTWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qMRMLLongitudinalPETCTWidgetPrivate(*this) )
{
}

//-----------------------------------------------------------------------------
qMRMLLongitudinalPETCTWidget
::~qMRMLLongitudinalPETCTWidget()
{
}

//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTWidget::setReportNode(vtkMRMLLongitudinalPETCTReportNode* reportNode)
{
  Q_D(qMRMLLongitudinalPETCTWidget);

  qvtkReconnect(d->ReportNode, reportNode, vtkCommand::ModifiedEvent, this, SLOT(updateWidgetFromMRML()) );
  d->ReportNode = reportNode;

  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
vtkMRMLLongitudinalPETCTReportNode* qMRMLLongitudinalPETCTWidget::reportNode() const
{
  Q_D(const qMRMLLongitudinalPETCTWidget);
  return d->ReportNode;
}

