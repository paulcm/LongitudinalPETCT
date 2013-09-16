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

#ifndef __qMRMLLongitudinalPETCTModuleSettingsWidget_h
#define __qMRMLLongitudinalPETCTModuleSettingsWidget_h

// Qt includes
#include <qMRMLLongitudinalPETCTWidget.h>

#include "qSlicerLongitudinalPETCTModuleWidgetsExport.h"



class qMRMLLongitudinalPETCTModuleSettingsWidgetPrivate;
class QAbstractButton;

/// \ingroup Slicer_QtModules_LongitudinalPETCT
class Q_SLICER_LONGITUDINALPETCT_MODULE_WIDGETS_EXPORT qMRMLLongitudinalPETCTModuleSettingsWidget
  : public qMRMLLongitudinalPETCTWidget
{
  Q_OBJECT

  Q_PROPERTY(bool petSettingsVisible READ petSettingsVisible WRITE setPetSettingsVisible)
  Q_PROPERTY(bool ctSettingsVisible READ ctSettingsVisible WRITE setCtSettingsVisible)
  Q_PROPERTY(bool segmentationSettingsVisible READ segmentationSettingsVisible WRITE setSegmentationSettingsVisible)
  Q_PROPERTY(bool registrationSettingsVisible READ registrationSettingsVisible WRITE setRegistrationSettingsVisible)
  Q_PROPERTY(bool quantificationSettingsVisible READ quantificationSettingsVisible WRITE setQuantificationSettingsVisible)

  Q_PROPERTY(bool segmentationSettingsVisible READ segmentationSettingsVisible WRITE setSegmentationSettingsVisible)
  Q_PROPERTY(bool registrationSettingsVisible READ registrationSettingsVisible WRITE setRegistrationSettingsVisible)
  Q_PROPERTY(bool quantificationSettingsVisible READ quantificationSettingsVisible WRITE setQuantificationSettingsVisible)

  Q_PROPERTY(int petColorButton READ petColorButton WRITE setPetColorButton)
  Q_PROPERTY(int ctWindowLevelButton READ ctWindowLevelButton WRITE setCtWindowLevelButton)

  Q_PROPERTY(bool volumeRendering READ volumeRendering WRITE setVolumeRendering)
  Q_PROPERTY(bool spinning READ spinning WRITE setSpinning)
  Q_PROPERTY(double petOpacityPow READ petOpacityPow WRITE setPetOpacityPow)


  Q_PROPERTY(bool outlineSegmentations READ outlineSegmentations WRITE setOutlineSegmentations)
  Q_PROPERTY(bool makeModels READ makeModels WRITE setMakeModels)

  Q_PROPERTY(bool registration READ registration WRITE setRegistration)




public:
  typedef qMRMLLongitudinalPETCTWidget Superclass;
  qMRMLLongitudinalPETCTModuleSettingsWidget(QWidget *parent=0);
  virtual ~qMRMLLongitudinalPETCTModuleSettingsWidget();

  bool petSettingsVisible();
  bool ctSettingsVisible();
  bool segmentationSettingsVisible();
  bool registrationSettingsVisible();
  bool quantificationSettingsVisible();


  void setPetSettingsVisible(bool visible);
  void setCtSettingsVisible(bool visible);
  void setSegmentationSettingsVisible(bool visible);
  void setRegistrationSettingsVisible(bool visible);
  void setQuantificationSettingsVisible(bool visible);

  int petColorButton();
  int ctWindowLevelButton();

  void setPetColorButton(int id);
  void setCtWindowLevelButton(int id);

  bool volumeRendering();
  bool spinning();
  bool outlineSegmentations();
  bool makeModels();
  double petOpacityPow();

  bool registration();

  void setVolumeRendering(bool on);
  void setSpinning(bool on);
  void setPetOpacityPow(double value);

  void setOutlineSegmentations(bool on);
  void setMakeModels(bool on);

  void setRegistration(bool on);

  Q_INVOKABLE double ctWindow();
  Q_INVOKABLE double ctLevel();
  Q_INVOKABLE const QString petColorNodeID();


signals:
  void petColorChanged(const QString& proceduralPETColorTableName);
  void ctWindowLevelChanged(double window, double level);

  void changeModuleSetting(const QString& settingName, bool on);
  void changeModuleSettingValue(const QString& settingName, double value);

  void volumeRenderingToggled(bool checked);
  void spinningToggled(bool checked);
  void opacityPowChanged(double value);
  void outlineSegmentationsToggled(bool checked);

  void quantificationModuleChanged(const QString& name);

public slots:
  void setPETVROpacityPowSliderRange(double min, double max);

protected slots:
  void sliderPETVROpacityPowValueChanged(double value);
  void showQuantificationCLIInputDialog();

  void petColorsButtonClicked(QAbstractButton* button);
  void ctWindowLevelPresetButtonClicked(QAbstractButton* button);

  void moduleSettingChanged(bool on);
  void moduleSettingValueChanged(int value);
  void moduleSettingDoubleValueChanged(double value);

protected:
  QScopedPointer<qMRMLLongitudinalPETCTModuleSettingsWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_DISABLE_COPY(qMRMLLongitudinalPETCTModuleSettingsWidget);


};

#endif
