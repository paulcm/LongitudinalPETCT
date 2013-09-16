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

// LongitudinalPETCTFindingSelection Widgets includes
#include "qMRMLLongitudinalPETCTModuleSettingsWidget.h"
#include "ui_qMRMLLongitudinalPETCTModuleSettingsWidget.h"

#include <QAbstractButton>
#include <QButtonGroup>
#include <QInputDialog>
#include <QSettings>


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LongitudinalPETCT
class qMRMLLongitudinalPETCTModuleSettingsWidgetPrivate : public Ui_qMRMLLongitudinalPETCTModuleSettingsWidget
{
  Q_DECLARE_PUBLIC(qMRMLLongitudinalPETCTModuleSettingsWidget);

protected:
  qMRMLLongitudinalPETCTModuleSettingsWidget* const q_ptr;

public:
  qMRMLLongitudinalPETCTModuleSettingsWidgetPrivate(
      qMRMLLongitudinalPETCTModuleSettingsWidget& object);

  virtual
  ~qMRMLLongitudinalPETCTModuleSettingsWidgetPrivate();

  virtual void
  setupUi(qMRMLLongitudinalPETCTModuleSettingsWidget* widget);

  QButtonGroup* ButtonGroupPETColors;
  QButtonGroup* ButtonGroupCTWIndowLevelPresets;

};

// --------------------------------------------------------------------------
qMRMLLongitudinalPETCTModuleSettingsWidgetPrivate::qMRMLLongitudinalPETCTModuleSettingsWidgetPrivate(
    qMRMLLongitudinalPETCTModuleSettingsWidget& object) :
    q_ptr(&object)
{
}

// --------------------------------------------------------------------------
qMRMLLongitudinalPETCTModuleSettingsWidgetPrivate::~qMRMLLongitudinalPETCTModuleSettingsWidgetPrivate()
{
}

// --------------------------------------------------------------------------
void
qMRMLLongitudinalPETCTModuleSettingsWidgetPrivate::setupUi(
    qMRMLLongitudinalPETCTModuleSettingsWidget* widget)
{
  Q_Q(qMRMLLongitudinalPETCTModuleSettingsWidget);

  this->Ui_qMRMLLongitudinalPETCTModuleSettingsWidget::setupUi(widget);


  this->ButtonGroupPETColors = new QButtonGroup(q);
  this->ButtonGroupPETColors->addButton(this->ButtonPETHeat,0);
  this->ButtonGroupPETColors->addButton(this->ButtonPETRainbow,1);
  this->ButtonGroupPETColors->addButton(this->ButtonPETMIP,2);

  this->ButtonGroupCTWIndowLevelPresets = new QButtonGroup(q);
  this->ButtonGroupCTWIndowLevelPresets->addButton(this->ButtonCTWLAbdomen,0);
  this->ButtonGroupCTWIndowLevelPresets->addButton(this->ButtonCTWLBrain,1);
  this->ButtonGroupCTWIndowLevelPresets->addButton(this->ButtonCTWLBone,2);
  this->ButtonGroupCTWIndowLevelPresets->addButton(this->ButtonCTWLLung,3);

  this->ExpandButton->mirrorOnExpand();
  this->ExpandButton->setChecked(true);

  this->SliderPETVROpacityPow->setMinimum(0.);
  this->SliderPETVROpacityPow->setMaximum(5.0);
  this->SliderPETVROpacityPow->setSingleStep(0.1);
  this->SliderPETVROpacityPow->setPageStep(1.0);
  this->SliderPETVROpacityPow->setTickInterval(1.0);

  this->FrameSegmentation->setVisible(false);
  this->FrameRegistration->setVisible(false);
  this->FrameQuantification->setVisible(false);

  QObject::connect(this->SliderPETVROpacityPow, SIGNAL(valueChanged(double)), q, SLOT(sliderPETVROpacityPowValueChanged(double)));

  QObject::connect(this->ButtonGroupPETColors, SIGNAL(buttonClicked(QAbstractButton*)), q, SLOT(petColorsButtonClicked(QAbstractButton*)));
  QObject::connect(this->ButtonGroupCTWIndowLevelPresets, SIGNAL(buttonClicked(QAbstractButton*)), q, SLOT(ctWindowLevelPresetButtonClicked(QAbstractButton*)));

  QObject::connect(this->ButtonPETVolumeRendering, SIGNAL(toggled(bool)), q, SIGNAL(volumeRenderingToggled(bool)));
  QObject::connect(this->ButtonPETSpinning, SIGNAL(toggled(bool)), q, SIGNAL(spinningToggled(bool)));

  QObject::connect(this->ButtonPETVolumeRendering, SIGNAL(toggled(bool)), q, SLOT(moduleSettingChanged(bool)));
  QObject::connect(this->ButtonPETSpinning, SIGNAL(toggled(bool)), q, SLOT(moduleSettingChanged(bool)));

  QObject::connect(this->ButtonOutline, SIGNAL(toggled(bool)), q, SLOT(moduleSettingChanged(bool)));
  QObject::connect(this->ButtonOutline, SIGNAL(toggled(bool)), q, SIGNAL(outlineSegmentationsToggled(bool)));
  QObject::connect(this->ButtonModels, SIGNAL(toggled(bool)), q, SLOT(moduleSettingChanged(bool)));


  QObject::connect(this->ButtonRegistration, SIGNAL(toggled(bool)), q, SLOT(moduleSettingChanged(bool)));
  QObject::connect(this->ButtonQuantificationModule, SIGNAL(clicked()), q, SLOT(showQuantificationCLIInputDialog()));
}


