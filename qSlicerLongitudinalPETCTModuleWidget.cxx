/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// Qt includes
#include <QDebug>

// SlicerQt includes
#include "qSlicerLongitudinalPETCTModuleWidget.h"


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerLongitudinalPETCTModuleWidgetPrivate
{
public:

  qSlicerLongitudinalPETCTModuleWidgetPrivate();
};


//-----------------------------------------------------------------------------
// qSlicerLongitudinalPETCTModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerLongitudinalPETCTModuleWidgetPrivate::qSlicerLongitudinalPETCTModuleWidgetPrivate()
{
}




//-----------------------------------------------------------------------------
// qSlicerLongitudinalPETCTModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerLongitudinalPETCTModuleWidget::qSlicerLongitudinalPETCTModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerLongitudinalPETCTModuleWidgetPrivate )
{

}

//-----------------------------------------------------------------------------
qSlicerLongitudinalPETCTModuleWidget::~qSlicerLongitudinalPETCTModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerLongitudinalPETCTModuleWidget::setup()
{
  Q_D(qSlicerLongitudinalPETCTModuleWidget);
  // d->setupUi(this); no UI file
  this->Superclass::setup();
}




