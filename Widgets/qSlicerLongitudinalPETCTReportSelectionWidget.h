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

#ifndef __qSlicerLongitudinalPETCTReportSelectionWidget_h
#define __qSlicerLongitudinalPETCTReportSelectionWidget_h

// Qt includes
#include <QWidget>

// CTK includes
#include <ctkVTKObject.h>

// LongitudinalPETCTReportSelection Widgets includes
#include "qSlicerLongitudinalPETCTModuleWidgetsExport.h"
#include <vtkSmartPointer.h>


class qSlicerLongitudinalPETCTReportSelectionWidgetPrivate;
class vtkMRMLScene;
class vtkMRMLNode;
class qMRMLNodeComboBox;
class vtkMRMLLongitudinalPETCTReportNode;


/// \ingroup Slicer_QtModules_LongitudinalPETCT
class Q_SLICER_LONGITUDINALPETCT_MODULE_WIDGETS_EXPORT qSlicerLongitudinalPETCTReportSelectionWidget
  : public QWidget
{
  Q_OBJECT
  QVTK_OBJECT
  Q_PROPERTY(vtkMRMLScene* mrmlScene READ mrmlScene WRITE setMRMLScene)
  //Q_PROPERTY(qMRMLNodeComboBox* mrmlNodeComboBoxReport READ mrmlNodeComboBoxReport WRITE setMRMLNodeComboBoxReport)

public:
  typedef QWidget Superclass;
  qSlicerLongitudinalPETCTReportSelectionWidget(QWidget *parent=0);
  virtual ~qSlicerLongitudinalPETCTReportSelectionWidget();

  qMRMLNodeComboBox* mrmlNodeComboBoxReports();
  Q_INVOKABLE void setMRMLScene(vtkMRMLScene* mrmlScene);
  vtkMRMLScene* mrmlScene();

  Q_INVOKABLE qMRMLNodeComboBox* mrmlNodeComboBoxReport();

signals:
  void showModuleSettingsDialog();

public slots:
  void updateView();

protected slots:
  void selectionChanged(vtkMRMLNode* node);

protected:
  QScopedPointer<qSlicerLongitudinalPETCTReportSelectionWidgetPrivate> d_ptr;
  void setReportNode(vtkSmartPointer<vtkMRMLLongitudinalPETCTReportNode> reportNode);

private:
  Q_DECLARE_PRIVATE(qSlicerLongitudinalPETCTReportSelectionWidget);
  Q_DISABLE_COPY(qSlicerLongitudinalPETCTReportSelectionWidget);



};

#endif