//-----------------------------------------------------------------------------
// qMRMLLongitudinalPETCTModuleSettingsWidget methods

//-----------------------------------------------------------------------------
qMRMLLongitudinalPETCTModuleSettingsWidget::qMRMLLongitudinalPETCTModuleSettingsWidget(
    QWidget* parentWidget) :
    Superclass(parentWidget), d_ptr(
        new qMRMLLongitudinalPETCTModuleSettingsWidgetPrivate(*this))
{
  Q_D(qMRMLLongitudinalPETCTModuleSettingsWidget);
  d->setupUi(this);

}

//-----------------------------------------------------------------------------
qMRMLLongitudinalPETCTModuleSettingsWidget::~qMRMLLongitudinalPETCTModuleSettingsWidget()
{
}


//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTModuleSettingsWidget::sliderPETVROpacityPowValueChanged(double value)
{
  Q_D(qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->LabelPETVROpacityPow);

  d->LabelPETVROpacityPow->setText(QString().setNum(value,'f',2));

  this->moduleSettingDoubleValueChanged(value);

  emit opacityPowChanged(value);
}

//-----------------------------------------------------------------------------
bool qMRMLLongitudinalPETCTModuleSettingsWidget::petSettingsVisible()
{
  Q_D(qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->ToolButtonPET);

  return d->ToolButtonPET->isChecked();
}
//-----------------------------------------------------------------------------
bool qMRMLLongitudinalPETCTModuleSettingsWidget::ctSettingsVisible()
{
  Q_D(qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->ToolButtonCT);

  return d->ToolButtonCT->isChecked();
}

//-----------------------------------------------------------------------------
bool qMRMLLongitudinalPETCTModuleSettingsWidget::segmentationSettingsVisible()
{
  Q_D(qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->ToolButtonSegmentation);

  return d->ToolButtonSegmentation->isChecked();
}
//-----------------------------------------------------------------------------
bool qMRMLLongitudinalPETCTModuleSettingsWidget::registrationSettingsVisible()
{
  Q_D(qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->ToolButtonRegistration);

  return d->ToolButtonRegistration->isChecked();
}
//-----------------------------------------------------------------------------
bool qMRMLLongitudinalPETCTModuleSettingsWidget::quantificationSettingsVisible()
{
  Q_D(qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->ToolButtonQuantification);

  return d->ToolButtonQuantification->isChecked();
}
//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTModuleSettingsWidget::setPetSettingsVisible(bool visible)
{
  Q_D(qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->ToolButtonPET);

  d->ToolButtonPET->setChecked(visible);
}
//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTModuleSettingsWidget::setCtSettingsVisible(bool visible)
{
  Q_D(qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->ToolButtonCT);

  d->ToolButtonCT->setChecked(visible);
}

//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTModuleSettingsWidget::setSegmentationSettingsVisible(bool visible)
{
  Q_D(qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->ToolButtonSegmentation);

  d->ToolButtonSegmentation->setChecked(visible);
}
//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTModuleSettingsWidget::setRegistrationSettingsVisible(bool visible)
{
  Q_D(qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->ToolButtonRegistration);

  d->ToolButtonRegistration->setChecked(visible);
}
//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTModuleSettingsWidget::setQuantificationSettingsVisible(bool visible)
{
  Q_D(qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->ToolButtonQuantification);

  d->ToolButtonQuantification->setChecked(visible);
}


