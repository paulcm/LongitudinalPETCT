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

#ifndef __qSlicerLongPETCTReportTree_h
#define __qSlicerLongPETCTReportTree_h

// Qt includes
#include <QTreeWidget>
#include <QString>

// LongPETCTStudySlider Widgets includes
#include "qSlicerLongPETCTModuleWidgetsExport.h"


class qSlicerLongPETCTReportTreePrivate;


/// \ingroup Slicer_QtModules_LongitudinalPETCT
class Q_SLICER_MODULE_LONGPETCT_WIDGETS_EXPORT qSlicerLongPETCTReportTree
  : public QTreeWidget
{
  Q_OBJECT
//  Q_PROPERTY(int maximum READ maximumValue WRITE setMaximumValue)
//  Q_PROPERTY(int current READ currentValue WRITE setCurrentValue)
//  Q_PROPERTY(QString selectedStudy READ selectedStudy WRITE setSelectedStudy)

public:
  typedef QTreeWidget Superclass;
  qSlicerLongPETCTReportTree(QWidget *parent=0);
  virtual ~qSlicerLongPETCTReportTree();


protected slots:

protected:
  QScopedPointer<qSlicerLongPETCTReportTreePrivate> d_ptr;


private:
  Q_DECLARE_PRIVATE(qSlicerLongPETCTReportTree);
  Q_DISABLE_COPY(qSlicerLongPETCTReportTree);



};

#endif
