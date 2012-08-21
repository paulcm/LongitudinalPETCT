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

#ifndef __qSlicerLongPETCTReportTableWidget_h
#define __qSlicerLongPETCTReportTableWidget_h

// Qt includes
#include <QTableWidget>

// LongPETCTReportTable Widgets includes
#include "qSlicerLongPETCTModuleWidgetsExport.h"
#include <vtkSmartPointer.h>
#include <vtkMRMLLongPETCTReportNode.h>

class qSlicerLongPETCTReportTableWidgetPrivate;

class vtkMRMLNode;



/// \ingroup Slicer_QtModules_LongitudinalPETCT
class Q_SLICER_LONGPETCT_MODULE_WIDGETS_EXPORT qSlicerLongPETCTReportTableWidget
  : public QTableWidget
{
  Q_OBJECT

public:
  typedef QTableWidget Superclass;
  qSlicerLongPETCTReportTableWidget(QWidget *parent=0);
  virtual ~qSlicerLongPETCTReportTableWidget();


public slots:
  void setReportNode(vtkMRMLLongPETCTReportNode* reportNode);

protected slots:
  void selectStudyColumn(int index);
  void selectFindingRow(int index);


protected:
  QScopedPointer<qSlicerLongPETCTReportTableWidgetPrivate> d_ptr;

  virtual void resizeEvent(QResizeEvent* event);
  void arrangeColumns();

private:
  Q_DECLARE_PRIVATE(qSlicerLongPETCTReportTableWidget);
  Q_DISABLE_COPY(qSlicerLongPETCTReportTableWidget);



};

#endif
