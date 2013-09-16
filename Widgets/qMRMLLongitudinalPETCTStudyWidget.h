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

#ifndef __qMRMLLongitudinalPETCTStudyWidget_h
#define __qMRMLLongitudinalPETCTStudyWidget_h


#include <qMRMLLongitudinalPETCTWidget.h>



#include <ctkVTKObject.h>

// LongitudinalPETCTStudySelection Widgets includes
#include "qSlicerLongitudinalPETCTModuleWidgetsExport.h"

class vtkMRMLNode;
class vtkMRMLLongitudinalPETCTReportNode;
class vtkMRMLLongitudinalPETCTStudyNode;
class qMRMLLongitudinalPETCTStudyWidgetPrivate;

/// \ingroup Slicer_QtModules_LongitudinalitudinalPETCT
class Q_SLICER_LONGITUDINALPETCT_MODULE_WIDGETS_EXPORT qMRMLLongitudinalPETCTStudyWidget
  : public qMRMLLongitudinalPETCTWidget
{
  Q_OBJECT
  QVTK_OBJECT


public:
  typedef qMRMLLongitudinalPETCTWidget Superclass;
  qMRMLLongitudinalPETCTStudyWidget(QWidget *parent=0);
  virtual ~qMRMLLongitudinalPETCTStudyWidget();

  bool volumeRendering();
  bool spinView();
  double opacityPow();

signals:
  void studySelectedForSegmentation(vtkMRMLLongitudinalPETCTStudyNode* study, bool selected);
  void registrationFixedStudyChanged(vtkMRMLLongitudinalPETCTStudyNode* study);

public slots:
  virtual void setReportNode(vtkMRMLLongitudinalPETCTReportNode* reportNode);

protected slots:
  void studySelectedInTable(int index, bool selected);

protected:
  QScopedPointer<qMRMLLongitudinalPETCTStudyWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qMRMLLongitudinalPETCTStudyWidget);
  Q_DISABLE_COPY(qMRMLLongitudinalPETCTStudyWidget);



};

#endif
