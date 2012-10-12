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

#ifndef __qSlicerLongitudinalPETCTColorSelectionDialog_h
#define __qSlicerLongitudinalPETCTColorSelectionDialog_h

// Qt includes
#include <QDialog>


// LongitudinalPETCTFindingSelection Widgets includes
#include "qSlicerLongitudinalPETCTModuleWidgetsExport.h"


class vtkMRMLColorNode;


class qSlicerLongitudinalPETCTColorSelectionDialogPrivate;


/// \ingroup Slicer_QtModules_LongitudinalPETCT
class Q_SLICER_LONGITUDINALPETCT_MODULE_WIDGETS_EXPORT qSlicerLongitudinalPETCTColorSelectionDialog
  : public QDialog
{
  Q_OBJECT

public:
  typedef QDialog Superclass;
  qSlicerLongitudinalPETCTColorSelectionDialog(QWidget *parent=0);
  virtual ~qSlicerLongitudinalPETCTColorSelectionDialog();

  static int colorIDSelectionForNode(QWidget* parent, const vtkMRMLColorNode* colorNode);
  static QColor getRGBColorFromDoubleValues(double r, double g, double b);

  int getColorIDByListName(const QString& name);

  void setColorNode(const vtkMRMLColorNode* colorNode);
  const vtkMRMLColorNode* colorNode();

  int selectedColorID();

public slots:
  void populateColorsList(const QString& filter = "");

protected slots:
  void colorSelectionChanged();

protected:
  QScopedPointer<qSlicerLongitudinalPETCTColorSelectionDialogPrivate> d_ptr;


private:
  Q_DECLARE_PRIVATE(qSlicerLongitudinalPETCTColorSelectionDialog);
  Q_DISABLE_COPY(qSlicerLongitudinalPETCTColorSelectionDialog);



};

#endif
