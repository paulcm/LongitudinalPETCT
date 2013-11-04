from __main__ import vtk, qt, ctk, slicer

from SlicerLongitudinalPETCTModuleViewHelper import SlicerLongitudinalPETCTModuleViewHelper as ViewHelper
from SlicerLongitudinalPETCTModuleSegmentationHelper import SlicerLongitudinalPETCTModuleSegmentationHelper as SegmentationHelper

import sys as SYS

#
# qSlicerLongitudinalPETCTModuleWidget
#

class qSlicerLongitudinalPETCTModuleWidget:
  def __init__(self, parent = None):
    if not parent:
      self.parent = slicer.qMRMLWidget()
      self.parent.setLayout(qt.QVBoxLayout())
      self.parent.setMRMLScene(slicer.mrmlScene)
    else:
      self.parent = parent
      self.layout = parent.layout()
    if not parent:
      self.setup()
      self.parent.show()
      
  def getActiveStudy(self):
    if self.activeReportNode:
      return self.activeReportNode.GetActiveStudyNode()

  def getActiveSegmentationROI(self):
    study = self.getActiveStudy()
    if study:
      vrdn = study.GetVolumeRenderingDisplayNode()
      if vrdn:
        return vrdn.GetROINode();            

  def getActiveFinding(self):
    if self.activeReportNode:
      return self.activeReportNode.GetActiveFindingNode()  
  
  def __createReportWidget(self):
    if not self.reportWidget:
      self.reportWidget = slicer.modulewidget.qMRMLLongitudinalPETCTReportWidget()    
      self.reportWidget.setObjectName("ReportWidget")
      self.reportWidget.setMRMLScene(slicer.mrmlScene)
      self.reportWidget.connect('reportNodeChanged(vtkMRMLLongitudinalPETCTReportNode*)',self.onActiveReportNodeChanged)
      
    return self.reportWidget  

  def __createReportOverviewWidget(self):
    
    if not self.reportOverviewWidget:
      
      self.reportOverviewWidget = slicer.modulewidget.qMRMLLongitudinalPETCTReportOverviewWidget()
      self.reportOverviewWidget.setObjectName("ReportOverviewWidget")
      self.reportOverviewWidget.setColumnSelectableOn()
      self.reportOverviewWidget.setReportNode(self.activeReportNode)
      self.reportOverviewWidget.connect('studySelected(vtkMRMLLongitudinalPETCTStudyNode*)',self.onActiveStudyNodeChanged)                
      self.reportOverviewWidget.connect('findingSelected(vtkMRMLLongitudinalPETCTFindingNode*)',self.onActiveFindingNodeChanged)                
          
    return self.reportOverviewWidget 

  def __createStudyWidget(self):
    
    if not self.studyWidget:
      
      self.studyWidget = slicer.modulewidget.qMRMLLongitudinalPETCTStudyWidget()    
      self.studyWidget.setObjectName("StudyWidget")
      self.studyWidget.setReportNode(self.activeReportNode)      
      self.studyWidget.connect('studySelectedForSegmentation(vtkMRMLLongitudinalPETCTStudyNode*, bool)',self.onStudySelectedForSegmentation)  

    return self.studyWidget    


  def __createFindingWidget(self):
    
    if not self.findingWidget:
      self.findingWidget = slicer.modulewidget.qMRMLLongitudinalPETCTFindingWidget()    
      self.findingWidget.setObjectName("FindingWidget")
      self.findingWidget.setMRMLScene(slicer.mrmlScene)
      self.findingWidget.setReportNode(self.activeReportNode)
      self.findingWidget.connect('findingNodeAddedByUser(vtkMRMLLongitudinalPETCTFindingNode*)', self.onFindingNodeAdded)
      self.findingWidget.connect('findingNodeSelectionChanged(vtkMRMLLongitudinalPETCTFindingNode*)', self.onFindingNodeSelected)
      self.findingWidget.connect('findingNodeAboutToBeRemoved(vtkMRMLLongitudinalPETCTFindingNode*)', self.onFindingNodeToBeRemoved)
      self.findingWidget.connect('roiVisibilityChanged(bool)', self.onROIVisibility)
      self.findingWidget.connect('placeROIChecked(bool)', self.onPlaceROI)
      self.findingWidget.connect('addSegmentationToFinding()', self.onAddSegmentationToFinding)
    
    #self.findingWidget.setEditorWidget(self.getEditorWidget().parent)
    
    return self.findingWidget
  
  def __createEditorWidget(self):
    
    if not self.editorWidget:      
      editorWidgetParent = slicer.qMRMLWidget()
      editorWidgetParent.setLayout(qt.QVBoxLayout())
      editorWidgetParent.setMRMLScene(slicer.mrmlScene)
      editorWidgetParent.hide()
      self.editorWidget = EditorWidget(editorWidgetParent, False)
      self.editorWidget.setup()
  
      self.editorWidget.editLabelMapsFrame.setText("Edit Segmentation")
      self.editorWidget.editLabelMapsFrame.connect('contentsCollapsed(bool)', self.onEditorCollapsed)      
      
      editorWidgetParent.show()
      
    return self.editorWidget
  
  def __createAnalysisWidget(self):
    
    if not self.analysisWidget:
      
      self.analysisWidget = slicer.modulewidget.qMRMLLongitudinalPETCTAnalysisWidget()    
      self.analysisWidget.setObjectName("AnalysisWidget")
      self.analysisWidget.setReportNode(self.activeReportNode)
      self.analysisWidget.connect('qualitativeAnalysisClicked(bool)', self.onShowQualitativeView)
      self.analysisWidget.connect('quantitativeAnalysisClicked(bool)', self.onShowQuantitativeView)
      self.analysisWidget.connect('studySelectedForAnalysis(vtkMRMLLongitudinalPETCTStudyNode*, bool)', self.onStudySelectedForAnalysis)
      self.analysisWidget.connect('exportCSV()',self.onSaveResultsCSV)
           
    return self.analysisWidget    
  
  
  def __createModuleSettingsWidget(self):
    if not self.moduleSettingsWidget:
      self.moduleSettingsWidget = slicer.modulewidget.qMRMLLongitudinalPETCTModuleSettingsWidget()    
      self.findingWidget.setObjectName("ModuleSettingsWidget")
      
      petColor = ViewHelper.getSetting("petColor")
      if petColor:
        self.moduleSettingsWidget.setProperty("petColorButton",petColor)
      else:
        self.moduleSettingsWidget.setProperty("petColorButton",0)          
      
      ctWindowLevel = ViewHelper.getSetting("ctWindowLevel")
      if ctWindowLevel:
        self.moduleSettingsWidget.setProperty("ctWindowLevelButton",ctWindowLevel)    
      else:
        self.moduleSettingsWidget.setProperty("ctWindowLevelButton",0)    
      
      volumeRendering = ViewHelper.getSetting("volumeRendering")
      if volumeRendering != None:
        self.moduleSettingsWidget.setProperty("volumeRendering",volumeRendering)    
      else:
        self.moduleSettingsWidget.setProperty("volumeRendering",True)    
      
      spinning = ViewHelper.getSetting("spinning")
      if spinning != None:
        self.moduleSettingsWidget.setProperty("spinning",spinning)    
      else:
        self.moduleSettingsWidget.setProperty("spinning",True) 
         
      opacityPow = ViewHelper.getSetting("opacityPow")
      if opacityPow != None:
        self.moduleSettingsWidget.setProperty("petOpacityPow",opacityPow)    
      else:
        self.moduleSettingsWidget.setProperty("petOpacityPow",2.0)  
      
      self.moduleSettingsWidget.connect("outlineSegmentationsToggled(bool)",self.onOutlineSegmentations)
      
      outlineSeg = ViewHelper.getSetting("outlineSegmentations")
      if outlineSeg != None:
        self.moduleSettingsWidget.setProperty("outlineSegmentations",outlineSeg)    
      else:
        self.moduleSettingsWidget.setProperty("outlineSegmentations",True)   
      
      makeModels = ViewHelper.getSetting("makeModels")
      if makeModels != None:
        self.moduleSettingsWidget.setProperty("makeModels",makeModels)    
      else:
        self.moduleSettingsWidget.setProperty("makeModels",True)  
        
      registration = ViewHelper.getSetting("registration")
      if registration != None:
        self.moduleSettingsWidget.setProperty("registration",registration)
      else:
        self.moduleSettingsWidget.setProperty("registration",True)  
        
      quantificationCLI = ViewHelper.getSetting("quantificationCLIModule")
      if quantificationCLI == None:
        ViewHelper.setSetting("quantificationCLIModule", "petsuvimagemaker")            
              
      self.moduleSettingsWidget.connect('petColorChanged(QString)', self.onPETColorChanged)
      self.moduleSettingsWidget.connect('ctWindowLevelChanged(double,double)', self.onCTWindowLevelChanged)
      self.moduleSettingsWidget.connect('spinningToggled(bool)',self.onVRSpinning)
      self.moduleSettingsWidget.connect('volumeRenderingToggled(bool)',self.onVRVisibility)
      
      self.moduleSettingsWidget.connect('opacityPowChanged(double)',self.onVROpacity)
    
      self.moduleSettingsWidget.connect("changeModuleSetting(QString,bool)",self.onChangeModuleSettingBool)
      self.moduleSettingsWidget.connect("changeModuleSettingValue(QString,double)",self.onChangeModuleSettingValue)
      
      self.moduleSettingsWidget.connect("quantificationModuleChanged(QString)", self.onQuantificationModuleChanged)
      
    return self.moduleSettingsWidget    

  def __createTempCroppedVolume(self):
    if self.__tempCroppedVol == None:
      self.__tempCroppedVol = slicer.mrmlScene.AddNode(slicer.vtkMRMLScalarVolumeNode())
      self.__tempCroppedVol.SetName("LongitudinalPETCT_CroppedVolume")
      self.__tempCroppedVol.SetHideFromEditors(False)
      self.__tempCroppedVol.SetSaveWithScene(False)

    return self.__tempCroppedVol     
   
  def __createTempLabelVolume(self):
    if self.__tempLabelVol == None:
      self.__tempLabelVol = slicer.mrmlScene.AddNode(slicer.vtkMRMLScalarVolumeNode())
      self.__tempLabelVol.SetName("LongitudinalPETCT_CroppedLabelVolume")
      self.__tempLabelVol.SetHideFromEditors(False) 
      self.__tempLabelVol.SetSaveWithScene(False)
      
    return self.__tempLabelVol 


  def __isEditorMode(self):
    return not self.editorWidget.editLabelMapsFrame.property('collapsed')

  def __isStandardViewActive(self):
    return (self.__isQualitativeViewActive() == False) & (self.__isQuantitativeViewActive() == False)                
  
  def __isQualitativeViewActive(self):
    return (self.analysisCollapsibleButton.property('collapsed') == False) & self.analysisWidget.property('qualitativeChecked')
         
  def __isQuantitativeViewActive(self):
    return (self.analysisCollapsibleButton.property('collapsed') == False) & self.analysisWidget.property('quantitativeChecked')
  
  def __isAnalysisViewActive(self):
      return (self.__isQualitativeViewActive() == True) | (self.__isQualitativeViewActive() == True)
 
  def __enterAnalysisMode(self, enter):
    lm = slicer.app.layoutManager()
    if lm:
      if enter:        
        self.__layoutBackup = lm.layout    
        if self.analysisWidget.property("qualitativeChecked"):
          self.onShowQualitativeView()
        elif self.analysisWidget.property("quantitativeChecked"):
          self.onShowQuantitativeView()      
      else:
        lm.setLayout(self.__layoutBackup)
        self.manageROIsVisualization()
        self.manageModelsVisibility()
        self.manageVRVisualization()
        
        self.__layoutBackup = None      
        
    
  def getUnsupportedVolumeRenderingMessageBox(self):
    if self.__unsupportedVolRendMessageBox == None:
      self.__unsupportedVolRendMessageBox = ctk.ctkMessageBox()
      self.__unsupportedVolRendMessageBox.setModal(True)
      self.__unsupportedVolRendMessageBox.setIcon(qt.QMessageBox.Information)
      self.__unsupportedVolRendMessageBox.setWindowTitle(ViewHelper.moduleDialogTitle())
      self.__unsupportedVolRendMessageBox.setText('NCIRayCastVolumeRendering is selected as default volume renderer. The Longitudinal PET/CT Analysis module does not support displaying of models from Finding segmentations with this volume renderer!')
      self.__unsupportedVolRendMessageBox.setProperty('dontShowAgainVisible', True)
      self.__unsupportedVolRendMessageBox.setDontShowAgain(False)
      
    return self.__unsupportedVolRendMessageBox 
 
  def setup(self): 
      
    self.manageLayout()
    
    self.activeReportNode = None  
    self.__segROIsObserverTags = []
    
    # Instantiate and connect widgets ...

    if ViewHelper.getSetting('OutlineSegmentations'):
      slicer.mrmlScene.GetNodeByID("vtkMRMLSliceNodeRed").SetUseLabelOutline(True)
      slicer.mrmlScene.GetNodeByID("vtkMRMLSliceNodeYellow").SetUseLabelOutline(True)
      slicer.mrmlScene.GetNodeByID("vtkMRMLSliceNodeGreen").SetUseLabelOutline(True)
    
    hbox = qt.QHBoxLayout()
    hbox.setSpacing(0)
    hbox.setContentsMargins(0,0,0,0)
    
    vbox = qt.QVBoxLayout()
    vbox.setSpacing(self.layout.property("spacing"))
    vbox.setContentsMargins(0,0,0,0)
    
    self.collapsibleButtonsGroup = qt.QButtonGroup()
    
    self.reportWidget = None
    self.__createReportWidget()
    self.reportsCollapsibleButton = ctk.ctkCollapsibleButton()
    self.reportsCollapsibleButton.setProperty('collapsed',False)
    self.reportsCollapsibleButton.text = "1. Report Selection"
    self.collapsibleButtonsGroup.addButton(self.reportsCollapsibleButton)
    reportsLayout = qt.QVBoxLayout(self.reportsCollapsibleButton)
    reportsLayout.addWidget(self.reportWidget)
    vbox.addWidget(self.reportsCollapsibleButton)
    
    self.studyWidget = None
    self.__createStudyWidget()
    self.studiesCollapsibleButton = ctk.ctkCollapsibleButton()
    self.studiesCollapsibleButton.text = "2. Study Selection"
    self.studiesCollapsibleButton.setProperty('collapsed',True)
    self.collapsibleButtonsGroup.addButton(self.studiesCollapsibleButton)
    studiesLayout = qt.QVBoxLayout(self.studiesCollapsibleButton)   
    studiesLayout.addWidget(self.studyWidget)
    vbox.addWidget(self.studiesCollapsibleButton)
    
    self.findingWidget = None
    self.__createFindingWidget()
    self.findingsCollapsibleButton = ctk.ctkCollapsibleButton()
    self.findingsCollapsibleButton.text = "3. Findings"
    self.findingsCollapsibleButton.setProperty('collapsed',True)
    self.collapsibleButtonsGroup.addButton(self.findingsCollapsibleButton)
    findingsLayout = qt.QVBoxLayout(self.findingsCollapsibleButton)
    findingsLayout.addWidget(self.findingWidget)
    vbox.addWidget(self.findingsCollapsibleButton)
    
    self.editorWidget = None
    self.__createEditorWidget()
    self.findingWidget.setEditorWidget(self.editorWidget.parent)
    
    self.analysisWidget = None
    self.__createAnalysisWidget()
    self.analysisCollapsibleButton = ctk.ctkCollapsibleButton()
    self.analysisCollapsibleButton.text = "4. Analysis"
    self.analysisCollapsibleButton.setProperty('collapsed',True)
    self.collapsibleButtonsGroup.addButton(self.analysisCollapsibleButton)
    analysisLayout = qt.QVBoxLayout(self.analysisCollapsibleButton)
    analysisLayout.addWidget(self.analysisWidget)
    vbox.addWidget(self.analysisCollapsibleButton)
    
    self.analysisCollapsibleButton.connect("toggled(bool)",self.__enterAnalysisMode) 
    
    # Add vertical spacer
    vbox.addStretch()
    
    
    self.reportOverviewWidget = None
    self.__createReportOverviewWidget()
    
    vbox.addWidget(self.reportOverviewWidget) 
    
    hbox.addLayout(vbox)
    
    self.moduleSettingsWidget = None
    self.__createModuleSettingsWidget()
    
    hbox.addWidget(self.moduleSettingsWidget)
    
    self.layout.addLayout(hbox)
    
    self.onActiveReportNodeChanged(self.reportWidget.currentSelectedReportNode())
    
    ViewHelper.SetForegroundOpacity(0.6) 
    
    self.__interactorsAndSliceWidgets = {}
    self.__interactorsObserverTags = []
    self.__roiPlacingPositionAndRadius = []
    self.__savedCursor = None
    self.__sliceWidgetsMappersAndActors = {}
    
    self.__tempCroppedVol = None
    self.__tempLabelVol = None
    
    self.__unsupportedVolRendMessageBox = None 
    
    self.__cropLogic = slicer.modules.cropvolume.logic()
    
    self.__qualitativeViewLastID = None
    
    self.__chartNode = None
    self.__chartArrayNodes = []
    self.__chartArrayNodeNames = []
    self.__saturationMultipliers = [1.0,0.85,0.75]
    
    self.__fileDialog = None
    self.__layoutBackup = None
    self.__switchingReports = False #workaround flag for SetHideFromEditor() issue with FindingNodes
  
  def updateComboBox(self, term = ""):
  
    cb.clear()
  
    d = [0.,0.,0.,0.]
  
    for i in xrange(cn.GetNumberOfColors()):
    
      isColor = cn.GetColor(i,d)  
    
      if (isColor == False) | ((d[0] == 0.) & (d[1] == 0.) & (d[2] == 0.)) | ( (len(term) > 0) & (term in cn.GetColorName(i) == False)):
        continue
    
      pxmp = qt.QPixmap(16,16)
      pxmp.fill(qt.QColor(d[0],d[1],d[2],d[3]))
      icn = qt.QIcon(pxmp)
    
      cb.addItem(icn, cn.GetColorName(i))
  
  
  def onActiveReportNodeChanged(self, reportNode):
    
    self.__switchingReports = True
    
    self.__hideVolumeRenderingDisplayNodes()  
    self.__hideSegmentationROINodes()
    
    if self.activeReportNode:
      self.__switchReportFindingNodes(self.activeReportNode, reportNode)    
    #if(self.activeReportNode):
      #self.__removeInactiveReportFindingNodesFromScene(self.activeReportNode)
      
    self.activeReportNode = reportNode 
    
    self.reportWidget.setReportNode(reportNode)
    self.studyWidget.setReportNode(reportNode)
    self.findingWidget.setReportNode(reportNode)
    self.analysisWidget.setReportNode(reportNode)
    self.reportOverviewWidget.setReportNode(reportNode)
    
    #self.__addActiveReportFindingNodesToScene()
    
    actStudy = self.getActiveStudy()
    if actStudy:
      self.reportOverviewWidget.selectStudyColumn(self.activeReportNode.GetIndexOfSelectedStudy(actStudy))
      vrdn = actStudy.GetVolumeRenderingDisplayNode()
      if vrdn:
        try:
          pow = float(vrdn.GetAttribute("OpacityPow"))
          self.moduleSettingsWidget.setProperty("petOpacityPow", pow)
        except:
          pass    
                         
    self.manageImageVolumesVisualization()
    self.manageVRVisualization()
    self.manageROIsVisualization()
    self.manageModelsVisibility() 
    
    self.manageCollapsibleButtonsAvailability()
    
  
  def __createVolumeRenderingDisplayNode(self, petVolume, opacityPow):
    
    vrDisplayNode = None
    vrLogic = slicer.modules.volumerendering.logic()
 
    vrDisplayNode = vrLogic.CreateVolumeRenderingDisplayNode()
    slicer.mrmlScene.AddNode(vrDisplayNode)
    vrDisplayNode.UnRegister(vrLogic)
    vrDisplayNode.SetCroppingEnabled(0)
    vrDisplayNode.SetAttribute("OpacityPow",str(opacityPow))
    vrDisplayNode.SetName(petVolume.GetName() +"_VR_Display")
    vrLogic.UpdateDisplayNodeFromVolumeNode(vrDisplayNode, petVolume)
    petVolume.AddAndObserveDisplayNodeID(vrDisplayNode.GetID())

    vrDisplayNode.AddViewNodeID(ViewHelper.getStandardViewNode().GetID())
      
    propNode = vrDisplayNode.GetVolumePropertyNode()
    if propNode:
      propNode.SetName(petVolume.GetName() + "_VR_VolumeProperty")
      
      self.__updateColorFunction(vrDisplayNode)
      
    roiNode = vrDisplayNode.GetROINode()
    if roiNode:
      roiNode.SetName(petVolume.GetName() + "_VR_ROI")
      roiNode.SetDisplayVisibility(False)
      
    self.__updateOpacityPow(vrDisplayNode)  

    return vrDisplayNode
                                
  
  def onActiveStudyNodeChanged(self, studyNode):
    
    if studyNode:
      self.activeReportNode.SetActiveStudyNodeID(studyNode.GetID())
      self.__updateSegmentationROIAssginment()
    else:
      self.activeReportNode.SetActiveStudyNodeID(None)
       
    self.manageImageVolumesVisualization(False)
    self.manageVRVisualization()
    self.manageModelsVisibility()
    self.manageCollapsibleButtonsAvailability()

  def manageCollapsibleButtonsAvailability(self):
    reports = False
    studies = False
    findings = False
    analysis = False
    
    
    notInEditorMode = self.editorWidget.editLabelMapsFrame.property('collapsed')
    
    if notInEditorMode:
      reports = True      
    
    if (self.activeReportNode != None) & notInEditorMode:
      studies = True
    
      if self.activeReportNode.GetNumberOfSelectedStudies() > 0:
        findings = True
        
        if self.activeReportNode.GetNumberOfFindingNodeIDs() > 0:
          for i in range(self.activeReportNode.GetNumberOfFindingNodeIDs()):
            finding = self.activeReportNode.GetFinding(i)
            if finding:
              if (finding.GetNumberOfSegmentations() > 0) & notInEditorMode:
                analysis = True      
                   
    self.reportsCollapsibleButton.setProperty('enabled',reports)                  
    self.studiesCollapsibleButton.setProperty('enabled',studies)
    self.findingsCollapsibleButton.setProperty('enabled',findings | (notInEditorMode == False))  
    self.analysisCollapsibleButton.setProperty('enabled',analysis) 
    

  def manageLayout(self):
    lm = slicer.app.layoutManager()
    if lm:
      id = lm.property("layout")
      if (id != slicer.vtkMRMLLayoutNode.SlicerLayoutConventionalView) & (id != slicer.vtkMRMLLayoutNode.SlicerLayoutFourUpView)  &  (id != slicer.vtkMRMLLayoutNode.SlicerLayoutTabbedSliceView) & (id != slicer.vtkMRMLLayoutNode.SlicerLayoutConventionalWidescreenView) & (id != slicer.vtkMRMLLayoutNode.SlicerLayoutOneUp3DView) & (id != slicer.vtkMRMLLayoutNode.SlicerLayoutOneUpRedSliceView) & (id != slicer.vtkMRMLLayoutNode.SlicerLayoutOneUpYellowSliceView) & (id != slicer.vtkMRMLLayoutNode.SlicerLayoutOneUpGreenSliceView):   
        lm.setLayout(slicer.vtkMRMLLayoutNode.SlicerLayoutFourUpView)             
       
  def manageVRVisualization(self):

    self.__hideVolumeRenderingDisplayNodes()


    if (self.activeReportNode != None) & (self.moduleSettingsWidget.property('volumeRendering') == True):
      
      if self.__isStandardViewActive():
          
        study = self.getActiveStudy()
        if study:
          vrdn = study.GetVolumeRenderingDisplayNode()
          petDisplayNode = study.GetPETVolumeNode().GetScalarVolumeDisplayNode()
          if vrdn:
            vrdn.AddViewNodeID(ViewHelper.getStandardViewNode().GetID())
            vrdn.SetVisibility(True)
            self.__updateOpacityPow(vrdn)   
            self.__updateColorFunction(vrdn)
        
    
      else:
        # Analysis View

        compareViewNodes = ViewHelper.getCompareViewNodes()
        for i in range(self.activeReportNode.GetNumberOfSelectedStudies()):
          study = self.activeReportNode.GetSelectedStudy(i)  
       
          vrdn = study.GetVolumeRenderingDisplayNode()
          if vrdn:
              id = self.activeReportNode.GetIndexOfSelectedStudySelectedForAnalysis(study)
              if (id >= 0) & (id < len(compareViewNodes)):
                vrdn.AddViewNodeID(compareViewNodes[id].GetID())  
                vrdn.SetVisibility(True)
                
    self.onVRSpinning(self.moduleSettingsWidget.property("spinning"))
                          
    
  def manageImageVolumesVisualization(self, fit = True):
    
    ctID = None
    petID = None
    lblID = None
      
    study = self.getActiveStudy()
      
    if study:
      if study.GetCTVolumeNodeID():   
        ctID = study.GetCTVolumeNodeID()
      petID = study.GetPETVolumeNodeID()
      lblID = study.GetPETLabelVolumeNodeID()
    
    if ctID:
      ViewHelper.SetRYGBgFgLblVolumes(ctID,petID,lblID,fit)
    else:
      ViewHelper.SetRYGBgFgLblVolumes(petID,None,lblID,fit)
                  
  def manageROIsVisualization(self, showCurrent = True):
    
    self.__hideSegmentationROINodes()
    
    if showCurrent & (self.getActiveStudy() != None): # if the Study exists, a Report must exists too
      finding = self.activeReportNode.GetActiveFindingNode()
    
      if finding:
        roi = self.getActiveSegmentationROI()
        if roi:
          visibility = self.findingWidget.property("roiVisibility") & finding.GetSegmentationROISpecified()
          roi.SetDisplayVisibility(visibility)                                    
  
  def manageModelsVisibility(self, findingRestricted = False):     

    if not self.activeReportNode:
      return    

    for i in range(self.activeReportNode.GetNumberOfFindingNodeIDs()):
        finding = self.activeReportNode.GetFinding(i)
        for j in range(self.activeReportNode.GetNumberOfSelectedStudies()):
          study = self.activeReportNode.GetSelectedStudy(j)
          
          seg = finding.GetSegmentationMappedByStudyNodeID(study.GetID())
          if seg:
            mh = seg.GetModelHierarchyNode()
            if mh:
              mn = mh.GetModelNode()
              if mn:
                #visibility = seg.GetModelVisible() & (study == self.getActiveStudy())  
                stdViewShow = self.__isStandardViewActive() & (study == self.getActiveStudy())
                qualViewShow = self.__isQualitativeViewActive()
                quantViewShow = self.__isQuantitativeViewActive() & ( (findingRestricted == False) | ( (findingRestricted == True) &  (finding == self.getActiveFinding()))) 
                
                if (seg.GetModelVisible() == True) & (stdViewShow | qualViewShow | quantViewShow):
                  mn.SetDisplayVisibility(True)
                else:
                  mn.SetDisplayVisibility(False)
                  
                  
    if self.__isQualitativeViewActive() | self.__isQuantitativeViewActive():                  
      self.__manageModelVisibilityCompareViewNodeIDs()

      
  def __manageModelVisibilityCompareViewNodeIDs(self):
    
    compareViewNodes = ViewHelper.getCompareViewNodes()
    
    if self.activeReportNode:
      for i in range(self.activeReportNode.GetNumberOfFindingNodeIDs()):
        finding = self.activeReportNode.GetFinding(i)
        for j in range(self.activeReportNode.GetNumberOfSelectedStudies()):
          study = self.activeReportNode.GetSelectedStudy(j)
          
          seg = finding.GetSegmentationMappedByStudyNodeID(study.GetID())
          if seg:
            mh = seg.GetModelHierarchyNode()
            if mh:
              mn = mh.GetModelNode()
              if mn:
                mdn = mn.GetModelDisplayNode()
                if mdn:
                  for cvn in compareViewNodes:
                    if mdn.IsViewNodeIDPresent(cvn.GetID()):
                      mdn.RemoveViewNodeID(cvn.GetID())
                  
                  id = self.activeReportNode.GetIndexOfSelectedStudySelectedForAnalysis(study)
                  if (id >= 0) & (id < len(compareViewNodes)):
                    mdn.AddViewNodeID(compareViewNodes[id].GetID())         
         

  
  def __registerStudy(self, fixedStudy, movingStudy):
    
    if not movingStudy.GetRegistrationTransformNode():
      transform = slicer.mrmlScene.AddNode(slicer.vtkMRMLLinearTransformNode())
      movingStudy.SetAndObserveRegistrationTransformNodeID(transform.GetID())
        
    if fixedStudy == movingStudy: 
      movingStudy.GetRegistrationTransformNode().GetMatrixTransformToParent().Identity()
    
    else:      
      parameters = {}
      #baseline
      parameters["fixedVolume"] = fixedStudy.GetPETVolumeNodeID()
      # current for registration
      parameters["movingVolume"] = movingStudy.GetPETVolumeNodeID()
      # transformation
      parameters["linearTransform"] = movingStudy.GetRegistrationTransformNodeID()
      # default settings
      parameters["useRigid"] = True
      parameters["numberOfIterations"] = 1000
      parameters["initializeTransformMode"] = "useMomentsAlign"
        
            
      dialog = ViewHelper.createBusyProgressBarDialog("Registering "+movingStudy.GetPETVolumeNode().GetName()+" with " + fixedStudy.GetPETVolumeNode().GetName() )
      dialog.show()
      self.cliRegistration = None
      self.cliRegistration = slicer.cli.run(slicer.modules.brainsfit, self.cliRegistration, parameters, wait_for_completion = True)
      dialog.done(0)
  
    movingStudy.GetRegistrationTransformNode().SetName(movingStudy.GetPETVolumeNode().GetName()+"_RegistrationTransform")
                     
    
  def onStudySelectedForSegmentation(self, studyNode, selected):    
    
    # index of the selected/deselected study
    selidx = self.activeReportNode.GetIndexOfSelectedStudy(studyNode) 
    # index of the current study
    curridx = self.activeReportNode.GetIndexOfSelectedStudy(self.getActiveStudy())  
    
    studyNode.SetSelectedForSegmentation(selected)
     
    # determine the index of the new selected table column header in the overview table   
    newidx = -1
    if selected:
      
      self.__initializeStudyVisualization(studyNode)
      
      if self.moduleSettingsWidget.property("registration") == True:
        if not self.activeReportNode.GetRegistrationFixedStudyNode():  
          self.activeReportNode.SetRegistrationFixedStudyNodeID(studyNode.GetID())
        
        self.__registerStudy(self.activeReportNode.GetRegistrationFixedStudyNode(), studyNode)                          
                    
      # if study was selected the new index is the column of this study
      newidx = self.activeReportNode.GetIndexOfSelectedStudy(studyNode)
    
              
    else:
      trans =  studyNode.GetRegistrationTransformNode() 
      if trans:
        trans.GetMatrixTransformToParent().Identity()   
            
      if self.activeReportNode.GetNumberOfSelectedStudies() == 0:
        self.activeReportNode.SetRegistrationFixedStudyNodeID(None)      
        
      # don't change anything if deselected study had higher index than current  
      if curridx < selidx:
        newidx = curridx
      # decrease index if deselected study had lower index than current  
      elif curridx > selidx:
        newidx = curridx-1          
      else:
        # don't change index if deselected study was current study but studies with higher indexes are available              
        if selidx < self.activeReportNode.GetNumberOfSelectedStudies():
          newidx = selidx
        # decrease index if deselected study was current study and had highest index                
        else:
          newidx = selidx-1 
        
    self.reportOverviewWidget.selectStudyColumn(newidx)

  
  def __initializeStudyVisualization(self, studyNode):
    
    if studyNode:
      petNode = studyNode.GetPETVolumeNode()
      ctNode = studyNode.GetCTVolumeNode()
      vrdNode = studyNode.GetVolumeRenderingDisplayNode()
      
      if petNode:
        petDisplayNode = petNode.GetScalarVolumeDisplayNode()
        if petDisplayNode:
          petDisplayNode.SetAndObserveColorNodeID(self.moduleSettingsWidget.petColorNodeID()) 
          
      if ctNode:
        ctDisplayNode = ctNode.GetScalarVolumeDisplayNode()
        if ctDisplayNode:
          ctDisplayNode.SetAutoWindowLevel(0)
          ctDisplayNode.SetWindowLevel(self.moduleSettingsWidget.ctWindow(),self.moduleSettingsWidget.ctLevel())
              
      if not vrdNode: 
        vrdNode = self.__createVolumeRenderingDisplayNode(studyNode.GetPETVolumeNode(), self.moduleSettingsWidget.property('opacityPow'))
        studyNode.SetAndObserveVolumeRenderingDisplayNodeID(vrdNode.GetID()) 
        ViewHelper.resetThreeDViewsFocalPoints(True)                     
              
  
  def onVRVisibility(self, visible):
    
    if not visible:
      self.__hideVolumeRenderingDisplayNodes()  
      
    else:
      if self.__isStandardViewActive():  
        study = self.getActiveStudy()
        if study:
          vrdn = study.GetVolumeRenderingDisplayNode()
          if vrdn:
            vrdn.SetVisibility(True)
            
      elif self.__isAnalysisViewActive() :
        for i in xrange(self.activeReportNode.GetNumberOfSelectedStudiesSelectedForAnalysis()):
          study = self.activeReportNode.GetSelectedStudySelectedForAnalysis(i)
          if study:
            vrdn = study.GetVolumeRenderingDisplayNode()
            if vrdn:
              vrdn.SetVisibility(True)             
  
  def onOutlineSegmentations(self, outline):
    slicer.mrmlScene.GetNodeByID("vtkMRMLSliceNodeRed").SetUseLabelOutline(outline)
    slicer.mrmlScene.GetNodeByID("vtkMRMLSliceNodeYellow").SetUseLabelOutline(outline)
    slicer.mrmlScene.GetNodeByID("vtkMRMLSliceNodeGreen").SetUseLabelOutline(outline)
    
    if(self.__isStandardViewActive() == False):
      for i in xrange(self.activeReportNode.GetNumberOfSelectedStudiesSelectedForAnalysis()):
        compSliceNodes = ViewHelper.getCompareSliceNodes(i)  
        for sn in compSliceNodes:
          sn.SetUseLabelOutline(outline)
  
  
  def __switchReportFindingNodes(self, oldReport, activeReport):
    
    self.findingWidget.mrmlNodeComboBoxFinding().setMRMLScene(None)
    
    if oldReport:
      
      for i in xrange(oldReport.GetNumberOfFindingNodeIDs()):
        finding = oldReport.GetFinding(i)
        if finding:
          for i in xrange(oldReport.GetNumberOfStudyNodeIDs()):
            study = oldReport.GetStudy(i)  
            seg = finding.GetSegmentationMappedByStudyNodeID(study.GetID())      
            if not seg: 
              continue  
            mhn = seg.GetModelHierarchyNode()
            if not mhn:
              continue
            mn = mhn.GetModelNode()
            if not mn:
              continue
            mn.SetDisplayVisibility(False) 
        
        finding.SetHideFromEditors(True)      

    if activeReport:
      for i in xrange(activeReport.GetNumberOfFindingNodeIDs()):
        finding = activeReport.GetFinding(i)         
        if finding:
          finding.SetHideFromEditors(False)    
          
    self.findingWidget.mrmlNodeComboBoxFinding().setMRMLScene(slicer.mrmlScene)            

  def __addActiveReportFindingNodesToScene(self):
    
    if self.activeReportNode:
      
      for i in xrange(self.activeReportNode.GetNumberOfFindingNodeIDs()):
        finding = self.activeReportNode.GetFinding(i)         
        slicer.mrmlScene.AddNode(finding)            
                                     
  def __hideVolumeRenderingDisplayNodes(self):
    
    if self.activeReportNode:  
      
      for i in xrange(self.activeReportNode.GetNumberOfStudyNodeIDs()):    
        study = self.activeReportNode.GetStudy(i)
        if study:
          vrdn = study.GetVolumeRenderingDisplayNode()
          if vrdn: 
            vrdn.SetVisibility(False)
            stdView = ViewHelper.getStandardViewNode()
            if vrdn.IsViewNodeIDPresent(stdView.GetID()):
              vrdn.RemoveViewNodeID(stdView.GetID())
            compareViewNodes = ViewHelper.getCompareViewNodes()  
            for cvn in compareViewNodes:
              if vrdn.IsViewNodeIDPresent(cvn.GetID()):
                vrdn.RemoveViewNodeID(cvn.GetID())  
      
  def __hideSegmentationROINodes(self):     
    
    if self.activeReportNode:
        
      for i in xrange(self.activeReportNode.GetNumberOfStudyNodeIDs()):
        study = self.activeReportNode.GetStudy(i)
        
        vrdn = study.GetVolumeRenderingDisplayNode()
        if vrdn:
          roi = vrdn.GetROINode()    
          if roi:
            roi.SetDisplayVisibility(False)                          
                    
  def onVRSpinning(self, spin):
    if self.getActiveStudy():
      ViewHelper.spinStandardViewNode(spin)
      ViewHelper.spinCompareViewNodes(spin)
        
  
  def onVROpacity(self, pow):
    
    if self.activeReportNode:
      
      currStudyVRDN = None   
      for i in xrange(self.activeReportNode.GetNumberOfStudyNodeIDs()):
        study = self.activeReportNode.GetStudy(i)
        if study:
          vrdn = study.GetVolumeRenderingDisplayNode()
          if vrdn:
            vrdn.SetAttribute("OpacityPow",str(pow))
            if study == self.getActiveStudy():
              currStudyVRDN = vrdn  
              
            if (self.__isAnalysisViewActive() == True) & (study.GetSelectedForAnalysis() == True):
              self.__updateOpacityPow(vrdn)
                
      if self.__isStandardViewActive():
        self.__updateOpacityPow(currStudyVRDN) 


  def __updateColorFunction(self, vrdn):
      
    propNode = vrdn.GetVolumePropertyNode()
    volume = vrdn.GetVolumeNode()
    volDisplayNode = volume.GetScalarVolumeDisplayNode()  
    
    ViewHelper.adjustColorFunction(propNode.GetColor(),volDisplayNode)  
  
  def __updateOpacityPow(self, vrdn):
    
    pow = None
    try:
      pow = float(vrdn.GetAttribute("OpacityPow"))
    except:
      pow = self.moduleSettingsWidget.property('petOpacityPow')
              
    volNode = vrdn.GetVolumeNode()
    propNode = vrdn.GetVolumePropertyNode()
    volNodeDisplNode = volNode.GetScalarVolumeDisplayNode()
    window = volNodeDisplNode.GetWindow()
    
    ViewHelper.adjustOpacityPowerFunction(propNode.GetScalarOpacity(),window,pow,10.)


  def onROIVisibility(self):
    self.manageROIsVisualization()

  def onFindingNodeAdded(self, finding):
      
    dialog = slicer.modulewidget.qMRMLLongitudinalPETCTFindingSettingsDialog()
    dialog.setReportNode(self.activeReportNode)
    
    dialog.exec_() 
    
    name = dialog.property("findingName")
    colorid = dialog.property("findingColorID")
    
    if (len(name) > 0) & (colorid != -1):
      finding.SetName(name)
      finding.SetColorID(colorid)
      
      self.activeReportNode.AddFindingNodeID(finding.GetID())
      self.reportOverviewWidget.selectFindingRow(self.activeReportNode.GetIndexOfFindingNodeID(finding.GetID()))
      
    else:
      slicer.mrmlScene.RemoveNode(finding)  
    
    self.manageCollapsibleButtonsAvailability()        


  def __updateSegmentationROIAssginment(self):
    study = self.getActiveStudy()
    finding = self.getActiveFinding()

    self.__removeObservers(self.__segROIsObserverTags)
    del self.__segROIsObserverTags[:]    
    
    if (finding == None) | (study == None):
      return
       
      
    seg = finding.GetSegmentationMappedByStudyNodeID(study.GetID())        
    roi = self.getActiveSegmentationROI()
      
    if finding.GetSegmentationROISpecified():
      c = [0.,0.,0.]
      r = [0.,0.,0.]
        
        
      if seg:
        seg.GetROIxyz(c)   
      else:
        finding.GetROIxyz(c)
        
      trans = study.GetRegistrationTransformNode()
      if trans:
        matrix = trans.GetMatrixTransformToParent()
        
        c = [c[0]-matrix.GetElement(0,3),c[1]-matrix.GetElement(1,3),c[2]-matrix.GetElement(2,3)]    
     
      if seg:
        seg.GetROIRadius(r)  
      else:
        finding.GetROIRadius(r)
        
      roi.SetXYZ(c)
      roi.SetRadiusXYZ(r)
 
    tag = roi.AddObserver(vtk.vtkCommand.ModifiedEvent, self.__observeSegmentationROI)
      
    self.__segROIsObserverTags.append([roi,tag])
    self.manageROIsVisualization() # show roi for currently assigned only if an assignment existed before 

  def __observeSegmentationROI(self,roi,event):
      
    finding = self.getActiveFinding()
    study = self.getActiveStudy()
    
    if (study != None) & (finding != None):
      c = [0.,0.,0.]
      r = [0.,0.,0.]
      
      roi.GetXYZ(c)
      trans = study.GetRegistrationTransformNode()
      if trans:
        matrix = trans.GetMatrixTransformToParent()
        
        c = [c[0]+matrix.GetElement(0,3),c[1]+matrix.GetElement(1,3),c[2]+matrix.GetElement(2,3)]    
      
      finding.SetROIxyz(c)
      
      roi.GetRadiusXYZ(r) 

      finding.SetROIRadius(r)
      
    if self.__isEditorMode():
      self.__prepareVolumesForSegmentation(roi, event)      
      
        
  def onFindingNodeSelected(self, finding):
    self.reportOverviewWidget.selectFindingRow(self.activeReportNode.GetIndexOfFindingNodeID(finding.GetID()))   
        
  def onActiveFindingNodeChanged(self, finding): 
    self.activeReportNode.SetActiveFindingNodeID(finding.GetID())
    
    self.__updateSegmentationROIAssginment()  
    
    self.manageModelsVisibility()
    self.manageCollapsibleButtonsAvailability()
    
        
  def onFindingNodeToBeRemoved(self, finding):
    
    if self.__switchingReports == True:
      return
    
        
    for i in range(self.activeReportNode.GetNumberOfSelectedStudies()):
      study = self.activeReportNode.GetSelectedStudy(i)
        
      seg = finding.GetSegmentationMappedByStudyNodeID(study.GetID())
      if seg:
        SegmentationHelper.removeSegmentationFromVolume(seg.GetLabelVolumeNode(), finding.GetColorID())
        finding.RemoveStudyNodeIDToSegmentationNodeIDMap(study.GetID())
          
        if seg.GetModelHierarchyNode():
          ViewHelper.removeModelHierarchyAndChildModelNodesFromScene(seg.GetModelHierarchyNode())
        slicer.mrmlScene.RemoveNode(seg)
          
    self.activeReportNode.RemoveFindingNodeID(finding.GetID())    
    
  def onPlaceROI(self, checked):
    
    self.__removeObservers(self.__interactorsObserverTags)
    del self.__interactorsObserverTags[:]  
    del self.__roiPlacingPositionAndRadius[:]
    
    if checked:
          
      sliceNodes = ViewHelper.GetRYGSliceNodes()
      layoutManager = slicer.app.layoutManager()
    
      if not self.__interactorsAndSliceWidgets:
        
        for sn in sliceNodes:  
          sliceWidget = layoutManager.sliceWidget(sn.GetLayoutName()) 
          
          if sliceWidget:       
            if not self.__savedCursor:
              self.__savedCursor = sliceWidget.cursor
            
            interactor = sliceWidget.sliceView().interactor()
            self.__interactorsAndSliceWidgets[interactor] = sliceWidget
            
      cursorPixmap = qt.QPixmap(":/Icons/ROICenter.png")
      cursor = qt.QCursor(cursorPixmap)           
    
      for interactor in self.__interactorsAndSliceWidgets.keys():
        tag = interactor.AddObserver("LeftButtonReleaseEvent", self.__processROIPlacingEvent, 2)
        self.__interactorsObserverTags.append([interactor,tag])
        
        sw = self.__interactorsAndSliceWidgets[interactor]
        if sw:
          sw.setCursor(cursor)  
 
    else:
      for interactor in self.__interactorsAndSliceWidgets.keys():
        sw = self.__interactorsAndSliceWidgets[interactor]
        if sw:  
          sw.unsetCursor() 
          self.__removeROICenterPosition(sw) 
                   
 
  def __removeObservers(self, observeeTagList):
    # remove observers and reset
    
    for observee,tag in observeeTagList:
      observee.RemoveObserver(tag)
 
           
  def __processROIPlacingEvent(self,interactor,event):
    
    sliceWidget = self.__interactorsAndSliceWidgets[interactor]
    
    if sliceWidget:
      xy = interactor.GetEventPosition()
      xyz = sliceWidget.sliceView().convertDeviceToXYZ(xy);
      ras = sliceWidget.sliceView().convertXYZToRAS(xyz)
      
      if len(self.__roiPlacingPositionAndRadius) < 2:
        self.__roiPlacingPositionAndRadius.append(ras)
        self.__displayROICenterPosition(sliceWidget,xy)
        
        cursorPixmap = qt.QPixmap(":/Icons/ROIRadius.png")
        cursor = qt.QCursor(cursorPixmap) 
        
        for interactor in self.__interactorsAndSliceWidgets.keys():
          sw = self.__interactorsAndSliceWidgets[interactor]
          if sw:  
            sw.setCursor(cursor)
      
      if len(self.__roiPlacingPositionAndRadius) == 2:
        study = self.getActiveStudy()
        if study:
          vrdn = study.GetVolumeRenderingDisplayNode()
          if vrdn:
            roi = vrdn.GetROINode()
            if roi:
              pos = self.__roiPlacingPositionAndRadius 
              
              x1 = pos[0][0]
              y1 = pos[0][1]
              z1 = pos[0][2]
              
              x2 = pos[1][0]
              y2 = pos[1][1]
              z2 = pos[1][2] 
              
              trans = study.GetRegistrationTransformNode()
              if trans:
                matrix = trans.GetMatrixTransformToParent()    
                x1 = x1 - matrix.GetElement(0,3)
                y1 = y1 - matrix.GetElement(1,3)
                z1 = z1 - matrix.GetElement(2,3)
                
                x2 = x2 - matrix.GetElement(0,3)
                y2 = y2 - matrix.GetElement(1,3)
                z2 = z2 - matrix.GetElement(2,3)
               
              roi.SetXYZ(x1,y1,z1)
              diff = [abs(x1-x2),abs(y1-y2),abs(z1-z2)]                    
              diff.sort()
              biggestRadius = diff[2]

              roi.SetRadiusXYZ(biggestRadius,biggestRadius,biggestRadius)
              
              self.manageROIsVisualization()
              
              self.findingWidget.setProperty("placeROIChecked",False)
              
              
  def __displayROICenterPosition(self, sliceWidget, xyPos):
    
    sv = sliceWidget.sliceView()
    renderer = sv.renderWindow().GetRenderers().GetItemAsObject(0)
    
    gs = vtk.vtkGlyphSource2D()
    gs.SetGlyphTypeToCircle()
    gs.FilledOn
    gs.SetColor(0.9,0.1,0)
    gs.SetScale(8)
    gs.SetCenter(xyPos[0],xyPos[1],0)
    gs.Update()
    
    
    if not sliceWidget in self.__sliceWidgetsMappersAndActors.keys():
      mpr = vtk.vtkPolyDataMapper2D()  
      actr = vtk.vtkActor2D()
      actr.SetMapper(mpr)
      self.__sliceWidgetsMappersAndActors[sliceWidget] = [mpr,actr]
    
    mapper = self.__sliceWidgetsMappersAndActors[sliceWidget][0]
    mapper.SetInput(gs.GetOutput())
    mapper.Update()
    
    actor = self.__sliceWidgetsMappersAndActors[sliceWidget][1]
    renderer.AddActor2D(actor)
    
    sv.scheduleRender()


  def __removeROICenterPosition(self, sliceWidget):
    
    sv = sliceWidget.sliceView()
    renderer = sv.renderWindow().GetRenderers().GetItemAsObject(0)
    
    if sliceWidget in self.__sliceWidgetsMappersAndActors.keys():
      actor = self.__sliceWidgetsMappersAndActors[sliceWidget][1]
      renderer.RemoveActor2D(actor)
      sv.scheduleRender()
    
    
  def onPETColorChanged(self, proceduralPETColorTableName):
    
    if self.activeReportNode:
      
      for i in xrange(self.activeReportNode.GetNumberOfStudyNodeIDs()):
        study = self.activeReportNode.GetStudy(i)
        petNode = study.GetPETVolumeNode()
        petDisplayNode = petNode.GetScalarVolumeDisplayNode()
        
        if petDisplayNode:
          petDisplayNode.SetAndObserveColorNodeID(proceduralPETColorTableName) 
          
          vrdn = study.GetVolumeRenderingDisplayNode()
          
          if (vrdn != None) & (study == self.getActiveStudy()):
            self.__updateColorFunction(vrdn)
    
    if self.__tempCroppedVol:
      svdn = self.__tempCroppedVol.GetScalarVolumeDisplayNode()
      if svdn:
        svdn.SetAndObserveColorNodeID(proceduralPETColorTableName)          
            
  def onCTWindowLevelChanged(self, window, level):

    if self.activeReportNode:
      for i in xrange(self.activeReportNode.GetNumberOfStudyNodeIDs()):
        study = self.activeReportNode.GetStudy(i)
        ctNode = study.GetCTVolumeNode()
        if ctNode:
          ctDisplayNode = ctNode.GetScalarVolumeDisplayNode()
          if ctDisplayNode:
            ctDisplayNode.SetWindow(window)
            ctDisplayNode.SetLevel(level)           
    
    
  def onChangeModuleSettingBool(self, setting, value):
    
    if value == True:
      value = "true"
    else:
      value= "false"
                  
    ViewHelper.setSetting(setting,value)    
    
    
  def onChangeModuleSettingValue(self, setting, value):
    ViewHelper.setSetting(setting,value)  
         
         
  def onEditorCollapsed(self, collapsed):
    
    if collapsed:
      self.__exitSegmentationMode()
      self.findingWidget.hideAddButton(True)
    else:
      self.__enterSegmentationMode()   
      self.findingWidget.hideAddButton(False)      
    
    self.manageCollapsibleButtonsAvailability()  

  
  def __exitSegmentationMode(self, manageImageVisualization = True):
    
    self.editorWidget.toolsColor.colorSpin.disconnect('valueChanged(int)',self.__onShowEditorColorWarning)
      
    self.editorWidget.editLabelMapsFrame.disconnect('contentsCollapsed(bool)', self.onEditorCollapsed)
    self.editorWidget.editLabelMapsFrame.setProperty('collapsed',True)
    self.findingWidget.hideAddButton(True)
    self.editorWidget.editLabelMapsFrame.connect('contentsCollapsed(bool)', self.onEditorCollapsed)   
       
    self.editorWidget.exit()
    
    if self.getActiveStudy():
      self.getActiveSegmentationROI().SetAndObserveTransformNodeID(self.getActiveStudy().GetRegistrationTransformNodeID()) 
    
    if manageImageVisualization:
      self.manageImageVolumesVisualization()
      
    self.reportsCollapsibleButton.setProperty('enabled',True)
    self.studiesCollapsibleButton.setProperty('enabled',True)
    self.reportOverviewWidget.setProperty('enabled',True)
    self.findingWidget.setProperty('selectionEnabled',True)    
    
    
    
  
  def __enterSegmentationMode(self):
    
    study = self.getActiveStudy()
    finding = self.getActiveFinding()
    
    self.editorWidget.toolsColor.colorSpin.connect('valueChanged(int)',self.__onShowEditorColorWarning)
     
    if (study == None) | (finding == None):
      return
  
    self.reportsCollapsibleButton.setProperty('enabled',False)
    self.studiesCollapsibleButton.setProperty('enabled',False)
    self.reportOverviewWidget.setProperty('enabled',False)  
    self.findingWidget.setProperty('selectionEnabled',False)
      
    self.__prepareVolumesForSegmentation(self, slicer.vtkMRMLAnnotationROINode.ValueModifiedEvent)
      
    seg = finding.GetSegmentationMappedByStudyNodeID(study.GetID())
    if seg != None:
      SegmentationHelper.pasteFromMainToCroppedLabelVolume(seg.GetLabelVolumeNode(), self.__tempLabelVol, finding.GetColorID())  
    

    self.editorWidget.setMasterNode(self.__tempCroppedVol) 
    self.editorWidget.setMergeNode(self.__tempLabelVol)      
      
    self.editorWidget.editUtil.getParameterNode().SetParameter('storedLabel',"")
    self.editorWidget.editUtil.getParameterNode().SetParameter('label',str(finding.GetColorID()))
    
    self.editorWidget.enter()  
     


  def __prepareVolumesForSegmentation(self,caller,event):
    
    study = self.getActiveStudy()
    finding = self.getActiveFinding()
    
    if (study==None) | (finding == None):
      return   
  
    if self.__tempCroppedVol == None:
      self.__createTempCroppedVolume()
    
    if self.__tempLabelVol == None:
      self.__createTempLabelVolume()
      
    self.__tempLabelVol.Copy(study.GetPETLabelVolumeNode())
    self.__tempLabelVol.SetName("LongitudinalPETCT_CroppedLabelVolume")
    self.__tempLabelVol.SetSaveWithScene(False)
    
    # temporary ROI in order to not invoke modified events with original roi
    croppingROI = slicer.vtkMRMLAnnotationROINode()
    croppingROI.Copy(self.getActiveSegmentationROI())
    
            
    self.__cropLogic.CropVoxelBased(self.getActiveSegmentationROI(), study.GetPETVolumeNode(), self.__tempCroppedVol)
    self.__cropLogic.CropVoxelBased(self.getActiveSegmentationROI(), study.GetPETLabelVolumeNode(), self.__tempLabelVol)
    
    self.__tempCroppedVol.SetAndObserveTransformNodeID(None)
    self.__tempLabelVol.SetAndObserveTransformNodeID(None)
    
    
    
    self.getActiveSegmentationROI().SetAndObserveTransformNodeID(None) 
    
    fitView = caller == self
    
    ViewHelper.SetRYGBgFgLblVolumes(self.__tempCroppedVol.GetID(),None,self.__tempLabelVol.GetID(), fitView)  
    
    petDisplayNode = study.GetPETVolumeNode().GetScalarVolumeDisplayNode()
    croppedPETDisplayNode = self.__tempCroppedVol.GetScalarVolumeDisplayNode()
    croppedPETDisplayNode.SetAndObserveColorNodeID(petDisplayNode.GetColorNodeID())
    croppedPETDisplayNode.SetAutoWindowLevel(False)
    croppedPETDisplayNode.SetWindow(petDisplayNode.GetWindow())
    croppedPETDisplayNode.SetLevel(petDisplayNode.GetLevel())
    
    self.__tempLabelVol.GetDisplayNode().SetAndObserveColorNodeID(self.activeReportNode.GetColorTableNodeID())       


  def onAddSegmentationToFinding(self):
          
    study = self.getActiveStudy()
    finding = self.getActiveFinding()
    roi = self.getActiveSegmentationROI()             

    if (study==None) | (finding == None) | (roi == None):
      return  
  
    pasted = False 
    
    seg = None
    
    if not finding.GetSegmentationMappedByStudyNodeID(study.GetID()):
      # create new segmentation  
      seg = slicer.mrmlScene.CreateNodeByClass('vtkMRMLLongitudinalPETCTSegmentationNode')
      seg.SetReferenceCount(seg.GetReferenceCount()-1) 
      slicer.mrmlScene.AddNode(seg)
      
      if self.moduleSettingsWidget.property("makeModels") == True:
        mh = slicer.mrmlScene.AddNode(slicer.vtkMRMLModelHierarchyNode())
        seg.SetAndObserveModelHierarchyNodeID(mh.GetID())
               
      seg.SetAndObserveLabelVolumeNodeID(study.GetPETLabelVolumeNode().GetID())
      finding.MapStudyNodeIDToSegmentationNodeID(study.GetID(),seg.GetID())
    
    else:
      seg = finding.GetSegmentationMappedByStudyNodeID(study.GetID())        
    
    SegmentationHelper.removeSegmentationFromVolume(seg.GetLabelVolumeNode(), finding.GetColorID())       
    
    pasted = SegmentationHelper.pasteFromCroppedToMainLabelVolume(self.__tempLabelVol, seg.GetLabelVolumeNode(), finding.GetColorID())    
    #set coordinates and extent of roi used for segmentation
    c = [0.,0.,0.]
    r = [0.,0.,0.]
    roi.GetXYZ(c)
    roi.GetRadiusXYZ(r)
    
    trans = study.GetRegistrationTransformNode()
    
    if trans:
      matrix = trans.GetMatrixTransformToParent() 
      c = [c[0]+matrix.GetElement(0,3),c[1]+matrix.GetElement(1,3),c[2]+matrix.GetElement(2,3)]    
      
    seg.SetROIxyz(c)
    seg.SetROIRadius(r)      
    
    if not SegmentationHelper.containsSegmentation(seg.GetLabelVolumeNode(),finding.GetColorID()):
      self.__removeSegmentationFromFinding(finding, study, seg)
    
    
    vrdn = study.GetVolumeRenderingDisplayNode()
        
    if ((self.moduleSettingsWidget.property("makeModels") == True) & (vrdn.GetClassName() != 'vtkMRMLNCIRayCastVolumeRenderingDisplayNode')):
      ViewHelper.makeModels(study,finding,self.activeReportNode.GetColorTableNode())     
      
    elif ViewHelper.getSetting('Models'):
      self.getUnsupportedVolumeRenderingMessageBox.show() 
  
    self.calculateSUVsAndStatistics(study, finding)
    
    self.__exitSegmentationMode(False)    
    
    self.manageImageVolumesVisualization(True)
    roiXYZ = ViewHelper.getROIPositionInRAS(self.getActiveSegmentationROI())
    ViewHelper.setSliceNodesCrossingPositionRAS(roiXYZ)
    
    self.__updateQuantitativeAnalysisCharts()
    
    self.manageCollapsibleButtonsAvailability()


  def __removeSegmentationFromFinding(self, finding, study, seg):
    
    finding.RemoveStudyNodeIDToSegmentationNodeIDMap(study.GetID())
    
    mh = seg.GetModelHierarchyNode()
    if mh:
      mn = mh.GetModelNode()
      if mn:
        mdn = mn.GetModelDisplayNode()
        slicer.mrmlScene.RemoveNode(mdn)
      slicer.mrmlScene.RemoveNode(mn)
              
    slicer.mrmlScene.RemoveNode(seg)        
              

  def calculateSUVsAndStatistics(self, study, finding):
        
    if (study == None) | (finding == None):
      return
      
    stats = LabelStatisticsLogic(study.GetPETVolumeNode(), study.GetPETLabelVolumeNode())
        
    if finding.GetColorID() in stats.labelStats["Labels"]:
      idx = stats.labelStats["Labels"].index(finding.GetColorID())
      idx = stats.labelStats["Labels"][idx]
      values = {}
      values["suvmax"] = stats.labelStats[idx,"Max"]
      values["suvmean"] = stats.labelStats[idx,"Mean"]
      values["suvmin"] = stats.labelStats[idx, "Min"]
      values["volcc"] = stats.labelStats[idx, "Volume cc"]
      values["volmm3"] = stats.labelStats[idx, "Volume mm^3"]
      values["count"] = stats.labelStats[idx, "Count"]
      values["stddev"] = stats.labelStats[idx, "StdDev"]
      
      seg = finding.GetSegmentationMappedByStudyNodeID(study.GetID())    
      if seg:
        seg.SetStatistics(values["suvmax"],values["suvmean"],values["suvmin"],values["volcc"],values["volmm3"],values["count"],values["stddev"])        
                   
    else:
      qt.QMessageBox.warning(None, SlicerLongitudinalPETCTModuleViewHelper.moduleDialogTitle(),'An error occured during the computation of the SUV values for the segmentation!')      
  
          
  def onShowQualitativeView(self, show = True):
    
    if show:   
      self.reportOverviewWidget.setAllSelectableOn()
      currentLayoutID = ViewHelper.updateQualitativeViewLayout(self.activeReportNode.GetNumberOfSelectedStudiesSelectedForAnalysis())
   
      self.manageVRVisualization()
      self.manageModelsVisibility()
      
      self.__hideSegmentationROINodes()
          
      if self.__qualitativeViewLastID != currentLayoutID:
        ViewHelper.SetForegroundOpacity(0.6, True)
        self.__qualitativeViewLastID = currentLayoutID
    
      for i in range(self.activeReportNode.GetNumberOfSelectedStudiesSelectedForAnalysis()):
        study = self.activeReportNode.GetSelectedStudySelectedForAnalysis(i)
        if study:
          if study.GetCTVolumeNode():  
            ViewHelper.SetCompareBgFgLblVolumes(self.activeReportNode.GetIndexOfSelectedStudySelectedForAnalysis(study),study.GetCTVolumeNode().GetID(),study.GetPETVolumeNode().GetID(),study.GetPETLabelVolumeNode().GetID(),True)      
          else:
            ViewHelper.SetCompareBgFgLblVolumes(self.activeReportNode.GetIndexOfSelectedStudySelectedForAnalysis(study),study.GetPETVolumeNode().GetID(),None, study.GetPETLabelVolumeNode().GetID(),True)      
             
      ViewHelper.resetThreeDViewsFocalPoints()            

  def __createChartArrayNodeNames(self):
      
    if not self.__chartArrayNodeNames:
      minStr = "SUVmin"
      meanStr = "SUVmean"
      maxStr = "SUVmax"        
      
      self.__chartArrayNodeNames.append(maxStr)
      self.__chartArrayNodeNames.append(meanStr)
      self.__chartArrayNodeNames.append(minStr)
    
    return self.__chartArrayNodeNames
  
  
  def __updateChartArrayNodes(self):
     
    m = len(self.__chartArrayNodes) % 3
    satMult = 1.0 - (m * 0.35)
    
    diff = len(self.__chartArrayNodeNames) * self.activeReportNode.GetNumberOfFindingNodeIDs() - len(self.__chartArrayNodes)
    
    if diff < 0:
      for i in xrange(diff*(-1)):   
        lastidx = len(self.__chartArrayNodes)-1   
        can = self.__chartArrayNodes[lastidx]
        self.__chartArrayNodes.pop() # pop last entry
        slicer.mrmlScene.RemoveNode(can)
    
    else:
      for i in xrange(diff):
        arrayNode = slicer.mrmlScene.AddNode(slicer.vtkMRMLDoubleArrayNode())          
        arrayNode.SetSaveWithScene(False)
        self.__chartArrayNodes.append(arrayNode)     


  def __updateQuantitativeAnalysisCharts(self):
  
    if self.__chartNode == None:
      self.__createChartNode()    
    
    self.__chartNode.ClearArrays()  
        
    self.__createChartArrayNodeNames()
      
    if len(self.__chartArrayNodes) != (self.activeReportNode.GetNumberOfFindingNodeIDs() * len(self.__chartArrayNodeNames)):
      self.__updateChartArrayNodes()
      
    tuples = self.activeReportNode.GetNumberOfSelectedStudiesSelectedForAnalysis()
    
    colorTable = self.activeReportNode.GetColorTableNode()
    lut = colorTable.GetLookupTable()    

      
    for i in xrange(len(self.__chartArrayNodes)):
      can = self.__chartArrayNodes[i]
      fid = int( i / len(self.__chartArrayNodeNames) )  
      finding = self.activeReportNode.GetFinding(fid)
      rgba = lut.GetTableValue(finding.GetColorID())
      can.SetName(finding.GetName()+" "+self.__chartArrayNodeNames[i % len(self.__chartArrayNodeNames)])
      array = can.GetArray()
      array.SetNumberOfTuples(tuples)     
    
      tuple = 0
      days = 0
      
      for j in xrange(tuples):
        study = self.activeReportNode.GetSelectedStudySelectedForAnalysis(j)
        
        if j > 0:
          oldstudy =  self.activeReportNode.GetSelectedStudySelectedForAnalysis(j-1)
            
          if (study != None) & (oldstudy != None):
            date = ViewHelper.dateFromDICOM(study.GetAttribute('DICOM.StudyDate'))
            olddate = ViewHelper.dateFromDICOM(oldstudy.GetAttribute('DICOM.StudyDate'))
            days += (date-olddate).days                  
        
        seg = finding.GetSegmentationMappedByStudyNodeID(study.GetID())
          
        if seg:
          stats = []
          stats.append(seg.GetSUVMax())
          stats.append(seg.GetSUVMean())
          stats.append(seg.GetSUVMin())
          
          array.SetComponent(tuple, 0, days)
          array.SetComponent(tuple, 1, stats[i % len(self.__chartArrayNodeNames)])
          array.SetComponent(tuple, 2, 0.)

          tuple += 1
        
          self.__chartNode.AddArray(can.GetName(), can.GetID())
          
      self.__chartNode.Modified()  
      colorStr = ViewHelper.RGBtoHex(rgba[0]*255,rgba[1]*255,rgba[2]*255,self.__saturationMultipliers[i % len(self.__chartArrayNodeNames)])
      self.__chartNode.SetProperty(can.GetName(), 'color', colorStr)
     
    try:
      ViewHelper.getStandardChartViewNode().Modified()
    except:
      pass

  def onShowQuantitativeView(self, show = True):
    
    if show:  
    
      self.reportOverviewWidget.setAllSelectableOn()
      
      currentLayoutID = ViewHelper.updateQuantitativeViewLayout(self.activeReportNode.GetNumberOfSelectedStudiesSelectedForAnalysis())
      
      self.__updateQuantitativeAnalysisCharts()
      
      chartViewNode = ViewHelper.getStandardChartViewNode() 
      chartViewNode.SetChartNodeID(self.__chartNode.GetID())
      
      self.manageVRVisualization()
      self.manageModelsVisibility()
      
      self.__hideSegmentationROINodes()
    

  def onStudySelectedForAnalysis(self, studyNode, selected):
      
    studyNode.SetSelectedForAnalysis(selected)

    if self.__isQualitativeViewActive():
      self.onShowQualitativeView()
    
    elif self.__isQuantitativeViewActive():
      self.onShowQuantitativeView()   



    
  def __createChartNode(self):
    if self.__chartNode == None:
      self.__chartNode = slicer.mrmlScene.AddNode(slicer.vtkMRMLChartNode())
      self.__chartNode.SetSaveWithScene(False)
      self.__chartNode.SetProperty('default', 'yAxisLabel', 'SUVbw')
      self.__chartNode.SetProperty('default', 'type', 'Scatter');
      self.__chartNode.SetProperty('default', 'showLegend', 'on') 
      self.__chartNode.SetProperty('default', 'title', 'Longitudinal PET/CT Analysis')
      self.__chartNode.SetProperty('default', 'xAxisLabel', 'days')
    
    return self.__chartNode                             


  def onSaveResultsCSV(self):
    if not self.__fileDialog:
      self.__fileDialog = qt.QFileDialog(self.parent)
      self.__fileDialog.options = self.__fileDialog.DontUseNativeDialog
      self.__fileDialog.acceptMode = self.__fileDialog.AcceptSave
      self.__fileDialog.defaultSuffix = "csv"
      self.__fileDialog.setNameFilter("Comma Separated Values (*.csv)")
      self.__fileDialog.connect("fileSelected(QString)", self.__onFileSelected)
    self.__fileDialog.show()
    
    
  def __onFileSelected(self,fileName):
    fp = open(fileName, "w")
    fp.write(self.__resultsAsCSV())
    fp.close()
        
        
  def __resultsAsCSV(self):
    """
    print comma separated value file with header keys in quotes
    """
    if self.activeReportNode:

      csv = "\"Finding_SUV\""  
      
      for i in xrange(self.activeReportNode.GetNumberOfSelectedStudies()):
        study = self.activeReportNode.GetSelectedStudy(i)
        if study:
          studyDate = study.GetAttribute('DICOM.StudyDate')
          # inserting dashes for date format
          studyDate = ViewHelper.insertStr(studyDate,'-',6)
          studyDate = ViewHelper.insertStr(studyDate,'-',4)  
        
          csv += ",\""+studyDate+"\""
          
          
      for j in xrange(self.activeReportNode.GetNumberOfFindingNodeIDs()):
        finding = self.activeReportNode.GetFinding(j)
        
        
        if not finding:
          continue
        
        maxrow = "\n\""+finding.GetName()+"_SUVmax\""
        meanrow = "\n\""+finding.GetName()+"_SUVmean\""
        minrow = "\n\""+finding.GetName()+"_SUVmin\""
        devrow = "\n\""+finding.GetName()+"_StdDev\""
        volccrow = "\n\""+finding.GetName()+"_Volcc\""
        count = "\n\""+finding.GetName()+"_Count\""
        
        for k in xrange(self.activeReportNode.GetNumberOfSelectedStudies()):
          study = self.activeReportNode.GetSelectedStudy(k)
          if study:
            seg = finding.GetSegmentationMappedByStudyNodeID(study.GetID())
            if not seg:
              empty = ","
              maxrow += empty
              meanrow += empty
              minrow += empty
              devrow += empty
              volccrow += empty
              count += empty
            else:
              maxrow += ","+str(seg.GetSUVMax())
              meanrow += ","+str(seg.GetSUVMean())
              minrow += ","+str(seg.GetSUVMin())
              devrow += ","+str(seg.GetStdDev())
              volccrow += ","+str(seg.GetVolcc())
              count += ","+str(seg.GetCount())
          
        csv += maxrow+meanrow+minrow+devrow+volccrow+count    

    return csv  


  def onQuantificationModuleChanged(self, name):
    
    module = None
    try:
      module = getattr(slicer.modules,name)
    except:
      try:
        name = name.lower()
        module = getattr(slicer.modules,name)
      except:
        qt.QMessageBox.warning(None,"CLI module not found","The entered module is not available. Restoring the previous setting.")               
    
    if module:
      ViewHelper.setSetting("quantificationCLIModule",name)     
          

  def __onShowEditorColorWarning(self):
    activeFinidng = self.getActiveFinding()
    if activeFinidng:
      selectedLabelValue = self.editorWidget.toolsColor.colorSpin.property('value')
      if (selectedLabelValue > 0) & ( selectedLabelValue != activeFinidng.GetColorID() ):     
        ViewHelper.getSegmentationColorChangeMessageBox()
        
     
            
                         