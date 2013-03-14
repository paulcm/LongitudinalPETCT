from __main__ import vtk, qt, ctk, slicer

from Editor import EditorWidget
from SlicerLongitudinalPETCTModuleViewHelper import SlicerLongitudinalPETCTModuleViewHelper as ViewHelper
from SlicerLongitudinalPETCTModuleSegmentationHelper import SlicerLongitudinalPETCTModuleSegmentationHelper as SegmentationHelper

import sys as SYS

import thread as Thread
import time
from locale import currency
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
      
  
  def getReportSelectionWidget(self):
    if not self.reportSelectionWidget:
      self.reportSelectionWidget = slicer.modulewidget.qSlicerLongitudinalPETCTReportSelectionWidget()    
      self.reportSelectionWidget.setMRMLScene(slicer.mrmlScene)
      if self.reportSelectionWidget.mrmlNodeComboBoxReport():
        self.getReportSelectionWidget().mrmlNodeComboBoxReport().connect('currentNodeChanged(vtkMRMLNode*)',self.onCurrentReportChanged)
      self.reportSelectionWidget.connect('showModuleSettingsDialog()',self.showModuleSettingsDialog)    
      
    return self.reportSelectionWidget  
  
  def getReportSelector(self):
    return self.getReportSelectionWidget().mrmlNodeComboBoxReport()  
  
  
  def getStudySelectionWidget(self):
    
    if not self.studySelectionWidget:
      
      self.studySelectionWidget = slicer.modulewidget.qSlicerLongitudinalPETCTStudySelectionWidget()    
      
      volRen = ViewHelper.getSetting('VolumeRendering')
      spinView = ViewHelper.getSetting('Spinning')
      opctypow = self.studySelectionWidget.property('opacityPow')
      
      currentStudy = self.getCurrentStudy()
      
      if currentStudy:
        vrdn = currentStudy.GetVolumeRenderingDisplayNode()
        
        viewNode = ViewHelper.getStandardViewNode()
        if viewNode:
          spinView = viewNode.GetAnimationMode() == slicer.vtkMRMLViewNode.Spin
        
        if vrdn:
          volRen = vrdn.GetVisibility()
          try:
            opctypow = float(vrdn.GetAttribute('OpacityPow'))
          except:
            opctypow = self.studySelectionWidget.property('opacityPow')
        
      self.studySelectionWidget.setProperty('volumeRendering',volRen)
      self.studySelectionWidget.setProperty('spinView',spinView)
      self.studySelectionWidget.setProperty('opacityPow',opctypow)
      
    
      self.studySelectionWidget.setReportNode(self.getCurrentReport())
    
      self.studySelectionWidget.connect('studySelected(int)',self.onStudySelected)
      self.studySelectionWidget.connect('studyDeselected(int)',self.onStudyDeselected)    
      self.studySelectionWidget.connect('spinViewToggled(bool)',ViewHelper.spinStandardViewNode)
      self.studySelectionWidget.connect('volumeRenderingToggled(bool)',self.manageVolumeRenderingVisibility)
      self.studySelectionWidget.connect('opacityPowChanged(double)',self.onSetOpacityPowForCurrentStudy)

    return self.studySelectionWidget  
  
  
  def getFindingSelectionWidget(self):
    
    if not self.findingSelectionWidget:
      self.findingSelectionWidget = slicer.modulewidget.qSlicerLongitudinalPETCTFindingSelectionWidget()    
      self.findingSelectionWidget.setMRMLScene(slicer.mrmlScene)
      self.findingSelectionWidget.setReportNode(self.getCurrentReport())
      self.findingSelectionWidget.connect('roiVisibilityChanged(bool)', self.onManageFindingROIsVisibility)
      self.findingSelectionWidget.connect('placeROIChecked(bool)', self.onActivateROIPlacement)
      self.findingSelectionWidget.connect('addSegmentationToFinding()', self.onCollapseEditor)
    
      findingSelector = self.findingSelectionWidget.mrmlNodeComboBoxFinding()
      if findingSelector:
        findingSelector.connect('currentNodeChanged(vtkMRMLNode*)', self.onFindingNodeChanged)
        findingSelector.connect('nodeAboutToBeEdited(vtkMRMLNode*)', self.onShowFindingSettingsDialog)
        findingSelector.connect('nodeAboutToBeRemoved(vtkMRMLNode*)', self.onFindingNodeToBeRemoved)
      
    self.findingSelectionWidget.setEditorWidget(self.getEditorWidget().parent)
    
    return self.findingSelectionWidget
 
 
  def getFindingSelector(self):
    return self.getFindingSelectionWidget().mrmlNodeComboBoxFinding()    
 
 
  def getEditorWidget(self):
    
    if not self.editorWidget:
      
      editorWidgetParent = slicer.qMRMLWidget()
      editorWidgetParent.setLayout(qt.QVBoxLayout())
      editorWidgetParent.setMRMLScene(slicer.mrmlScene)
      editorWidgetParent.hide()
      self.editorWidget = EditorWidget(editorWidgetParent, False)
      self.editorWidget.setup()
  
      self.editorWidget.editLabelMapsFrame.setText("Edit Segmentation")
      self.editorWidget.editLabelMapsFrame.setEnabled(False)
      self.editorWidget.editLabelMapsFrame.connect('contentsCollapsed(bool)', self.onEditorCollapsed)      

    
      editorWidgetParent.show()
      
    return self.editorWidget
  
  
  def getAnalysisSettingsWidget(self):
    
    if not self.analysisSettingsWidget:
      
      self.analysisSettingsWidget = slicer.modulewidget.qSlicerLongitudinalPETCTAnalysisSettingsWidget()    
      self.analysisSettingsWidget.setReportNode(self.getCurrentReport())
    
      self.analysisSettingsWidget.connect('qualitativeAnalysisClicked(bool)', self.showQualitativeView)
      self.analysisSettingsWidget.connect('quantitativeAnalysisClicked(bool)', self.showQuantitativeView)
      self.analysisSettingsWidget.connect('studySelectedForAnalysis(int, bool)', self.onStudyForAnalysisChanged)
      self.analysisSettingsWidget.connect('volumeRenderingToggled(bool)',self.manageVolumeRenderingVisibility)
      self.analysisSettingsWidget.connect('spinViewToggled(bool)',ViewHelper.spinCompareViewNodes)
      self.analysisSettingsWidget.connect('exportCSV()',self.onSaveResultsCSV)
           
    return self.analysisSettingsWidget  
  
  def getReportTableWidget(self):
    
    if not self.reportTableWidget:
      
      self.reportTableWidget = slicer.modulewidget.qSlicerLongitudinalPETCTReportTableWidget()
      self.reportTableWidget.setColumnSelectableOn()
      self.reportTableWidget.setReportNode(self.getCurrentReport())
      self.reportTableWidget.connect('studyClicked(int)',self.changeSelectedStudy)
      self.reportTableWidget.connect('findingClicked(int)',self.getFindingSelector().setCurrentNodeIndex)                 
          
    return self.reportTableWidget 
  
  def getModuleSettingsDialog(self):
    if not self.moduleSettingsDialog:  
      self.moduleSettingsDialog = slicer.modulewidget.qSlicerLongitudinalPETCTModuleSettingsDialog()
      self.moduleSettingsDialog.connect('accepted()',self.saveModuleSettings)
    
    return self.moduleSettingsDialog
  
  def showModuleSettingsDialog(self):
      
    self.getModuleSettingsDialog().setProperty('registration',ViewHelper.getSetting('Registration'))
    self.getModuleSettingsDialog().setProperty('volumeRendering',ViewHelper.getSetting('VolumeRendering'))
    self.getModuleSettingsDialog().setProperty('spinning',ViewHelper.getSetting('Spinning'))    
    self.getModuleSettingsDialog().setProperty('makeModels',ViewHelper.getSetting('Models'))
    self.getModuleSettingsDialog().setProperty('outlineSegmentations',ViewHelper.getSetting('OutlineSegmentations'))
        
    self.getModuleSettingsDialog().show()
    
  def saveModuleSettings(self):
    
    value = "LongitudinalPETCT/Registration"
    if self.getModuleSettingsDialog().property('registration'):
      qt.QSettings().setValue(value,'true')
    else:
      qt.QSettings().setValue(value,'false')

      
    value = "LongitudinalPETCT/VolumeRendering"
    if self.getModuleSettingsDialog().property('volumeRendering'):
      qt.QSettings().setValue(value,'true')
    else:
      qt.QSettings().setValue(value,'false')

      
    value = "LongitudinalPETCT/Spinning"
    if self.getModuleSettingsDialog().property('spinning'):
      qt.QSettings().setValue(value,'true')
    else:
      qt.QSettings().setValue(value,'false')
      
    value = "LongitudinalPETCT/Models"
    if self.getModuleSettingsDialog().property('makeModels'):
      qt.QSettings().setValue(value,'true')
    else:
      qt.QSettings().setValue(value,'false')      
    
    value = "LongitudinalPETCT/OutlineSegmentations"
    if self.getModuleSettingsDialog().property('outlineSegmentations'):
      qt.QSettings().setValue(value,'true')
    else:
      qt.QSettings().setValue(value,'false')    
      
      
  def getFindingSettingsDialog(self):
    
    if not self.findingSettingsDialog:
        self.findingSettingsDialog = slicer.modulewidget.qSlicerLongitudinalPETCTFindingSettingsDialog(self.parent)
           
    return self.findingSettingsDialog
  
  
  def getUnsupportedVolumeRenderingMessageBox(self):
    if self.unsupportedVolRendMessageBox == None:
      self.unsupportedVolRendMessageBox = ctk.ctkMessageBox()
      self.unsupportedVolRendMessageBox.setModal(True)
      self.unsupportedVolRendMessageBox.setIcon(qt.QMessageBox.Information)
      self.unsupportedVolRendMessageBox.setWindowTitle(ViewHelper.moduleDialogTitle())
      self.unsupportedVolRendMessageBox.setText('NCIRayCastVolumeRendering is selected as default volume renderer. The Longitudinal PET/CT Analysis module does not support displaying of models from Finding segmentations with this volume renderer!')
      self.unsupportedVolRendMessageBox.setProperty('dontShowAgainVisible', True)
      self.unsupportedVolRendMessageBox.setDontShowAgain(False)
      
    return self.unsupportedVolRendMessageBox 
  
  
  def getSegmentationColorChangeMessageBox(self):
    
    if not self.segmentationColorChangeMessageBox:
     
      self.segmentationColorChangeMessageBox = ctk.ctkMessageBox()
      self.segmentationColorChangeMessageBox.setModal(True)
      self.segmentationColorChangeMessageBox.setIcon(qt.QMessageBox.Information)
      self.segmentationColorChangeMessageBox.setWindowTitle(ViewHelper.moduleDialogTitle())
      self.segmentationColorChangeMessageBox.setText("Please notice that only segmentations with the same color label as the current Finding's Type will be used!")
      self.segmentationColorChangeMessageBox.setProperty('dontShowAgainVisible',True)
      self.segmentationColorChangeMessageBox.setDontShowAgain(False)
      
    return self.segmentationColorChangeMessageBox
  
   
  def setup(self): 
    # Instantiate and connect widgets ...

    # switch to FourUp Layout
    lm = slicer.app.layoutManager()
    if lm:
      lm.setLayout(slicer.vtkMRMLLayoutNode.SlicerLayoutFourUpView) # two over two
    
    if ViewHelper.getSetting('OutlineSegmentations'):
      slicer.mrmlScene.GetNodeByID("vtkMRMLSliceNodeRed").SetUseLabelOutline(True)
      slicer.mrmlScene.GetNodeByID("vtkMRMLSliceNodeYellow").SetUseLabelOutline(True)
      slicer.mrmlScene.GetNodeByID("vtkMRMLSliceNodeGreen").SetUseLabelOutline(True)
    
    self.reportSelectionWidget = None
    self.reportSelector = None
    self.studySelectionWidget = None
    self.findingSelectionWidget = None
    self.findingSelector = None
    self.editorWidget = None
    self.analysisSettingsWidget = None
    self.reportTableWidget = None
    self.moduleSettingsDialog = None
    self.findingSettingsDialog = None
    self.unsupportedVolRendMessageBox = None
    self.segmentationColorChangeMessageBox = None
    
    self.roiEndPlacementEventObserverID = None
    self.roiPlacementNodeAddedEventObserverID = None
    self.lastAddedROINode = None
    self.qualitativeViewLastID = -1
    self.reportObserverIDs = {}
    self.tempObserverEditorTag = None
    self.viewNodeObserverID = -1
    #self.segmentationROIOldPosition = [0.,0.,0.]
    
    
    self.chartArrayNodes = []
    self.chartNode = None
    
    self.fileDialog = None
    
    self.unopenedAnalysis = True

    self.logic  = slicer.modules.longitudinalpetct.logic()
    self.vrLogic = slicer.modules.volumerendering.logic()
    self.anLogic = logic = slicer.modules.annotations.logic()

    self.tempCroppedVol = None
    self.tempLabelVol = None
    
    
    self.cliSUV = None
    self.cliModelMaker = None

    self.findingROIVisiblityBackup = True
    self.reportNodesList = []
       
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
       
    # Report Selection Panel
    self.reportsCollapsibleButton.setProperty('collapsed',False)
    self.reportsCollapsibleButton.text = "1. Report Selection"
    
    self.layout.addWidget(self.reportsCollapsibleButton)
    
    reportsLayout = qt.QVBoxLayout(self.reportsCollapsibleButton)
    reportsLayout.addWidget(self.getReportSelectionWidget())
    
    
    # Studies Selection Panel   
    self.studiesCollapsibleButton.text = "2. Study Selection"
    self.studiesCollapsibleButton.setProperty('collapsed',True)
    
    self.layout.addWidget(self.studiesCollapsibleButton)

    studiesLayout = qt.QVBoxLayout(self.studiesCollapsibleButton)   
    studiesLayout.addWidget(self.getStudySelectionWidget())


    # Findings Selection Panel
    self.findingsCollapsibleButton.text = "3. Findings"
    self.findingsCollapsibleButton.setProperty('collapsed',True)    
     
    self.layout.addWidget(self.findingsCollapsibleButton)
    
    findingsLayout = qt.QVBoxLayout(self.findingsCollapsibleButton)
    findingsLayout.addWidget(self.getFindingSelectionWidget())
        
    
    #Analysis Settings Panel
    self.analysisCollapsibleButton.text = "4. Analysis"
    self.analysisCollapsibleButton.setProperty('collapsed',True)
    self.analysisCollapsibleButton.connect('contentsCollapsed(bool)', self.onSwitchToAnalysis)      
    
    self.layout.addWidget(self.analysisCollapsibleButton)

    analysisLayout = qt.QVBoxLayout(self.analysisCollapsibleButton)
    analysisLayout.addWidget(self.getAnalysisSettingsWidget())  


    # Add vertical spacer
    self.layout.addStretch()
    
    self.separator = qt.QFrame()
    self.separator.setFrameStyle(qt.QFrame.HLine | qt.QFrame.Sunken)
    self.layout.addWidget(self.separator)

    # Report Overview Table Panel
    self.layout.addWidget(self.getReportTableWidget())  
    
    
    ViewHelper.SetForegroundOpacity(0.6)       
    
    #self.manageCollapsibleButtonsAbility(self, vtk.vtkCommand.ModifiedEvent)
    self.onCurrentReportChanged(self.getReportSelector().currentNode())
    
  def onEditorCollapsed(self,collapsed):
    self.getFindingSelectionWidget().setProperty('selectionEnabled',collapsed)    
    self.onEnterEditMode( collapsed != True )
    self.getFindingSelectionWidget().hideAddButton(collapsed)
    
    self.manageCollapsibleButtonsAbility(self, vtk.vtkCommand.ModifiedEvent)
    
 
  def onCollapseEditor(self):
    self.getEditorWidget().editLabelMapsFrame.setProperty('collapsed', True)
 
       
  def changeSelectedStudy(self, newIndex):
    
    currentReport = self.getCurrentReport()
    # update model/logic
    if currentReport:
      study = currentReport.GetSelectedStudy(newIndex)
      if study:
        currentReport.SetUserSelectedStudyNodeID(study.GetID())
      else:
        currentReport.SetUserSelectedStudyNodeID(None)
              
      currentFinding = self.getCurrentFinding()
      currentStudy = self.getCurrentStudy()
    
      if study:
        self.getStudySelectionWidget().selectStudyInRow(currentReport.GetIndexOfStudyNodeID(study.GetID()))
        
      if currentFinding:
        currentSegROI = currentFinding.GetSegmentationROINode()
      
        if (currentStudy != None) & (currentSegROI != None):
          remPos = ViewHelper.getROITranslationFromTransform(currentSegROI)
          currentStudy.SetAndObserveSegmentationROINodeID(currentSegROI.GetID())
          addPos = ViewHelper.getROITranslationFromTransform(currentSegROI)
          
          if not currentFinding.GetSegmentationMappedByStudyNodeID(currentStudy.GetID()):
            xyz = [0.,0.,0.]
            currentSegROI.GetXYZ(xyz)
            xyz = [xyz[0]+remPos[0]-addPos[0],xyz[1]+remPos[1]-addPos[1],xyz[2]+remPos[2]-addPos[2]] 
            currentSegROI.SetXYZ(xyz)
          else:          
            self.updateSegmentationROIPosition()
          
        elif study:
          currentStudy.SetAndObserveSegmentationROINodeID(None) #TODO see why None doesn't work
    
    # update view
    self.updateBgFgToUserSelectedStudy(study)
    self.manageVolumeRenderingVisibility()
    
    self.manageModelsVisibility(self.isQuantitativeViewActive()) 
    
    if self.isQuantitativeViewActive():
      self.updateQuantitativeView(currentReport.GetUserSelectedFindingNode())    
                
       
  def updateSegmentationROIPosition(self):
    currentStudy = self.getCurrentStudy()
    currentFinding = self.getCurrentFinding()
    
    if (currentStudy != None) & (currentFinding != None):
      currentSegmentation = currentFinding.GetSegmentationMappedByStudyNodeID(currentStudy.GetID())
      
      xyz = [0.,0.,0.]
          
      if currentSegmentation:
        radius = [0.,0.,0.]
            
        currentSegmentation.GetROIxyz(xyz)
        currentSegmentation.GetROIRadius(radius)
        
        if currentStudy.GetSegmentationROINode():
          currentStudy.GetSegmentationROINode().SetXYZ(xyz)
          currentStudy.GetSegmentationROINode().SetRadiusXYZ(radius)
            
          xyzRAS = ViewHelper.getROIPositionInRAS(currentStudy.GetSegmentationROINode())
            
          ViewHelper.setSliceNodesCrossingPositionRAS(xyzRAS)
          
          if self.analysisCollapsibleButton.property('collapsed') == False:
            ViewHelper.setCompareSliceNodesCrossingPositionRAS(self.getCurrentReport().GetIndexOfSelectedStudySelectedForAnalysis(currentStudy), xyzRAS)   
      
                
  
  
  def onStudySelected(self, idx):
    currentReport = self.getReportSelector().currentNode()
    if currentReport:
      selectedStudy = currentReport.GetStudy(idx)
      if selectedStudy:
                
        petID = ""
        ctID = ""
        
        #Registration
        if (ViewHelper.getSetting('Registration')) & (selectedStudy.GetCenteringTransformNode() == None) & (idx > 0):
            transform = slicer.mrmlScene.AddNode(slicer.vtkMRMLLinearTransformNode())
            
            parameters = {}
            #baseline
            parameters["fixedVolume"] = currentReport.GetStudy(0).GetPETVolumeNodeID()
            # current for registration
            parameters["movingVolume"] = selectedStudy.GetPETVolumeNodeID()
            # transformation
            parameters["linearTransform"] = transform.GetID()
            # default settings
            parameters["useRigid"] = True
            parameters["numberOfIterations"] = 1000
            parameters["initializeTransformMode"] = "useMomentsAlign"
            
            
            dialog = ViewHelper.createBusyProgressBarDialog("Registering "+selectedStudy.GetPETVolumeNode().GetName()+" with " + currentReport.GetStudy(0).GetPETVolumeNode().GetName() )
            dialog.show()
            cliNode = None
            cliNode = slicer.cli.run(slicer.modules.brainsfit, cliNode, parameters, wait_for_completion = True)
            dialog.done(0)
            transform.SetName(selectedStudy.GetPETVolumeNode().GetName()+"_BaselineRegistrationTransform")
            selectedStudy.SetAndObserveCenteringTransformNodeID(transform.GetID())
               
        
        firstDisplayPet = selectedStudy.GetPETVolumeNode().GetScalarVolumeDisplayNode() == None
        firstDisplayCt = selectedStudy.GetCTVolumeNode().GetScalarVolumeDisplayNode() == None
        firstDisplayPetLabels = selectedStudy.GetPETLabelVolumeNode().GetScalarVolumeDisplayNode() == None
                
        if firstDisplayPet | firstDisplayCt:
          self.updateBgFgToUserSelectedStudy(selectedStudy, True)

        if firstDisplayPetLabels:
          selectedStudy.GetPETLabelVolumeNode().GetDisplayNode().SetAndObserveColorNodeID(currentReport.GetFindingTypesColorTableNodeID())
        
        if firstDisplayCt:
          selectedStudy.GetCTVolumeNode().GetScalarVolumeDisplayNode().SetAutoWindowLevel(0)
          selectedStudy.GetCTVolumeNode().GetScalarVolumeDisplayNode().SetAndObserveColorNodeID("vtkMRMLColorTableNodeGrey")
          selectedStudy.GetCTVolumeNode().GetScalarVolumeDisplayNode().SetWindowLevel(250,50)
        
          
        if selectedStudy.GetPETVolumeNode().GetScalarVolumeDisplayNode().GetColorNodeID() == selectedStudy.GetCTVolumeNode().GetScalarVolumeDisplayNode().GetColorNodeID():
            selectedStudy.GetPETVolumeNode().GetScalarVolumeDisplayNode().SetAndObserveColorNodeID("vtkMRMLPETProceduralColorNodePET-Heat")
          
        if firstDisplayPet:
          selectedStudy.GetPETVolumeNode().GetScalarVolumeDisplayNode().SetAutoWindowLevel(True)

      
        viewNode = ViewHelper.getStandardViewNode()
        if viewNode.GetBoxVisible() != 0:
          viewNode.SetBoxVisible(0)
        
        
        if selectedStudy.GetVolumeRenderingDisplayNode() == None:
          vrDisplayNode = self.vrLogic.CreateVolumeRenderingDisplayNode()
          slicer.mrmlScene.AddNode(vrDisplayNode)
          if (vrDisplayNode != None) & (viewNode != None):
            vrDisplayNode.SetCroppingEnabled(0)
            pow = self.getStudySelectionWidget().property('opacityPow')
            vrDisplayNode.SetAttribute("OpacityPow",str(pow))
          
            petVolume = selectedStudy.GetPETVolumeNode()
            if petVolume:
              vrDisplayNode.SetAndObserveVolumeNodeID(petVolume.GetID())
              vrDisplayNode.SetName(petVolume.GetName() +"_VR_Display")
              self.vrLogic.UpdateDisplayNodeFromVolumeNode(vrDisplayNode, petVolume)  
              
              propNode = vrDisplayNode.GetVolumePropertyNode()
              if propNode: 
                propNode.SetReferenceCount(propNode.GetReferenceCount()-1)
                print "PROPERTY COUNT " + str(vrDisplayNode.GetVolumePropertyNode().GetReferenceCount())
                vrDisplayNode.GetVolumePropertyNode().SetName(petVolume.GetName() + "_VR_VolumeProperty")
                
              roiNode = vrDisplayNode.GetROINode()
              if roiNode:
                roiNode.SetReferenceCount(roiNode.GetReferenceCount()-1)
                vrDisplayNode.GetROINode().SetName(petVolume.GetName() + "_VR_ROI")
                vrDisplayNode.GetROINode().SetDisplayVisibility(False)
                
                ViewHelper.resetThreeDViewsFocalPoints(resetFirst = True)
                

            
            selectedStudy.SetAndObserveVolumeRenderingDisplayNodeID(vrDisplayNode.GetID())
            vrDisplayNode.AddViewNodeID(viewNode.GetID())
            self.updateOpacityPow(vrDisplayNode, pow)
      
        if viewNode:
          viewNode.InvokeEvent(slicer.vtkMRMLViewNode.ResetFocalPointRequestedEvent)
          viewNode.Modified()
        #self.onUpdateVolumeRendering(selectedStudy.GetPETVolumeNode())
        self.getStudySelectionWidget().selectStudyInRow(idx)       
        self.changeSelectedStudy(currentReport.GetIndexOfSelectedStudy(selectedStudy))
      
      if self.viewNodeObserverID == -1:
        self.viewNodeObserverID = viewNode.AddObserver(vtk.vtkCommand.ModifiedEvent, self.viewNodeModified) 
                   
   
  
  def manageVRDisplayNodesVisibility(self, selectedStudy):
    
    viewNode = ViewHelper.getStandardViewNode()
    currentReport = self.getCurrentReport()
    
    if currentReport:
      for i in range(currentReport.GetNumberOfStudyNodeIDs()):
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
              
          if self.getStudySelectionWidget().property('volumeRendering'):
            selVrDisplayNode.VisibilityOn()
            viewNode.SetAxisLabelsVisible(True)
          else:
            selVrDisplayNode.VisibilityOff()
            viewNode.SetAxisLabelsVisible(False)
          
          if self.getStudySelectionWidget().property('spinView') & self.getStudySelectionWidget().property('volumeRendering'):
            viewNode.SetAnimationMode(viewNode.Spin)
          else:
            viewNode.SetAnimationMode(viewNode.Off)
             
    viewNode.Modified() 
      
    
        
  def onStudyDeselected(self, idx):
    currentReport = self.getReportSelector().currentNode()
    if currentReport:
      study = currentReport.GetStudy(idx)
      if study:
        study.SetSelectedForSegmentation(False)
        
        if currentReport.IsStudyInUse(study):
          qt.QMessageBox.question(None,ViewHelper.moduleDialogTitle(),"Segmentations have been performed on this Study!")
      
      lastSelectedStudy = currentReport.GetSelectedStudyLast()
      self.changeSelectedStudy(currentReport.GetIndexOfSelectedStudy(lastSelectedStudy))   
      
      
  def onCurrentReportChanged(self, reportNode):
    
    for report in self.reportNodesList:
      ViewHelper.hideReport(report)
    

    if reportNode:
      try:
        self.reportNodesList.index(reportNode)
      except:
        self.onReportNodeAdded(reportNode)  
        self.reportNodesList.append(reportNode)
    
    self.getStudySelectionWidget().setReportNode(reportNode)
    self.getFindingSelectionWidget().setReportNode(reportNode)
    self.getAnalysisSettingsWidget().setReportNode(reportNode)
    self.getReportTableWidget().setReportNode(reportNode)
    
    if reportNode:
      self.updateBgFgToUserSelectedStudy(reportNode.GetUserSelectedStudyNode())
      
      self.getFindingSelector().disconnect('currentNodeChanged(vtkMRMLNode*)', self.onFindingNodeChanged)
            
      self.getFindingSelector().setMRMLScene(None)  
       
      for i in xrange(reportNode.GetNumberOfFindingNodeIDs()):
        finding = reportNode.GetFinding(i)
        if finding:
          finding.SetHideFromEditors(False)

      self.getFindingSelector().setMRMLScene(slicer.mrmlScene)
      
      self.getFindingSelector().connect('currentNodeChanged(vtkMRMLNode*)', self.onFindingNodeChanged)
      
      self.getFindingSelector().setCurrentNode(reportNode.GetUserSelectedFindingNode())
          
      
    
    else:
      self.updateBgFgToUserSelectedStudy(None)  
      
    self.manageVolumeRenderingVisibility()
    self.manageModelsVisibility()
    self.onManageFindingROIsVisibility()
    self.manageCollapsibleButtonsAbility(reportNode,vtk.vtkCommand.ModifiedEvent)
    
    
    
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
      for i in range(currentReport.GetNumberOfStudyNodeIDs()):
        study = currentReport.GetStudy(i)
        if study:
          vrdn = study.GetVolumeRenderingDisplayNode()
          if vrdn:
            vrdn.SetVisibility(on)
      
      ViewHelper.getStandardViewNode().SetAxisLabelsVisible(on)
      for vn in ViewHelper.getCompareViewNodes():
        vn.SetAxisLabelsVisible(on)      
      
      
  
  def getTempCroppedVolume(self):
    if self.tempCroppedVol == None:
      self.tempCroppedVol = slicer.mrmlScene.AddNode(slicer.vtkMRMLScalarVolumeNode())
      self.tempCroppedVol.SetName("TempCroppedVolume")
      self.tempCroppedVol.SetHideFromEditors(False)
      self.tempCroppedVol.SetSaveWithScene(False)

    return self.tempCroppedVol     
   
  def getTempLabelVolume(self):
    if self.tempLabelVol == None:
      self.tempLabelVol = slicer.mrmlScene.AddNode(slicer.vtkMRMLScalarVolumeNode())
      self.tempLabelVol.SetName("TempLabelVolume")
      self.tempLabelVol.SetHideFromEditors(False) 
      self.tempLabelVol.SetSaveWithScene(False)
      
    return self.tempLabelVol     

  def onSegmentationROIModified(self, caller, event):
    
    #if caller != self:
      #minFloat = SYS.float_info.min
      #xyz = [minFloat,minFloat,minFloat]
      
      #caller.GetXYZ(xyz)
 
      #if (xyz[0] == self.segmentationROIOldPosition[0]) & (xyz[1] == self.segmentationROIOldPosition[1]) & (xyz[2] == self.segmentationROIOldPosition[2]):
        #return
    
      #else:
        #self.segmentationROIOldPosition = xyz
    
    currentStudy = self.getCurrentStudy()
    currentFinding = self.getCurrentFinding()
    
    setLookupTable = False
    
    if (currentStudy != None) & (currentFinding != None):

      if self.getTempCroppedVolume().GetScalarVolumeDisplayNode() == None:
        setLookupTable = True
          
      #self.getTempCroppedVolume().Copy(currentStudy.GetPETVolumeNode())
      self.getTempCroppedVolume().SetName("LongitudinalPETCT_CroppedVolume") 
    
      self.getTempLabelVolume().Copy(currentStudy.GetPETLabelVolumeNode())
      self.getTempLabelVolume().SetName("LongitudinalPETCT_CroppedLabelVolume")
      
      cropLogic = slicer.modules.cropvolume.logic()
      
      # temporary ROI in order to not invoke modified events with original roi
      croppingROI = slicer.vtkMRMLAnnotationROINode()
      croppingROI.Copy(currentFinding.GetSegmentationROINode())
      
      cropLogic.CropVoxelBased(croppingROI,currentStudy.GetPETVolumeNode(),self.getTempCroppedVolume())
      cropLogic.CropVoxelBased(croppingROI,currentStudy.GetPETLabelVolumeNode(),self.tempLabelVol)
      
      #///
      self.getTempCroppedVolume().SetAndObserveTransformNodeID(None)
      self.getTempLabelVolume().SetAndObserveTransformNodeID(None)
            
      currentFinding.GetSegmentationROINode().SetAndObserveTransformNodeID(None)
      
      propagate = caller == self
      ViewHelper.SetRYGBgFgLblVolumes(self.getTempCroppedVolume().GetID(),None,self.getTempLabelVolume().GetID(),propagate)  
      
      petDisplayNode = currentStudy.GetPETVolumeNode().GetScalarVolumeDisplayNode()
      croppedPETDisplayNode = self.getTempCroppedVolume().GetScalarVolumeDisplayNode()
      
      croppedPETDisplayNode.SetAndObserveColorNodeID(petDisplayNode.GetColorNodeID())
        
      croppedPETDisplayNode.SetAutoWindowLevel(False)
      croppedPETDisplayNode.SetWindow(petDisplayNode.GetWindow())
      croppedPETDisplayNode.SetLevel(petDisplayNode.GetLevel())
                
        
    
    self.getTempLabelVolume().GetDisplayNode().SetAndObserveColorNodeID(self.getCurrentReport().GetFindingTypesColorTableNodeID())
        

  def onEnterEditMode(self,enter):
    
    self.getEditorWidget().toolsColor.colorSpin.connect('valueChanged(int)',self.onEditorColorWarning)
      
    currentStudy = self.getCurrentStudy()
    currentFinding = self.getCurrentFinding()
      
    if (currentStudy != None) & (currentFinding != None) & (enter == True):
     
      self.reportsCollapsibleButton.setProperty('enabled',False)
      self.studiesCollapsibleButton.setProperty('enabled',False)
      self.getReportTableWidget().setProperty('enabled',False)
     
      self.onSegmentationROIModified(self, slicer.vtkMRMLAnnotationROINode.ValueModifiedEvent)
      
      studySeg = currentFinding.GetSegmentationMappedByStudyNodeID(currentStudy.GetID())
      
      if studySeg != None:
        SegmentationHelper.pasteFromMainToCroppedLabelVolume(studySeg.GetLabelVolumeNode(), self.getTempLabelVolume(), currentFinding.GetColorID())  
    
      self.getEditorWidget().setMasterNode(self.getTempCroppedVolume()) 
      self.getEditorWidget().setMergeNode(self.getTempLabelVolume())      
      
      self.getEditorWidget().editUtil.getParameterNode().SetParameter('storedLabel',"")
      self.getEditorWidget().editUtil.getParameterNode().SetParameter('label',str(currentFinding.GetColorID()))
    
      self.getEditorWidget().enter()  
    
      self.tempObserverEditorTag = currentFinding.GetSegmentationROINode().AddObserver(vtk.vtkCommand.ModifiedEvent, self.onSegmentationROIModified)      

    elif enter == False:
      self.getEditorWidget().toolsColor.colorSpin.disconnect('valueChanged(int)',self.onEditorColorWarning)
      self.getEditorWidget().exit()   

      pasted = self.pasteFromCroppedToMainLblVolume()

      studySeg = currentFinding.GetSegmentationMappedByStudyNodeID(currentStudy.GetID())

      if studySeg:
        vrdn = currentStudy.GetVolumeRenderingDisplayNode()

        if ViewHelper.getSetting('Models') & (vrdn.GetClassName() != 'vtkMRMLNCIRayCastVolumeRenderingDisplayNode'):
          qt.QTimer.singleShot(0,self.makeModels)

        elif ViewHelper.getSetting('Models'):
          self.getUnsupportedVolumeRenderingMessageBox.show()  
      
        qt.QTimer.singleShot(0,self.calculateSUVs)


      #if self.getStudySelectionWidget().property('centeredSelected'):
      currentFinding.GetSegmentationROINode().SetAndObserveTransformNodeID(currentStudy.GetCenteringTransformNodeID())
        
      self.updateBgFgToUserSelectedStudy(currentStudy,True)

      roiXYZ = ViewHelper.getROIPositionInRAS(currentFinding.GetSegmentationROINode())
      ViewHelper.setSliceNodesCrossingPositionRAS(roiXYZ)
      self.reportsCollapsibleButton.setProperty('enabled',True)
      self.studiesCollapsibleButton.setProperty('enabled',True)
      self.getReportTableWidget().setProperty('enabled',True)
    
  
     
                 
  def pasteFromCroppedToMainLblVolume(self):
    

    currentStudy = self.getCurrentStudy()
    currentFinding = self.getCurrentFinding()
    
    pasted = False
    
    if (currentStudy != None) & (currentFinding != None):
      segmentationROI = currentFinding.GetSegmentationROINode()

      if segmentationROI:
        segmentationROI.RemoveObserver(self.tempObserverEditorTag)
        self.tempObserverEditorTag = None
     
      studySeg = None
     
      if currentFinding.GetSegmentationMappedByStudyNodeID(currentStudy.GetID()) == None:

        a = time.time()
        studySeg = slicer.mrmlScene.CreateNodeByClass('vtkMRMLLongitudinalPETCTSegmentationNode')
        studySeg.SetReferenceCount(studySeg.GetReferenceCount()-1) 
        slicer.mrmlScene.AddNode(studySeg)

        if ViewHelper.getSetting('Models'):
          mh = slicer.mrmlScene.AddNode(slicer.vtkMRMLModelHierarchyNode())
          studySeg.SetAndObserveModelHierarchyNodeID(mh.GetID())
        
        studySeg.SetAndObserveLabelVolumeNodeID(currentStudy.GetPETLabelVolumeNode().GetID())

        currentFinding.MapStudyNodeIDToSegmentationNodeID(currentStudy.GetID(),studySeg.GetID())

        b = time.time()
        print "DURATION OF SEGMENTATION CREATION: " + str(b-a)+  " s" 
      else:
        studySeg = currentFinding.GetSegmentationMappedByStudyNodeID(currentStudy.GetID())        
      
      a = time.time()
      SegmentationHelper.removeSegmentationFromVolume(studySeg.GetLabelVolumeNode(), currentFinding.GetColorID())       
      b = time.time()
      print "DURATION OF SEGMENTATION REMOVAL: " + str(b-a)+  " s" 
      pasted = SegmentationHelper.pasteFromCroppedToMainLabelVolume(self.getTempLabelVolume(), studySeg.GetLabelVolumeNode(), currentFinding.GetColorID())    
 
      if segmentationROI:
        xyz = [0.,0.,0.]
        radius = [0.,0.,0.]
        segmentationROI.GetXYZ(xyz)
        segmentationROI.GetRadiusXYZ(radius)
        studySeg.SetROIxyz(xyz)
        studySeg.SetROIRadius(radius)
      
      if SegmentationHelper.containsSegmentation(studySeg.GetLabelVolumeNode(),currentFinding.GetColorID()) == False:
        currentFinding.RemoveStudyNodeIDToSegmentationNodeIDMap(currentStudy.GetID())
        
        mh = studySeg.GetModelHierarchyNode()
        if mh:
          mn = mh.GetModelNode()
          if mn:
            mdn = mn.GetModelDisplayNode()
            slicer.mrmlScene.RemoveNode(mdn)
          slicer.mrmlScene.RemoveNode(mn)
              
        slicer.mrmlScene.RemoveNode(studySeg)
        
    return pasted 
  
  def onShowFindingSettingsDialog(self, findingNode):
          
    accepted = False
    if findingNode:
      currentReport = self.getCurrentReport()
      if currentReport:
        currentReport.SetUserSelectedFindingNodeID(findingNode.GetID()) 
        segROI = findingNode.GetSegmentationROINode()
        if (segROI != None) & (self.getCurrentStudy() != None):
          self.getCurrentStudy().SetAndObserveSegmentationROINodeID(segROI.GetID())
          
        
        self.getFindingSettingsDialog().setReportNode(currentReport)      
        result = self.getFindingSettingsDialog().exec_()

        accepted = result == qt.QDialog.Accepted

    return accepted    
   
    
  def onFindingNodeChanged(self, findingNode):

    currentReport = self.getCurrentReport()
   
    if currentReport:   
      idx = 0
            
      if findingNode:
        idx = currentReport.GetIndexOfFindingNodeID(findingNode.GetID())
      
      if idx == -1:
        applied = self.onShowFindingSettingsDialog(findingNode)

        if applied:
          currentReport.AddFindingNodeID(findingNode.GetID())
          currentReport.SetUserSelectedFindingNodeID(findingNode.GetID())

        else: 
          self.getFindingSelector().disconnect('currentNodeChanged(vtkMRMLNode*)', self.onFindingNodeChanged)
          slicer.mrmlScene.RemoveNode(findingNode)

          if self.getFindingSelector().currentNode():
            currentReport.SetUserSelectedFindingNodeID(self.getFindingSelector().currentNode().GetID())
          else:
            currentReport.SetUserSelectedFindingNodeID(None)
   
          self.getFindingSelector().connect('currentNodeChanged(vtkMRMLNode*)', self.onFindingNodeChanged)
    
          return
                 
          
      else:
        if findingNode:
          currentReport.SetUserSelectedFindingNodeID(findingNode.GetID())
        else:
          currentReport.SetUserSelectedFindingNodeID(None)
    
    if currentReport.GetUserSelectedFindingNode():   
      segROI = currentReport.GetUserSelectedFindingNode().GetSegmentationROINode()
      
      currentStudy = self.getCurrentStudy()
      if (segROI != None) & (currentStudy != None):
        self.getEditorWidget().editLabelMapsFrame.setEnabled(True)
        print "Setting SegmentationROI "+ segROI.GetName() + " to Study "+ currentStudy.GetName()
        currentStudy.SetAndObserveSegmentationROINodeID(segROI.GetID())
        self.updateSegmentationROIPosition()
      else:
        self.getEditorWidget().editLabelMapsFrame.setEnabled(False)
        
      
      if self.isQuantitativeViewActive():
        self.updateQuantitativeView(currentReport.GetUserSelectedFindingNode())    
        
    else:
      self.getEditorWidget().editLabelMapsFrame.setEnabled(False)
      if self.getCurrentStudy():
        self.getCurrentStudy().SetAndObserveSegmentationROINodeID("") # TODO: check why None not possible    

    self.onManageFindingROIsVisibility()

    if self.isQualitativeViewActive() | self.isQuantitativeViewActive():
      self.manageModelsVisibility(self.isQuantitativeViewActive())

     
  def onFindingNodeToBeRemoved(self, findingNode):
    currentReport = self.getReportSelector().currentNode()
    if currentReport:
            
      for i in range(currentReport.GetNumberOfSelectedStudies()):
        study = currentReport.GetStudy(i)
        
        seg = findingNode.GetSegmentationMappedByStudyNodeID(study.GetID())
        if seg:
          SegmentationHelper.removeSegmentationFromVolume(seg.GetLabelVolumeNode(), findingNode.GetColorID())
          findingNode.RemoveStudyNodeIDToSegmentationNodeIDMap(study.GetID())
          
          if seg.GetModelHierarchyNode():
            ViewHelper.removeModelHierarchyAndChildModelNodesFromScene(seg.GetModelHierarchyNode())
          slicer.mrmlScene.RemoveNode(seg)
          
          
      slicer.mrmlScene.RemoveNode(findingNode.GetSegmentationROINode())
      currentReport.RemoveFindingNodeID(findingNode.GetID())  
   
  def onManageFindingROIsVisibility(self):
    currentStudy = self.getCurrentStudy()
    currentFinding = self.getCurrentFinding()
    
    if (currentStudy != None) & (currentFinding != None):
      
      nrOfFindings = self.getCurrentReport().GetNumberOfFindingNodeIDs()
      
      for x in range(nrOfFindings):
        finding = self.getCurrentReport().GetFinding(x)
        if finding: 
          roi = finding.GetSegmentationROINode()
          if roi:
            if (finding == currentFinding) & (self.isStandardViewActive()):
              # if editor mode active, observer hast to be removed from roi in order to not delete not yet added segmentation 
              if self.isEditorModeActive() & (self.tempObserverEditorTag != None):
                roi.RemoveObserver(self.tempObserverEditorTag)
        
              roi.SetDisplayVisibility(self.getFindingSelectionWidget().property('roiVisibility'))
              roi.Modified()
        
              # reset observer after visibility change
              if self.isEditorModeActive() & (self.tempObserverEditorTag != None):
                self.tempObserverEditorTag = roi.AddObserver(vtk.vtkCommand.ModifiedEvent, self.onSegmentationROIModified)      
   
            else:
              roi.SetDisplayVisibility(0)     
              roi.Modified()              
             
             
  def getCurrentReport(self):
    return self.getReportSelector().currentNode()  
  
  def getCurrentStudy(self):
    currentReport = self.getCurrentReport()
    if currentReport:
      return currentReport.GetUserSelectedStudyNode()
    
    return None    
  
  def getCurrentFinding(self):
    currentReport = self.getCurrentReport()
    if currentReport:
      return currentReport.GetUserSelectedFindingNode()
    
    return None    


  def viewNodeModified(self, caller, event):
    
    stdViewNode = ViewHelper.getStandardViewNode()
    compareViewNodes = ViewHelper.getCompareViewNodes()
    
    if caller == stdViewNode:
      if stdViewNode.GetAnimationMode() == slicer.vtkMRMLViewNode.Spin:
        self.getStudySelectionWidget().setProperty('spinView',True)
      else:
        self.getStudySelectionWidget().setProperty('spinView',False)
    
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
        self.getAnalysisSettingsWidget().setProperty('spinView',True)    
      elif allOff:
        self.getAnalysisSettingsWidget().setProperty('spinView',False)  
      
        
  
  def makeModels(self):
    a = time.time()
    currentStudy = self.getCurrentStudy()
    currentFinding = self.getCurrentFinding()
    
    if (currentStudy != None) & (currentFinding != None):
     
      currentSeg = currentFinding.GetSegmentationMappedByStudyNodeID(currentStudy.GetID())
      
      if currentSeg:
        labelVolume = currentSeg.GetLabelVolumeNode()

        #create a temporary model hierarchy for generating models
        tempMH = slicer.vtkMRMLModelHierarchyNode()
        slicer.mrmlScene.AddNode(tempMH)
      
        colorTable = self.getCurrentReport().GetFindingTypesColorTableNode()
        
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
                  if currentSeg.GetModelHierarchyNode():
                    ViewHelper.removeModelHierarchyAndChildModelNodesFromScene(currentSeg.GetModelHierarchyNode())
                  
                  hnode.SetName(currentSeg.GetName()+"_Model")
                  currentSeg.SetAndObserveModelHierarchyNodeID(hnode.GetID())
                  modelNode.SetName(labelVolume.GetName() + "_" + currentFinding.GetName()+"_M")
                  if modelNode.GetDisplayNode():
                    modelNode.GetDisplayNode().SetName(labelVolume.GetName() + "_" + currentFinding.GetName()+"_D")
                    modelNode.GetDisplayNode().AddViewNodeID(ViewHelper.getStandardViewNode().GetID())
                  hnode.SetName(labelVolume.GetName() + "_" + currentFinding.GetName()+"_H")
                  modelNode.SetHideFromEditors(False)
                else:
                  currentSeg.SetAndObserveModelHierarchyNodeID(None)
                  slicer.mrmlScene.RemoveNode(modelNode)   
                  slicer.mrmlScene.RemoveNode(hnode)
   
          slicer.mrmlScene.RemoveNode(tempMH.GetDisplayNode())
          slicer.mrmlScene.RemoveNode(tempMH)         
        
    b = time.time()
    print "DURATION OF MAKE MODELS: " + str(b-a)+  "s"            
        
   
  def calculateSUVs(self):
    
    currentStudy = self.getCurrentStudy()
    currentFinding = self.getCurrentFinding()
        
    if (currentStudy != None) & (currentFinding != None):
         
      stats = LabelStatisticsLogic(currentStudy.GetPETVolumeNode(), currentStudy.GetPETLabelVolumeNode())
      idx = None
      idx = stats.labelStats["Labels"].index(currentFinding.GetColorID())
        
      if idx:
          idx = stats.labelStats["Labels"][idx]
          values = {}
          values["suvmax"] = stats.labelStats[idx,"Max"]
          values["suvmean"] = stats.labelStats[idx,"Mean"]
          values["suvmin"] = stats.labelStats[idx, "Min"]
          values["volcc"] = stats.labelStats[idx, "Volume cc"]
          values["volmm3"] = stats.labelStats[idx, "Volume mm^3"]
          values["count"] = stats.labelStats[idx, "Count"]
          values["stddev"] = stats.labelStats[idx, "StdDev"]
            
          print values
      
          currentSeg = currentFinding.GetSegmentationMappedByStudyNodeID(currentStudy.GetID())
        
          if currentSeg:
            currentSeg.SetStatistics(values["suvmax"],values["suvmean"],values["suvmin"],values["volcc"],values["volmm3"],values["count"],values["stddev"])        
                   
      else:
        qt.QMessageBox.warning(None, ViewHelper.moduleDialogTitle(),'An error occured during the computation of the SUV values for the segmentation!')      
        
      
  def manageCollapsibleButtonsAbility(self, caller, event):
    reports = False
    studies = False
    findings = False
    analysis = False
    
    currentReport = self.getCurrentReport()
    
    notInEditorMode = self.getEditorWidget().editLabelMapsFrame.property('collapsed')
    
    if notInEditorMode:
      reports = True      
    
    if (currentReport != None) & notInEditorMode:
      studies = True
    
      if currentReport.GetNumberOfSelectedStudies() > 0:
        findings = True
        
        if currentReport.GetNumberOfFindingNodeIDs() > 0:
          for i in range(currentReport.GetNumberOfFindingNodeIDs()):
            finding = currentReport.GetFinding(i)
            if finding:
              if (finding.GetNumberOfSegmentations() > 0) & notInEditorMode:
                analysis = True      
                   
    self.reportsCollapsibleButton.setProperty('enabled',reports)                  
    self.studiesCollapsibleButton.setProperty('enabled',studies)
    self.findingsCollapsibleButton.setProperty('enabled',findings | (notInEditorMode == False))  
    self.analysisCollapsibleButton.setProperty('enabled',analysis) 
    
  
       
  
  def onReportNodeAdded(self, reportNode):
    id = reportNode.AddObserver(vtk.vtkCommand.ModifiedEvent, self.manageCollapsibleButtonsAbility)
    self.reportObserverIDs[reportNode] = id
    #self.manageCollapsibleButtonsAbility(self, vtk.vtkCommand.ModifiedEvent)
          
  def manageVolumeRenderingVisibility(self):
    currentReport = self.getCurrentReport()
    currentStudy = self.getCurrentStudy()
        
    stdVolRen = self.getStudySelectionWidget().property('volumeRendering')
    stdSpin = self.getStudySelectionWidget().property('spinView')
    
    anaVolRen = self.getAnalysisSettingsWidget().property('volumeRendering') 
    anaSpin = self.getAnalysisSettingsWidget().property('spinView')
    
    viewNode = ViewHelper.getStandardViewNode()
    
    if currentReport:
    # remove viewnode from all studies and set them invisible
      for i in xrange(currentReport.GetNumberOfStudyNodeIDs()):
        study = currentReport.GetStudy(i)
        if study:
          vrdn = study.GetVolumeRenderingDisplayNode()
          if vrdn:
            if vrdn.IsViewNodeIDPresent(viewNode.GetID()):
              vrdn.RemoveViewNodeID(viewNode.GetID())
            vrdn.SetVisibility(False)    
    
      # collect all VR volumedisplaynodes which should be made visible
      for i in xrange(currentReport.GetNumberOfSelectedStudies()):
        study = currentReport.GetSelectedStudy(i)  
        if study:
          vrdn = study.GetVolumeRenderingDisplayNode()
          if (vrdn != None) & self.isStandardViewActive() & (study == currentStudy):
            vrdn.AddViewNodeID(viewNode.GetID())
            pow = None
            pow = float(vrdn.GetAttribute("OpacityPow"))
            if pow:
              self.getStudySelectionWidget().setProperty('opacityPow',pow)
              viewNode.Modified()
          
          if (vrdn != None) & (self.isStandardViewActive() & stdVolRen & (study == currentStudy)) | ( (self.isStandardViewActive() == False) & anaVolRen):
            vrdn.SetVisibility(True)
                      
      if currentReport.GetNumberOfSelectedStudies() > 0:
        viewNode.SetAxisLabelsVisible(stdVolRen)   
        ViewHelper.spinStandardViewNode(stdSpin)

      else:
        viewNode.SetAxisLabelsVisible(False)
        #ViewHelper.spinStandardViewNode(False)
    
      if self.isQualitativeViewActive() | self.isQuantitativeViewActive():
        ViewHelper.spinCompareViewNodes(anaSpin)
      
        compareViewNodes = ViewHelper.getCompareViewNodes()
        for j in range(len(compareViewNodes)):
          if (j < currentReport.GetNumberOfSelectedStudiesSelectedForAnalysis()) & (anaVolRen == True):
            compareViewNodes[j].SetAxisLabelsVisible(True)
          else:
            compareViewNodes[j].SetAxisLabelsVisible(False)       
          
  
  def manageModelsVisibility(self, findingRestricted = False):
    currentReport = self.getCurrentReport()
    currentStudy = self.getCurrentStudy()

    if currentStudy:
      for i in range(currentReport.GetNumberOfFindingNodeIDs()):
        finding = currentReport.GetFinding(i)
        for j in range(currentReport.GetNumberOfSelectedStudies()):
          study = currentReport.GetSelectedStudy(j)
          
          seg = finding.GetSegmentationMappedByStudyNodeID(study.GetID())
          if seg:
            mh = seg.GetModelHierarchyNode()
            if mh:
              mn = mh.GetModelNode()
              if mn:
                stdViewShow = self.isStandardViewActive() & (study == currentStudy)
                qualViewShow = self.isQualitativeViewActive()
                quantViewShow = self.isQuantitativeViewActive() & ( (findingRestricted == False) | ( (findingRestricted == True) &  (finding == self.getCurrentFinding()))) 
                if (seg.GetModelVisible() == True) & (stdViewShow | qualViewShow | quantViewShow):
                  mn.SetDisplayVisibility(True)
                else:
                  mn.SetDisplayVisibility(False)
   
   
  def manageVolumeRenderingCompareViewNodeIDs(self):
    currentReport = self.getCurrentReport()
    
    compareViewNodes = ViewHelper.getCompareViewNodes()
    qualView = self.analysisCollapsibleButton.property('collapsed') != True
    
    if currentReport:
      for i in range(currentReport.GetNumberOfSelectedStudies()):
        study = currentReport.GetSelectedStudy(i)  
       
        vrdn = study.GetVolumeRenderingDisplayNode()
        if vrdn:
          for cvn in compareViewNodes:
            if vrdn.IsViewNodeIDPresent(cvn.GetID()):
              vrdn.RemoveViewNodeID(cvn.GetID())
              
            id = currentReport.GetIndexOfSelectedStudySelectedForAnalysis(study)
            if (id >= 0) & (id < len(compareViewNodes)) & (self.isQualitativeViewActive() | self.isQuantitativeViewActive()):
              vrdn.AddViewNodeID(compareViewNodes[id].GetID())
  
  
  def isStandardViewActive(self):
    return (self.isQualitativeViewActive() == False) & (self.isQuantitativeViewActive() == False)                
  
  def isQualitativeViewActive(self):
    return (self.analysisCollapsibleButton.property('collapsed') == False) & self.getAnalysisSettingsWidget().property('qualitativeChecked')
         
  def isQuantitativeViewActive(self):
    return (self.analysisCollapsibleButton.property('collapsed') == False) & self.getAnalysisSettingsWidget().property('quantitativeChecked')
         
  def isEditorModeActive(self):
    return self.getEditorWidget().editLabelMapsFrame.property('collapsed') == False
  
                      
  def manageModelDisplayCompareViewNodeIDs(self):
    
    currentReport = self.getCurrentReport()
    compareViewNodes = ViewHelper.getCompareViewNodes()
    
    if currentReport:
      for i in range(currentReport.GetNumberOfFindingNodeIDs()):
        finding = currentReport.GetFinding(i)
        for j in range(currentReport.GetNumberOfSelectedStudies()):
          study = currentReport.GetSelectedStudy(j)
          
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
                  
                  id = currentReport.GetIndexOfSelectedStudySelectedForAnalysis(study)
                  if (id >= 0) & (id < len(compareViewNodes)) & (self.isQualitativeViewActive() | self.isQuantitativeViewActive()):
                    mdn.AddViewNodeID(compareViewNodes[id].GetID())         
    
  
  def onStudyForAnalysisChanged(self):
    if self.isQualitativeViewActive():
      self.showQualitativeView()
    if self.isQuantitativeViewActive():
      self.showQuantitativeView()
                           
  
  def onSwitchToAnalysis(self, analysisCollapsed):
    
    # if analysis panel is opened first time volumerendering and spinning settings are adopted from study selection panel
    if self.unopenedAnalysis:
      self.getAnalysisSettingsWidget().setProperty('volumeRendering',self.getStudySelectionWidget().property('volumeRendering'))
      self.getAnalysisSettingsWidget().setProperty('spinView',self.getStudySelectionWidget().property('spinView'))
      self.unopenedAnalysis = False   
      
    if analysisCollapsed:
      self.getFindingSelectionWidget().setProperty('roiVisibility',self.findingROIVisiblityBackup)
      
      lm = slicer.app.layoutManager()
      if lm:
        lm.setLayout(slicer.vtkMRMLLayoutNode.SlicerLayoutFourUpView)
      
      self.onManageFindingROIsVisibility()
      
      self.manageVolumeRenderingCompareViewNodeIDs()
      self.manageVolumeRenderingVisibility()
      self.manageModelDisplayCompareViewNodeIDs()
      self.manageModelsVisibility()
      
      self.getReportTableWidget().setColumnSelectableOn() 

    else:
    
      #self.setVolumeRendering(self.analysisSettingsWidget.property('volumeRendering'))
      roiNodes = ViewHelper.getROINodesCollection()
      for i in range(roiNodes.GetNumberOfItems()):
        roi = roiNodes.GetItemAsObject(i)  
        if roi: 
          roi.SetDisplayVisibility(0)
          roi.Modified()
    
      if self.getAnalysisSettingsWidget().property('qualitativeChecked'):
        self.showQualitativeView()      
      
      elif self.getAnalysisSettingsWidget().property('quantitativeChecked'):  
        self.showQuantitativeView()           
      #self.manageVRDisplayNodesVisibility(None) 
  
  def showQuantitativeView(self):
    
    self.getReportTableWidget().setAllSelectableOn()
     
    currentLayoutID = ViewHelper.updateQuantitativeViewLayout(self.getCurrentReport().GetNumberOfSelectedStudiesSelectedForAnalysis())
    
    self.manageVolumeRenderingCompareViewNodeIDs()
    self.manageVolumeRenderingVisibility()
    self.manageModelDisplayCompareViewNodeIDs()

    self.manageModelsVisibility()

    currentReport = self.getCurrentReport()
        
    if self.chartNode == None:
      self.chartNode = slicer.mrmlScene.AddNode(slicer.vtkMRMLChartNode())
      self.chartNode.SetSaveWithScene(False)
      self.chartNode.SetProperty('default', 'yAxisLabel', 'SUVbw')
      self.chartNode.SetProperty('default', 'type', 'Scatter');
      self.chartNode.SetProperty('default', 'showLegend', 'on') 
      
    chartViewNode = ViewHelper.getStandardChartViewNode() 
    chartViewNode.SetChartNodeID(self.chartNode.GetID())
          
    self.updateQuantitativeView()  
            
          
  def updateQuantitativeView(self, finding = None):
    
    currentReport = self.getCurrentReport()
    
    if finding:
      self.getReportTableWidget().setRowSelectableOn()
    
    for can in self.chartArrayNodes:
      self.chartNode.RemoveArray(can.GetName())
      slicer.mrmlScene.RemoveNode(can)
    
    if self.chartArrayNodes:
      del self.chartArrayNodes[:]
      
    if currentReport:
      arrayNodeNames = ['SUV<span style=\"vertical-align:sub;font-size:80%;\">MAX</span>','SUV<span style=\"vertical-align:sub;font-size:80%;\">MEAN</span>','SUV<span style=\"vertical-align:sub;font-size:80%;\">MIN</span>']
      saturationMultipliers = [1.0,0.75,0.5] # should be as many as different arraynodes
      colorTable = currentReport.GetFindingTypesColorTableNode()
      lut = colorTable.GetLookupTable()
      suvs = []
      findings = []
      
      if finding == None:
        for i in xrange(currentReport.GetNumberOfFindingNodeIDs()):
          fnd = currentReport.GetFinding(i)
          if fnd:
            findings.append(fnd)
      else:
        findings.append(finding)
         
      for finding in findings:
        
        rgba = lut.GetTableValue(finding.GetColorID())
      
        for i in xrange(len(arrayNodeNames)):
      
          arrayNode = slicer.mrmlScene.AddNode(slicer.vtkMRMLDoubleArrayNode())
          arrayNode.SetSaveWithScene(False)
          arrayNode.SetName(finding.GetName()+" "+arrayNodeNames[i])
          self.chartArrayNodes.append(arrayNode)
      
          array = arrayNode.GetArray()
          samples = currentReport.GetNumberOfSelectedStudiesSelectedForAnalysis()
          tuples = samples
          array.SetNumberOfTuples(tuples)
          tuple = 0
       
          t = 0
          
          for j in xrange(samples):
            
            study = currentReport.GetSelectedStudySelectedForAnalysis(j)

            if j > 0:
              oldstudy = currentReport.GetSelectedStudySelectedForAnalysis(j-1)
              if (study != None) & (oldstudy != None):
                
                date = ViewHelper.dateFromDICOM(study.GetAttribute('DICOM.StudyDate'))
                olddate = ViewHelper.dateFromDICOM(oldstudy.GetAttribute('DICOM.StudyDate'))
                t += (date-olddate).days                                                    
              
            seg = finding.GetSegmentationMappedByStudyNodeID(study.GetID())
            
            del suvs[:]
            suvs.append(0.)
            suvs.append(0.)
            suvs.append(0.)

            if seg:
              suvs[0] = seg.GetSUVMax()
              suvs[1] = seg.GetSUVMean()
              suvs[2] = seg.GetSUVMin()

            array.SetComponent(tuple, 0, t)
            array.SetComponent(tuple, 1, suvs[i])
            array.SetComponent(tuple, 2, 0.)
              
            tuple += 1
              
          self.chartNode.AddArray(arrayNode.GetName(), arrayNode.GetID())
          
          colorStr = ViewHelper.RGBtoHex(rgba[0]*255,rgba[1]*255,rgba[2]*255,saturationMultipliers[i])
          self.chartNode.SetProperty(arrayNode.GetName(), 'color', colorStr)
        
        if len(findings) == 1:
          self.chartNode.SetProperty('default', 'title', 'Longitudinal PET/CT Analysis: '+finding.GetName()+' SUVbw')
      
      
      if len(findings) > 1:
        self.chartNode.SetProperty('default', 'title', 'Longitudinal PET/CT Analysis: All Findings SUVbw')
      
      self.chartNode.SetProperty('default', 'xAxisLabel', 'days')
    
      
    ViewHelper.getStandardChartViewNode().Modified()
                
      
      
  def showQualitativeView(self, show = True):
    
    if show:
      
      self.getReportTableWidget().setAllSelectableOn()
      currentLayoutID = ViewHelper.updateQualitativeViewLayout(self.getCurrentReport().GetNumberOfSelectedStudiesSelectedForAnalysis())
   
      self.manageVolumeRenderingCompareViewNodeIDs()
      self.manageVolumeRenderingVisibility()
      self.manageModelDisplayCompareViewNodeIDs()
      self.manageModelsVisibility()
          
      if self.qualitativeViewLastID != currentLayoutID:
        ViewHelper.SetForegroundOpacity(0.6, True)
        self.qualitativeViewLastID = currentLayoutID
    
      for i in range(self.getCurrentReport().GetNumberOfSelectedStudiesSelectedForAnalysis()):
        study = self.getCurrentReport().GetSelectedStudySelectedForAnalysis(i)
        if study:
          ViewHelper.SetCompareBgFgLblVolumes(self.getCurrentReport().GetIndexOfSelectedStudySelectedForAnalysis(study),study.GetCTVolumeNode().GetID(),study.GetPETVolumeNode().GetID(),study.GetPETLabelVolumeNode().GetID(),True)      
          
      
      ViewHelper.resetThreeDViewsFocalPoints()  
          
          
          

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
          
          self.getReportTableWidget().setProperty('enabled',False)


  def assignNewROIToFinding(self, caller, event):
    
    if caller == slicer.mrmlScene:
      nrOfNodes = slicer.mrmlScene.GetNumberOfNodes()
      lastAddedNode = slicer.mrmlScene.GetNthNode(nrOfNodes-1)
      if lastAddedNode.IsA('vtkMRMLAnnotationROINode') == False:
        return
      
      
    if self.roiPlacementNodeAddedEventObserverID != None:
      roiNodes = ViewHelper.getROINodesCollection()
      roiNode = roiNodes.GetItemAsObject(roiNodes.GetNumberOfItems()-1)
      currentStudy = self.getCurrentStudy()
      currentFinding = self.getCurrentFinding()
      

      if (roiNode != None) & (roiNode != self.lastAddedROINode) & (currentStudy != None) & (currentFinding != None):
        roiXYZ = [0.,0.,0.]
        roiNode.GetXYZ(roiXYZ)
        
        if currentStudy.GetCenteringTransformNode() != None:
          centerTransform = currentStudy.GetCenteringTransformNode()
          centerTransformMatrix = centerTransform.GetMatrixTransformToParent()
          roiXYZ = [roiXYZ[0]-centerTransformMatrix.GetElement(0,3),roiXYZ[1]-centerTransformMatrix.GetElement(1,3),roiXYZ[2]-centerTransformMatrix.GetElement(2,3)] 
          
        if currentFinding.GetSegmentationROINode() == None:

          #adjust roi position if volumes in study are centered so that it's position doesn't get changed when set under transform
          roiNode.SetXYZ(roiXYZ)  
          
          roiNode.SetDisplayVisibility(self.getFindingSelectionWidget().property('roiVisibility'))  
          currentFinding.SetAndObserveSegmentationROINodeID(roiNode.GetID())
          currentStudy.SetAndObserveSegmentationROINodeID(roiNode.GetID())
          
          self.lastAddedROINode = roiNode
          self.getEditorWidget().editLabelMapsFrame.setEnabled(True)
         
        else:

          roiRadius = [0.,0.,0.]
          roiNode.GetRadiusXYZ(roiRadius)
          
          roiInFinding = currentFinding.GetSegmentationROINode()
          roiInFinding.SetXYZ(roiXYZ)
          roiInFinding.SetRadiusXYZ(roiRadius)
          
          if(self.anLogic):
            hn = slicer.vtkMRMLDisplayableHierarchyNode.GetDisplayableHierarchyNode(slicer.mrmlScene,roiNode.GetID())  
            self.anLogic.RemoveAnnotationNode(roiNode)

        
        appLogic = slicer.app.applicationLogic()
        if appLogic:
          intnode = appLogic.GetInteractionNode()
          if intnode:
            slicer.mrmlScene.RemoveObserver(self.roiPlacementNodeAddedEventObserverID)
            self.roiPlacementNodeAddedEventObserverID = None           
   
   
  def endROIPlacement(self, caller, event): 
    appLogic = slicer.app.applicationLogic()
    if appLogic:
      intnode = appLogic.GetInteractionNode()
      if intnode:
        intnode.RemoveObserver(self.roiEndPlacementEventObserverID)
        self.roiEndPlacementEventObserverID = None
          
        self.getFindingSelectionWidget().setProperty('placeROIChecked',False)
        self.getReportTableWidget().setProperty('enabled',True)    
  
  
  def onEditorColorWarning(self):
    currentFinding = self.getCurrentFinding()
    if currentFinding:
      selectedLabelValue = self.getEditorWidget().toolsColor.colorSpin.property('value')
      if (selectedLabelValue > 0) & ( selectedLabelValue != currentFinding.GetColorID() ):     
        self.getSegmentationColorChangeMessageBox().show()
    

  def onSaveResultsCSV(self):
    if not self.fileDialog:
      self.fileDialog = qt.QFileDialog(self.parent)
      self.fileDialog.options = self.fileDialog.DontUseNativeDialog
      self.fileDialog.acceptMode = self.fileDialog.AcceptSave
      self.fileDialog.defaultSuffix = "csv"
      self.fileDialog.setNameFilter("Comma Separated Values (*.csv)")
      self.fileDialog.connect("fileSelected(QString)", self.onFileSelected)
    self.fileDialog.show()
    
    
  def onFileSelected(self,fileName):
    fp = open(fileName, "w")
    fp.write(self.resultsAsCSV())
    fp.close()
        
        
  def resultsAsCSV(self):
    """
    print comma separated value file with header keys in quotes
    """
    currentReport = self.getCurrentReport()
    if currentReport:

      csv = "\"Finding_SUV\""  
      
      for i in xrange(currentReport.GetNumberOfSelectedStudies()):
        study = currentReport.GetSelectedStudy(i)
        if study:
          studyDate = study.GetAttribute('DICOM.StudyDate')
          # inserting dashes for date format
          studyDate = ViewHelper.insertStr(studyDate,'-',6)
          studyDate = ViewHelper.insertStr(studyDate,'-',4)  
        
          csv += ",\""+studyDate+"\""
          
          
      for j in xrange(currentReport.GetNumberOfFindingNodeIDs()):
        finding = currentReport.GetFinding(j)
        
        
        if not finding:
          continue
        
        maxrow = "\n\""+finding.GetName()+"_SUVmax\""
        meanrow = "\n\""+finding.GetName()+"_SUVmean\""
        minrow = "\n\""+finding.GetName()+"_SUVmin\""
        devrow = "\n\""+finding.GetName()+"_StdDev\""
        volccrow = "\n\""+finding.GetName()+"_Volcc\""
        count = "\n\""+finding.GetName()+"_Count\""
        
        for k in xrange(currentReport.GetNumberOfSelectedStudies()):
          study = currentReport.GetSelectedStudy(k)
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
       
   
