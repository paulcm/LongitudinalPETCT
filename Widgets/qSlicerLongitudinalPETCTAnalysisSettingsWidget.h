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

#ifndef __qSlicerLongitudinalPETCTAnalysisSettingsWidget_h
#define __qSlicerLongitudinalPETCTAnalysisSettingsWidget_h

// Qt includes
#include <QWidget>

// CTK includes
#include <ctkVTKObject.h>

// LongitudinalPETCTAnalysisSettings Widgets includes
#include "qSlicerLongitudinalPETCTModuleWidgetsExport.h"
#include <vtkSmartPointer.h>


class qSlicerLongitudinalPETCTAnalysisSettingsWidgetPrivate;
class vtkMRMLScene;
class vtkMRMLNode;
class vtkMRMLLongitudinalPETCTReportNode;
class QListWidgetItem;


/// \ingroup Slicer_QtModules_LongitudinalPETCT
class Q_SLICER_LONGITUDINALPETCT_MODULE_WIDGETS_EXPORT qSlicerLongitudinalPETCTAnalysisSettingsWidget
  : public QWidget
{
  Q_OBJECT
  QVTK_OBJECT
  Q_PROPERTY(bool volumeRendering READ volumeRendering WRITE setVolumeRendering)
  Q_PROPERTY(bool spinView READ spinView WRITE setSpinView)
  Q_PROPERTY(bool qualitativeChecked READ qualitativeChecked WRITE setQualitativeChecked)
  Q_PROPERTY(bool quantitativeChecked READ quantitativeChecked WRITE setQuantitativeChecked)

public:
  typedef QWidget Superclass;
  qSlicerLongitudinalPETCTAnalysisSettingsWidget(QWidget *parent=0);
  virtual ~qSlicerLongitudinalPETCTAnalysisSettingsWidget();

  Q_INVOKABLE void setReportNode(vtkMRMLLongitudinalPETCTReportNode* reportNode);
  vtkMRMLLongitudinalPETCTReportNode* reportNode();

  bool qualitativeChecked();
  bool quantitativeChecked();
  bool volumeRendering();
  bool spinView();

  void setQualitativeChecked(bool checked);
  void setQuantitativeChecked(bool checked);
  void setVolumeRendering(bool checked);
  void setSpinView(bool checked);


signals:
  void qualitativeAnalysisClicked(bool checked);
  void quantitativeAnalysisClicked(bool checked);
  void studySelectedForAnalysis(int index, bool selected);
  void volumeRenderingToggled(bool toggled);
  void spinViewToggled(bool toggled);
  void exportCSV();

public slots:
  void updateView();

protected slots:
  void studySelectedInTable(int index, bool selected);

protected:
  QScopedPointer<qSlicerLongitudinalPETCTAnalysisSettingsWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerLongitudinalPETCTAnalysisSettingsWidget);
  Q_DISABLE_COPY(qSlicerLongitudinalPETCTAnalysisSettingsWidget);



};

#endif