//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTModuleSettingsWidget::setPETVROpacityPowSliderRange(double min, double max)
{
  Q_D(qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->SliderPETVROpacityPow);

  d->SliderPETVROpacityPow->setRange(min,max);
}

//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTModuleSettingsWidget::petColorsButtonClicked(QAbstractButton* button)
{
  Q_D(qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->ButtonGroupPETColors);

  QString accessibleName = button->accessibleName();

  emit this->petColorChanged(accessibleName);
  this->moduleSettingValueChanged(d->ButtonGroupPETColors->id(button));
}

//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTModuleSettingsWidget::ctWindowLevelPresetButtonClicked(QAbstractButton* button)
{
  Q_D(qMRMLLongitudinalPETCTModuleSettingsWidget);

  double window = this->ctWindow();
  double level = this->ctLevel();

  emit this->ctWindowLevelChanged(window, level);

  this->moduleSettingValueChanged(d->ButtonGroupCTWIndowLevelPresets->id(button));
}

//-----------------------------------------------------------------------------
int
qMRMLLongitudinalPETCTModuleSettingsWidget::petColorButton()
{
  Q_D (qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->ButtonGroupPETColors);

  return d->ButtonGroupPETColors->checkedId();
}

//-----------------------------------------------------------------------------
int
qMRMLLongitudinalPETCTModuleSettingsWidget::ctWindowLevelButton()
{
  Q_D (qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->ButtonGroupCTWIndowLevelPresets);

  return d->ButtonGroupCTWIndowLevelPresets->checkedId();
}

//-----------------------------------------------------------------------------
void
qMRMLLongitudinalPETCTModuleSettingsWidget::setPetColorButton(int id)
{
  Q_D (qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->ButtonGroupPETColors);

  d->ButtonGroupPETColors->button(id)->setChecked(true);
}

//-----------------------------------------------------------------------------
void
qMRMLLongitudinalPETCTModuleSettingsWidget::setCtWindowLevelButton(int id)
{
  Q_D (qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->ButtonGroupCTWIndowLevelPresets);

  d->ButtonGroupCTWIndowLevelPresets->button(id)->setChecked(true);
}

//-----------------------------------------------------------------------------
bool qMRMLLongitudinalPETCTModuleSettingsWidget::volumeRendering()
{
  Q_D (qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->ButtonPETVolumeRendering);

  return d->ButtonPETVolumeRendering->isChecked();
}

//-----------------------------------------------------------------------------
bool qMRMLLongitudinalPETCTModuleSettingsWidget::spinning()
{
  Q_D (qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->ButtonPETSpinning);

  return d->ButtonPETSpinning->isChecked();
}

//-----------------------------------------------------------------------------
double qMRMLLongitudinalPETCTModuleSettingsWidget::petOpacityPow()
{
  Q_D (qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->SliderPETVROpacityPow);

  return d->SliderPETVROpacityPow->value();
}


//-----------------------------------------------------------------------------
bool qMRMLLongitudinalPETCTModuleSettingsWidget::outlineSegmentations()
{
  Q_D (qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->ButtonOutline);

  return d->ButtonOutline->isChecked();
}

//-----------------------------------------------------------------------------
bool qMRMLLongitudinalPETCTModuleSettingsWidget::makeModels()
{
  Q_D (qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->ButtonModels);

  return d->ButtonModels->isChecked();
}

//-----------------------------------------------------------------------------
bool qMRMLLongitudinalPETCTModuleSettingsWidget::registration()
{
  Q_D (qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->ButtonRegistration);

  return d->ButtonRegistration->isChecked();
}


//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTModuleSettingsWidget::setVolumeRendering(bool on)
{
  Q_D (qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->ButtonPETVolumeRendering);

  d->ButtonPETVolumeRendering->setChecked(on);
}
//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTModuleSettingsWidget::setSpinning(bool on)
{
  Q_D (qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->ButtonPETSpinning);

  d->ButtonPETSpinning->setChecked(on);
}

//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTModuleSettingsWidget::setPetOpacityPow(double value)
{
  Q_D (qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->SliderPETVROpacityPow);

  d->SliderPETVROpacityPow->setValue(value);
}

//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTModuleSettingsWidget::setOutlineSegmentations(bool on)
{
  Q_D (qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->ButtonOutline);

  d->ButtonOutline->setChecked(on);
}

