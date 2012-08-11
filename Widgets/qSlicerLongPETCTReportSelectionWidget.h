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

#ifndef __qSlicerLongPETCTReportSelectionWidget_h
#define __qSlicerLongPETCTReportSelectionWidget_h

// Qt includes
#include <QWidget>





// LongPETCTReportSelection Widgets includes
#include "qSlicerLongPETCTModuleWidgetsExport.h"

class qSlicerLongPETCTReportSelectionWidgetPrivate;
class qMRMLNodeComboBox;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_LongPETCT
class Q_SLICER_LONGPETCT_MODULE_WIDGETS_EXPORT qSlicerLongPETCTReportSelectionWidget
  : public QWidget
{
  Q_OBJECT
//Q_PROPERTY(QWidget* mrmlNodeComboboxReports READ mrmlNodeComboboxReports WRITE setMrmlNodeComboboxReports)

public:
  typedef QWidget Superclass;
  qSlicerLongPETCTReportSelectionWidget(QWidget *parent=0);
  virtual ~qSlicerLongPETCTReportSelectionWidget();

  //qMRMLNodeComboBox* mrmlNodeComboBoxReports();
  Q_INVOKABLE void setMRMLNodeComboBoxReports(qMRMLNodeComboBox* reportsComboBox);

public slots:

  void updateReportInformation(vtkMRMLNode* node);


protected:
  QScopedPointer<qSlicerLongPETCTReportSelectionWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerLongPETCTReportSelectionWidget);
  Q_DISABLE_COPY(qSlicerLongPETCTReportSelectionWidget);



};

#endif
