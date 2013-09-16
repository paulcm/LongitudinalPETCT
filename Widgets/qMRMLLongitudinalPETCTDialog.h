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

#ifndef __qMRMLLongitudinalPETCTDialog_h
#define __qMRMLLongitudinalPETCTDialog_h


#include <QDialog>

// CTK includes
#include <ctkVTKObject.h>

// LongitudinalPETCTReportSelection Widgets includes
#include "qSlicerLongitudinalPETCTModuleWidgetsExport.h"

class qMRMLLongitudinalPETCTDialogPrivate;
class vtkMRMLLongitudinalPETCTReportNode;


/// \ingroup Slicer_QtModules_LongitudinalPETCT
class Q_SLICER_LONGITUDINALPETCT_MODULE_WIDGETS_EXPORT qMRMLLongitudinalPETCTDialog
  : public QDialog
{
  Q_OBJECT
  QVTK_OBJECT

public:
  typedef QDialog Superclass;
  qMRMLLongitudinalPETCTDialog(QWidget *parent=0);
  virtual ~qMRMLLongitudinalPETCTDialog();

  Q_INVOKABLE vtkMRMLLongitudinalPETCTReportNode* reportNode() const;

signals:
  void reportNodeChanged(vtkMRMLLongitudinalPETCTReportNode* reportNode);

public slots:
  void setReportNode(vtkMRMLLongitudinalPETCTReportNode* reportNode);
  virtual void updateDialogFromMRML(){};

protected:
  QScopedPointer<qMRMLLongitudinalPETCTDialogPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qMRMLLongitudinalPETCTDialog);
  Q_DISABLE_COPY(qMRMLLongitudinalPETCTDialog);


};

#endif
