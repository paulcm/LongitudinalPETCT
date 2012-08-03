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
#include <QtPlugin>
#include <QFileInfo>
#include <QScopedPointer>

// ExtensionTemplate Logic includes
#include <vtkSlicerLongPETCTLogic.h>

// ExtensionTemplate includes
#include "qSlicerLongPETCTModule.h"

// SlicerQT includes
#include <qSlicerModuleManager.h>
#include <qSlicerScriptedLoadableModuleWidget.h>
#include <qSlicerUtils.h>
#include <vtkSlicerConfigure.h>

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerLongPETCTModule, qSlicerLongPETCTModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerLongPETCTModulePrivate
{
public:
  qSlicerLongPETCTModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerLongPETCTModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerLongPETCTModulePrivate::qSlicerLongPETCTModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerLongPETCTModule methods

//-----------------------------------------------------------------------------
qSlicerLongPETCTModule::qSlicerLongPETCTModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerLongPETCTModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerLongPETCTModule::~qSlicerLongPETCTModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerLongPETCTModule::helpText()const
{
  return "This is a loadable module bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerLongPETCTModule::acknowledgementText()const
{
  return "This work was was partially funded by NIH grant 3P41RR013218-12S1";
}

//-----------------------------------------------------------------------------
QStringList qSlicerLongPETCTModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Jean-Christophe Fillion-Robin (Kitware)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerLongPETCTModule::icon()const
{
  return QIcon(":/Icons/LongPETCT.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerLongPETCTModule::categories() const
{
  return QStringList() << "Quantification";
}

//-----------------------------------------------------------------------------
QStringList qSlicerLongPETCTModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerLongPETCTModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerLongPETCTModule::createWidgetRepresentation()
{
  QString pythonPath = qSlicerUtils::pathWithoutIntDir(
              QFileInfo(this->path()).path(), Slicer_QTLOADABLEMODULES_LIB_DIR);

  QScopedPointer<qSlicerScriptedLoadableModuleWidget> widget(new qSlicerScriptedLoadableModuleWidget);
  QString classNameToLoad = "qSlicerLongPETCTModuleWidget";
  bool ret = widget->setPythonSource(
        pythonPath + "/Python/" + classNameToLoad + ".py", classNameToLoad);
  if (!ret)
    {
    return 0;
    }
  return widget.take();
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerLongPETCTModule::createLogic()
{
  vtkSlicerLongPETCTLogic* logic = vtkSlicerLongPETCTLogic::New();
  logic->Initialize(); // need to initialize because vtkNewMacro doesn't take care of that

  return logic;
}
