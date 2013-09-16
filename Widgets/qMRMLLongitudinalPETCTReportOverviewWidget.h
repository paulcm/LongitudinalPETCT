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

#ifndef __qMRMLLongitudinalPETCTReportOverviewWidget_h
#define __qMRMLLongitudinalPETCTReportOverviewWidget_h

#include <qMRMLLongitudinalPETCTWidget.h>

// LongitudinalPETCTReportTable Widgets includes
#include "qSlicerLongitudinalPETCTModuleWidgetsExport.h"
#include <vtkSmartPointer.h>
#include <vtkMRMLLongitudinalPETCTReportNode.h>

#include <ctkVTKObject.h>

class qMRMLLongitudinalPETCTReportOverviewWidgetPrivate;

class vtkMRMLNode;



/// \ingroup Slicer_QtModules_LongitudinalitudinalPETCT
class Q_SLICER_LONGITUDINALPETCT_MODULE_WIDGETS_EXPORT qMRMLLongitudinalPETCTReportOverviewWidget
  : public qMRMLLongitudinalPETCTWidget
{
  Q_OBJECT
  QVTK_OBJECT

public:
  typedef qMRMLLongitudinalPETCTWidget Superclass;
  qMRMLLongitudinalPETCTReportOverviewWidget(QWidget *parent=0);
  virtual ~qMRMLLongitudinalPETCTReportOverviewWidget();

  enum SelectableMode {
    AllSelectable,
    RowSelectable,
    ColumnSelectable
  };

  Q_INVOKABLE void setAllSelectableOn(){SelMode = AllSelectable; this->updateWidgetFromMRML();};
  Q_INVOKABLE void setColumnSelectableOn(){SelMode = ColumnSelectable; this->updateWidgetFromMRML();};
  Q_INVOKABLE void setRowSelectableOn(){SelMode = RowSelectable; this->updateWidgetFromMRML();};

signals:
  void studySelected(vtkMRMLLongitudinalPETCTStudyNode* study);
  void findingSelected(vtkMRMLLongitudinalPETCTFindingNode* finding);

  void segmentationModelVisiblityChecked(int findID, int studID, bool checked);

public slots:
  virtual void updateWidgetFromMRML();
  void selectStudyColumn(int index);
  void selectFindingRow(int index);

protected slots:
  void segmentationModelVisibilityChecked(bool toggled);
  void segmentationCellClicked(int row, int column);

protected:
  QScopedPointer<qMRMLLongitudinalPETCTReportOverviewWidgetPrivate> d_ptr;

  virtual void resizeEvent(QResizeEvent* event);

  SelectableMode SelMode;



private:
  Q_DECLARE_PRIVATE(qMRMLLongitudinalPETCTReportOverviewWidget);
  Q_DISABLE_COPY(qMRMLLongitudinalPETCTReportOverviewWidget);

};

#endif
