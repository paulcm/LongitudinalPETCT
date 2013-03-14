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

#ifndef __qSlicerLongitudinalPETCTStudySelectionWidget_h
#define __qSlicerLongitudinalPETCTStudySelectionWidget_h

// Qt includes
#include <QWidget>
#include <QString>

#include <ctkVTKObject.h>

// LongitudinalPETCTStudySelection Widgets includes
#include "qSlicerLongitudinalPETCTModuleWidgetsExport.h"

class vtkMRMLLongitudinalPETCTReportNode;
class qSlicerLongitudinalPETCTStudySelectionWidgetPrivate;

/// \ingroup Slicer_QtModules_LongitudinalitudinalPETCT
class Q_SLICER_LONGITUDINALPETCT_MODULE_WIDGETS_EXPORT qSlicerLongitudinalPETCTStudySelectionWidget
  : public QWidget
{
  Q_OBJECT
  QVTK_OBJECT
  Q_PROPERTY(bool volumeRendering READ volumeRendering WRITE setVolumeRendering)
  Q_PROPERTY(bool spinView READ spinView WRITE setSpinView)
  Q_PROPERTY(double opacityPow READ opacityPow WRITE setOpacityPow)


public:
  typedef QWidget Superclass;
  qSlicerLongitudinalPETCTStudySelectionWidget(QWidget *parent=0);
  virtual ~qSlicerLongitudinalPETCTStudySelectionWidget();

  bool volumeRendering();
  bool spinView();
  double opacityPow();


  void setVolumeRendering(bool checked);
  void setSpinView(bool checked);
  void setOpacityPow(double opacityPow);

  Q_INVOKABLE void setReportNode(vtkMRMLLongitudinalPETCTReportNode* reportNode);
  Q_INVOKABLE vtkMRMLLongitudinalPETCTReportNode* reportNode();


signals:
  void studySelected(int index);
  void studyDeselected(int index);
  void volumeRenderingToggled(bool toggled);
  void linearOpacityToggled(bool toggled);
  void spinViewToggled(bool toggled);
  void opacityPowChanged(double d);

public slots:
  void updateView();
  void selectStudyInRow(int row);

protected slots:
  void studySelectedInTable(int index, bool selected);


protected:
  QScopedPointer<qSlicerLongitudinalPETCTStudySelectionWidgetPrivate> d_ptr;


private:
  Q_DECLARE_PRIVATE(qSlicerLongitudinalPETCTStudySelectionWidget);
  Q_DISABLE_COPY(qSlicerLongitudinalPETCTStudySelectionWidget);



};

#endif
