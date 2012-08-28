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

#ifndef __qSlicerLongPETCTFindingSettingsDialog_h
#define __qSlicerLongPETCTFindingSettingsDialog_h

// Qt includes
#include <QDialog>


// LongPETCTFindingSelection Widgets includes
#include "qSlicerLongPETCTModuleWidgetsExport.h"

#include "vtkMRMLScene.h"

#include <ctkVTKObject.h>

#include <vtkMRMLLongPETCTReportNode.h>
class vtkMRMLColorNode;


class qSlicerLongPETCTFindingSettingsDialogPrivate;


/// \ingroup Slicer_QtModules_LongPETCT
class Q_SLICER_LONGPETCT_MODULE_WIDGETS_EXPORT qSlicerLongPETCTFindingSettingsDialog
  : public QDialog
{
  Q_OBJECT
  QVTK_OBJECT
  Q_PROPERTY(bool applied READ applied WRITE setApplied)
  Q_PROPERTY(QString findingName READ findingName WRITE setFindingName)
  Q_PROPERTY(int colorID READ colorID WRITE setColorID)
  Q_PROPERTY(QString typeName READ typeName WRITE setTypeName)

public:
  typedef QDialog Superclass;
  qSlicerLongPETCTFindingSettingsDialog(QWidget *parent=0);
  virtual ~qSlicerLongPETCTFindingSettingsDialog();

  QString findingName();
  void setFindingName(const QString& name);

  int colorID();
  void setColorID(int colorID);

  QString typeName();
  void setTypeName(const QString& typeName);

  bool applied();
  void setApplied(bool applied);

  Q_INVOKABLE void setReportNode(vtkMRMLLongPETCTReportNode* reportNode);
  vtkMRMLLongPETCTReportNode* reportNode();



public slots:
  void updateView();
  virtual void show();

protected slots:
  void applyClicked();
  void colorSelectionButtonClicked();
  void addTypeButtonClicked();
  void removeTypeButtonClicked();
  void selectionChanged(int);


protected:
  QScopedPointer<qSlicerLongPETCTFindingSettingsDialogPrivate> d_ptr;


private:
  Q_DECLARE_PRIVATE(qSlicerLongPETCTFindingSettingsDialog);
  Q_DISABLE_COPY(qSlicerLongPETCTFindingSettingsDialog);



};

#endif
