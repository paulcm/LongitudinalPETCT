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

#ifndef __qSlicerLongitudinalPETCTFindingSettingsDialog_h
#define __qSlicerLongitudinalPETCTFindingSettingsDialog_h

// Qt includes
#include <QDialog>


// LongitudinalPETCTFindingSelection Widgets includes
#include "qSlicerLongitudinalPETCTModuleWidgetsExport.h"

#include "vtkMRMLScene.h"

#include <ctkVTKObject.h>

#include <vtkMRMLLongitudinalPETCTReportNode.h>
class vtkMRMLColorNode;


class qSlicerLongitudinalPETCTFindingSettingsDialogPrivate;


/// \ingroup Slicer_QtModules_LongitudinalPETCT
class Q_SLICER_LONGITUDINALPETCT_MODULE_WIDGETS_EXPORT qSlicerLongitudinalPETCTFindingSettingsDialog
  : public QDialog
{
  Q_OBJECT
  QVTK_OBJECT

public:
  typedef QDialog Superclass;
  qSlicerLongitudinalPETCTFindingSettingsDialog(QWidget *parent=0);
  virtual ~qSlicerLongitudinalPETCTFindingSettingsDialog();

  Q_INVOKABLE void setReportNode(vtkMRMLLongitudinalPETCTReportNode* reportNode);
  vtkMRMLLongitudinalPETCTReportNode* reportNode();

  void accept();
  void hide();

public slots:
  void updateView();

protected slots:
  void colorSelectionButtonClicked();
  void addTypeButtonClicked();
  void removeTypeButtonClicked();
  void selectionChanged(int index);

protected:
  QScopedPointer<qSlicerLongitudinalPETCTFindingSettingsDialogPrivate> d_ptr;


private:
  Q_DECLARE_PRIVATE(qSlicerLongitudinalPETCTFindingSettingsDialog);
  Q_DISABLE_COPY(qSlicerLongitudinalPETCTFindingSettingsDialog);


};

#endif
