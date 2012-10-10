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

#ifndef __qSlicerLongPETCTStudySelectionTableWidget_h
#define __qSlicerLongPETCTStudySelectionTableWidget_h

// Qt includes
#include <QTableWidget>

// LongPETCTReportTable Widgets includes
#include "qSlicerLongPETCTModuleWidgetsExport.h"
#include <vtkSmartPointer.h>
#include <vtkMRMLLongPETCTReportNode.h>

#include <ctkVTKObject.h>

class qSlicerLongPETCTStudySelectionTableWidgetPrivate;

class vtkMRMLNode;



/// \ingroup Slicer_QtModules_LongitudinalPETCT
class Q_SLICER_LONGPETCT_MODULE_WIDGETS_EXPORT qSlicerLongPETCTStudySelectionTableWidget
  : public QWidget
{
  Q_OBJECT

public:
  typedef QWidget Superclass;
  qSlicerLongPETCTStudySelectionTableWidget(QWidget *parent=0);
  virtual ~qSlicerLongPETCTStudySelectionTableWidget();

  enum TableMode {
     WorkflowSelection,
     AnalysisSelection
  };

  void addStudyToTable(vtkMRMLLongPETCTStudyNode* study, bool disabled = false);

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
  QScopedPointer<qSlicerLongPETCTStudySelectionTableWidgetPrivate> d_ptr;


private:
  Q_DECLARE_PRIVATE(qSlicerLongPETCTStudySelectionTableWidget);
  Q_DISABLE_COPY(qSlicerLongPETCTStudySelectionTableWidget);

};

#endif
