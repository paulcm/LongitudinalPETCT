from __main__ import vtk, qt, ctk, slicer

from Editor import EditorWidget
from SlicerLongPETCTModuleViewHelper import SlicerLongPETCTModuleViewHelper as ViewHelper
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
    
    # reload button
    # (use this during development, but remove it when delivering
    #  your module to users)
    self.reloadButton = qt.QPushButton("Reload")
    self.reloadButton.toolTip = "Reload this module."
    self.reloadButton.name = "LongPETCT Reload"
    #self.layout.addWidget(self.reloadButton)
    self.reloadButton.connect('clicked()', self.onReload)

    self.tempObserverEditorTag = 0

    self.logic  = slicer.modules.longpetct.logic()
    self.vrLogic = slicer.modules.volumerendering.logic()
    self.vrDisplayNode = None

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
    
    self.opacityFunction = vtk.vtkPiecewiseFunction()
    
    # instanciate all collapsible buttons
    self.reportsCollapsibleButton = ctk.ctkCollapsibleButton()
    self.studiesCollapsibleButton = ctk.ctkCollapsibleButton()
    self.findingsCollapsibleButton = ctk.ctkCollapsibleButton()
    
    # put all collapsible buttons in a button group so that only one can be uncollapsed at a time
    self.collapsibleButtonsGroup = qt.QButtonGroup()
    self.collapsibleButtonsGroup.addButton(self.reportsCollapsibleButton)
    self.collapsibleButtonsGroup.addButton(self.studiesCollapsibleButton)
    self.collapsibleButtonsGroup.addButton(self.findingsCollapsibleButton)
    
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
    
    
    if self.reportSelector.currentNode():
      self.enableStudiesCollapsibleButton(True)
    else:
      self.enableStudiesCollapsibleButton(False)
    
    self.layout.addWidget(self.studiesCollapsibleButton)

    studiesLayout = qt.QVBoxLayout(self.studiesCollapsibleButton)

    self.studySelectionWidget = slicer.modulewidget.qSlicerLongPETCTStudySelectionWidget()    
    self.studySelectionWidget.setProperty('volumeRendering',True)
    self.studySelectionWidget.setProperty('gpuRendering',False)
    self.studySelectionWidget.setProperty('linearOpacity',False)
    self.studySelectionWidget.setProperty('rockView',True)
    
    self.studySelectionWidget.setReportNode(self.reportSelector.currentNode())
    self.reportSelector.connect('currentNodeChanged(vtkMRMLNode*)',self.onCurrentReportChanged)
    
    studiesLayout.addWidget(self.studySelectionWidget)

    self.studySelectionWidget.connect('studySelected(int)',self.onStudySelected)
    self.studySelectionWidget.connect('studyDeselected(int)',self.onStudyDeselected)    
    self.studySelectionWidget.connect('rockViewToggled(bool)',self.onStudySelectionWidgetRockView)
    self.studySelectionWidget.connect('volumeRenderingToggled(bool)',self.onStudySelectionWidgetVolumeRendering)
    self.studySelectionWidget.connect('opacityPowChanged(double)',self.onStudySelectionWidgetOpacityPow)
    self.studySelectionWidget.connect('gpuRenderingToggled(bool)',self.onStudySelectionWidgetGPUVolumeRendering)
    self.studySelectionWidget.connect('showStudiesCentered(bool)',self.onStudySelectionWidgetShowStudiesCentered)


    # Findings Collapsible button
    self.findingsCollapsibleButton.text = "Findings"
    self.findingsCollapsibleButton.setProperty('collapsed',True)
    
    if self.reportSelector.currentNode():
      if self.reportSelector.currentNode().GetUserSelectedStudy():
        self.enableFindingsCollapsibleButton(True)
      else:
        self.enableFindingsCollapsibleButton(False)
   
    
    self.layout.addWidget(self.findingsCollapsibleButton)
    
    findingsLayout = qt.QVBoxLayout(self.findingsCollapsibleButton)
      
    self.findingSelectionWidget = slicer.modulewidget.qSlicerLongPETCTFindingSelectionWidget()    
    self.findingSelectionWidget.setMRMLScene(slicer.mrmlScene)
    
    self.findingSelector = self.findingSelectionWidget.mrmlNodeComboBoxFinding()
    self.findingSelector.connect('nodeAddedByUser(vtkMRMLNode*)', self.onFindingNodeCreated)
    self.findingSelector.connect('currentNodeChanged(vtkMRMLNode*)', self.onFindingNodeChanged)
    self.findingSelector.connect('nodeAboutToBeEdited(vtkMRMLNode*)', self.onShowFindingSettingsDialog)
    self.findingSelector.connect('nodeAboutToBeRemoved(vtkMRMLNode*)', self.onFindingNodeToBeRemoved)
    
    findingsLayout.addWidget(self.findingSelectionWidget)
    
    # Editor widget in Findings
    editorWidgetParent = slicer.qMRMLWidget()
    editorWidgetParent.setLayout(qt.QVBoxLayout())
    editorWidgetParent.setMRMLScene(slicer.mrmlScene)
    self.editorWidget = EditorWidget(parent=editorWidgetParent,showVolumesFrame=False)
    self.editorWidget.setup()
    self.editorWidget.toolsColor.frame.setVisible(False)
    
    self.editorWidget.editLabelMapsFrame.setText("Edit Label Map")
    self.editorWidget.editLabelMapsFrame.setEnabled(False)
    
    if self.reportSelector.currentNode():
      if self.reportSelector.currentNode().GetUserSelectedFinding():
        self.editorWidget.editLabelMapsFrame.setEnabled(True)
        
        
    
    
    self.editorWidget.editLabelMapsFrame.connect('contentsCollapsed(bool)', self.onEditorCollapsed)      
    
    self.findingSelectionWidget.setEditorWidget(editorWidgetParent)

    
    # Add vertical spacer
    self.layout.addStretch()
    
    self.separator = qt.QFrame()
    self.separator.setFrameStyle(qt.QFrame.HLine | qt.QFrame.Sunken)
    self.layout.addWidget(self.separator)
    
    # Add Study Slider
    self.studySliderWidget = slicer.modulewidget.qSlicerLongPETCTStudySliderWidget()
    self.studySliderWidget.setReportNode(self.reportSelector.currentNode())
    self.layout.addWidget(self.studySliderWidget)
    self.studySliderWidget.connect('sliderValueChanged(int)',self.onSliderWidgetValueChanged)
    

    # Add Report Table
    self.reportTableWidget = slicer.modulewidget.qSlicerLongPETCTReportTableWidget()
    self.reportTableWidget.setReportNode(self.reportSelector.currentNode())
    self.layout.addWidget(self.reportTableWidget) 

  def onEditorCollapsed(self,collapsed):
    self.findingSelectionWidget.setProperty('selectionEnabled',collapsed)
    
    self.onEnterEditMode( collapsed != True )


  def onSliderWidgetValueChanged(self, value):
    currentReport = self.reportSelector.currentNode()
    if currentReport:
      currentSelectedStudy = currentReport.GetSelectedStudy(value)
      
      self.updateBgFgToUserSelectedStudy(currentSelectedStudy)
      
      if currentSelectedStudy:
        currentReport.SetUserSelectedStudy(currentSelectedStudy)
      
        studyIdx = currentReport.GetIndexOfStudy(currentSelectedStudy)
        self.studySelectionWidget.selectStudyInRow(studyIdx)
      
             
        self.onUpdateVolumeRendering(currentSelectedStudy.GetPETVolumeNode())
      else:
        self.onUpdateVolumeRendering(None)
        
        

  def onStudySelected(self, idx):
    currentReport = self.reportSelector.currentNode()
    if currentReport:
      selectedStudy = currentReport.GetStudy(idx)
      if selectedStudy:
        selectedStudy.SetSelected(True)
        currentReport.SetUserSelectedStudy(selectedStudy)
           
        
        petID = ""
        ctID = ""
        
        firstDisplayPet = selectedStudy.GetPETVolumeNode().GetScalarVolumeDisplayNode() == None
        firstDisplayCt = selectedStudy.GetCTVolumeNode().GetScalarVolumeDisplayNode() == None
                
        if firstDisplayPet | firstDisplayCt:
          self.updateBgFgToUserSelectedStudy(selectedStudy)

        if firstDisplayCt:
          selectedStudy.GetCTVolumeNode().GetScalarVolumeDisplayNode().SetAutoWindowLevel(0)
          selectedStudy.GetCTVolumeNode().GetScalarVolumeDisplayNode().SetAndObserveColorNodeID("vtkMRMLColorTableNodeGrey")
          selectedStudy.GetCTVolumeNode().GetScalarVolumeDisplayNode().SetWindowLevel(350,40)
        
        if firstDisplayPet:
          selectedStudy.GetPETVolumeNode().GetScalarVolumeDisplayNode().SetAndObserveColorNodeID("vtkMRMLPETProceduralColorNodePET-Heat");
          compositeNodes = slicer.util.getNodes('vtkMRMLSliceCompositeNode*')
          for compositeNode in compositeNodes.values():
            compositeNode.SetForegroundVolumeID(petID)
            compositeNode.SetForegroundOpacity(0.6)
      
        viewNode = slicer.util.getNode('vtkMRMLViewNode1')
        
        # volume rendering
        if self.vrDisplayNode == None:
          self.vrDisplayNode = self.vrLogic.CreateVolumeRenderingDisplayNode('vtkMRMLGPUTextureMappingVolumeRenderingDisplayNode')
                    
          viewNode.SetBoxVisible(0)
          
          self.vrDisplayNode.AddViewNodeID(viewNode.GetID())
          self.vrDisplayNode.SetCroppingEnabled(0)
          
          if self.studySelectionWidget.property('volumeRendering'):
            self.vrDisplayNode.VisibilityOn()
            viewNode.SetAxisLabelsVisible(1)
          else:
            self.vrDisplayNode.VisibilityOff()
            viewNode.SetAxisLabelsVisible(0)
          
          if self.studySelectionWidget.property('rockView') & self.studySelectionWidget.property('volumeRendering'):
            viewNode.SetAnimationMode(viewNode.Rock)
          else:
            viewNode.SetAnimationMode(viewNode.Off)
          
        #viewNode.InvokeEvent(slicer.vtkMRMLViewNode.ResetFocalPointRequestedEvent)
        
        self.onUpdateVolumeRendering(selectedStudy.GetPETVolumeNode())
        self.studySelectionWidget.selectStudyInRow(idx)       
        
        if viewNode:
            viewNode.AddObserver(vtk.vtkCommand.ModifiedEvent, self.viewNodeModified)
            
      self.updateBgFgToUserSelectedStudy(selectedStudy)
      
      if currentReport.GetUserSelectedStudy():
        self.enableFindingsCollapsibleButton(True)
      else:
        self.enableFindingsCollapsibleButton(False)  

            
      
      
  def onStudyDeselected(self, idx):
    currentReport = self.reportSelector.currentNode()
    if currentReport:        
      currentReport.GetStudy(idx).SetSelected(False)
      lastSelectedStudy = currentReport.GetSelectedStudyLast()
      currentReport.SetUserSelectedStudy(lastSelectedStudy)
      
      if currentReport.GetUserSelectedStudy():
        self.onUpdateVolumeRendering(currentReport.GetUserSelectedStudy().GetPETVolumeNode())
      else:
        self.onUpdateVolumeRendering(None)
      
      self.updateBgFgToUserSelectedStudy(currentReport.GetUserSelectedStudy())   
      
      if currentReport.GetUserSelectedStudy():
        self.enableFindingsCollapsibleButton(True)
      else:
        self.enableFindingsCollapsibleButton(False)        
   
      
      
  def onCurrentReportChanged(self, reportNode):
    self.logic.SetSelectedReportNode = reportNode
    self.studySliderWidget.setReportNode(reportNode)
    self.reportTableWidget.setReportNode(reportNode)
    
    if reportNode:
 
      self.enableStudiesCollapsibleButton(True)

      self.studySelectionWidget.setReportNode(reportNode)
      self.reportTableWidget.setReportNode(reportNode)
      
      if reportNode.GetUserSelectedStudy():
        self.onUpdateVolumeRendering(reportNode.GetUserSelectedStudy().GetPETVolumeNode())
      else:
        self.onUpdateVolumeRendering(None)
    
      self.updateBgFgToUserSelectedStudy(reportNode.GetUserSelectedStudy())
    
    else:
      self.enableStudiesCollapsibleButton(False)
      
      self.onUpdateVolumeRendering(None)
      self.updateBgFgToUserSelectedStudy(None)  
    
    
      
  
  def updateBgFgToUserSelectedStudy(self, userSelectedStudy):    
    
    petID = ""
    ctID = ""
      
    if userSelectedStudy:
      petVolume = userSelectedStudy.GetPETVolumeNode()
      if petVolume:
        petID = petVolume.GetID()
        petDisplayNode = petVolume.GetScalarVolumeDisplayNode()
        if petDisplayNode:
          self.selectedPETWindow = petDisplayNode.GetWindow()
          self.selectedPETLevel = petDisplayNode.GetLevel()
      if userSelectedStudy.GetCTVolumeNode():
        ctID = userSelectedStudy.GetCTVolumeNode().GetID()

    ViewHelper.SetBgFgLblVolumes(ctID,petID)


  def onUpdateVolumeRendering(self, volumeNode):
    if volumeNode:
      self.vrDisplayNode.SetAndObserveVolumeNodeID(volumeNode.GetID())
      # TODO: clean this hack
      slicer.util.getNode('vtkMRMLViewNode1').InvokeEvent(slicer.vtkMRMLViewNode.ResetFocalPointRequestedEvent)

      self.vrLogic.UpdateDisplayNodeFromVolumeNode(self.vrDisplayNode, volumeNode)
    
      self.onStudySelectionWidgetOpacityPow(self.studySelectionWidget.property('opacityPow'))
      
      self.vrDisplayNode.VisibilityOn()
      self.onStudySelectionWidgetVolumeRendering(self.studySelectionWidget.property('volumeRendering'))   
      
    else:
      self.onStudySelectionWidgetVolumeRendering(False) 
      if self.vrDisplayNode:
        self.vrDisplayNode.VisibilityOff()  
      
      
    #viewNode = slicer.util.getNode('vtkMRMLViewNode1')
    #print "YELLOW" + str(yellow.GetFieldOfView())
    #print "GREEN" + str(green.GetFieldOfView())
  
  def onStudySelectionWidgetOpacityPow(self, pow):
    currentReport = self.reportSelector.currentNode()
    
    if currentReport:
      selectedStudy = currentReport.GetUserSelectedStudy()
      
      if selectedStudy:
        petVolumeNode = selectedStudy.GetPETVolumeNode()
      
        if petVolumeNode:
          
          if petVolumeNode.GetScalarVolumeDisplayNode():
            window = petVolumeNode.GetScalarVolumeDisplayNode().GetWindow()
    
            if window > 0.:
              self.opacityFunction.RemoveAllPoints()
              
              self.opacityFunction.AddPoint(0.,0.,0.5,0.)  
              i = 1
              while (i < 9):
                m = i/10.0
                self.opacityFunction.AddPoint((window*m),m**pow,0.5,0.)  
                i = i+1
          
              self.opacityFunction.AddPoint(window,1.,0.5,0.)  
              
              if self.vrDisplayNode:
                if self.vrDisplayNode.GetVolumePropertyNode():
                  self.vrDisplayNode.GetVolumePropertyNode().SetScalarOpacity(self.opacityFunction)
                  self.vrDisplayNode.Modified()
                
    
  
  def onStudySelectionWidgetVolumeRendering(self, on):
    
    viewNode = slicer.util.getNode('vtkMRMLViewNode1')
    
    if self.vrDisplayNode:
      if on:
        self.vrDisplayNode.VisibilityOn()
        if viewNode:
          viewNode.SetAxisLabelsVisible(1)
          if self.studySelectionWidget.property('rockView'):
            viewNode.SetAnimationMode(viewNode.Rock)
      else:
        self.vrDisplayNode.VisibilityOff()
        self.onStudySelectionWidgetRockView(False)  
        if viewNode:
          viewNode.SetAxisLabelsVisible(0)
          
        
    
    
  def onStudySelectionWidgetRockView(self, rock):
    
    viewNode = slicer.util.getNode('vtkMRMLViewNode1')
    
    if viewNode:
      if rock:
        viewNode.SetAnimationMode(viewNode.Rock)
        
      else:
        viewNode.SetAnimationMode(viewNode.Off)
        viewNode.Modified()
        
        
  def onStudySelectionWidgetShowStudiesCentered(self, centered):
    currentReport = self.reportSelector.currentNode()
    if currentReport:
      i = 0
      while i < currentReport.GetStudiesCount():
        study = currentReport.GetStudy(i)
        
        if centered & (study.GetCenteringTransform() != None):
          study.GetPETVolumeNode().SetAndObserveTransformNodeID(study.GetCenteringTransform().GetID()) 
          study.GetCTVolumeNode().SetAndObserveTransformNodeID(study.GetCenteringTransform().GetID()) 
        else:
          study.GetPETVolumeNode().SetAndObserveTransformNodeID("")
          study.GetCTVolumeNode().SetAndObserveTransformNodeID("")
          
        i += 1
        
      currentStudy = currentReport.GetUserSelectedStudy()
      
      if currentStudy:
        self.onUpdateVolumeRendering(currentStudy.GetPETVolumeNode())
        ViewHelper.SetBgFgLblVolumes(currentStudy.GetCTVolumeNode().GetID(), currentStudy.GetPETVolumeNode().GetID())     
        
  def onStudySelectionWidgetGPUVolumeRendering(self, useGPU):
  
    if useGPU:
      displayNode = self.vrLogic.CreateVolumeRenderingDisplayNode() #'vtkMRMLGPURayCastVolumeRenderingDisplayNode')
    else:
      displayNode = self.vrLogic.CreateVolumeRenderingDisplayNode()
      
    if self.vrDisplayNode:
      displayNode.Copy(self.vrDisplayNode)  

    self.vrDisplayNode = displayNode
    
      

  def enableStudiesCollapsibleButton(self, enable):
    if enable:
      self.studiesCollapsibleButton.setProperty('enabled',True)
    else:
      self.studiesCollapsibleButton.setProperty('collapsed',True)
      self.studiesCollapsibleButton.setProperty('enabled',False)
      # cascade disabling
      self.enableFindingsCollapsibleButton(enable)
  
  def enableFindingsCollapsibleButton(self, enable):
    if enable:
      self.findingsCollapsibleButton.setProperty('enabled',True)
    else:
      self.findingsCollapsibleButton.setProperty('collapsed',True)
      self.findingsCollapsibleButton.setProperty('enabled',False)           


  def onSegmentationROIModified(self, caller, event):
    
    currentReport = self.reportSelector.currentNode()
      
      
    self.tempCroppedVol.Copy(currentReport.GetUserSelectedStudy().GetPETVolumeNode())
    self.tempCroppedVol.SetName("TempCroppedVolume") 
    
    cropLogic = slicer.modules.cropvolume.logic()
    croppingROI = slicer.vtkMRMLAnnotationROINode()
    croppingROI.Copy(currentReport.GetUserSelectedFinding().GetSegmentationROI())
    croppingROI.SetAndObserveTransformNodeID(None)
    cropLogic.CropVoxelBased(croppingROI,currentReport.GetUserSelectedStudy().GetPETVolumeNode(),self.tempCroppedVol)

    volLogic  = slicer.modules.volumes.logic()    
    
    createdLabelVol = volLogic.CreateLabelVolume(slicer.mrmlScene, self.tempCroppedVol,'TempLabelVolume')
    self.tempLabelVol.Copy(createdLabelVol)   
    self.tempLabelVol.SetName("TempLabelVolume")
    slicer.mrmlScene.RemoveNode(createdLabelVol)
          
    propagate = caller == self
    
    ViewHelper.SetBgFgLblVolumes(self.tempCroppedVol.GetID(),None,self.tempLabelVol.GetID(), propagate)  
    
    

    self.editorWidget.setMasterNode(self.tempCroppedVol) 
    self.editorWidget.setMergeNode(self.tempLabelVol)      

  def onEnterEditMode(self,enter):
    currentReport = self.reportSelector.currentNode()
    
    if (currentReport != None) & (enter == True):
      
      self.onSegmentationROIModified(self, slicer.vtkMRMLAnnotationROINode.ValueModifiedEvent)
      ViewHelper.SetBackgroundWindowLevel(self.selectedPETWindow,self.selectedPETLevel)
      
      self.editorWidget.toolsColor.colorNode = currentReport.GetColorNode()
      volNode = currentReport.GetUserSelectedStudy().GetPETVolumeNode()
      #currentReport.GetUserSelectedFinding().AddLabelMapForStudy(currentReport.GetUserSelectedStudy(),mergeNode)
      self.editorWidget.enter()
      self.editorWidget.toolsColor.colorNode = currentReport.GetFindingTypesColorTable()
      self.editorWidget.editUtil.setLabel(currentReport.GetUserSelectedFinding().GetColorID())

      self.tempObserverEditorTag = currentReport.GetUserSelectedFinding().GetSegmentationROI().AddObserver(vtk.vtkCommand.ModifiedEvent, self.onSegmentationROIModified, False)
      
    
    elif enter == False:
      self.editorWidget.exit()
      currentReport.GetUserSelectedFinding().GetSegmentationROI().RemoveObserver(self.tempObserverEditorTag)
      ViewHelper.SetBgFgLblVolumes(currentReport.GetUserSelectedStudy().GetCTVolumeNode().GetID(), currentReport.GetUserSelectedStudy().GetPETVolumeNode().GetID(),None, True)
      ViewHelper.SetForegroundWindowLevel(self.selectedPETWindow,self.selectedPETLevel)
     
      currentFinding = currentReport.GetUserSelectedFinding()
      if currentFinding:
        findingROI = currentFinding.GetSegmentationROI()
        if findingROI:
          roiXYZ = [0.,0.,0.]
          findingROI.GetXYZ(roiXYZ)
     
          if (self.studySelectionWidget.property('centeredSelected') == True) & (currentReport.GetUserSelectedFinding != None):
            centeringMatrix = vtk.vtkMatrix4x4()
            currentStudy = currentReport.GetUserSelectedStudy() 
            if currentStudy:
              centeringTransform = currentStudy.GetCenteringTransform()
              if centeringTransform:
                centeringTransform.GetMatrixTransformToWorld(centeringMatrix)
             
                roiXYZ = [roiXYZ[0],roiXYZ[1],roiXYZ[2],1.0]
                centeringMatrix.MultiplyPoint(roiXYZ,roiXYZ)
                roiXYZ = [roiXYZ[0],roiXYZ[1],roiXYZ[2]]
          
          ViewHelper.setSliceNodesCrossingPositionRAS(roiXYZ)

  def onFindingNodeCreated(self, findingNode):
    currentReport = self.reportSelector.currentNode()
    applied = self.onShowFindingSettingsDialog(findingNode)
  
    if (currentReport != None) & (currentReport.GetUserSelectedStudy() != None) & (applied == True):
      currentReport.AddFinding(findingNode)
      
      roi = slicer.vtkMRMLAnnotationROINode()
      roi.SetScene(slicer.mrmlScene)
      roi.SetHideFromEditors(False)
      roi.SetName(findingNode.GetName()+"_ROI")   
        
          
      if slicer.mrmlScene:
        slicer.mrmlScene.AddNode(roi)
        findingNode.SetSegmentationROI(roi)
          
        #if self.vrDisplayNode:
          #self.vrDisplayNode.SetAndObserveROINodeID(self.roi.GetID())
          #self.vrDisplayNode.SetCroppingEnabled(True)
      
      currentStudy = currentReport.GetUserSelectedStudy()
      
      if currentStudy:
        centered = self.studySelectionWidget.property('centeredSelected')
        if centered:
          self.centerROIonVolumeNode(roi,currentStudy.GetPETVolumeNode(), currentStudy.GetCenteringTransform())
        else:
          self.centerROIonVolumeNode(roi,currentStudy.GetPETVolumeNode(), None)
               
    else:
      scene = findingNode.GetScene()
      if scene:
        scene.RemoveNode(findingNode)  
    
    
    
  def centerROIonVolumeNode(self, roi, volume, centerTransform):
    
    roiXYZ = ViewHelper.getSliceNodesCrossingPositionRAS()
    
    if centerTransform:
      roi.SetAndObserveTransformNodeID(centerTransform.GetID())   
      centerTransformMatrix = centerTransform.GetMatrixTransformToParent()
      roi.SetXYZ(roiXYZ[0]-centerTransformMatrix.GetElement(0,3),roiXYZ[1]-centerTransformMatrix.GetElement(1,3),roiXYZ[2]-centerTransformMatrix.GetElement(2,3))  
    else:
      roi.SetXYZ(roiXYZ)
    
    roi.SetRadiusXYZ(50,50,50)        
    
  
  def onShowFindingSettingsDialog(self, findingNode):
          
    accepted = False

    if findingNode:
      currentReport = self.reportSelector.currentNode()
      if currentReport:
        currentReport.SetUserSelectedFinding(findingNode)
        dialog = slicer.modulewidget.qSlicerLongPETCTFindingSettingsDialog(self.parent)
        dialog.setReportNode(currentReport)
        result = dialog.exec_()
      
        if result == qt.QDialog.Accepted:
          name = dialog.property('findingName')
          if name:
            findingNode.SetName(dialog.property('findingName'))
          findingNode.SetTypeName(dialog.property('typeName'))
          findingNode.SetColorID(dialog.property('colorID'))
          print "TYPENAME:" + findingNode.GetTypeName()
          print "COLORID:" + str(findingNode.GetColorID())
          accepted = True
    
    return accepted    
    
  def onFindingNodeChanged(self, findingNode):
    currentReport = self.reportSelector.currentNode()
    #self.editorWidget.exit()
    if currentReport:
      currentReport.SetUserSelectedFinding(findingNode)   
    
    if findingNode:
      self.editorWidget.editLabelMapsFrame.setEnabled(True) 
    else:
      self.editorWidget.editLabelMapsFrame.setEnabled(False)
    
  
  def onFindingNodeToBeRemoved(self, findingNode):
    currentReport = self.reportSelector.currentNode()
    if currentReport:
      currentReport.RemoveFinding(findingNode)
           
    
    
    #if currentReport:
     # currentReport.SetUserSelectedFinding(findingNode)
     # findingSettingsDialog = slicer.modulewidget.qSlicerLongPETCTFindingSettingsDialog()
     # findingSettingsDialog.update(currentReport)
     # findingSettingsDialog.execDialog()    
     
    
    
    #red = slicer.util.getNode('vtkMRMLSliceNodeRed')
    #yellow = slicer.util.getNode('vtkMRMLSliceNodeYellow')
    #green = slicer.util.getNode('vtkMRMLSliceNodeGreen')
    
    #print "PETVOL" + str(volumeNode.GetScalarVolumeDisplayNode().GetInputImageData())
    #print "VOLSP" + str(volumeNode.GetSpacing())
    #print "PRESSP" + str(red.GetPrescribedSliceSpacing())


  def viewNodeModified(self, caller, event):
    viewNode = slicer.util.getNode('vtkMRMLViewNode1')
    if viewNode == caller:
      if viewNode.GetAnimationMode() == viewNode.Rock:
        self.studySelectionWidget.setProperty('rockView',True)
      else:
        self.studySelectionWidget.setProperty('rockView',False)
              

  def onReload(self,moduleName="LongPETCT"):
    """Generic reload method for any scripted module.
    ModuleWizard will subsitute correct default moduleName.
    """
    import imp, sys, os, slicer

    widgetName = moduleName + "Widget"

    # reload the source code
    # - set source file path
    # - load the module to the global space
    filePath = eval('slicer.modules.%s.path' % moduleName.lower())
    p = os.path.dirname(filePath)
    
    print p
    
    if not sys.path.__contains__(p):
      sys.path.insert(0,p)
    fp = open(filePath, "r")
    globals()[moduleName] = imp.load_module(
        moduleName, fp, filePath, ('.py', 'r', imp.PY_SOURCE))
    fp.close()
    
    print moduleName

    # rebuild the widget
    # - find and hide the existing widget
    # - create a new widget in the existing parent
    parent = slicer.util.findChildren(name='%s Reload' % moduleName)[0].parent()
    for child in parent.children():
      try:
        child.hide()
      except AttributeError:
        pass
    globals()[widgetName.lower()] = eval(
        'globals()["%s"].%s(parent)' % (moduleName, widgetName))
    globals()[widgetName.lower()].setup()
