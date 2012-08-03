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
#include <QStringList>

// LongPETCTStudySelection Widgets includes
#include "qSlicerLongPETCTModuleWidgetsExport.h"


class qSlicerLongPETCTStudySelectionWidgetPrivate;


/// \ingroup Slicer_QtModules_LongitudinalPETCT
class Q_SLICER_MODULE_LONGPETCT_WIDGETS_EXPORT qSlicerLongPETCTStudySelectionWidget
  : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(QStringList patientNames READ patientNames WRITE setPatientNames)



public:
  typedef QWidget Superclass;
  qSlicerLongPETCTStudySelectionWidget(QWidget *parent=0);
  virtual ~qSlicerLongPETCTStudySelectionWidget();

  QStringList patientNames();
  void setPatientNames(const QStringList& patientNames);

protected slots:

protected:
  QScopedPointer<qSlicerLongPETCTStudySelectionWidgetPrivate> d_ptr;


private:
  Q_DECLARE_PRIVATE(qSlicerLongPETCTStudySelectionWidget);
  Q_DISABLE_COPY(qSlicerLongPETCTStudySelectionWidget);



};

#endif
