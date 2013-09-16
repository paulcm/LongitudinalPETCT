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

#ifndef __qMRMLLongitudinalPETCTReportWidget_h
#define __qMRMLLongitudinalPETCTReportWidget_h

#include <qMRMLLongitudinalPETCTWidget.h>

// CTK includes
#include <ctkVTKObject.h>

// LongitudinalPETCTReportSelection Widgets includes
#include "qSlicerLongitudinalPETCTModuleWidgetsExport.h"


class qMRMLLongitudinalPETCTReportWidgetPrivate;

// MRML
class vtkMRMLNode;
class vtkMRMLLongitudinalPETCTReportNode;


/// \ingroup Slicer_QtModules_LongitudinalPETCT
class Q_SLICER_LONGITUDINALPETCT_MODULE_WIDGETS_EXPORT qMRMLLongitudinalPETCTReportWidget
  : public qMRMLLongitudinalPETCTWidget
{
  Q_OBJECT
  QVTK_OBJECT

public:
  typedef qMRMLLongitudinalPETCTWidget Superclass;
  qMRMLLongitudinalPETCTReportWidget(QWidget *parent=0);
  virtual ~qMRMLLongitudinalPETCTReportWidget();

  Q_INVOKABLE vtkMRMLLongitudinalPETCTReportNode* currentSelectedReportNode();

signals:
  void showModuleSettingsDialog();
  void reportNodeChanged(vtkMRMLLongitudinalPETCTReportNode* reportNode);

public slots:
  virtual void updateWidgetFromMRML();
  virtual void setMRMLScene(vtkMRMLScene *newScene);

protected slots:
  void reportNodeChanged(vtkMRMLNode* node);

protected:
  QScopedPointer<qMRMLLongitudinalPETCTReportWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qMRMLLongitudinalPETCTReportWidget);
  Q_DISABLE_COPY(qMRMLLongitudinalPETCTReportWidget);



};

#endif