//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTModuleSettingsWidget::setMakeModels(bool on)
{
  Q_D (qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->ButtonModels);

  d->ButtonModels->setChecked(on);
}

//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTModuleSettingsWidget::setRegistration(bool on)
{
  Q_D (qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->ButtonRegistration);

  d->ButtonRegistration->setChecked(on);
}

//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTModuleSettingsWidget::moduleSettingChanged(bool on)
{
  Q_D (qMRMLLongitudinalPETCTModuleSettingsWidget);

  QObject* sender = QObject::sender();

  QString settingName;

  if(sender == d->ButtonPETVolumeRendering)
    settingName = "volumeRendering";

  else if(sender == d->ButtonPETSpinning)
    settingName = "spinning";

  else if(sender == d->ButtonRegistration)
    settingName = "registration";

  else if(sender == d->ButtonOutline)
    settingName = "outlineSegmentations";

  else if(sender == d->ButtonModels)
    settingName = "makeModels";


  emit changeModuleSetting(settingName, on);
}

//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTModuleSettingsWidget::moduleSettingDoubleValueChanged(double value)
{
  Q_D (qMRMLLongitudinalPETCTModuleSettingsWidget);

  QObject* sender = QObject::sender();

  QString settingName;

  if(sender == d->SliderPETVROpacityPow)
    settingName = "opacityPow";

  emit changeModuleSettingValue(settingName, value);
}

//-----------------------------------------------------------------------------
void qMRMLLongitudinalPETCTModuleSettingsWidget::moduleSettingValueChanged(int value)
{
  Q_D (qMRMLLongitudinalPETCTModuleSettingsWidget);

  QObject* sender = QObject::sender();

  QString settingName;

  if(sender == d->ButtonGroupPETColors)
    settingName = "petColor";

  else if(sender == d->ButtonGroupCTWIndowLevelPresets)
    settingName = "ctWindowLevel";

  emit changeModuleSettingValue(settingName, value);
}

//-----------------------------------------------------------------------------
double qMRMLLongitudinalPETCTModuleSettingsWidget::ctWindow()
{
  Q_D (qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->ButtonGroupCTWIndowLevelPresets);

  QAbstractButton* checkedButton = d->ButtonGroupCTWIndowLevelPresets->checkedButton();

  QString accessibleName = checkedButton->accessibleName();

  double window = 0;

  if(accessibleName.compare("Abdomen") == 0)
    {
      window = 350;
    }
  else if(accessibleName.compare("Brain") == 0)
    {
      window = 100;
    }
  else if(accessibleName.compare("Bone") == 0)
    {
      window = 1000;
    }
  else if(accessibleName.compare("Lung") == 0)
    {
      window = 1400;
    }

  return window;
}

//-----------------------------------------------------------------------------
double qMRMLLongitudinalPETCTModuleSettingsWidget::ctLevel()
{
  Q_D (qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->ButtonGroupCTWIndowLevelPresets);

  QAbstractButton* checkedButton = d->ButtonGroupCTWIndowLevelPresets->checkedButton();

  QString accessibleName = checkedButton->accessibleName();

  double level = 0;

  if(accessibleName.compare("Abdomen") == 0)
    {
      level = 40;
    }
  else if(accessibleName.compare("Brain") == 0)
    {
      level = 50;
    }
  else if(accessibleName.compare("Bone") == 0)
    {
      level = 400;
    }
  else if(accessibleName.compare("Lung") == 0)
    {
      level = -500;
    }

  return level;
}

//-----------------------------------------------------------------------------
const QString qMRMLLongitudinalPETCTModuleSettingsWidget::petColorNodeID()
{
  Q_D (qMRMLLongitudinalPETCTModuleSettingsWidget);
  Q_ASSERT(d->ButtonGroupPETColors);

  QAbstractButton* checkedButton = d->ButtonGroupPETColors->checkedButton();

  QString accessibleName = checkedButton->accessibleName();

  return accessibleName;
}

void qMRMLLongitudinalPETCTModuleSettingsWidget::showQuantificationCLIInputDialog()
{
	QSettings settings;
	QString name = QInputDialog::getText(NULL, "CLI Module for Quantification", "Quantification Module:",QLineEdit::Normal, settings.value("LongitudinalPETCT/quantificationCLIModule").toString());

	if( ! name.isEmpty())
		emit quantificationModuleChanged(name);
}

