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

#include <qMRMLNodeComboBox.h>


/// \ingroup Slicer_QtModules_LongitudinalPETCT
class Q_SLICER_MODULE_LONGPETCT_WIDGETS_EXPORT qSlicerLongPETCTReportSelectionWidget
  : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(qMRMLNodeComboBox* mrmlNodeComboboxReports READ mrmlNodeComboboxReports WRITE setMrmlNodeComboboxReports)

public:
  typedef QWidget Superclass;
  qSlicerLongPETCTReportSelectionWidget(qMRMLNodeComboBox* mrmlNodeComboboxReports, QWidget *parent=0);
  virtual ~qSlicerLongPETCTReportSelectionWidget();

  qMRMLNodeComboBox* mrmlNodeComboboxReports();
  void setMrmlNodeComboboxReports(qMRMLNodeComboBox* mrmlNodeComboboxReports);

protected slots:

protected:
  QScopedPointer<qSlicerLongPETCTReportSelectionWidgetPrivate> d_ptr;


private:
  Q_DECLARE_PRIVATE(qSlicerLongPETCTReportSelectionWidget);
  Q_DISABLE_COPY(qSlicerLongPETCTReportSelectionWidget);



};

#endif
