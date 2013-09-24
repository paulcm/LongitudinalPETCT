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

#ifndef __qMRMLLongitudinalPETCTFindingSettingsDialog_h
#define __qMRMLLongitudinalPETCTFindingSettingsDialog_h


#include <qMRMLLongitudinalPETCTDialog.h>


// LongitudinalPETCTFindingSelection Widgets includes
#include "qSlicerLongitudinalPETCTModuleWidgetsExport.h"

#include "vtkMRMLScene.h"

#include <ctkVTKObject.h>

#include <vtkMRMLLongitudinalPETCTFindingNode.h>
class vtkMRMLColorNode;


class qMRMLLongitudinalPETCTFindingSettingsDialogPrivate;
class QAbstractButton;

/// \ingroup Slicer_QtModules_LongitudinalPETCT
class Q_SLICER_LONGITUDINALPETCT_MODULE_WIDGETS_EXPORT qMRMLLongitudinalPETCTFindingSettingsDialog
  : public qMRMLLongitudinalPETCTDialog
{
  Q_OBJECT

  Q_PROPERTY(QString findingName READ findingName WRITE setFindingName)
  Q_PROPERTY(int findingColorID READ findingColorID WRITE setFindingColorID)


public:
  typedef qMRMLLongitudinalPETCTDialog Superclass;
  qMRMLLongitudinalPETCTFindingSettingsDialog(QWidget *parent=0);
  virtual ~qMRMLLongitudinalPETCTFindingSettingsDialog();

  QString findingName();
  int findingColorID();

  void setFindingName(const QString& name);
  void setFindingColorID(int colorID);

signals:
  void findingSpecified(const QString& findingName, int colorID);

public slots:
  virtual void accept();
  virtual void updateDialogFromMRML();

protected slots:
  void presetButtonClicked(QAbstractButton* button);


protected:
  QScopedPointer<qMRMLLongitudinalPETCTFindingSettingsDialogPrivate> d_ptr;


private:
  Q_DECLARE_PRIVATE(qMRMLLongitudinalPETCTFindingSettingsDialog);
  Q_DISABLE_COPY(qMRMLLongitudinalPETCTFindingSettingsDialog);


};

#endif
