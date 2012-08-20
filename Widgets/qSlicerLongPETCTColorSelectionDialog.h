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

#ifndef __qSlicerLongPETCTColorSelectionDialog_h
#define __qSlicerLongPETCTColorSelectionDialog_h

// Qt includes
#include <QDialog>


// LongPETCTFindingSelection Widgets includes
#include "qSlicerLongPETCTModuleWidgetsExport.h"


class vtkMRMLColorNode;


class qSlicerLongPETCTColorSelectionDialogPrivate;


/// \ingroup Slicer_QtModules_LongPETCT
class Q_SLICER_LONGPETCT_MODULE_WIDGETS_EXPORT qSlicerLongPETCTColorSelectionDialog
  : public QDialog
{
  Q_OBJECT

public:
  typedef QDialog Superclass;
  qSlicerLongPETCTColorSelectionDialog(QWidget *parent=0);
  virtual ~qSlicerLongPETCTColorSelectionDialog();

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
  QScopedPointer<qSlicerLongPETCTColorSelectionDialogPrivate> d_ptr;


private:
  Q_DECLARE_PRIVATE(qSlicerLongPETCTColorSelectionDialog);
  Q_DISABLE_COPY(qSlicerLongPETCTColorSelectionDialog);



};

#endif
