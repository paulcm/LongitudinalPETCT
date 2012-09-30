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

#ifndef __qSlicerLongPETCTAnalysisSettingsWidget_h
#define __qSlicerLongPETCTAnalysisSettingsWidget_h

// Qt includes
#include <QWidget>

// CTK includes
#include <ctkVTKObject.h>

// LongPETCTAnalysisSettings Widgets includes
#include "qSlicerLongPETCTModuleWidgetsExport.h"
#include <vtkSmartPointer.h>


class qSlicerLongPETCTAnalysisSettingsWidgetPrivate;
class vtkMRMLScene;
class vtkMRMLNode;
class vtkMRMLLongPETCTReportNode;
class QListWidgetItem;


/// \ingroup Slicer_QtModules_LongPETCT
class Q_SLICER_LONGPETCT_MODULE_WIDGETS_EXPORT qSlicerLongPETCTAnalysisSettingsWidget
  : public QWidget
{
  Q_OBJECT
  QVTK_OBJECT
  Q_PROPERTY(bool volumeRendering READ volumeRendering WRITE setVolumeRendering)
  Q_PROPERTY(bool spinView READ spinView WRITE setSpinView)

public:
  typedef QWidget Superclass;
  qSlicerLongPETCTAnalysisSettingsWidget(QWidget *parent=0);
  virtual ~qSlicerLongPETCTAnalysisSettingsWidget();

  Q_INVOKABLE void setReportNode(vtkMRMLLongPETCTReportNode* reportNode);
  vtkMRMLLongPETCTReportNode* reportNode();

  bool volumeRendering();
  bool spinView();

  void setVolumeRendering(bool checked);
  void setSpinView(bool checked);

signals:
  void qualitativeAnalysisClicked();
  void quantitativeAnalysisClicked();
  void studySelectedForAnalysis(int index, bool selected);
  void volumeRenderingToggled(bool toggled);
  void spinViewToggled(bool toggled);

public slots:
  void updateView();
  void listItemChanged(QListWidgetItem* item);

protected:
  QScopedPointer<qSlicerLongPETCTAnalysisSettingsWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerLongPETCTAnalysisSettingsWidget);
  Q_DISABLE_COPY(qSlicerLongPETCTAnalysisSettingsWidget);



};

#endif
