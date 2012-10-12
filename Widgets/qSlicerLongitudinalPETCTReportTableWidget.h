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

#ifndef __qSlicerLongitudinalPETCTReportTableWidget_h
#define __qSlicerLongitudinalPETCTReportTableWidget_h

// Qt includes
#include <QWidget>

// LongitudinalPETCTReportTable Widgets includes
#include "qSlicerLongitudinalPETCTModuleWidgetsExport.h"
#include <vtkSmartPointer.h>
#include <vtkMRMLLongitudinalPETCTReportNode.h>

#include <ctkVTKObject.h>

class qSlicerLongitudinalPETCTReportTableWidgetPrivate;

class vtkMRMLNode;



/// \ingroup Slicer_QtModules_LongitudinalitudinalPETCT
class Q_SLICER_LONGITUDINALPETCT_MODULE_WIDGETS_EXPORT qSlicerLongitudinalPETCTReportTableWidget
  : public QWidget
{
  Q_OBJECT
  QVTK_OBJECT

public:
  typedef QWidget Superclass;
  qSlicerLongitudinalPETCTReportTableWidget(QWidget *parent=0);
  virtual ~qSlicerLongitudinalPETCTReportTableWidget();

  Q_INVOKABLE void setReportNode(vtkMRMLLongitudinalPETCTReportNode* reportNode);
  vtkMRMLLongitudinalPETCTReportNode* reportNode();

  enum SelectableMode {
    AllSelectable,
    RowSelectable,
    ColumnSelectable
  };

  Q_INVOKABLE void setAllSelectableOn(){SelMode = AllSelectable; this->updateView();};
  Q_INVOKABLE void setColumnSelectableOn(){SelMode = ColumnSelectable; this->updateView();};
  Q_INVOKABLE void setRowSelectableOn(){SelMode = RowSelectable; this->updateView();};

signals:
  void studyClicked(int idx);
  void findingClicked(int idx);
  void segmentationModelVisiblityChecked(int findID, int studID, bool checked);

public slots:
  void updateView();

protected slots:
  void selectStudyColumn(int index);
  void selectFindingRow(int index);

  void segmentationModelVisibilityChecked(bool toggled);
  void segmentationCellClicked(int row, int column);


protected:
  QScopedPointer<qSlicerLongitudinalPETCTReportTableWidgetPrivate> d_ptr;

  virtual void resizeEvent(QResizeEvent* event);
  void arrangeColumns();

  void prepareHorizontalHeaders();
  void prepareVerticalHeaders();

  void updateHorizontalHeaders();
  void updateVerticalHeaders();

  SelectableMode SelMode;


private:
  Q_DECLARE_PRIVATE(qSlicerLongitudinalPETCTReportTableWidget);
  Q_DISABLE_COPY(qSlicerLongitudinalPETCTReportTableWidget);

};

#endif
