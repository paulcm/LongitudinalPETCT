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

#ifndef __qMRMLLongitudinalPETCTStudyTableWidget_h
#define __qMRMLLongitudinalPETCTStudyTableWidget_h


#include <qMRMLLongitudinalPETCTWidget.h>

// LongitudinalPETCTReportTable Widgets includes
#include "qSlicerLongitudinalPETCTModuleWidgetsExport.h"


class qMRMLLongitudinalPETCTStudyTableWidgetPrivate;

class vtkMRMLLongitudinalPETCTStudyNode;
class QTableWidgetItem;

/// \ingroup Slicer_QtModules_LongitudinalitudinalPETCT
class Q_SLICER_LONGITUDINALPETCT_MODULE_WIDGETS_EXPORT qMRMLLongitudinalPETCTStudyTableWidget
  : public qMRMLLongitudinalPETCTWidget
{
  Q_OBJECT


public:
  typedef qMRMLLongitudinalPETCTWidget Superclass;
  qMRMLLongitudinalPETCTStudyTableWidget(QWidget *parent=0);
  virtual ~qMRMLLongitudinalPETCTStudyTableWidget();

  enum TableMode {
     SegmentationSelection = 0,
     AnalysisSelection = 1
  };

  void addStudyToTable(vtkMRMLLongitudinalPETCTStudyNode* study, bool disabled = false);

  void setTableMode(TableMode mode);
  TableMode tableMode();

signals:
  void studySelectionChanged(int index, bool selected);

public slots:
  void updateWidgetFromMRML();

protected slots:
  void tableItemChanged(QTableWidgetItem* item);

protected:
  QScopedPointer<qMRMLLongitudinalPETCTStudyTableWidgetPrivate> d_ptr;


private:
  Q_DECLARE_PRIVATE(qMRMLLongitudinalPETCTStudyTableWidget);
  Q_DISABLE_COPY(qMRMLLongitudinalPETCTStudyTableWidget);

};

#endif
