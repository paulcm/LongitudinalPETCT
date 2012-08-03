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

#ifndef __qSlicerLongPETCTStudySliderWidget_h
#define __qSlicerLongPETCTStudySliderWidget_h

// Qt includes
#include <QWidget>
#include <QString>

// LongPETCTStudySlider Widgets includes
#include "qSlicerLongPETCTModuleWidgetsExport.h"


class qSlicerLongPETCTStudySliderWidgetPrivate;


/// \ingroup Slicer_QtModules_LongitudinalPETCT
class Q_SLICER_MODULE_LONGPETCT_WIDGETS_EXPORT qSlicerLongPETCTStudySliderWidget
  : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(int maximum READ maximumValue WRITE setMaximumValue)
  Q_PROPERTY(int current READ currentValue WRITE setCurrentValue)
  Q_PROPERTY(QString selectedStudy READ selectedStudy WRITE setSelectedStudy)

public:
  typedef QWidget Superclass;
  qSlicerLongPETCTStudySliderWidget(QWidget *parent=0);
  virtual ~qSlicerLongPETCTStudySliderWidget();

  int maximumValue();
  int currentValue();
  QString selectedStudy();

  void setMaximumValue(int value);
  void setCurrentValue(int value);
  void setSelectedStudy(const QString& selectedStudy);

protected slots:

protected:
  QScopedPointer<qSlicerLongPETCTStudySliderWidgetPrivate> d_ptr;


private:
  Q_DECLARE_PRIVATE(qSlicerLongPETCTStudySliderWidget);
  Q_DISABLE_COPY(qSlicerLongPETCTStudySliderWidget);



};

#endif
