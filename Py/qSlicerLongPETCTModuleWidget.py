from __main__ import vtk, qt, ctk, slicer

from Editor import EditorWidget
from SlicerLongPETCTModuleViewHelper import SlicerLongPETCTModuleViewHelper as ViewHelper
import sys as SYS

import thread as Thread
#
# qSlicerLongPETCTModuleWidget
#

class qSlicerLongPETCTModuleWidget:
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
      
      

  def setup(self): 
    # Instantiate and connect widgets ...

    # switch to FourUp Layout
    lm = slicer.app.layoutManager()
    if lm:
      lm.setLayout(slicer.vtkMRMLLayoutNode.SlicerLayoutFourUpView) # two over two
    

    self.qualitativeViewLastID = -1
    self.reportObserverIDs = []
    self.tempObserverEditorTag = -1
    self.viewNodeObserverID = -1
    self.compareViewNodeObserverIDs = []
    self.segmentationROIOldPosition = [0.,0.,0.]
    #self.tempCroppedLblVolObserverTag = -1

    self.logic  = slicer.modules.longpetct.logic()
    self.vrLogic = slicer.modules.volumerendering.logic()

    self.findingSettingsDialog = None

    self.tempCroppedVol = slicer.vtkMRMLScalarVolumeNode()
    self.tempCroppedVol.SetName("TempCroppedVolume")
    self.tempCroppedVol.SetHideFromEditors(False)
    slicer.mrmlScene.AddNode(self.tempCroppedVol)
    
    self.tempLabelVol = slicer.vtkMRMLScalarVolumeNode()
    self.tempLabelVol.SetName("TempLabelVolume")
    self.tempLabelVol.SetHideFromEditors(False)
    slicer.mrmlScene.AddNode(self.tempLabelVol)
    
    self.selectedPETWindow = 0.0
    self.selectedPETLevel = 0.0
    
    self.cliNode = None
    self.cliModelMaker = None
    
    
    self.findingsInfoMessageBox = None
    self.unsupportedVolRendMessageBox = None
    
    self.findingROIVisiblityBackup = True
       
    #self.opacityFunction = vtk.vtkPiecewiseFunction()
    
    # instanciate all collapsible buttons
    self.reportsCollapsibleButton = ctk.ctkCollapsibleButton()
    self.reportsCollapsibleButton.setToolTip(ViewHelper.reportsHelpText())
    self.studiesCollapsibleButton = ctk.ctkCollapsibleButton()
    self.findingsCollapsibleButton = ctk.ctkCollapsibleButton()
    self.analysisCollapsibleButton = ctk.ctkCollapsibleButton()
    
    # put all collapsible buttons in a button group so that only one can be uncollapsed at a time
    self.collapsibleButtonsGroup = qt.QButtonGroup()
    self.collapsibleButtonsGroup.addButton(self.reportsCollapsibleButton)
    self.collapsibleButtonsGroup.addButton(self.studiesCollapsibleButton)
    self.collapsibleButtonsGroup.addButton(self.findingsCollapsibleButton)
    self.collapsibleButtonsGroup.addButton(self.analysisCollapsibleButton)
       
    # show Report selection widget
    self.reportsCollapsibleButton.setProperty('collapsed',False)
    
    # Reports Collapsible button
    self.reportsCollapsibleButton.text = "Report Selection"
    
    self.layout.addWidget(self.reportsCollapsibleButton)
    
    reportsLayout = qt.QVBoxLayout(self.reportsCollapsibleButton)
      
    self.reportSelectionWidget = slicer.modulewidget.qSlicerLongPETCTReportSelectionWidget()    
    self.reportSelectionWidget.setMRMLScene(slicer.mrmlScene)
    self.reportSelector = self.reportSelectionWidget.mrmlNodeComboBoxReport()
      
    reportsLayout.addWidget(self.reportSelectionWidget)
    
    # Studies Collapsible button    
    self.studiesCollapsibleButton.text = "Study Selection"
    self.studiesCollapsibleButton.setProperty('collapsed',True)
    
    
    self.layout.addWidget(self.studiesCollapsibleButton)

    studiesLayout = qt.QVBoxLayout(self.studiesCollapsibleButton)

    self.studySelectionWidget = slicer.modulewidget.qSlicerLongPETCTStudySelectionWidget()    
    self.studySelectionWidget.setProperty('volumeRendering',True)
    self.studySelectionWidget.setProperty('gpuRendering',False)
    self.studySelectionWidget.setProperty('linearOpacity',False)
    self.studySelectionWidget.setProperty('spinView',True)
    
    self.studySelectionWidget.setReportNode(self.getCurrentReport())
    self.reportSelector.connect('currentNodeChanged(vtkMRMLNode*)',self.onCurrentReportChanged)
    
    studiesLayout.addWidget(self.studySelectionWidget)

    self.studySelectionWidget.connect('studySelected(int)',self.onStudySelected)
    self.studySelectionWidget.connect('studyDeselected(int)',self.onStudyDeselected)    
    self.studySelectionWidget.connect('spinViewToggled(bool)',ViewHelper.spinStandardViewNode)
    self.studySelectionWidget.connect('volumeRenderingToggled(bool)',self.setVolumeRendering)
    self.studySelectionWidget.connect('opacityPowChanged(double)',self.onSetOpacityPowForCurrentStudy)
    self.studySelectionWidget.connect('showStudiesCentered(bool)',self.onStudySelectionWidgetShowStudiesCentered)

    # Findings Collapsible button
    self.findingsCollapsibleButton.text = "Findings"
    self.findingsCollapsibleButton.setProperty('collapsed',True)
    self.findingsCollapsibleButton.connect('contentsCollapsed(bool)', self.onShowFindingsInfoMessageBox)      
    
    
    #if self.reportSelector.currentNode():
      #if self.reportSelector.currentNode().GetUserSelectedStudy():
        #self.enableFindingsCollapsibleButton(True)
      #else:
        #self.enableFindingsCollapsibleButton(False)
   
    
    self.layout.addWidget(self.findingsCollapsibleButton)
    
    findingsLayout = qt.QVBoxLayout(self.findingsCollapsibleButton)
      
    self.findingSelectionWidget = slicer.modulewidget.qSlicerLongPETCTFindingSelectionWidget()    
    self.findingSelectionWidget.setMRMLScene(slicer.mrmlScene)
    self.findingSelectionWidget.connect('roiVisibilityChanged(bool)', self.onFindingROIVisibilityChanged)
    self.findingSelectionWidget.connect('helpRequested()', self.onFindingSelectionHelp)
    self.findingSelectionWidget.connect('addSegmentationToFinding()', self.onCollapseEditor)
    
    self.findingSelector = self.findingSelectionWidget.mrmlNodeComboBoxFinding()
    #self.findingSelector.connect('nodeAddedByUser(vtkMRMLNode*)', self.onFindingNodeCreated)
    self.findingSelector.connect('currentNodeChanged(vtkMRMLNode*)', self.onFindingNodeChanged)
    self.findingSelector.connect('nodeAboutToBeEdited(vtkMRMLNode*)', self.onShowFindingSettingsDialog)
    self.findingSelector.connect('nodeAboutToBeRemoved(vtkMRMLNode*)', self.onFindingNodeToBeRemoved)
    
    # Editor widget in Findings
    editorWidgetParent = slicer.qMRMLWidget()
    editorWidgetParent.setLayout(qt.QVBoxLayout())
    editorWidgetParent.setMRMLScene(slicer.mrmlScene)
    self.editorWidget = EditorWidget(parent=editorWidgetParent,showVolumesFrame=False)
    self.editorWidget.setup()
    self.editorWidget.toolsColor.frame.setVisible(False)
    
    self.editorWidget.editLabelMapsFrame.setText("Edit Segmentation")
    self.editorWidget.editLabelMapsFrame.setEnabled(False)
    
    findingsLayout.addWidget(self.findingSelectionWidget)
    
    if self.reportSelector.currentNode():
      if self.reportSelector.currentNode().GetUserSelectedFinding():
        self.editorWidget.editLabelMapsFrame.setEnabled(True)
        
    self.editorWidget.editLabelMapsFrame.connect('contentsCollapsed(bool)', self.onEditorCollapsed)      
    
    self.findingSelectionWidget.setEditorWidget(editorWidgetParent)


    #Analysis Collapsible Button
    self.analysisCollapsibleButton.text = "Analysis"
    self.analysisCollapsibleButton.setProperty('collapsed',True)
    self.analysisCollapsibleButton.connect('contentsCollapsed(bool)', self.onSwitchToQualitativeView)      
    
    self.layout.addWidget(self.analysisCollapsibleButton)
    
    analysisLayout = qt.QVBoxLayout(self.analysisCollapsibleButton)
    self.analysisSettingsWidget = slicer.modulewidget.qSlicerLongPETCTAnalysisSettingsWidget()    
    self.analysisSettingsWidget.setReportNode(self.getCurrentReport())
    self.analysisSettingsWidget.connect('studySelectedForAnalysis(int,bool)', self.showQualitativeView)
    self.analysisSettingsWidget.connect('volumeRenderingToggled(bool)',self.setVolumeRendering)
    self.analysisSettingsWidget.connect('spinViewToggled(bool)',ViewHelper.spinCompareViewNodes)
    
    analysisLayout.addWidget(self.analysisSettingsWidget)  


    # Add vertical spacer
    self.layout.addStretch()
    
    self.separator = qt.QFrame()
    self.separator.setFrameStyle(qt.QFrame.HLine | qt.QFrame.Sunken)
    self.layout.addWidget(self.separator)

    # Add Study Slider
    self.studySliderWidget = slicer.modulewidget.qSlicerLongPETCTStudySliderWidget()
    self.studySliderWidget.setReportNode(self.getCurrentReport())
    self.layout.addWidget(self.studySliderWidget)
    self.studySliderWidget.connect('sliderValueChanged(int)',self.onSliderWidgetValueChanged)
    
    # Add Report Table
    self.reportTableWidget = slicer.modulewidget.qSlicerLongPETCTReportTableWidget()
    self.reportTableWidget.setReportNode(self.getCurrentReport())
    self.reportTableWidget.connect('studyClicked(int)',self.onReportTableStudyClicked)
    self.reportTableWidget.connect('findingClicked(int)',self.onReportTableFindingClicked)                 
    self.layout.addWidget(self.reportTableWidget) 
    
    
    ViewHelper.SetForegroundOpacity(0.6)       
    self.observeReportNode(self.reportSelector.currentNode())
    
  def onEditorCollapsed(self,collapsed):
    print "C1"
    self.findingSelectionWidget.setProperty('selectionEnabled',collapsed)    
    print "C2"
    self.onEnterEditMode( collapsed != True )
    print "C3"
    self.findingSelectionWidget.hideAddButton(collapsed)
    
 
  def onCollapseEditor(self):
    self.editorWidget.editLabelMapsFrame.setProperty('collapsed', True)

  def onSliderWidgetValueChanged(self, value):
    currentStudy = self.getCurrentStudy()
    
    currentReport = self.reportSelector.currentNode()
    if currentStudy:
      currentSelectedStudy = self.getCurrentReport().GetSelectedStudy(value)
    
      self.updateBgFgToUserSelectedStudy(currentSelectedStudy)
      
      if currentSelectedStudy:
        self.setCurrentStudy(currentSelectedStudy)

        studyIdx = currentReport.GetIndexOfStudy(currentSelectedStudy)
        self.studySelectionWidget.selectStudyInRow(studyIdx)
     
        self.manageVRDisplayNodesVisibility(currentSelectedStudy)
        #self.onUpdateVolumeRendering(currentSelectedStudy.GetPETVolumeNode())

      else:
        #self.onUpdateVolumeRendering(None)
        self.manageVRDisplayNodesVisibility(None)
        
        
  def setCurrentStudy(self, study):

    currentReport = self.getCurrentReport()   
    currentFinding = self.getCurrentFinding()
    
    if currentReport:
      currentReport.SetUserSelectedStudy(study)   
      currentStudy = self.getCurrentStudy()     
      
      if currentStudy:
        vrdn = currentStudy.GetVolumeRenderingDisplayNode()
        if vrdn:
          pow = None
          pow = float(vrdn.GetAttribute("OpacityPow"))
          if pow:
            self.studySelectionWidget.setProperty('opacityPow',pow)
            print str(pow)
            self.onSetOpacityPowForCurrentStudy(pow)
            
        #self.setVisibleModelHierarchy(currentReport.GetIndexOfSelectedStudy(currentStudy))    
      
      if currentFinding:
        currentSegROI = currentFinding.GetSegmentationROI()
      
        if (currentStudy != None) & ( currentSegROI != None) :
          currentStudy.SetSegmentationROI(currentSegROI)
          self.updateSegmentationROIPosition()
          
        elif currentStudy:
          currentStudy.SetSegmentationROI(None)
        
       
  def updateSegmentationROIPosition(self):
    currentStudy = self.getCurrentStudy()
    currentFinding = self.getCurrentFinding()
    
    if (currentStudy != None) & (currentFinding != None):
      currentSegmentation = currentFinding.GetSegmentationForStudy(currentStudy)
          
      if currentSegmentation:
        xyz = [0.,0.,0.]
        radius = [0.,0.,0.]
            
        currentSegmentation.GetROIxyz(xyz)
        currentSegmentation.GetROIRadius(radius)
        
        if currentStudy.GetSegmentationROI() != None:
          currentStudy.GetSegmentationROI().SetXYZ(xyz)
          currentStudy.GetSegmentationROI().SetRadiusXYZ(radius)
            
          xyzRAS = ViewHelper.getROIPositionInRAS(currentStudy.GetSegmentationROI())
            
          ViewHelper.setSliceNodesCrossingPositionRAS(xyzRAS)
          
          if self.analysisCollapsibleButton.property('collapsed') == False:
            ViewHelper.setCompareSliceNodesCrossingPositionRAS(self.getCurrentReport().GetIndexOfStudySelectedForAnalysis(currentStudy), xyzRAS)   


  def onStudySelected(self, idx):
    currentReport = self.reportSelector.currentNode()
    if currentReport:
      selectedStudy = currentReport.GetStudy(idx)
      if selectedStudy:
        selectedStudy.SetSelectedForSegmentation(True)
        self.setCurrentStudy(selectedStudy)           
        
        petID = ""
        ctID = ""
        
        firstDisplayPet = selectedStudy.GetPETVolumeNode().GetScalarVolumeDisplayNode() == None
        firstDisplayCt = selectedStudy.GetCTVolumeNode().GetScalarVolumeDisplayNode() == None
        firstDisplayPetLabels = selectedStudy.GetPETLabelVolumeNode().GetScalarVolumeDisplayNode() == None
                
        if firstDisplayPet | firstDisplayCt | firstDisplayPetLabels:
          self.updateBgFgToUserSelectedStudy(selectedStudy, True)

        if firstDisplayPetLabels:
          selectedStudy.GetPETLabelVolumeNode().GetDisplayNode().SetAndObserveColorNodeID(currentReport.GetFindingTypesColorTable().GetID())
        
        if firstDisplayCt:
          selectedStudy.GetCTVolumeNode().GetScalarVolumeDisplayNode().SetAutoWindowLevel(0)
          selectedStudy.GetCTVolumeNode().GetScalarVolumeDisplayNode().SetAndObserveColorNodeID("vtkMRMLColorTableNodeGrey")
          selectedStudy.GetCTVolumeNode().GetScalarVolumeDisplayNode().SetWindowLevel(350,40)
          
        if firstDisplayPet:
          petDisplayNode = selectedStudy.GetPETVolumeNode().GetScalarVolumeDisplayNode()
          if petDisplayNode:
            petDisplayNode.SetAutoWindowLevel(0)
            petDisplayNode.AddObserver(vtk.vtkCommand.ModifiedEvent, self.petDisplayNodeModified) 
            petDisplayNode.SetAndObserveColorNodeID("vtkMRMLPETProceduralColorNodePET-Heat");

          #compositeNodes = slicer.util.getNodes('vtkMRMLSliceCompositeNode*')
          #for compositeNode in compositeNodes.values():
            #compositeNode.SetForegroundVolumeID(petID)
            #compositeNode.SetForegroundOpacity(0.6)
      
        viewNode = ViewHelper.getStandardViewNode()
        if viewNode.GetBoxVisible() != 0:
          viewNode.SetBoxVisible(0)
        
        
        if selectedStudy.GetVolumeRenderingDisplayNode() == None:
          vrDisplayNode = self.vrLogic.CreateVolumeRenderingDisplayNode()
          if vrDisplayNode:
            vrDisplayNode.SetCroppingEnabled(0)
            vrDisplayNode.SetAttribute("OpacityPow",str(self.studySelectionWidget.property('opacityPow')))
          
            petVolume = selectedStudy.GetPETVolumeNode()
            if petVolume:
              vrDisplayNode.SetAndObserveVolumeNodeID(petVolume.GetID())
              vrDisplayNode.SetName(petVolume.GetName() +"_VR_Display")
              self.vrLogic.UpdateDisplayNodeFromVolumeNode(vrDisplayNode, petVolume)  
              if vrDisplayNode.GetVolumePropertyNode():
                vrDisplayNode.GetVolumePropertyNode().SetName(petVolume.GetName() + "_VR_VolumeProperty")
              if vrDisplayNode.GetROINode():
                vrDisplayNode.GetROINode().SetName(petVolume.GetName() + "_VR_ROI")
            selectedStudy.SetVolumeRenderingDisplayNode(vrDisplayNode)
          
        self.manageVRDisplayNodesVisibility(selectedStudy)         
        viewNode.InvokeEvent(slicer.vtkMRMLViewNode.ResetFocalPointRequestedEvent)
        
        #self.onUpdateVolumeRendering(selectedStudy.GetPETVolumeNode())
        self.studySelectionWidget.selectStudyInRow(idx)       
        
      
      self.updateBgFgToUserSelectedStudy(selectedStudy)
      
      if self.viewNodeObserverID == -1:
        self.viewNodeObserverID = viewNode.AddObserver(vtk.vtkCommand.ModifiedEvent, self.viewNodeModified) 

      #if currentReport.GetUserSelectedStudy():
        #self.enableFindingsCollapsibleButton(True)
      #else:
        #self.enableFindingsCollapsibleButton(False)                  
   
  
  def manageVRDisplayNodesVisibility(self, selectedStudy):
    
    viewNode = ViewHelper.getStandardViewNode()
    currentReport = self.getCurrentReport()
    
    if currentReport:
      for i in range(currentReport.GetStudiesCount()):
        study = currentReport.GetStudy(i)
        if study:
          vrDisplayNode = study.GetVolumeRenderingDisplayNode()
          if vrDisplayNode:
            vrDisplayNode.RemoveViewNodeID(viewNode.GetID())
            if selectedStudy == study:
              vrDisplayNode.AddViewNodeID(viewNode.GetID()) # check if already added in vtkMRMLDisplayNode
              vrDisplayNode.Modified()
              
      if selectedStudy:
              
        selVrDisplayNode = selectedStudy.GetVolumeRenderingDisplayNode()
        if selVrDisplayNode:
          
          self.onSetOpacityPowForCurrentStudy(float(selVrDisplayNode.GetAttribute('OpacityPow')))
              
          if self.studySelectionWidget.property('volumeRendering'):
            selVrDisplayNode.VisibilityOn()
            viewNode.SetAxisLabelsVisible(True)
          else:
            selVrDisplayNode.VisibilityOff()
            viewNode.SetAxisLabelsVisible(False)
          
          if self.studySelectionWidget.property('spinView') & self.studySelectionWidget.property('volumeRendering'):
            viewNode.SetAnimationMode(viewNode.Spin)
          else:
            viewNode.SetAnimationMode(viewNode.Off)
             
    viewNode.Modified() 
      
    
        
  def onStudyDeselected(self, idx):
    currentReport = self.reportSelector.currentNode()
    if currentReport:
      study = currentReport.GetStudy(idx)
      if study:
        study.SetSelectedForSegmentation(False)
        
        if currentReport.IsStudyInUse(study):
          qt.QMessageBox.question(None,ViewHelper.moduleDialogTitle(),"Segmentations have been performed on this Study!")
      
      lastSelectedStudy = currentReport.GetSelectedStudyLast()
      self.setCurrentStudy(lastSelectedStudy)
      

      self.manageVRDisplayNodesVisibility(currentReport.GetUserSelectedStudy())
        #self.onUpdateVolumeRendering(currentReport.GetUserSelectedStudy().GetPETVolumeNode())
      
      self.updateBgFgToUserSelectedStudy(currentReport.GetUserSelectedStudy())   
      
      #if currentReport.GetUserSelectedStudy():
        #self.enableFindingsCollapsibleButton(True)
      #else:
        #self.enableFindingsCollapsibleButton(False)        
   
      
      
  def onCurrentReportChanged(self, reportNode):
    
    self.observeReportNode(reportNode)
    
    self.logic.SetSelectedReportNode = reportNode
    self.studySliderWidget.setReportNode(reportNode)
    self.studySelectionWidget.setReportNode(reportNode)
    self.analysisSettingsWidget.setReportNode(reportNode)
    #self.findingSelectionWidget.setReportNode(reportNode)
    self.reportTableWidget.setReportNode(reportNode)
    
    if reportNode:
      
      self.manageVRDisplayNodesVisibility(reportNode.GetUserSelectedStudy())
      
      #if reportNode.GetUserSelectedStudy():
        #self.onUpdateVolumeRendering(reportNode.GetUserSelectedStudy().GetPETVolumeNode())
      #else:
        #self.onUpdateVolumeRendering(None)
    
      self.updateBgFgToUserSelectedStudy(reportNode.GetUserSelectedStudy())
    
    else:
      self.manageVRDisplayNodesVisibility(None)
      self.updateBgFgToUserSelectedStudy(None)  
    
    
  def updateBgFgToUserSelectedStudy(self, userSelectedStudy, fitVolumes = False):    
    
    petID = ""
    ctID = ""
    petLblID = ""
      
    if userSelectedStudy:
      petVolume = userSelectedStudy.GetPETVolumeNode()
      ctVolume = userSelectedStudy.GetCTVolumeNode()
      petLblVolume = userSelectedStudy.GetPETLabelVolumeNode()
      
      if petVolume:
        petID = petVolume.GetID()
      if ctVolume:
        ctID = userSelectedStudy.GetCTVolumeNode().GetID()
      if petLblVolume:
        petLblID = petLblVolume.GetID()

    ViewHelper.SetRYGBgFgLblVolumes(ctID,petID,petLblID, fitVolumes)
    #ViewHelper.SetBgFgLblVolumes(ctID,petID,petLblID,False) 
      

  
  def onSetOpacityPowForCurrentStudy(self, pow):
    
    currentStudy = self.getCurrentStudy()
    if currentStudy:
      vrdn = currentStudy.GetVolumeRenderingDisplayNode()
      if vrdn:
        vrdn.SetAttribute("OpacityPow",str(pow))
        
        self.updateOpacityPow(currentStudy)
          
      
  
  def updateOpacityPow(self, study):
    
    if study:
      petVolumeNode = study.GetPETVolumeNode()

      if petVolumeNode:
        if petVolumeNode.GetScalarVolumeDisplayNode():
          
          window = petVolumeNode.GetScalarVolumeDisplayNode().GetWindow()

          vrdn = study.GetVolumeRenderingDisplayNode()
          
          if vrdn:

            pow = None
            pow = float(vrdn.GetAttribute("OpacityPow"))
            opacityFunction = ViewHelper.opacityPowerFunction(window,pow,10.)
   
            if opacityFunction:
              propNode = vrdn.GetVolumePropertyNode()

              if propNode:
                propNode.SetScalarOpacity(opacityFunction)
                propNode.Modified()
                
            vrdn.Modified()
                
   
      
              
         
  
  def setVolumeRendering(self, on):
    currentReport = self.getCurrentReport()
    
    if currentReport:
      for i in range(currentReport.GetStudiesCount()):
        study = currentReport.GetStudy(i)
        if study:
          vrdn = study.GetVolumeRenderingDisplayNode()
          if vrdn:
            vrdn.SetVisibility(on)
      
      ViewHelper.getStandardViewNode().SetAxisLabelsVisible(on)
      for vn in ViewHelper.getCompareViewNodes():
        vn.SetAxisLabelsVisible(on)      
      
                          
        
  def onStudySelectionWidgetShowStudiesCentered(self, centered):
    currentReport = self.getCurrentReport()
    currentStudy = self.getCurrentStudy()
    if currentReport:

      for i in range(0,currentReport.GetStudiesCount(),1): 
        currentReport.GetStudy(i).SetCenteredVolumes(centered)
      
      if currentStudy:
        self.manageVRDisplayNodesVisibility(currentStudy)
        self.updateBgFgToUserSelectedStudy(currentStudy)
        

  def onSegmentationROIModified(self, caller, event):
    
    if caller != self:
      minFloat = SYS.float_info.min
      xyz = [minFloat,minFloat,minFloat]
      
      caller.GetXYZ(xyz)
 
      if (xyz[0] == self.segmentationROIOldPosition[0]) & (xyz[1] == self.segmentationROIOldPosition[1]) & (xyz[2] == self.segmentationROIOldPosition[2]):
        return
    
      else:
        self.segmentationROIOldPosition = xyz
    
    currentStudy = self.getCurrentStudy()
    currentFinding = self.getCurrentFinding()
    
    if (currentStudy != None) & (currentFinding != None):
      
      self.tempCroppedVol.Copy(currentStudy.GetPETVolumeNode())
      self.tempCroppedVol.SetName("LongitudinalPETCT_CroppedVolume") 
    
      cropLogic = slicer.modules.cropvolume.logic()
      
      # temporary ROI in order to not invoke modified events with original roi
      croppingROI = slicer.vtkMRMLAnnotationROINode()
      croppingROI.Copy(currentFinding.GetSegmentationROI())
      
      cropLogic.CropVoxelBased(croppingROI,currentStudy.GetPETVolumeNode(),self.tempCroppedVol)

      #///
      self.tempCroppedVol.SetAndObserveTransformNodeID(None)
            
      currentFinding.GetSegmentationROI().SetAndObserveTransformNodeID(None)
      
      volLogic  = slicer.modules.volumes.logic()    
    
      createdLabelVol = volLogic.CreateLabelVolume(slicer.mrmlScene, self.tempCroppedVol,'TempLabelVolume')
   

      #if self.tempLabelVol.GetScalarVolumeDisplayNode():
        #slicer.mrmlScene.RemoveNode(tempLabelVol.GetScalarVolumeDisplayNode())
      if self.tempLabelVol.GetDisplayNode():
        slicer.mrmlScene.RemoveNode(self.tempLabelVol.GetDisplayNode())
      self.tempLabelVol.Copy(createdLabelVol)   
      self.tempLabelVol.SetName("LongitudinalPETCT_CroppedLabelVolume")
      
      slicer.mrmlScene.RemoveNode(createdLabelVol)
      
      propagate = caller == self
      ViewHelper.SetRYGBgFgLblVolumes(self.tempCroppedVol.GetID(),None,self.tempLabelVol.GetID(),propagate)  
    
    self.tempLabelVol.GetDisplayNode().SetAndObserveColorNodeID(self.getCurrentReport().GetFindingTypesColorTable().GetID())
        

  
  def onEnterEditMode(self,enter):
    
    currentStudy = self.getCurrentStudy()
    currentFinding = self.getCurrentFinding()
      
    if (currentStudy != None) & (currentFinding != None) & (enter == True):
     
      self.reportsCollapsibleButton.setProperty('enabled',False)
      self.studiesCollapsibleButton.setProperty('enabled',False)
      self.studySliderWidget.setProperty('enabled',False)
      self.reportTableWidget.setProperty('enabled',False)
     
      self.onSegmentationROIModified(self, slicer.vtkMRMLAnnotationROINode.ValueModifiedEvent)
      
      studySeg = currentFinding.GetSegmentationForStudy(currentStudy)
      
      if studySeg != None:
        ViewHelper.pasteFromMainToCroppedLabelVolume(studySeg.GetLabelVolumeNode(), self.tempLabelVol, currentFinding.GetColorID())  
                    
      self.editorWidget.editUtil.setLabel(currentFinding.GetColorID())
      self.editorWidget.setMasterNode(self.tempCroppedVol) 
      self.editorWidget.setMergeNode(self.tempLabelVol)
    
      self.editorWidget.enter()
    
      self.tempObserverEditorTag = currentFinding.GetSegmentationROI().AddObserver(vtk.vtkCommand.ModifiedEvent, self.onSegmentationROIModified)      
      #self.tempCroppedLblVolObserverTag = self.tempLabelVol.GetImageData().AddObserver(vtk.vtkCommand.ModifiedEvent, self.pasteFromCroppedToMainLblVolume)    

    elif enter == False:
      self.editorWidget.exit()     
      pasted = self.pasteFromCroppedToMainLblVolume(self, vtk.vtkCommand.ModifiedEvent)
      studySeg = currentFinding.GetSegmentationForStudy(currentStudy)

      if studySeg:
        #self.calculateSUVs()
        qt.QTimer.singleShot(0,self.calculateSUVs)

        vrdn = currentStudy.GetVolumeRenderingDisplayNode()
        if vrdn.GetClassName() != 'vtkMRMLNCIRayCastVolumeRenderingDisplayNode':
          qt.QTimer.singleShot(0,self.makeModels)

        else:
          if self.unsupportedVolRendMessageBox == None:
            self.unsupportedVolRendMessageBox = ctk.ctkMessageBox()
            self.unsupportedVolRendMessageBox.setModal(True)
            self.unsupportedVolRendMessageBox.setIcon(qt.QMessageBox.Information)
            self.unsupportedVolRendMessageBox.setWindowTitle(ViewHelper.moduleDialogTitle())
            self.unsupportedVolRendMessageBox.setText('NCIRayCastVolumeRendering is selected as default volume renderer. The Longitudinal PET/CT Analysis module does not support displaying of models from Finding segmentations with this volume renderer!')
            self.unsupportedVolRendMessageBox.setProperty('dontShowAgainVisible',True)
            self.unsupportedVolRendMessageBox.setDontShowAgain(False)  
          
          self.unsupportedVolRendMessageBox.show()  
      #else:
        #if self.reportTableWidget:
          #self.reportTableWidget.clearSegmentationSUVs(currentStudy,currentFinding)           
      
        

      if self.studySelectionWidget.property('centeredSelected'):
        currentFinding.GetSegmentationROI().SetAndObserveTransformNodeID(currentStudy.GetCenteringTransform().GetID())
      
      self.updateBgFgToUserSelectedStudy(currentStudy,True)
      
      roiXYZ = ViewHelper.getROIPositionInRAS(currentFinding.GetSegmentationROI())
      ViewHelper.setSliceNodesCrossingPositionRAS(roiXYZ)
      
      self.reportsCollapsibleButton.setProperty('enabled',True)
      self.studiesCollapsibleButton.setProperty('enabled',True)
      self.studySliderWidget.setProperty('enabled',True)
      self.reportTableWidget.setProperty('enabled',True)
                 
  def pasteFromCroppedToMainLblVolume(self, caller, event):
    
    currentStudy = self.getCurrentStudy()
    currentFinding = self.getCurrentFinding()
    
    pasted = False
    
    if (currentStudy != None) & (currentFinding != None):
      segmentationROI = currentFinding.GetSegmentationROI()
      
      if segmentationROI:
        segmentationROI.RemoveObserver(self.tempObserverEditorTag)
     
      studySeg = None
     
      if currentFinding.GetSegmentationForStudy(currentStudy) == None:
        studySeg = slicer.mrmlScene.CreateNodeByClass('vtkMRMLLongPETCTSegmentationNode')
        studySeg.SetReferenceCount(studySeg.GetReferenceCount()-1) 
        
        slicer.mrmlScene.AddNode(studySeg)
        studySeg.SetLabelVolumeNode(currentStudy.GetPETLabelVolumeNode())
        
        currentFinding.AddSegmentationForStudy(currentStudy,studySeg)
      
      else:
        studySeg = currentFinding.GetSegmentationForStudy(currentStudy)        
            
      pasted = ViewHelper.pasteFromCroppedToMainLabelVolume(self.tempLabelVol, studySeg.GetLabelVolumeNode(), currentFinding.GetColorID())    
    
      if segmentationROI:
        xyz = [0.,0.,0.]
        radius = [0.,0.,0.]
        
        segmentationROI.GetXYZ(xyz)
        segmentationROI.GetRadiusXYZ(radius)
        
        studySeg.SetROIxyz(xyz)
        studySeg.SetROIRadius(radius)
        
        
      if ViewHelper.containsSegmentation(studySeg.GetLabelVolumeNode(),currentFinding.GetColorID()) == False:
        currentFinding.RemoveSegmentationForStudy(currentStudy)
        slicer.mrmlScene.RemoveNode(studySeg)

    
    #if self.tempCroppedLblVolObserverTag != -1:      
      #self.tempLabelVol.GetImageData().RemoveObserver(self.tempCroppedLblVolObserverTag) 
            
    return pasted 
  

  def setupFindingNode(self, findingNode):

    currentStudy = self.getCurrentStudy()
    
    if (currentStudy != None) & (findingNode != None):
      
      if findingNode.GetSegmentationROI() == None:
        roi = slicer.vtkMRMLAnnotationROINode()
        roi.SetScene(slicer.mrmlScene)
        roi.SetHideFromEditors(False)
        roi.SetName(findingNode.GetName()+"_ROI")  
        roi.SetVisibility(self.findingSelectionWidget.property('roiVisibility')) 
        
        r = 50.0
        redSlice = slicer.mrmlScene.GetNodeByID("vtkMRMLSliceNodeRed")
        if redSlice:
          fov = redSlice.GetFieldOfView()
          fov = sorted(fov)
          # set ROI radius to 1/10 of short side from Field of View of red slice 
          r = fov[1]/10
        
        roi.SetRadiusXYZ(r,r,r)
                
        if slicer.mrmlScene:
          slicer.mrmlScene.AddNode(roi)
          findingNode.SetSegmentationROI(roi)
          currentStudy.SetSegmentationROI(roi)
            
      centered = self.studySelectionWidget.property('centeredSelected')
      roiXYZ = ViewHelper.getSliceNodesCrossingPositionRAS()      
      
      roi = currentStudy.GetSegmentationROI()
      
      if roi != None:
        if centered:
          centerTransform = currentStudy.GetCenteringTransform()
          if centerTransform:
            centerTransformMatrix = centerTransform.GetMatrixTransformToParent()
            roi.SetXYZ(roiXYZ[0]-centerTransformMatrix.GetElement(0,3),roiXYZ[1]-centerTransformMatrix.GetElement(1,3),roiXYZ[2]-centerTransformMatrix.GetElement(2,3))  
        else:
          roi.SetXYZ(roiXYZ)    
  
  def onShowFindingSettingsDialog(self, findingNode):
          
    accepted = False
      
    if findingNode:
      currentReport = self.getCurrentReport()
      if currentReport:
        currentReport.SetUserSelectedFinding(findingNode)
        segROI = findingNode.GetSegmentationROI()
        if (segROI != None) & (self.getCurrentStudy() != None):
          self.getCurrentStudy().SetSegmentationROI(segROI)
          
        if self.findingSettingsDialog == None:
          self.findingSettingsDialog = slicer.modulewidget.qSlicerLongPETCTFindingSettingsDialog(self.parent)
        
        self.findingSettingsDialog.setReportNode(currentReport)
                
        result = self.findingSettingsDialog.exec_()
        
        name = self.findingSettingsDialog.property('findingName')
        typename = self.findingSettingsDialog.property('typeName')
        colorID = self.findingSettingsDialog.property('colorID')
       
        if result == qt.QDialog.Accepted:
          if name:
            findingNode.SetName(name)
          findingNode.SetTypeName(typename)
          findingNode.SetColorID(colorID)
          
          accepted = True
    
    return accepted    
   
    
  def onFindingNodeChanged(self, findingNode):

    currentReport = self.getCurrentReport()
        
    if currentReport:
            
      idx = currentReport.GetIndexOfFinding(findingNode)
      
      if idx == -1:
        applied = self.onShowFindingSettingsDialog(findingNode)
        #       & (findingNode != None):
        if applied:
          currentReport.AddFinding(findingNode)
          currentReport.SetUserSelectedFinding(findingNode)
          findingNode.AddObserver(vtk.vtkCommand.ModifiedEvent, self.findingNodeModified)     
          self.setupFindingNode(currentReport.GetUserSelectedFinding())
          
        else:
          self.findingSelector.disconnect('currentNodeChanged(vtkMRMLNode*)', self.onFindingNodeChanged)
          slicer.mrmlScene.RemoveNode(findingNode)
          currentReport.SetUserSelectedFinding(self.findingSelector.currentNode())
          self.findingSelector.connect('currentNodeChanged(vtkMRMLNode*)', self.onFindingNodeChanged)
          return
          
      else:
        currentReport.SetUserSelectedFinding(findingNode) 
        
    if currentReport.GetUserSelectedFinding():
      self.editorWidget.editLabelMapsFrame.setEnabled(True)
      segROI = currentReport.GetUserSelectedFinding().GetSegmentationROI()
      if (segROI != None) & (self.getCurrentStudy() != None):
        self.getCurrentStudy().SetSegmentationROI(segROI)
        self.updateSegmentationROIPosition()
        
        
  
    else:
      self.editorWidget.editLabelMapsFrame.setEnabled(False)
      if self.getCurrentStudy():
        self.getCurrentStudy().SetSegmentationROI(None)    
    
    self.onManageFindingROIsVisibility()
    
    

  
  def onFindingROIVisibilityChanged(self, visible): 
    currentFinding = self.getCurrentFinding()
    if currentFinding:
      currentROI = currentFinding.GetSegmentationROI()
      if currentROI:
        currentROI.SetVisibility(visible)
     
     
  def onFindingNodeToBeRemoved(self, findingNode):
    currentReport = self.reportSelector.currentNode()
    if currentReport:
      
      for i in range(currentReport.GetSelectedStudiesCount()):
        study = currentReport.GetStudy(i)
        
        seg = findingNode.GetSegmentationForStudy(study)
        if seg:
          ViewHelper.removeSegmentationFromVolume(seg.GetLabelVolumeNode(), findingNode.GetColorID())
          findingNode.RemoveSegmentationForStudy(study)
          slicer.mrmlScene.RemoveNode(seg)
          study.SetSegmentationROI(None)  
          
      
      slicer.mrmlScene.RemoveNode(findingNode.GetSegmentationROI())
      currentReport.RemoveFinding(findingNode)
           
   
  def onManageFindingROIsVisibility(self):
    currentStudy = self.getCurrentStudy()
    currentFinding = self.getCurrentFinding()
    
    if (currentStudy != None) & (currentFinding != None):
      
      nrOfFindings = self.getCurrentReport().GetFindingsCount()
      
      for x in range(0,nrOfFindings,1):
        finding = self.getCurrentReport().GetFinding(x)
        if finding: 
          roi = finding.GetSegmentationROI()
          if roi:
            if (currentFinding != None) & (self.getCurrentReport().GetIndexOfFinding(currentFinding) == x) & (self.analysisCollapsibleButton.property('collapsed') == True):
              roi.SetVisibility(self.findingSelectionWidget.property('roiVisibility'))
            else:
              roi.SetVisibility(0)                   
                      
  def onReportTableStudyClicked(self,index):
    if self.studySliderWidget:
      self.studySliderWidget.changeValue(index)
      
  def onReportTableFindingClicked(self,index):
    if self.findingSelector:
      self.findingSelector.setCurrentNodeIndex(index)
                             
             
  def getCurrentReport(self):
    return self.reportSelector.currentNode()  
  
  def getCurrentStudy(self):
    currentReport = self.getCurrentReport()
    if currentReport:
      return currentReport.GetUserSelectedStudy()
    
    return None    
  
  def getCurrentFinding(self):
    currentReport = self.getCurrentReport()
    if currentReport:
      return currentReport.GetUserSelectedFinding()
    
    return None    
                 
    
  def petDisplayNodeModified(self, caller, event):
    petDisplayNode = caller
    if petDisplayNode:
      self.selectedPETWindow = petDisplayNode.GetWindow()
      self.selectedPETLevel = petDisplayNode.GetLevel()


  def viewNodeModified(self, caller, event):
    
    stdViewNode = ViewHelper.getStandardViewNode()
    compareViewNodes = ViewHelper.getCompareViewNodes()
    
    if caller == stdViewNode:
      if stdViewNode.GetAnimationMode() == slicer.vtkMRMLViewNode.Spin:
        self.studySelectionWidget.setProperty('spinView',True)
      else:
        self.studySelectionWidget.setProperty('spinView',False)
    
    elif caller in compareViewNodes:
      allOn = True
      allOff = True
      for vn in compareViewNodes:
        
        if vn.GetAnimationMode() != slicer.vtkMRMLViewNode.Spin:
          allOn = False
        if vn.GetAnimationMode() != slicer.vtkMRMLViewNode.Off:
          allOff = False
      
        if (allOff == False) & (allOn == False):
          break
      
      if allOn:
        self.analysisSettingsWidget.setProperty('spinView',True)    
      elif allOff:
        self.analysisSettingsWidget.setProperty('spinView',False)  
      
          
  
  def findingNodeModified(self, caller, event):
    findingNode = caller
    
    if findingNode:
      # Update Finding's segmentation ROI name
      findingROI = findingNode.GetSegmentationROI()
      
      if findingROI:
        findingROI.SetName(findingNode.GetName()+"_ROI")
      
      # Update Finding's Segmentation names
      currentReport = self.getCurrentReport()
      
      if currentReport:
        studiesCount = currentReport.GetSelectedStudiesCount()
        
        for i in range(0,studiesCount,1):
          study = currentReport.GetSelectedStudy(i)
          
          if study:
            seg = findingNode.GetSegmentationForStudy(study)
            petVolume = study.GetPETVolumeNode()
            
            if (seg != None) & (petVolume != None):
              seg.SetName(findingNode.GetName()+"_Segmentation_"+petVolume.GetName())        
  
  
  def makeModels(self):
    currentStudy = self.getCurrentStudy()
    currentFinding = self.getCurrentFinding()
    
    if (currentStudy != None) & (currentFinding != None):
     
      currentSeg = currentFinding.GetSegmentationForStudy(currentStudy)
      
      if currentSeg:
        labelVolume = currentSeg.GetLabelVolumeNode()

        #create a temporary model hierarchy for generating models
        tempMH = slicer.vtkMRMLModelHierarchyNode()
        slicer.mrmlScene.AddNode(tempMH)
      
        colorTable = self.getCurrentReport().GetFindingTypesColorTable()
        
        if (labelVolume != None) & (tempMH != None) & (colorTable != None):
          parameters = {}
          parameters['InputVolume'] = labelVolume.GetID()
          parameters['ColorTable'] = colorTable.GetID()
          parameters['ModelSceneFile'] = tempMH.GetID()
          parameters['GenerateAll'] = False
          parameters['StartLabel'] = currentFinding.GetColorID()
          parameters['EndLabel'] = currentFinding.GetColorID()
          parameters['Name'] = labelVolume.GetName() + "_" + currentFinding.GetName()+"_M"

          self.cliModelMaker = slicer.cli.run(slicer.modules.modelmaker, self.cliModelMaker, parameters, wait_for_completion = True)  
          genModelNodes = vtk.vtkCollection()
          tempMH.GetChildrenModelNodes(genModelNodes)

          if genModelNodes.GetNumberOfItems() > 0:
            modelNode = genModelNodes.GetItemAsObject(0)
            if modelNode:
              if modelNode.IsA('vtkMRMLModelNode'):
                hnode = slicer.vtkMRMLHierarchyNode.GetAssociatedHierarchyNode(modelNode.GetScene(), modelNode.GetID())
                if hnode:
                  currentSeg.SetModelHierarchyNode(hnode)
                  modelNode.SetName(labelVolume.GetName() + "_" + currentFinding.GetName()+"_M")
                  if modelNode.GetDisplayNode():
                    modelNode.GetDisplayNode().SetName(labelVolume.GetName() + "_" + currentFinding.GetName()+"_D")
                  hnode.SetName(labelVolume.GetName() + "_" + currentFinding.GetName()+"_H")
                  modelNode.SetHideFromEditors(False)
                  modelNode.SetHideFromEditors(False)
                else:
                  currentSeg.SetModelHierarchyNode(None)
                  slicer.mrmlScene.RemoveNode(modelNode)   
                  slicer.mrmlScene.RemoveNode(hnode)
   
          slicer.mrmlScene.RemoveNode(tempMH.GetDisplayNode())
          slicer.mrmlScene.RemoveNode(tempMH)         
        
              
                
          
        #create current model nodes
        
        
        
        #count = 0
        #while self.cliModelMaker.GetStatusString() != 'Running':
          #count = count + 1
            
        #self.timerMakeModels.start(50)
        
        
  def onUpdateModels(self):
    
    if self.cliModelMaker.GetStatusString() == 'Completed':
      if self.getCurrentStudy():
        if self.getCurrentStudy().GetModelHierarchy():
          self.getCurrentStudy().GetModelHierarchy().Modified()
      
      self.timerMakeModels.stop()   
    
    elif self.cliModelMaker.GetStatusString() == 'CompletedWithErrors':    
      qt.QMessageBox.warning(None, ViewHelper.moduleDialogTitle(),'An error occured during the computation of the models for the segmentation!')
      self.timerMakeModels.stop()
                
        
   
  def calculateSUVs(self):
    
    currentStudy = self.getCurrentStudy()
    currentFinding = self.getCurrentFinding()
        
    if (currentStudy != None) & (currentFinding != None):
         
      instanceUIDs = currentStudy.GetPETVolumeNode().GetAttribute('DICOM.instanceUIDs')
      instanceUID  = instanceUIDs.split(" ",1)[0]
      petDir = slicer.modules.longpetct.logic().GetDirectoryOfDICOMSeries(instanceUID)

      if petDir:
        parameters = {}
        parameters['PETDICOMPath'] = petDir
        parameters['PETVolume'] = currentStudy.GetPETVolumeNode().GetID()
        parameters['VOIVolume'] = currentStudy.GetPETLabelVolumeNode().GetID()
        parameters['ColorTable'] = self.getCurrentReport().GetFindingTypesColorTable().GetID()
      
        self.cliNode = slicer.cli.run(slicer.modules.petstandarduptakevaluecomputation, self.cliNode, parameters, wait_for_completion = True)
        
        if self.cliNode.GetStatusString() == 'Completed':
          labelValues = self.cliNode.GetParameterAsString('OutputLabelValue')
          # values as comma separated string
          max = self.cliNode.GetParameterAsString('SUVMax')
          mean = self.cliNode.GetParameterAsString('SUVMean')
          min = self.cliNode.GetParameterAsString('SUVMin')
      
          splitter = ', '
          labelValues = labelValues.split(splitter)
          # values as string list
          max = max.split(splitter)
          mean = mean.split(splitter)
          min = min.split(splitter)
      
          idx = labelValues.index(str(currentFinding.GetColorID()))
        
          # values as float values
          max = float(max[idx])
          mean = float(mean[idx])
          min = float(min[idx])
      
        #if self.reportTableWidget:
          #self.reportTableWidget.updateSegmentationSUVs(currentStudy,currentFinding,max,mean,min)   
      
          currentSeg = currentFinding.GetSegmentationForStudy(currentStudy)
        
          if currentSeg:
            currentSeg.SetSUVs(max,mean,min)
                     
        elif self.cliNode.GetStatusString() == 'CompletedWithErrors':    
          qt.QMessageBox.warning(None, ViewHelper.moduleDialogTitle(),'An error occured during the computation of the SUV values for the segmentation!')


  def onUpdateSUVTable(self):
    
    currentStudy = self.getCurrentStudy()
    currentFinding = self.getCurrentFinding()
    
    if (currentStudy != None) & (currentFinding != None):
      
      if self.cliNode.GetStatusString() == 'Completed':
      
        self.timerUpdateTable.stop()
        
        
          
      
  
  
           
      
  def onShowFindingsInfoMessageBox(self, findingsCollapsed = False):
    if findingsCollapsed == False:
      if self.findingsInfoMessageBox == None:
        self.findingsInfoMessageBox = ctk.ctkMessageBox()
        self.findingsInfoMessageBox.setModal(True)
        self.findingsInfoMessageBox.setIcon(qt.QMessageBox.Information)
        self.findingsInfoMessageBox.setWindowTitle(ViewHelper.moduleDialogTitle())
        self.findingsInfoMessageBox.setText(ViewHelper.findingInfoMessage())
        self.findingsInfoMessageBox.setProperty('dontShowAgainVisible',True)
        self.findingsInfoMessageBox.setDontShowAgain(False)
      
      self.findingsInfoMessageBox.setVisible(True)
   
  def onFindingSelectionHelp(self):
    ViewHelper.showInformationMessageBox(ViewHelper.moduleDialogTitle(), ViewHelper.findingInfoMessage())


  def manageCollapsibleButtonsAbility(self, caller, event):
    studies = False
    findings = False
    analysis = False
    
    currentReport = self.getCurrentReport()
    
    if currentReport:
      studies = True
    
      if currentReport.GetSelectedStudiesCount() > 0:
        findings = True
        
        if currentReport.GetFindingsCount() > 0:
          for i in range(currentReport.GetFindingsCount()):
            finding = currentReport.GetFinding(i)
            if finding:
              if finding.GetSegmentationsCount() > 0:
                analysis = True      
                      
    self.studiesCollapsibleButton.setProperty('enabled',studies)
    self.findingsCollapsibleButton.setProperty('enabled',findings)  
    self.analysisCollapsibleButton.setProperty('enabled',analysis) 
     
  def observeReportNode(self, reportNode):
    if reportNode:
      notObserved = True
      for id in self.reportObserverIDs:
        if reportNode.HasObserver(id, vtk.vtkCommand.ModifiedEvent) == 1:
          notObserved = False
          break
    
      if notObserved:
        observerID = reportNode.AddObserver(vtk.vtkCommand.ModifiedEvent, self.manageCollapsibleButtonsAbility)
        self.reportObserverIDs.append(observerID)
    
    self.manageCollapsibleButtonsAbility(self, vtk.vtkCommand.ModifiedEvent)

  
  def onSwitchToQualitativeView(self, analysisCollapsed):
   
    if analysisCollapsed:
      self.findingSelectionWidget.setProperty('roiVisibility',self.findingROIVisiblityBackup)
      lm = slicer.app.layoutManager()
     
      self.setVolumeRendering(self.studySelectionWidget.property('volumeRendering')) 
      if lm:
        lm.setLayout(slicer.vtkMRMLLayoutNode.SlicerLayoutFourUpView)
      
      
      self.onManageFindingROIsVisibility()
      #self.setVisibleModelHierarchy(self.getCurrentReport().GetIndexOfSelectedStudy(self.getCurrentStudy()))  
         

    else:
      
      self.setVolumeRendering(self.analysisSettingsWidget.property('volumeRendering'))
      
      roiNodes = slicer.mrmlScene.GetNodesByClass('vtkMRMLAnnotationROINode')
      for i in range(roiNodes.GetNumberOfItems()):
        roi = roiNodes.GetItemAsObject(i)  
        if roi: 
          roi.SetVisibility(0)
      
      self.showQualitativeView()
      
      
  def setVisibleModelHierarchy(self, index):
    currentReport = self.getCurrentReport()
    
    if currentReport:
      for i in range(currentReport.GetSelectedStudiesCount()):
        modelHierarchy = currentReport.GetSelectedStudy(i).GetModelHierarchy()
        if modelHierarchy:
          
          slicer.vtkMRMLModelHierarchyLogic.SetChildrenVisibility(modelHierarchy, i == index)
          
          if modelHierarchy.GetDisplayNode():
            modelHierarchy.GetDisplayNode().SetVisibility(i == index)
              
        
                   
      #self.manageVRDisplayNodesVisibility(None) 
  def showQualitativeView(self):
    
    currentLayoutID = ViewHelper.updateQualitativeViewLayout(self.getCurrentReport().GetStudiesSelectedForAnalysisCount())
    
    ViewHelper.removeObserversFromCompareViewNodes(self.compareViewNodeObserverIDs)
    del self.compareViewNodeObserverIDs[:]
    viewNodes = ViewHelper.getCompareViewNodes()   
    
    
    for i in range(self.getCurrentReport().GetStudiesSelectedForAnalysisCount()):
      study = self.getCurrentReport().GetStudySelectedForAnalysis(i)
      
      viewNode = viewNodes[self.getCurrentReport().GetIndexOfStudySelectedForAnalysis(study)]
      
      if study:
        #mh = study.GetModelHierarchy()
        #if mh:
          #nodes = vtk.vtkCollection()
          #mh.GetChildrenModelNodes(nodes)
          #for j in range(nodes.GetNumberOfItems()):
            #node = nodes.GetItemAsObject(j)
            #displayNode = node.GetDisplayNode()
            #if displayNode:
              #displayNode.RemoveAllViewNodeIDs()
              #displayNode.AddViewNodeID(ViewHelper.getStandardViewNode().GetID())
              #displayNode.AddViewNodeID(viewNode.GetID())
              #displayNode.SetVisibility(True)
        #self.setVisibleModelHierarchy(-1)      
        vrdn = study.GetVolumeRenderingDisplayNode()
        if vrdn:
          
          vrdn.AddViewNodeID(viewNode.GetID())
          
          
          if self.analysisSettingsWidget.property('spinView'):
            viewNode.SetAnimationMode(slicer.vtkMRMLViewNode.Spin)
          else:
            viewNode.SetAnimationMode(slicer.vtkMRMLViewNode.Off)
          
          observerID = viewNode.AddObserver(vtk.vtkCommand.ModifiedEvent, self.viewNodeModified) 
          self.compareViewNodeObserverIDs.append(observerID)
            
          if self.qualitativeViewLastID != currentLayoutID:
            ViewHelper.SetForegroundOpacity(0.6, True)
            self.qualitativeViewLastID = currentLayoutID
          
          
            
          ViewHelper.SetCompareBgFgLblVolumes(self.getCurrentReport().GetIndexOfStudySelectedForAnalysis(study),study.GetCTVolumeNode().GetID(),study.GetPETVolumeNode().GetID(),study.GetPETLabelVolumeNode().GetID(),True)
          
          
     
       
   #lm = slicer.app.layoutManager()
   
   #if analysisCollapsed:
     #if lm:
      #lm.setLayout(slicer.vtkMRMLLayoutNode.SlicerLayoutFourUpView)
