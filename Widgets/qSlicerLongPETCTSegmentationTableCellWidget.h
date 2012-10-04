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

#ifndef __qSlicerLongPETCTSegmentationTableCellWidget_h
#define __qSlicerLongPETCTSegmentationTableCellWidget_h

// Qt includes
#include <QWidget>
#include <QString>

#include <ctkVTKObject.h>

// LongPETCTStudySlider Widgets includes
#include "qSlicerLongPETCTModuleWidgetsExport.h"

class vtkMRMLLongPETCTReportNode;

class qSlicerLongPETCTSegmentationTableCellWidgetPrivate;


/// \ingroup Slicer_QtModules_LongitudinalPETCT
class Q_SLICER_LONGPETCT_MODULE_WIDGETS_EXPORT qSlicerLongPETCTSegmentationTableCellWidget
  : public QWidget
{
  Q_OBJECT

  Q_PROPERTY(bool modelVisibilityChecked READ modelVisibilityChecked WRITE setModelVisibilityChecked)
  Q_PROPERTY(bool modelVisibilityVisible READ modelVisibilityVisible WRITE setModelVisibilityVisible)


public:
  typedef QWidget Superclass;
  qSlicerLongPETCTSegmentationTableCellWidget(QWidget *parent=0);
  virtual ~qSlicerLongPETCTSegmentationTableCellWidget();

  bool modelVisibilityChecked();
  bool modelVisibilityVisible();

public slots:
  void setModelVisibilityChecked(bool checked);
  void setModelVisibilityVisible(bool visible);

signals:
  void modelVisibilityCheckBoxChecked(bool toggled);


protected:
  QScopedPointer<qSlicerLongPETCTSegmentationTableCellWidgetPrivate> d_ptr;


private:
  Q_DECLARE_PRIVATE(qSlicerLongPETCTSegmentationTableCellWidget);
  Q_DISABLE_COPY(qSlicerLongPETCTSegmentationTableCellWidget);



};

#endif
