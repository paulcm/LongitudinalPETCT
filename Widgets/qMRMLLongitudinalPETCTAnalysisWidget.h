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

#ifndef __qMRMLLongitudinalPETCTAnalysisWidget_h
#define __qMRMLLongitudinalPETCTAnalysisWidget_h

// Qt includes
#include <qMRMLLongitudinalPETCTWidget.h>

// CTK includes
#include <ctkVTKObject.h>

// LongitudinalPETCTAnalysisSettings Widgets includes
#include "qSlicerLongitudinalPETCTModuleWidgetsExport.h"




class vtkMRMLNode;
class vtkMRMLLongitudinalPETCTReportNode;
class vtkMRMLLongitudinalPETCTStudyNode;
class qMRMLLongitudinalPETCTAnalysisWidgetPrivate;

/// \ingroup Slicer_QtModules_LongitudinalPETCT
class Q_SLICER_LONGITUDINALPETCT_MODULE_WIDGETS_EXPORT qMRMLLongitudinalPETCTAnalysisWidget
  : public qMRMLLongitudinalPETCTWidget
{
  Q_OBJECT
  QVTK_OBJECT

  Q_PROPERTY(bool qualitativeChecked READ qualitativeChecked WRITE setQualitativeChecked)
  Q_PROPERTY(bool quantitativeChecked READ quantitativeChecked WRITE setQuantitativeChecked)

public:
  typedef qMRMLLongitudinalPETCTWidget Superclass;
  qMRMLLongitudinalPETCTAnalysisWidget(QWidget *parent=0);
  virtual ~qMRMLLongitudinalPETCTAnalysisWidget();

  bool qualitativeChecked();
  bool quantitativeChecked();

  void setQualitativeChecked(bool checked);
  void setQuantitativeChecked(bool checked);

signals:
  void qualitativeAnalysisClicked(bool checked);
  void quantitativeAnalysisClicked(bool checked);
  void studySelectedForAnalysis(vtkMRMLLongitudinalPETCTStudyNode* study, bool selected);
  void exportCSV();

public slots:
  virtual void setReportNode(vtkMRMLLongitudinalPETCTReportNode* reportNode);


protected slots:
  void studySelectedInTable(int index, bool selected);

protected:
  QScopedPointer<qMRMLLongitudinalPETCTAnalysisWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qMRMLLongitudinalPETCTAnalysisWidget);
  Q_DISABLE_COPY(qMRMLLongitudinalPETCTAnalysisWidget);



};

#endif
