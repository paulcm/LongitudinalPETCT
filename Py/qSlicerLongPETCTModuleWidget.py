from __main__ import vtk, qt, ctk, slicer


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

    # reload button
    # (use this during development, but remove it when delivering
    #  your module to users)
    self.reloadButton = qt.QPushButton("Reload")
    self.reloadButton.toolTip = "Reload this module."
    self.reloadButton.name = "LongPETCT Reload"
    #self.layout.addWidget(self.reloadButton)
    self.reloadButton.connect('clicked()', self.onReload)


    self.logic  = slicer.modules.longpetct.logic()
    self.vrLogic = slicer.modules.volumerendering.logic()
    self.vrDisplayNode = None

    self.opacityFunction = vtk.vtkPiecewiseFunction()
    
    # instanciate all collapsible buttons
    self.reportsCollapsibleButton = ctk.ctkCollapsibleButton()
    self.studiesCollapsibleButton = ctk.ctkCollapsibleButton()
    self.findingsCollapsibleButton = ctk.ctkCollapsibleButton()
    
    
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
    
    self.studySelectionWidget.update(self.reportSelector.currentNode())
    self.reportSelector.connect('currentNodeChanged(vtkMRMLNode*)',self.onCurrentReportChanged)
    
    studiesLayout.addWidget(self.studySelectionWidget)

    self.studySelectionWidget.connect('studySelected(int)',self.onStudySelected)
    self.studySelectionWidget.connect('studyDeselected(int)',self.onStudyDeselected)    
    self.studySelectionWidget.connect('rockViewToggled(bool)',self.onStudySelectionWidgetRockView)
    self.studySelectionWidget.connect('volumeRenderingToggled(bool)',self.onStudySelectionWidgetVolumeRendering)
    self.studySelectionWidget.connect('opacityPowChanged(double)',self.onStudySelectionWidgetOpacityPow)
    self.studySelectionWidget.connect('gpuRenderingToggled(bool)',self.onStudySelectionWidgetGPUVolumeRendering)


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
    self.findingSelector.connect('nodeAddedByUser(vtkMRMLNode*)', self.on_FindingNodeCreated)
    
    findingsLayout.addWidget(self.findingSelectionWidget)
    
    
    # Add vertical spacer
    self.layout.addStretch()
    
    self.separator = qt.QFrame()
    self.separator.setFrameStyle(qt.QFrame.HLine | qt.QFrame.Sunken)
    self.layout.addWidget(self.separator)
    
    # Add Study Slider
    self.studySliderWidget = slicer.modulewidget.qSlicerLongPETCTStudySliderWidget()
    self.layout.addWidget(self.studySliderWidget)
    self.studySliderWidget.connect('sliderValueChanged(int)',self.onSliderWidgetValueChanged)
    

    # Add Report Table
    self.reportTableWidget = slicer.modulewidget.qSlicerLongPETCTReportTableWidget()
    self.layout.addWidget(self.reportTableWidget) 


  def onSliderWidgetValueChanged(self, value):
    currentReport = self.reportSelector.currentNode()
    if currentReport:
      currentSelectedStudy = currentReport.GetSelectedStudy(value)
      
      self.updateBgFgToUserSelectedStudy(currentSelectedStudy)
      
      if currentSelectedStudy:
        currentReport.SetUserSelectedStudy(currentSelectedStudy)
      
        studyIdx = currentReport.GetIndexOfStudy(currentSelectedStudy)
        self.studySelectionWidget.selectStudyInRow(studyIdx)
      
        self.reportTableWidget.update(currentReport)
        self.studySliderWidget.update(currentReport)
             
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
        
        self.onUpdateSliderAndTableWidgets(currentReport)        
        
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
      
        
        # volume rendering
        if self.vrDisplayNode == None:
          self.vrDisplayNode = self.vrLogic.CreateVolumeRenderingDisplayNode()
          viewNode = slicer.util.getNode('vtkMRMLViewNode1')
          viewNode.SetBoxVisible(0)
          
          viewNode.InvokeEvent(viewNode.ResetFocalPointRequestedEvent)
          
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
          
          
        self.onUpdateVolumeRendering(selectedStudy.GetPETVolumeNode())     

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
      self.onUpdateSliderAndTableWidgets(currentReport)     
      
      if currentReport.GetUserSelectedStudy():
        self.enableFindingsCollapsibleButton(True)
      else:
        self.enableFindingsCollapsibleButton(False)    
      
      
  def onCurrentReportChanged(self, reportNode):
    self.logic.SetSelectedReportNode = reportNode
    if reportNode:
      self.enableStudiesCollapsibleButton(True)

      self.studySelectionWidget.update(reportNode)
      self.studySliderWidget.update(reportNode)
      self.reportTableWidget.update(reportNode)
      
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
      if userSelectedStudy.GetPETVolumeNode():
        petID = userSelectedStudy.GetPETVolumeNode().GetID()
      if userSelectedStudy.GetCTVolumeNode():
        ctID = userSelectedStudy.GetCTVolumeNode().GetID()

    self.SetBgFgVolumes(ctID, petID)


  def onUpdateSliderAndTableWidgets(self, reportNode):
    self.studySliderWidget.update(reportNode)
    self.reportTableWidget.update(reportNode)


  def onUpdateVolumeRendering(self, volumeNode):
    if volumeNode:
      self.vrDisplayNode.SetAndObserveVolumeNodeID(volumeNode.GetID())
      self.vrLogic.UpdateDisplayNodeFromVolumeNode(self.vrDisplayNode, volumeNode)
    
      self.onStudySelectionWidgetOpacityPow(self.studySelectionWidget.property('opacityPow'))
          
    elif self.vrDisplayNode:
      #self.vrDisplayNode.GetROINode().SetVisibility(0)
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
        if viewNode:
          viewNode.SetAxisLabelsVisible(0)
          viewNode.SetAnimationMode(viewNode.Off)
        
    
    
  def onStudySelectionWidgetRockView(self, rock):
    
    viewNode = slicer.util.getNode('vtkMRMLViewNode1')
    
    if viewNode:
      if rock:
        viewNode.SetAnimationMode(viewNode.Rock)
        
      else:
        viewNode.SetAnimationMode(viewNode.Off)
        
        
  def onStudySelectionWidgetGPUVolumeRendering(self, useGPU):
    
    
    if useGPU:
      displayNode = self.vrLogic.CreateVolumeRenderingDisplayNode('vtkMRMLGPURayCastVolumeRenderingDisplayNode')
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


  def on_FindingNodeCreated(self, findingNode):
    currentReport = self.reportSelector.currentNode()
    currentReport.SetUserSelectedFinding(findingNode)
    dialog = slicer.modulewidget.qSlicerLongPETCTFindingSettingsDialog(self.parent)
    dialog.setReportNode(currentReport)
    dialog.exec_()
    if dialog.property('applied'):
      findingNode.SetName(dialog.property('findingName'))
      findingNode.SetFindingType(dialog.property('colorID'),dialog.property('typeName'))
      print dialog.property('colorID')
      print dialog.property('typeName')
      currentReport.AddFinding(findingNode)
      
    else:
      scene = findingNode.GetScene()
      if scene:
        scene.RemoveNode(findingNode)
    
    
    #if currentReport:
     # currentReport.SetUserSelectedFinding(findingNode)
     # findingSettingsDialog = slicer.modulewidget.qSlicerLongPETCTFindingSettingsDialog()
     # findingSettingsDialog.update(currentReport)
     # findingSettingsDialog.execDialog()

           
  @staticmethod
  def SetBgFgVolumes(bg, fg):
    appLogic = slicer.app.applicationLogic()
    selectionNode = appLogic.GetSelectionNode()
    selectionNode.SetReferenceActiveVolumeID(bg)
    selectionNode.SetReferenceSecondaryVolumeID(fg)
    appLogic.PropagateVolumeSelection()
    
    
    #red = slicer.util.getNode('vtkMRMLSliceNodeRed')
    #yellow = slicer.util.getNode('vtkMRMLSliceNodeYellow')
    #green = slicer.util.getNode('vtkMRMLSliceNodeGreen')
    
    #print "PETVOL" + str(volumeNode.GetScalarVolumeDisplayNode().GetInputImageData())
    #print "VOLSP" + str(volumeNode.GetSpacing())
    #print "PRESSP" + str(red.GetPrescribedSliceSpacing())



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
