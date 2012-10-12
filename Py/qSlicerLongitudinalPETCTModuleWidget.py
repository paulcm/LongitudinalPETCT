from __main__ import vtk, qt, ctk, slicer

from Editor import EditorWidget
from SlicerLongitudinalPETCTModuleViewHelper import SlicerLongitudinalPETCTModuleViewHelper as ViewHelper
from SlicerLongitudinalPETCTModuleSegmentationHelper import SlicerLongitudinalPETCTModuleSegmentationHelper as SegmentationHelper

import sys as SYS

import thread as Thread
import time
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
      
      
  def setup(self): 
    # Instantiate and connect widgets ...

    # switch to FourUp Layout
    lm = slicer.app.layoutManager()
    if lm:
      lm.setLayout(slicer.vtkMRMLLayoutNode.SlicerLayoutFourUpView) # two over two
    
    
    self.roiEndPlacementEventObserverID = None
    self.roiPlacementNodeAddedEventObserverID = None
    self.lastAddedROINode = None
    self.qualitativeViewLastID = -1
    self.reportObserverIDs = []
    self.tempObserverEditorTag = None
    self.viewNodeObserverID = -1
    self.segmentationROIOldPosition = [0.,0.,0.]
    #self.tempCroppedLblVolObserverTag = -1
    
    self.chartArrayNodes = []
    self.chartNode = None

    self.logic  = slicer.modules.longitudinalpetct.logic()
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
    
    self.showCompareAll = False
    
    self.unsupportedVolRendMessageBox = None
    self.segmentationColorChangeMessageBox = None

    
    self.findingROIVisiblityBackup = True
       
    #self.opacityFunction = vtk.vtkPiecewiseFunction()
    # instanciate all collapsible buttons
    self.reportsCollapsibleButton = ctk.ctkCollapsibleButton()
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
      
    self.reportSelectionWidget = slicer.modulewidget.qSlicerLongitudinalPETCTReportSelectionWidget()    
    self.reportSelectionWidget.setMRMLScene(slicer.mrmlScene)
    self.reportSelector = self.reportSelectionWidget.mrmlNodeComboBoxReport()
      
    reportsLayout.addWidget(self.reportSelectionWidget)
    # Studies Collapsible button    
    self.studiesCollapsibleButton.text = "Study Selection"
    self.studiesCollapsibleButton.setProperty('collapsed',True)
    
    self.layout.addWidget(self.studiesCollapsibleButton)

    studiesLayout = qt.QVBoxLayout(self.studiesCollapsibleButton)

    self.studySelectionWidget = slicer.modulewidget.qSlicerLongitudinalPETCTStudySelectionWidget()    
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
    self.studySelectionWidget.connect('volumeRenderingToggled(bool)',self.manageVolumeRenderingVisibility)
    self.studySelectionWidget.connect('opacityPowChanged(double)',self.onSetOpacityPowForCurrentStudy)
    self.studySelectionWidget.connect('showStudiesCentered(bool)',self.onStudySelectionWidgetShowStudiesCentered)

    # Findings Collapsible button
    self.findingsCollapsibleButton.text = "Findings"
    self.findingsCollapsibleButton.setProperty('collapsed',True)    
     
    self.layout.addWidget(self.findingsCollapsibleButton)
    
    findingsLayout = qt.QVBoxLayout(self.findingsCollapsibleButton)
      
    self.findingSelectionWidget = slicer.modulewidget.qSlicerLongitudinalPETCTFindingSelectionWidget()    
    self.findingSelectionWidget.setMRMLScene(slicer.mrmlScene)
    self.findingSelectionWidget.setReportNode(self.getCurrentReport())
    self.findingSelectionWidget.connect('roiVisibilityChanged(bool)', self.onFindingROIVisibilityChanged)
    self.findingSelectionWidget.connect('placeROIChecked(bool)', self.onActivateROIPlacement)
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
    self.editorWidget = EditorWidget(editorWidgetParent, False)
    self.editorWidget.setup()
    #self.editorWidget.toolsColor.frame.setVisible(False)
    
    
    self.editorWidget.editLabelMapsFrame.setText("Edit Segmentation")
    self.editorWidget.editLabelMapsFrame.setEnabled(False)
    
    findingsLayout.addWidget(self.findingSelectionWidget)
    
    if self.reportSelector.currentNode():
      if self.reportSelector.currentNode().GetUserSelectedFinding():
        if self.reportSelector.currentNode().GetUserSelectedFinding().GetSegmentationROI():
          self.editorWidget.editLabelMapsFrame.setEnabled(True)
        
    self.editorWidget.editLabelMapsFrame.connect('contentsCollapsed(bool)', self.onEditorCollapsed)      
    
    self.findingSelectionWidget.setEditorWidget(editorWidgetParent)

    #Analysis Collapsible Button
    self.analysisCollapsibleButton.text = "Analysis"
    self.analysisCollapsibleButton.setProperty('collapsed',True)
    self.analysisCollapsibleButton.connect('contentsCollapsed(bool)', self.onSwitchToAnalysis)      
    
    self.layout.addWidget(self.analysisCollapsibleButton)

    analysisLayout = qt.QVBoxLayout(self.analysisCollapsibleButton)
    self.analysisSettingsWidget = slicer.modulewidget.qSlicerLongitudinalPETCTAnalysisSettingsWidget()    
    self.analysisSettingsWidget.setReportNode(self.getCurrentReport())
    
    self.analysisSettingsWidget.connect('qualitativeAnalysisChecked(bool)', self.showQualitativeView)
    self.analysisSettingsWidget.connect('quantitativeAnalysisChecked(bool)', self.showQuantitativeView)
    self.analysisSettingsWidget.connect('studySelectedForAnalysis(int, bool)', self.onStudyForAnalysisChanged)
    self.analysisSettingsWidget.connect('volumeRenderingToggled(bool)',self.manageVolumeRenderingVisibility)
    self.analysisSettingsWidget.connect('spinViewToggled(bool)',ViewHelper.spinCompareViewNodes)
    
    analysisLayout.addWidget(self.analysisSettingsWidget)  

    # Add vertical spacer
    self.layout.addStretch()
    
    self.separator = qt.QFrame()
    self.separator.setFrameStyle(qt.QFrame.HLine | qt.QFrame.Sunken)
    self.layout.addWidget(self.separator)

    # Add Report Table
    self.reportTableWidget = slicer.modulewidget.qSlicerLongitudinalPETCTReportTableWidget()
    self.reportTableWidget.setColumnSelectableOn()
    self.reportTableWidget.setReportNode(self.getCurrentReport())
    self.reportTableWidget.connect('studyClicked(int)',self.changeSelectedStudy)
    self.reportTableWidget.connect('findingClicked(int)',self.findingSelector.setCurrentNodeIndex)                 
    self.layout.addWidget(self.reportTableWidget)  
    ViewHelper.SetForegroundOpacity(0.6)       
    self.observeReportNode(self.reportSelector.currentNode())
    
  
  def onEditorCollapsed(self,collapsed):
    self.findingSelectionWidget.setProperty('selectionEnabled',collapsed)    
    self.onEnterEditMode( collapsed != True )
    self.findingSelectionWidget.hideAddButton(collapsed)
    
    self.manageCollapsibleButtonsAbility(self, vtk.vtkCommand.ModifiedEvent)
    
 
  def onCollapseEditor(self):
    self.editorWidget.editLabelMapsFrame.setProperty('collapsed', True)
 
       
  def changeSelectedStudy(self, newIndex):
    currentReport = self.getCurrentReport()
    
    # update model/logic
    if currentReport:
      study = currentReport.GetSelectedStudy(newIndex)
      currentReport.SetUserSelectedStudy(study)      
    
      currentFinding = self.getCurrentFinding()
      currentStudy = self.getCurrentStudy()
    
      if currentStudy:
        self.studySelectionWidget.selectStudyInRow(currentReport.GetIndexOfStudy(study))
        
      if currentFinding:
        currentSegROI = currentFinding.GetSegmentationROI()
      
        if (currentStudy != None) & ( currentSegROI != None):
          currentStudy.SetSegmentationROI(currentSegROI)
          self.updateSegmentationROIPosition()
          
        elif study:
          currentStudy.SetSegmentationROI(None)
    
    # update view
    self.updateBgFgToUserSelectedStudy(currentStudy)
    self.manageVolumeRenderingVisibility()
    
    self.manageModelsVisibility(self.isQuantitativeViewActive()) 
    
    if self.isQuantitativeViewActive():
      self.updateQuantitativeView(currentReport.GetUserSelectedFinding())    
                
          
       
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

      
        viewNode = ViewHelper.getStandardViewNode()
        if viewNode.GetBoxVisible() != 0:
          viewNode.SetBoxVisible(0)
        
        
        if selectedStudy.GetVolumeRenderingDisplayNode() == None:
          vrDisplayNode = self.vrLogic.CreateVolumeRenderingDisplayNode()
          if (vrDisplayNode != None) & (viewNode != None):
            vrDisplayNode.SetCroppingEnabled(0)
            pow = self.studySelectionWidget.property('opacityPow')
            vrDisplayNode.SetAttribute("OpacityPow",str(pow))
          
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
            vrDisplayNode.AddViewNodeID(viewNode.GetID())
            self.updateOpacityPow(vrDisplayNode, pow)
      
        if viewNode:
          viewNode.InvokeEvent(slicer.vtkMRMLViewNode.ResetFocalPointRequestedEvent)
          viewNode.Modified()
        #self.onUpdateVolumeRendering(selectedStudy.GetPETVolumeNode())
        self.studySelectionWidget.selectStudyInRow(idx)       
        self.changeSelectedStudy(currentReport.GetIndexOfSelectedStudy(selectedStudy))
      
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
            if vrDisplayNode.IsViewNodeIDPresent(viewNode.GetID()):
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
      self.changeSelectedStudy(currentReport.GetIndexOfSelectedStudy(lastSelectedStudy))   
      
      
  def onCurrentReportChanged(self, reportNode):
    
    self.observeReportNode(reportNode)
    
    self.logic.SetSelectedReportNode = reportNode
    self.studySelectionWidget.setReportNode(reportNode)
    self.studySelectionWidget.setReportNode(reportNode)
    self.analysisSettingsWidget.setReportNode(reportNode)
    #self.findingSelectionWidget.setReportNode(reportNode)
    self.reportTableWidget.setReportNode(reportNode)
    
    if reportNode:
      
      #self.manageVRDisplayNodesVisibility(reportNode.GetUserSelectedStudy())
      
      #if reportNode.GetUserSelectedStudy():
        #self.onUpdateVolumeRendering(reportNode.GetUserSelectedStudy().GetPETVolumeNode())
      #else:
        #self.onUpdateVolumeRendering(None)
    
      self.updateBgFgToUserSelectedStudy(reportNode.GetUserSelectedStudy())
    
    else:
     # self.manageVRDisplayNodesVisibility(None)
      self.updateBgFgToUserSelectedStudy(None)  
      
    self.manageVolumeRenderingVisibility()
    
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
        self.updateOpacityPow(vrdn,pow)
          
  
  def updateOpacityPow(self, vrdn, pow):
    if vrdn:
      volNode = vrdn.GetVolumeNode()
      propNode = vrdn.GetVolumePropertyNode()
      
      if volNode:
        volNodeDisplNode = volNode.GetScalarVolumeDisplayNode()
        if volNodeDisplNode:
          window = volNodeDisplNode.GetWindow()
          if window:
            opacityFunction = ViewHelper.opacityPowerFunction(window,pow,10.)
            if (opacityFunction!= None) & (propNode != None):
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
        self.manageVolumeRenderingVisibility()
        #self.manageVRDisplayNodesVisibility(currentStudy)
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
    
      self.tempLabelVol.Copy(currentStudy.GetPETLabelVolumeNode())
      self.tempLabelVol.SetName("LongitudinalPETCT_CroppedLabelVolume")
      
      cropLogic = slicer.modules.cropvolume.logic()
      
      # temporary ROI in order to not invoke modified events with original roi
      croppingROI = slicer.vtkMRMLAnnotationROINode()
      croppingROI.Copy(currentFinding.GetSegmentationROI())
      
      cropLogic.CropVoxelBased(croppingROI,currentStudy.GetPETVolumeNode(),self.tempCroppedVol)
      cropLogic.CropVoxelBased(croppingROI,currentStudy.GetPETLabelVolumeNode(),self.tempLabelVol)
      
      #///
      self.tempCroppedVol.SetAndObserveTransformNodeID(None)
      self.tempLabelVol.SetAndObserveTransformNodeID(None)
            
      currentFinding.GetSegmentationROI().SetAndObserveTransformNodeID(None)
      
      #volLogic  = slicer.modules.volumes.logic()    
    
      #createdLabelVol = volLogic.CreateLabelVolume(slicer.mrmlScene, self.tempCroppedVol,'TempLabelVolume')
   

      #if self.tempLabelVol.GetScalarVolumeDisplayNode():
        #slicer.mrmlScene.RemoveNode(tempLabelVol.GetScalarVolumeDisplayNode())
      #if self.tempLabelVol.GetDisplayNode():
        #slicer.mrmlScene.RemoveNode(self.tempLabelVol.GetDisplayNode())
      #self.tempLabelVol.Copy(createdLabelVol)   
      #self.tempLabelVol.SetName("LongitudinalPETCT_CroppedLabelVolume")
      
      #slicer.mrmlScene.RemoveNode(createdLabelVol)
      
      propagate = caller == self
      ViewHelper.SetRYGBgFgLblVolumes(self.tempCroppedVol.GetID(),None,self.tempLabelVol.GetID(),propagate)  
    
    self.tempLabelVol.GetDisplayNode().SetAndObserveColorNodeID(self.getCurrentReport().GetFindingTypesColorTable().GetID())
        

  
  def onEnterEditMode(self,enter):
    
    self.editorWidget.toolsBox.buttons['GrowCutEffect'].setVisible(False)
    self.editorWidget.toolsBox.buttons['ChangeLabelEffect'].setVisible(False)
    self.editorWidget.toolsBox.buttons['MakeModelEffect'].setVisible(False)
    
    
    currentStudy = self.getCurrentStudy()
    currentFinding = self.getCurrentFinding()
      
    if (currentStudy != None) & (currentFinding != None) & (enter == True):
     
      self.reportsCollapsibleButton.setProperty('enabled',False)
      self.studiesCollapsibleButton.setProperty('enabled',False)
      self.reportTableWidget.setProperty('enabled',False)
     
      self.onSegmentationROIModified(self, slicer.vtkMRMLAnnotationROINode.ValueModifiedEvent)
      
      studySeg = currentFinding.GetSegmentationForStudy(currentStudy)
      
      if studySeg != None:
        SegmentationHelper.pasteFromMainToCroppedLabelVolume(studySeg.GetLabelVolumeNode(), self.tempLabelVol, currentFinding.GetColorID())  
    

      self.editorWidget.setMasterNode(self.tempCroppedVol) 
      self.editorWidget.setMergeNode(self.tempLabelVol)      
      
      self.editorWidget.editUtil.getParameterNode().SetParameter('storedLabel',"")
      self.editorWidget.editUtil.getParameterNode().SetParameter('label',str(currentFinding.GetColorID()))
        
    
      self.editorWidget.enter()  
    
      self.tempObserverEditorTag = currentFinding.GetSegmentationROI().AddObserver(vtk.vtkCommand.ModifiedEvent, self.onSegmentationROIModified)      
      #self.tempCroppedLblVolObserverTag = self.tempLabelVol.GetImageData().AddObserver(vtk.vtkCommand.ModifiedEvent, self.pasteFromCroppedToMainLblVolume)    

    elif enter == False:
      
      self.editorWidget.toolsBox.buttons['GrowCutEffect'].setVisible(True)
      self.editorWidget.toolsBox.buttons['ChangeLabelEffect'].setVisible(True)
      self.editorWidget.toolsBox.buttons['MakeModelEffect'].setVisible(True)
      self.editorWidget.exit()   
      
          
      pasted = self.pasteFromCroppedToMainLblVolume(self, vtk.vtkCommand.ModifiedEvent)
      studySeg = currentFinding.GetSegmentationForStudy(currentStudy)

      if studySeg:
        #self.calculateSUVs()
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
      
        
        qt.QTimer.singleShot(0,self.calculateSUVs)
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
      self.reportTableWidget.setProperty('enabled',True)
                 
  def pasteFromCroppedToMainLblVolume(self, caller, event):
    

    currentStudy = self.getCurrentStudy()
    currentFinding = self.getCurrentFinding()
    
    pasted = False
    
    if (currentStudy != None) & (currentFinding != None):
      segmentationROI = currentFinding.GetSegmentationROI()
      
      if segmentationROI:
        segmentationROI.RemoveObserver(self.tempObserverEditorTag)
        self.tempObserverEditorTag = None
     
      studySeg = None
     
      if currentFinding.GetSegmentationForStudy(currentStudy) == None:
        a = time.time()
        studySeg = slicer.mrmlScene.CreateNodeByClass('vtkMRMLLongitudinalPETCTSegmentationNode')
        studySeg.SetReferenceCount(studySeg.GetReferenceCount()-1) 
        
        slicer.mrmlScene.AddNode(studySeg)
        
        studySeg.SetLabelVolumeNode(currentStudy.GetPETLabelVolumeNode())
        
        currentFinding.AddSegmentationForStudy(currentStudy,studySeg)
        b = time.time()
        print "DURATION OF SEGMENTATION CREATION: " + str(b-a)+  " s" 
      else:
        studySeg = currentFinding.GetSegmentationForStudy(currentStudy)        
      
      a = time.time()
      SegmentationHelper.removeSegmentationFromVolume(studySeg.GetLabelVolumeNode(), currentFinding.GetColorID())       
      b = time.time()
      print "DURATION OF SEGMENTATION REMOVAL: " + str(b-a)+  " s" 
      pasted = SegmentationHelper.pasteFromCroppedToMainLabelVolume(self.tempLabelVol, studySeg.GetLabelVolumeNode(), currentFinding.GetColorID())    
    
      if segmentationROI:
        xyz = [0.,0.,0.]
        radius = [0.,0.,0.]
        
        segmentationROI.GetXYZ(xyz)
        segmentationROI.GetRadiusXYZ(radius)
        
        studySeg.SetROIxyz(xyz)
        studySeg.SetROIRadius(radius)
        
        
      if SegmentationHelper.containsSegmentation(studySeg.GetLabelVolumeNode(),currentFinding.GetColorID()) == False:
        currentFinding.RemoveSegmentationForStudy(currentStudy)
        
        mh = studySeg.GetModelHierarchyNode()
        if mh:
          mn = mh.GetModelNode()
          if mn:
            mdn = mn.GetModelDisplayNode()
            slicer.mrmlScene.RemoveNode(mdn)
          slicer.mrmlScene.RemoveNode(mn)
              
        slicer.mrmlScene.RemoveNode(studySeg)
        
    
    #if self.tempCroppedLblVolObserverTag != -1:      
      #self.tempLabelVol.GetImageData().RemoveObserver(self.tempCroppedLblVolObserverTag) 
    
    
            
    return pasted 
  
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
          self.findingSettingsDialog = slicer.modulewidget.qSlicerLongitudinalPETCTFindingSettingsDialog(self.parent)
          
        self.findingSettingsDialog.setReportNode(currentReport)
                
        result = self.findingSettingsDialog.exec_()
        
        accepted = result == qt.QDialog.Accepted
    
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
          
        else:
          self.findingSelector.disconnect('currentNodeChanged(vtkMRMLNode*)', self.onFindingNodeChanged)
          slicer.mrmlScene.RemoveNode(findingNode)

          currentReport.SetUserSelectedFinding(self.findingSelector.currentNode())

          self.findingSelector.connect('currentNodeChanged(vtkMRMLNode*)', self.onFindingNodeChanged)
          
          return
          
      else:
        currentReport.SetUserSelectedFinding(findingNode) 
        
    if currentReport.GetUserSelectedFinding():
      
      segROI = currentReport.GetUserSelectedFinding().GetSegmentationROI()
      
      if (segROI != None) & (self.getCurrentStudy() != None):
        self.editorWidget.editLabelMapsFrame.setEnabled(True)
        self.getCurrentStudy().SetSegmentationROI(segROI)
        self.updateSegmentationROIPosition()
      else:
        self.editorWidget.editLabelMapsFrame.setEnabled(False)
        
      
      if self.isQuantitativeViewActive():
        self.updateQuantitativeView(currentReport.GetUserSelectedFinding())    
        
  
    else:
      self.editorWidget.editLabelMapsFrame.setEnabled(False)
      if self.getCurrentStudy():
        self.getCurrentStudy().SetSegmentationROI(None)    
    
    self.onManageFindingROIsVisibility()
    
    if self.isQualitativeViewActive() | self.isQuantitativeViewActive():
      self.manageModelsVisibility(self.isQuantitativeViewActive())
    

  
  def onFindingROIVisibilityChanged(self, visible): 
    currentFinding = self.getCurrentFinding()
    if currentFinding:
      currentROI = currentFinding.GetSegmentationROI()
      if currentROI:
        if self.tempObserverEditorTag:
          currentROI.RemoveObserver(self.tempObserverEditorTag)
        
        currentROI.SetVisibility(visible)
        
        if self.tempObserverEditorTag:
          self.tempObserverEditorTag = currentROI.AddObserver(vtk.vtkCommand.ModifiedEvent, self.onSegmentationROIModified)      
        
     
     
  def onFindingNodeToBeRemoved(self, findingNode):
    currentReport = self.reportSelector.currentNode()
    if currentReport:
            
      for i in range(currentReport.GetSelectedStudiesCount()):
        study = currentReport.GetStudy(i)
        
        seg = findingNode.GetSegmentationForStudy(study)
        if seg:
          SegmentationHelper.removeSegmentationFromVolume(seg.GetLabelVolumeNode(), findingNode.GetColorID())
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
    a = time.time()
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
                    modelNode.GetDisplayNode().AddViewNodeID(ViewHelper.getStandardViewNode().GetID())
                  hnode.SetName(labelVolume.GetName() + "_" + currentFinding.GetName()+"_H")
                  modelNode.SetHideFromEditors(False)
                  modelNode.SetHideFromEditors(False)
                else:
                  currentSeg.SetModelHierarchyNode(None)
                  slicer.mrmlScene.RemoveNode(modelNode)   
                  slicer.mrmlScene.RemoveNode(hnode)
   
          slicer.mrmlScene.RemoveNode(tempMH.GetDisplayNode())
          slicer.mrmlScene.RemoveNode(tempMH)         
        
    b = time.time()
    print "DURATION OF MAKE MODELS: " + str(b-a)+  "s"            
        
   
  def calculateSUVs(self):
    a = time.time()
    
    currentStudy = self.getCurrentStudy()
    currentFinding = self.getCurrentFinding()
        
    if (currentStudy != None) & (currentFinding != None):
         
      instanceUIDs = currentStudy.GetPETVolumeNode().GetAttribute('DICOM.instanceUIDs')
      instanceUID  = instanceUIDs.split(" ",1)[0]
      petDir = slicer.modules.longitudinalpetct.logic().GetDirectoryOfDICOMSeries(instanceUID)

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

    b = time.time()
    print "DURATION OF SUV COMPUTATION: " + str(b-a)+  "s"         
        
      
  def manageCollapsibleButtonsAbility(self, caller, event):
    reports = False
    studies = False
    findings = False
    analysis = False
    
    currentReport = self.getCurrentReport()
    
    notInEditorMode = self.editorWidget.editLabelMapsFrame.property('collapsed')
    
    if notInEditorMode:
      reports = True      
    
    if (currentReport != None) & notInEditorMode:
      studies = True
    
      if currentReport.GetSelectedStudiesCount() > 0:
        findings = True
        
        if currentReport.GetFindingsCount() > 0:
          for i in range(currentReport.GetFindingsCount()):
            finding = currentReport.GetFinding(i)
            if finding:
              if (finding.GetSegmentationsCount() > 0) & notInEditorMode:
                analysis = True      
                   
    self.reportsCollapsibleButton.setProperty('enabled',reports)                  
    self.studiesCollapsibleButton.setProperty('enabled',studies)
    self.findingsCollapsibleButton.setProperty('enabled',findings | (notInEditorMode == False))  
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

      
  
  def manageVolumeRenderingVisibility(self):
    currentReport = self.getCurrentReport()
    currentStudy = self.getCurrentStudy()
        
    stdVolRen = self.studySelectionWidget.property('volumeRendering')
    stdSpin = self.studySelectionWidget.property('spinView')
    
    anaVolRen = self.analysisSettingsWidget.property('volumeRendering') 
    anaSpin = self.analysisSettingsWidget.property('spinView')
    
    viewNode = ViewHelper.getStandardViewNode()
    
    if currentReport:
    # remove viewnode from all studies and set them invisible
      for i in xrange(currentReport.GetStudiesCount()):
        study = currentReport.GetStudy(i)
        if study:
          vrdn = study.GetVolumeRenderingDisplayNode()
          if vrdn:
            if vrdn.IsViewNodeIDPresent(viewNode.GetID()):
              vrdn.RemoveViewNodeID(viewNode.GetID())
            vrdn.SetVisibility(False)    
    
      # collect all VR volumedisplaynodes which should be made visible
      for i in xrange(currentReport.GetSelectedStudiesCount()):
        study = currentReport.GetSelectedStudy(i)  
        if study:
          vrdn = study.GetVolumeRenderingDisplayNode()
          if self.isStandardViewActive() & (study == currentStudy):
            vrdn.AddViewNodeID(viewNode.GetID())
            pow = None
            pow = float(vrdn.GetAttribute("OpacityPow"))
            if pow:
              self.studySelectionWidget.setProperty('opacityPow',pow)
              viewNode.Modified()
          
          if (self.isStandardViewActive() & stdVolRen & (study == currentStudy)) | ( (self.isStandardViewActive() == False) & anaVolRen):
            vrdn.SetVisibility(True)
                      
      if currentReport.GetSelectedStudiesCount() > 0:
        viewNode.SetAxisLabelsVisible(True & stdVolRen)   
        ViewHelper.spinStandardViewNode(stdSpin & stdVolRen)

      else:
        viewNode.SetAxisLabelsVisible(False)
        ViewHelper.spinStandardViewNode(False)
    
      if self.isQualitativeViewActive() | self.isQuantitativeViewActive():
        ViewHelper.spinCompareViewNodes(anaSpin & anaVolRen)
      
        compareViewNodes = ViewHelper.getCompareViewNodes()
        for j in range(len(compareViewNodes)):
          if (j < currentReport.GetStudiesSelectedForAnalysisCount()) & (anaVolRen == True):
            compareViewNodes[j].SetAxisLabelsVisible(True)
          else:
            compareViewNodes[j].SetAxisLabelsVisible(False)       
          
  
  def manageModelsVisibility(self, findingRestricted = False):
    currentReport = self.getCurrentReport()
    currentStudy = self.getCurrentStudy()

    if currentStudy:
      for i in range(currentReport.GetFindingsCount()):
        finding = currentReport.GetFinding(i)
        for j in range(currentReport.GetSelectedStudiesCount()):
          study = currentReport.GetSelectedStudy(j)
          
          seg = finding.GetSegmentationForStudy(study)
          if seg:
            mh = seg.GetModelHierarchyNode()
            if mh:
              mn = mh.GetModelNode()
              if mn:
                mdn = mn.GetModelDisplayNode()
                if mdn:
                  stdViewShow = self.isStandardViewActive() & (study == currentStudy)
                  qualViewShow = self.isQualitativeViewActive()
                  quantViewShow = self.isQuantitativeViewActive() & ( (findingRestricted == False) | ( (findingRestricted == True) &  (finding == self.getCurrentFinding()))) 
                  if (seg.GetModelVisible() == True) & (stdViewShow | qualViewShow | quantViewShow):
                    mdn.SetVisibility(True)
                  else:
                    mdn.SetVisibility(False)
   
   
  
  def manageVolumeRenderingCompareViewNodeIDs(self):
    currentReport = self.getCurrentReport()
    
    compareViewNodes = ViewHelper.getCompareViewNodes()
    qualView = self.analysisCollapsibleButton.property('collapsed') != True
    
    if currentReport:
      for i in range(currentReport.GetSelectedStudiesCount()):
        study = currentReport.GetSelectedStudy(i)  
       
        vrdn = study.GetVolumeRenderingDisplayNode()
        if vrdn:
          for cvn in compareViewNodes:
            if vrdn.IsViewNodeIDPresent(cvn.GetID()):
              vrdn.RemoveViewNodeID(cvn.GetID())
              
            id = currentReport.GetIndexOfStudySelectedForAnalysis(study)
            if (id >= 0) & (id < len(compareViewNodes)) & (self.isQualitativeViewActive() | self.isQuantitativeViewActive()):
              vrdn.AddViewNodeID(compareViewNodes[id].GetID())
  
  def isStandardViewActive(self):
    return (self.isQualitativeViewActive() == False) & (self.isQuantitativeViewActive() == False)                
  
  def isQualitativeViewActive(self):
    return (self.analysisCollapsibleButton.property('collapsed') != True) & self.analysisSettingsWidget.property('qualitativeChecked')
         
  def isQuantitativeViewActive(self):
    return (self.analysisCollapsibleButton.property('collapsed') != True) & self.analysisSettingsWidget.property('quantitativeChecked')
         
                    
  def manageModelDisplayCompareViewNodeIDs(self):
    
    currentReport = self.getCurrentReport()
    compareViewNodes = ViewHelper.getCompareViewNodes()
    
    if currentReport:
      for i in range(currentReport.GetFindingsCount()):
        finding = currentReport.GetFinding(i)
        for j in range(currentReport.GetSelectedStudiesCount()):
          study = currentReport.GetSelectedStudy(j)
          
          seg = finding.GetSegmentationForStudy(study)
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
                  
                  id = currentReport.GetIndexOfStudySelectedForAnalysis(study)
                  if (id >= 0) & (id < len(compareViewNodes)) & (self.isQualitativeViewActive() | self.isQuantitativeViewActive()):
                    mdn.AddViewNodeID(compareViewNodes[id].GetID())         
    
  
  def onStudyForAnalysisChanged(self):
    if self.isQualitativeViewActive():
      self.showQualitativeView()
    if self.isQuantitativeViewActive():
      self.showQuantitativeView()
                           
  
  def onSwitchToAnalysis(self, analysisCollapsed):
      
    if analysisCollapsed:
      self.findingSelectionWidget.setProperty('roiVisibility',self.findingROIVisiblityBackup)
      
      lm = slicer.app.layoutManager()
      if lm:
        lm.setLayout(slicer.vtkMRMLLayoutNode.SlicerLayoutFourUpView)
      
      self.onManageFindingROIsVisibility()
      
      self.manageVolumeRenderingCompareViewNodeIDs()
      self.manageVolumeRenderingVisibility()
      self.manageModelDisplayCompareViewNodeIDs()
      self.manageModelsVisibility()
      
      self.reportTableWidget.setColumnSelectableOn() 

    else:
    
      #self.setVolumeRendering(self.analysisSettingsWidget.property('volumeRendering'))
      roiNodes = ViewHelper.getROINodesCollection()
      for i in range(roiNodes.GetNumberOfItems()):
        roi = roiNodes.GetItemAsObject(i)  
        if roi: 
          roi.SetVisibility(0)
    
      if self.analysisSettingsWidget.property('qualitativeChecked'):
        self.showQualitativeView()      
      
      elif self.analysisSettingsWidget.property('quantitativeChecked'):  
        self.showQuantitativeView()           
      #self.manageVRDisplayNodesVisibility(None) 
  
  def showQuantitativeView(self, show = True):
    
    if show:
      
      self.reportTableWidget.setAllSelectableOn()
      
      currentLayoutID = ViewHelper.updateQuantitativeViewLayout(self.getCurrentReport().GetStudiesSelectedForAnalysisCount())
    
      self.manageVolumeRenderingCompareViewNodeIDs()
      self.manageVolumeRenderingVisibility()
      self.manageModelDisplayCompareViewNodeIDs()

      self.manageModelsVisibility()

      #lm = slicer.app.layoutManager()
    
      #if lm:
        #ViewHelper.getStandardChartViewNode().Modified()
        #lm.setLayout(slicer.vtkMRMLLayoutNode.SlicerLayoutConventionalQuantitativeView)
        
      currentReport = self.getCurrentReport()
        
      if self.chartNode == None:
        self.chartNode = slicer.mrmlScene.AddNode(slicer.vtkMRMLChartNode())
        self.chartNode.SetProperty('default', 'yAxisLabel', 'SUVbw')
        self.chartNode.SetProperty('default', 'type', 'Scatter');
        self.chartNode.SetProperty('default', 'showLegend', 'on') 
        self.chartNode.SetProperty('default', 'xAxisType', 'categorical')
         
      chartViewNode = ViewHelper.getStandardChartViewNode()
      chartViewNode.SetChartNodeID(self.chartNode.GetID())
        
      self.updateQuantitativeView()
            
          
  def updateQuantitativeView(self, finding = None):
    
    currentReport = self.getCurrentReport()
    
    if finding:
      self.reportTableWidget.setRowSelectableOn()
    
    for can in self.chartArrayNodes:
      self.chartNode.RemoveArray(can.GetName())
      slicer.mrmlScene.RemoveNode(can)
    
    if self.chartArrayNodes:
      del self.chartArrayNodes[:]  
    
      
    if currentReport:
      arrayNodeNames = ['SUV<span style=\"vertical-align:sub;font-size:80%;\">MAX</span>','SUV<span style=\"vertical-align:sub;font-size:80%;\">MEAN</span>','SUV<span style=\"vertical-align:sub;font-size:80%;\">MIN</span>']
      saturationMultipliers = [1.0,0.75,0.5] # should be as many as different arraynodes
      colorTable = currentReport.GetFindingTypesColorTable()
      lut = colorTable.GetLookupTable()
      suvs = []
      findings = []
      
      if finding == None:
        for i in xrange(currentReport.GetFindingsCount()):
          fnd = currentReport.GetFinding(i)
          if fnd:
            findings.append(fnd)
      else:
        findings.append(finding)        
          
      for finding in findings:
        
        rgba = lut.GetTableValue(finding.GetColorID())
      
        for i in xrange(len(arrayNodeNames)):
      
          arrayNode = slicer.mrmlScene.AddNode(slicer.vtkMRMLDoubleArrayNode())
          arrayNode.SetName(finding.GetName()+" "+arrayNodeNames[i])
          self.chartArrayNodes.append(arrayNode)
      
          array = arrayNode.GetArray()
          samples = currentReport.GetStudiesSelectedForAnalysisCount()
          tuples = samples  
          array.SetNumberOfTuples(tuples)
          tuple = 0
       
          
          for j in xrange(samples):
            study = currentReport.GetStudySelectedForAnalysis(j)
            seg = finding.GetSegmentationForStudy(study)
            del suvs[:]
            suvs.append(0.)
            suvs.append(0.)
            suvs.append(0.)

            if seg:
              suvs[0] = seg.GetSUVMax()
              suvs[1] = seg.GetSUVMean()
              suvs[2] = seg.GetSUVMin()

            array.SetComponent(tuple, 0, float(study.GetAttribute('DICOM.StudyDate')))       
            array.SetComponent(tuple, 1, suvs[i])
            array.SetComponent(tuple, 2, 0.)
              
            tuple += 1
           
            colorStr = ViewHelper.RGBtoHex(rgba[0]*255,rgba[1]*255,rgba[2]*255,saturationMultipliers[i])
            self.chartNode.SetProperty(arrayNode.GetName(), 'color', colorStr)
              
          self.chartNode.AddArray(arrayNode.GetName(), arrayNode.GetID())
        
        if len(findings) == 1:
          self.chartNode.SetProperty('default', 'title', 'Longitudinal PET/CT Analysis: '+finding.GetName()+' SUVbw')         
      
      if len(findings) > 1:
        self.chartNode.SetProperty('default', 'title', 'Longitudinal PET/CT Analysis: All Findings SUVbw')  
      
      self.chartNode.SetProperty('default', 'xAxisLabel', 'DICOM Study Dates')  
    
    ViewHelper.getStandardChartViewNode().Modified()
                
        
  
  
  def showQualitativeView(self, show = True):
    
    if show:
      
      self.reportTableWidget.setAllSelectableOn()
      currentLayoutID = ViewHelper.updateQualitativeViewLayout(self.getCurrentReport().GetStudiesSelectedForAnalysisCount())
   
      self.manageVolumeRenderingCompareViewNodeIDs()
      self.manageVolumeRenderingVisibility()
      self.manageModelDisplayCompareViewNodeIDs()
      self.manageModelsVisibility()
          
      if self.qualitativeViewLastID != currentLayoutID:
        ViewHelper.SetForegroundOpacity(0.6, True)
        self.qualitativeViewLastID = currentLayoutID
    
      for i in range(self.getCurrentReport().GetStudiesSelectedForAnalysisCount()):
        study = self.getCurrentReport().GetStudySelectedForAnalysis(i)
        if study:
          ViewHelper.SetCompareBgFgLblVolumes(self.getCurrentReport().GetIndexOfStudySelectedForAnalysis(study),study.GetCTVolumeNode().GetID(),study.GetPETVolumeNode().GetID(),study.GetPETLabelVolumeNode().GetID(),True)      
        


  def onActivateROIPlacement(self, activate):
    appLogic = slicer.app.applicationLogic()
    if activate & (appLogic != None):
      intnode = appLogic.GetInteractionNode()
      selnode = appLogic.GetSelectionNode()
      
      if selnode:
        selnode.SetActiveAnnotationID('vtkMRMLAnnotationROINode')
        if intnode:
          roiNodes = ViewHelper.getROINodesCollection()
          nrOfRoiNodes = roiNodes.GetNumberOfItems()
          if nrOfRoiNodes > 0:
            self.lastAddedROINode = roiNodes.GetItemAsObject(nrOfRoiNodes-1)
          intnode.SetPlaceModePersistence(False)
         
          intnode.SetCurrentInteractionMode(slicer.vtkMRMLInteractionNode.Place)
          
          self.roiPlacementNodeAddedEventObserverID = slicer.mrmlScene.AddObserver(slicer.vtkMRMLScene.NodeAddedEvent, self.assignNewROIToFinding)
          self.roiEndPlacementEventObserverID = intnode.AddObserver(slicer.vtkMRMLInteractionNode.EndPlacementEvent, self.endROIPlacement)
          
          self.reportTableWidget.setProperty('enabled',False)

  def assignNewROIToFinding(self, caller, event):
    
    if self.roiPlacementNodeAddedEventObserverID != None:
      roiNodes = ViewHelper.getROINodesCollection()
      roiNode = roiNodes.GetItemAsObject(roiNodes.GetNumberOfItems()-1)
      currentStudy = self.getCurrentStudy()
      currentFinding = self.getCurrentFinding()
      

      if (roiNode != None) & (roiNode != self.lastAddedROINode) & (currentStudy != None) & (currentFinding != None):
        if currentFinding.GetSegmentationROI() == None:
          #adjust roi position if volumes in study are centered so that it's position doesn't get changed when set under transform
          if currentStudy.GetCenteredVolumes() & (currentStudy.GetCenteringTransform != None):
            roiXYZ = [0.,0.,0.]
            roiNode.GetXYZ(roiXYZ)
            centerTransform = currentStudy.GetCenteringTransform()
            centerTransformMatrix = centerTransform.GetMatrixTransformToParent()
            roiNode.SetXYZ(roiXYZ[0]-centerTransformMatrix.GetElement(0,3),roiXYZ[1]-centerTransformMatrix.GetElement(1,3),roiXYZ[2]-centerTransformMatrix.GetElement(2,3))  
          
          roiNode.SetVisibility(self.findingSelectionWidget.property('roiVisibility'))  
          currentFinding.SetSegmentationROI(roiNode)
          currentStudy.SetSegmentationROI(roiNode)
          appLogic = slicer.app.applicationLogic()
          if appLogic:
            intnode = appLogic.GetInteractionNode()
            if intnode:
              slicer.mrmlScene.RemoveObserver(self.roiPlacementNodeAddedEventObserverID)
              self.roiPlacementNodeAddedEventObserverID = None
              self.lastAddedROINode = roiNode
              self.editorWidget.editLabelMapsFrame.setEnabled(True)
     
                 
   
  def endROIPlacement(self, caller, event): 
    appLogic = slicer.app.applicationLogic()
    if appLogic:
      intnode = appLogic.GetInteractionNode()
      if intnode:
        intnode.RemoveObserver(self.roiEndPlacementEventObserverID)
        self.roiEndPlacementEventObserverID = None
          
        self.findingSelectionWidget.setProperty('placeROIChecked',False)
        self.reportTableWidget.setProperty('enabled',True)    
  
  def onEditorColorWarning(self):
    currentFinding = self.getCurrentFinding()
    if currentFinding:
      if self.segmentationColorChangeMessageBox == None:
        self.segmentationColorChangeMessageBox = ctk.ctkMessageBox()
        self.segmentationColorChangeMessageBox.setModal(True)
        self.segmentationColorChangeMessageBox.setIcon(qt.QMessageBox.Information)
        self.segmentationColorChangeMessageBox.setWindowTitle(ViewHelper.moduleDialogTitle())
        self.segmentationColorChangeMessageBox.setText("Please notice that only segmentations with the same color label as the current Finding Type ("+currentFinding.GetTypeName()+") will be used!")
        self.segmentationColorChangeMessageBox.setProperty('dontShowAgainVisible',True)
        self.segmentationColorChangeMessageBox.setDontShowAgain(False)  
    

      selectedLabelValue = self.editorWidget.toolsColor.colorSpin.property('value')
      if (selectedLabelValue > 0) & ( selectedLabelValue != currentFinding.GetColorID() ):     
        self.segmentationColorChangeMessageBox.show()
    
   
   #lm = slicer.app.layoutManager()
   
   #if analysisCollapsed:
     #if lm:
      #lm.setLayout(slicer.vtkMRMLLayoutNode.SlicerLayoutFourUpView)
