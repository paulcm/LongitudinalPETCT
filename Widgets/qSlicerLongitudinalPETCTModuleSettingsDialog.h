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

#ifndef __qSlicerLongitudinalPETCTModuleSettingsDialog_h
#define __qSlicerLongitudinalPETCTModuleSettingsDialog_h

// Qt includes
#include <QDialog>


#include "qSlicerLongitudinalPETCTModuleWidgetsExport.h"



class qSlicerLongitudinalPETCTModuleSettingsDialogPrivate;


/// \ingroup Slicer_QtModules_LongitudinalPETCT
class Q_SLICER_LONGITUDINALPETCT_MODULE_WIDGETS_EXPORT qSlicerLongitudinalPETCTModuleSettingsDialog
  : public QDialog
{
  Q_OBJECT

  Q_PROPERTY(bool registration READ registration WRITE setRegistration)
  Q_PROPERTY(bool volumeRendering READ volumeRendering WRITE setVolumeRendering)
  Q_PROPERTY(bool spinning READ spinning WRITE setSpinning)
  Q_PROPERTY(bool makeModels READ makeModels WRITE setMakeModels)
  Q_PROPERTY(bool outlineSegmentations READ outlineSegmentations WRITE setOutlineSegmentations)


public:
  typedef QDialog Superclass;
  qSlicerLongitudinalPETCTModuleSettingsDialog(QWidget *parent=0);
  virtual ~qSlicerLongitudinalPETCTModuleSettingsDialog();


  bool registration();
  bool volumeRendering();
  bool spinning();
  bool makeModels();
  bool outlineSegmentations();

  void setRegistration(bool checked);
  void setVolumeRendering(bool checked);
  void setSpinning(bool checked);
  void setMakeModels(bool checked);
  void setOutlineSegmentations(bool checked);



protected:
  QScopedPointer<qSlicerLongitudinalPETCTModuleSettingsDialogPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerLongitudinalPETCTModuleSettingsDialog);
  Q_DISABLE_COPY(qSlicerLongitudinalPETCTModuleSettingsDialog);


};

#endif
