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

// LongPETCTStudySlider Widgets includes
#include "qSlicerLongPETCTReportTree.h"


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongitudinalPETCT
class qSlicerLongPETCTReportTreePrivate
  : public QWidget
{
  Q_DECLARE_PUBLIC(qSlicerLongPETCTReportTree);
protected:
  qSlicerLongPETCTReportTree* const q_ptr;

public:
  qSlicerLongPETCTReportTreePrivate(
    qSlicerLongPETCTReportTree& object);
  virtual void setupUi(qSlicerLongPETCTReportTree*);

};

// --------------------------------------------------------------------------
qSlicerLongPETCTReportTreePrivate
::qSlicerLongPETCTReportTreePrivate(
  qSlicerLongPETCTReportTree& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerLongPETCTReportTreePrivate
::setupUi(qSlicerLongPETCTReportTree* widget)
{
  Q_Q(qSlicerLongPETCTReportTree);

  this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

}

//-----------------------------------------------------------------------------
// qSlicerLongPETCTReportTree methods

//-----------------------------------------------------------------------------
qSlicerLongPETCTReportTree
::qSlicerLongPETCTReportTree(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerLongPETCTReportTreePrivate(*this) )
{
  Q_D(qSlicerLongPETCTReportTree);
  d->setupUi(this);

  this->headerItem()->setText(0,"Findings");
}

//-----------------------------------------------------------------------------
qSlicerLongPETCTReportTree
::~qSlicerLongPETCTReportTree()
{
}
