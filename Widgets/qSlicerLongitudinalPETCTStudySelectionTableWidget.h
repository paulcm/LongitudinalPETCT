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

#ifndef __qSlicerLongitudinalPETCTStudySelectionTableWidget_h
#define __qSlicerLongitudinalPETCTStudySelectionTableWidget_h

// Qt includes
#include <QTableWidget>

// LongitudinalPETCTReportTable Widgets includes
#include "qSlicerLongitudinalPETCTModuleWidgetsExport.h"
#include <vtkSmartPointer.h>
#include <vtkMRMLLongitudinalPETCTReportNode.h>

#include <ctkVTKObject.h>

class qSlicerLongitudinalPETCTStudySelectionTableWidgetPrivate;

class vtkMRMLNode;



/// \ingroup Slicer_QtModules_LongitudinalitudinalPETCT
class Q_SLICER_LONGITUDINALPETCT_MODULE_WIDGETS_EXPORT qSlicerLongitudinalPETCTStudySelectionTableWidget
  : public QWidget
{
  Q_OBJECT

public:
  typedef QWidget Superclass;
  qSlicerLongitudinalPETCTStudySelectionTableWidget(QWidget *parent=0);
  virtual ~qSlicerLongitudinalPETCTStudySelectionTableWidget();

  enum TableMode {
     WorkflowSelection,
     AnalysisSelection
  };

  void addStudyToTable(vtkMRMLLongitudinalPETCTStudyNode* study, bool disabled = false);

  void setTableMode(TableMode mode);
  TableMode tableMode();

signals:
  void studySelectionChanged(int index, bool selected);

public slots:
  void deselectTableAll();
  void selectTableRow(int row, bool select);
  void clearRows();

protected slots:
  void tableCellClicked(int row, int col);
  void tableItemChanged(QTableWidgetItem* item);

protected:
  QScopedPointer<qSlicerLongitudinalPETCTStudySelectionTableWidgetPrivate> d_ptr;


private:
  Q_DECLARE_PRIVATE(qSlicerLongitudinalPETCTStudySelectionTableWidget);
  Q_DISABLE_COPY(qSlicerLongitudinalPETCTStudySelectionTableWidget);

};

#endif
