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

#ifndef __qSlicerLongPETCTStudySelectionWidget_h
#define __qSlicerLongPETCTStudySelectionWidget_h

// Qt includes
#include <QWidget>
#include <QString>

#include <ctkVTKObject.h>

// LongPETCTStudySelection Widgets includes
#include "qSlicerLongPETCTModuleWidgetsExport.h"

class vtkMRMLLongPETCTReportNode;
class qSlicerLongPETCTStudySelectionWidgetPrivate;

/// \ingroup Slicer_QtModules_LongitudinalPETCT
class Q_SLICER_LONGPETCT_MODULE_WIDGETS_EXPORT qSlicerLongPETCTStudySelectionWidget
  : public QWidget
{
  Q_OBJECT
  QVTK_OBJECT
  Q_PROPERTY(bool volumeRendering READ volumeRendering WRITE setVolumeRendering)
  Q_PROPERTY(bool spinView READ spinView WRITE setSpinView)
  Q_PROPERTY(double opacityPow READ opacityPow WRITE setOpacityPow)
  Q_PROPERTY(bool centeredSelected READ centeredSelected WRITE setCenteredSelected)


public:
  typedef QWidget Superclass;
  qSlicerLongPETCTStudySelectionWidget(QWidget *parent=0);
  virtual ~qSlicerLongPETCTStudySelectionWidget();

  bool volumeRendering();
  bool spinView();
  double opacityPow();
  bool centeredSelected();


  void setVolumeRendering(bool checked);
  void setSpinView(bool checked);
  void setOpacityPow(double opacityPow);
  void setCenteredSelected(bool selected);

  Q_INVOKABLE void setReportNode(vtkMRMLLongPETCTReportNode* reportNode);
  Q_INVOKABLE vtkMRMLLongPETCTReportNode* reportNode();


signals:
  void studySelected(int index);
  void studyDeselected(int index);
  void volumeRenderingToggled(bool toggled);
  void linearOpacityToggled(bool toggled);
  void spinViewToggled(bool toggled);
  void opacityPowChanged(double d);
  void showStudiesCentered(bool centered);


public slots:
  void updateView();
  void selectStudyInRow(int row);


protected slots:
  void studySelectedInTable(int index, bool selected);


protected:
  QScopedPointer<qSlicerLongPETCTStudySelectionWidgetPrivate> d_ptr;


private:
  Q_DECLARE_PRIVATE(qSlicerLongPETCTStudySelectionWidget);
  Q_DISABLE_COPY(qSlicerLongPETCTStudySelectionWidget);



};

#endif
