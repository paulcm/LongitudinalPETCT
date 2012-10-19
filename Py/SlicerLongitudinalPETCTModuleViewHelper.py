# slicer imports
from __main__ import vtk, slicer, tcl, qt

# python includes
import sys
import time

class SlicerLongitudinalPETCTModuleViewHelper( object ):
  '''
  classdocs
  '''
  
  # TODO: add a capability to control the level of messages
  @staticmethod
  def Info( message ):
    print("[LongitudinalPETCTAnalysis " + time.strftime( "%m/%d/%Y %H:%M:%S" ) + "]: INFO: " + str( message ))
    sys.stdout.flush()


  @staticmethod
  def SetForegroundOpacity(opacity, rYGExcluded = False):
    compositeNodes = slicer.util.getNodes('vtkMRMLSliceCompositeNode*')
    for compositeNode in compositeNodes.values():
      
      if (rYGExcluded == True) & ((compositeNode.GetID() == 'vtkMRMLSliceCompositeNodeRed') | (compositeNode.GetID() == 'vtkMRMLSliceCompositeNodeYellow') | (compositeNode.GetID() == 'vtkMRMLSliceCompositeNodeGreen')):
        continue
      
      compositeNode.SetForegroundOpacity(opacity)  

  @staticmethod
  def SetRYGBgFgLblVolumes(bgID, fgID = None, lblID = None, fit = False):
    
    compositeNodes = []
    
    compositeNodes.append(slicer.util.getNode('vtkMRMLSliceCompositeNodeRed'))
    compositeNodes.append(slicer.util.getNode('vtkMRMLSliceCompositeNodeYellow'))
    compositeNodes.append(slicer.util.getNode('vtkMRMLSliceCompositeNodeGreen'))
    
    for cnode in compositeNodes:
      cnode.SetBackgroundVolumeID( bgID );
      cnode.SetForegroundVolumeID( fgID );
      cnode.SetLabelVolumeID( lblID );
    
    if fit:
      appLogic = slicer.app.applicationLogic()
      if appLogic:
        appLogic.FitSliceToAll()
        
        
  @staticmethod
  def SetCompareBgFgLblVolumes(index, bgID, fgID = None, lblID = None, fit = False):
    
    label = SlicerLongitudinalPETCTModuleViewHelper.compareLabel()
     
    compositeNodes = SlicerLongitudinalPETCTModuleViewHelper.getCompareSliceCompositeNodes(index)
    
    for cnode in compositeNodes:
      cnode.SetBackgroundVolumeID( bgID );
      cnode.SetForegroundVolumeID( fgID );
      cnode.SetLabelVolumeID( lblID );
    
    if fit:
      appLogic = slicer.app.applicationLogic()
      if appLogic:
        appLogic.FitSliceToAll()
    
  @staticmethod
  def SetBgFgLblVolumes(bgID, fgID = None, lblID = None, propagate = True):
    appLogic = slicer.app.applicationLogic()
    selectionNode = appLogic.GetSelectionNode()
    selectionNode.SetReferenceActiveVolumeID(bgID)
    selectionNode.SetReferenceSecondaryVolumeID(fgID)
    selectionNode.SetReferenceActiveLabelVolumeID(lblID)
    
    if fgID:
      compositeNodes = slicer.util.getNodes('vtkMRMLSliceCompositeNode*')
      for compositeNode in compositeNodes.values():
        #if (compositeNode.GetID() == 'vtkMRMLSlicerCompositeNodeRed') | (compositeNode.GetID() == 'vtkMRMLSlicerCompositeNodeYellow') | (compositeNode.GetID() == 'vtkMRMLSlicerCompositeNodeGreen'):
        compositeNode.SetForegroundOpacity(0.6)
          
    if propagate:
      appLogic.PropagateVolumeSelection()

  @staticmethod
  def SetForegroundWindowLevel(window, level):
    appLogic = slicer.app.applicationLogic()
    selectionNode = appLogic.GetSelectionNode()
    fgVolumeNodeID = selectionNode.GetSecondaryVolumeID()
    fgVolumeNode = slicer.mrmlScene.GetNodeByID(fgVolumeNodeID)
    
    if fgVolumeNode:
      fgDisplayNode = fgVolumeNode.GetScalarVolumeDisplayNode()
      if fgDisplayNode:
        fgDisplayNode.SetWindow(window)
        fgDisplayNode.SetLevel(level)
        
  @staticmethod
  def SetBackgroundWindowLevel(window, level):
    appLogic = slicer.app.applicationLogic()
    selectionNode = appLogic.GetSelectionNode()
    bgVolumeNodeID = selectionNode.GetActiveVolumeID()
    bgVolumeNode = slicer.mrmlScene.GetNodeByID(bgVolumeNodeID)
    
    if bgVolumeNode:
      bgDisplayNode = bgVolumeNode.GetScalarVolumeDisplayNode()
      if bgDisplayNode:
        bgDisplayNode.SetWindow(window)
        bgDisplayNode.SetLevel(level)

  @staticmethod
  def SetLabelVolume(lblID):
    appLogic = slicer.app.applicationLogic()
    selectionNode = appLogic.GetSelectionNode()
    selectionNode.SetReferenceActiveLabelVolumeID(label)
    appLogic.PropagateVolumeSelection()

  @staticmethod
  def getEditorParameterNode():
    """Get the Editor parameter node - a singleton in the scene"""
    node = None
    size =  slicer.mrmlScene.GetNumberOfNodesByClass("vtkMRMLScriptedModuleNode")
    for i in xrange(size):
      n  = slicer.mrmlScene.GetNthNodeByClass( i, "vtkMRMLScriptedModuleNode" )
      if n.GetModuleName() == "Editor":
        node = n

    if not node:
      node = slicer.vtkMRMLScriptedModuleNode()
      node.SetSingletonTag( "Editor" )
      node.SetModuleName( "Editor" )
      node.SetParameter( "label", "1" )
      slicer.mrmlScene.AddNode(node)
    return node
  
  
  @staticmethod 
  def getSliceNodesCrossingPositionRAS():
    red = slicer.mrmlScene.GetNodeByID("vtkMRMLSliceNodeRed")
    yellow = slicer.mrmlScene.GetNodeByID("vtkMRMLSliceNodeYellow")
    green = slicer.mrmlScene.GetNodeByID("vtkMRMLSliceNodeGreen")
    
    crossing = [0,0,0]
    
    orientation = "Sagittal"
    if (red.GetOrientationString() == orientation) | (yellow.GetOrientationString() == orientation) | (green.GetOrientationString() == orientation):
      if red.GetOrientationString() == orientation:
        crossing[0] = red.GetSliceOffset()
      elif yellow.GetOrientationString() == orientation:
        crossing[0] = yellow.GetSliceOffset() 
      elif green.GetOrientationString() == orientation:
        crossing[0] = green.GetSliceOffset() 
    
    orientation = "Coronal"
    if (red.GetOrientationString() == orientation) | (yellow.GetOrientationString() == orientation) | (green.GetOrientationString() == orientation):
      if red.GetOrientationString() == orientation:
        crossing[1] = red.GetSliceOffset()
      elif yellow.GetOrientationString() == orientation:
        crossing[1] = yellow.GetSliceOffset() 
      elif green.GetOrientationString() == orientation:
        crossing[1] = green.GetSliceOffset() 
        
    orientation = "Axial"
    if (red.GetOrientationString() == orientation) | (yellow.GetOrientationString() == orientation) | (green.GetOrientationString() == orientation):
      if red.GetOrientationString() == orientation:
        crossing[2] = red.GetSliceOffset()
      elif yellow.GetOrientationString() == orientation:
        crossing[2] = yellow.GetSliceOffset() 
      elif green.GetOrientationString() == orientation:
        crossing[2] = green.GetSliceOffset()   
        
    return crossing
  
  
  @staticmethod 
  def setSliceNodesCrossingPositionRAS(crossingRAS = [0.,0.,0.]):
    red = slicer.mrmlScene.GetNodeByID("vtkMRMLSliceNodeRed")
    yellow = slicer.mrmlScene.GetNodeByID("vtkMRMLSliceNodeYellow")
    green = slicer.mrmlScene.GetNodeByID("vtkMRMLSliceNodeGreen")
    
    orientation = "Sagittal"
    if (red.GetOrientationString() == orientation) | (yellow.GetOrientationString() == orientation) | (green.GetOrientationString() == orientation):
      if red.GetOrientationString() == orientation:
        red.SetSliceOffset(crossingRAS[0])
      if yellow.GetOrientationString() == orientation:
        yellow.SetSliceOffset(crossingRAS[0])
      if green.GetOrientationString() == orientation:
        green.SetSliceOffset(crossingRAS[0])
    
    orientation = "Coronal"
    if (red.GetOrientationString() == orientation) | (yellow.GetOrientationString() == orientation) | (green.GetOrientationString() == orientation):
      if red.GetOrientationString() == orientation:
        red.SetSliceOffset(crossingRAS[1])
      if yellow.GetOrientationString() == orientation:
        yellow.SetSliceOffset(crossingRAS[1])
      if green.GetOrientationString() == orientation:
        green.SetSliceOffset(crossingRAS[1])
        
    orientation = "Axial"
    if (red.GetOrientationString() == orientation) | (yellow.GetOrientationString() == orientation) | (green.GetOrientationString() == orientation):
      if red.GetOrientationString() == orientation:
        red.SetSliceOffset(crossingRAS[2])
      if yellow.GetOrientationString() == orientation:
        yellow.SetSliceOffset(crossingRAS[2])
      if green.GetOrientationString() == orientation:
        green.SetSliceOffset(crossingRAS[2])
        
  
  @staticmethod 
  def setCompareSliceNodesCrossingPositionRAS(index, crossingRAS = [0.,0.,0.]):
    
    compareSliceNodes = SlicerLongitudinalPETCTModuleViewHelper.getCompareSliceNodes(index)
    if (index > -1) & (index < len(compareSliceNodes)):
      
      orientation = "Sagittal"
      if (compareSliceNodes[0].GetOrientationString() == orientation) | (compareSliceNodes[1].GetOrientationString() == orientation) | (compareSliceNodes[2].GetOrientationString() == orientation):
        if compareSliceNodes[0].GetOrientationString() == orientation:
          compareSliceNodes[0].SetSliceOffset(crossingRAS[0])
        if compareSliceNodes[1].GetOrientationString() == orientation:
          compareSliceNodes[1].SetSliceOffset(crossingRAS[0])
        if compareSliceNodes[2].GetOrientationString() == orientation:
          compareSliceNodes[2].SetSliceOffset(crossingRAS[0])
    
      orientation = "Coronal"
      if (compareSliceNodes[0].GetOrientationString() == orientation) | (compareSliceNodes[1].GetOrientationString() == orientation) | (compareSliceNodes[2].GetOrientationString() == orientation):
        if compareSliceNodes[0].GetOrientationString() == orientation:
          compareSliceNodes[0].SetSliceOffset(crossingRAS[1])
        if compareSliceNodes[1].GetOrientationString() == orientation:
          compareSliceNodes[1].SetSliceOffset(crossingRAS[1])
        if compareSliceNodes[2].GetOrientationString() == orientation:
          compareSliceNodes[2].SetSliceOffset(crossingRAS[1])
        
      orientation = "Axial"
      if (compareSliceNodes[0].GetOrientationString() == orientation) | (compareSliceNodes[1].GetOrientationString() == orientation) | (compareSliceNodes[2].GetOrientationString() == orientation):
        if compareSliceNodes[0].GetOrientationString() == orientation:
          compareSliceNodes[0].SetSliceOffset(crossingRAS[2])
        if compareSliceNodes[1].GetOrientationString() == orientation:
          compareSliceNodes[1].SetSliceOffset(crossingRAS[2])
        if compareSliceNodes[2].GetOrientationString() == orientation:
          compareSliceNodes[2].SetSliceOffset(crossingRAS[2])
  
  @staticmethod
  def centerROIonSliceNodesOffset(roi, centeringTransform):
    red = slicer.mrmlScene.GetNodeByID("vtkMRMLSliceNodeRed")
    yellow = slicer.mrmlScene.GetNodeByID("vtkMRMLSliceNodeYellow")
    green = slicer.mrmlScene.GetNodeByID("vtkMRMLSliceNodeGreen")
    roiXYZ = [0,0,0]
    
    orientation = "Sagittal"
    if (red.GetOrientationString() == orientation) | (yellow.GetOrientationString() == orientation) | (green.GetOrientationString() == orientation):
      if red.GetOrientationString() == orientation:
        roiXYZ[0] = red.GetSliceOffset()
      elif yellow.GetOrientationString() == orientation:
        roiXYZ[0] = yellow.GetSliceOffset() 
      elif green.GetOrientationString() == orientation:
        roiXYZ[0] = green.GetSliceOffset() 
    
    orientation = "Coronal"
    if (red.GetOrientationString() == orientation) | (yellow.GetOrientationString() == orientation) | (green.GetOrientationString() == orientation):
      if red.GetOrientationString() == orientation:
        roiXYZ[1] = red.GetSliceOffset()
      elif yellow.GetOrientationString() == orientation:
        roiXYZ[1] = yellow.GetSliceOffset() 
      elif green.GetOrientationString() == orientation:
        roiXYZ[1] = green.GetSliceOffset() 
        
    orientation = "Axial"
    if (red.GetOrientationString() == orientation) | (yellow.GetOrientationString() == orientation) | (green.GetOrientationString() == orientation):
      if red.GetOrientationString() == orientation:
        roiXYZ[2] = red.GetSliceOffset()
      elif yellow.GetOrientationString() == orientation:
        roiXYZ[2] = yellow.GetSliceOffset() 
      elif green.GetOrientationString() == orientation:
        roiXYZ[2] = green.GetSliceOffset()   
    
    if centerTransform:
      roi.SetAndObserveTransformNodeID(centerTransform.GetID())   
      centerTransformMatrix = centerTransform.GetMatrixTransformToParent()
      roi.SetXYZ(roiXYZ[0]-centerTransformMatrix.GetElement(0,3),roiXYZ[1]-centerTransformMatrix.GetElement(1,3),roiXYZ[2]-centerTransformMatrix.GetElement(2,3))  
    else:
      roi.SetXYZ(roiXYZ)      
      
      
              
  
  @staticmethod
  def getROIPositionInRAS(roi):
    xyz = [0.,0.,0.]
    if roi:
      roi.GetXYZ(xyz)
      
      xyz = [xyz[0],xyz[1],xyz[2],1.0]
      
      roiTransform = roi.GetParentTransformNode()
      
      if roiTransform:
        matrix = vtk.vtkMatrix4x4()
        roiTransform.GetMatrixTransformToWorld(matrix)
        
        xyz = matrix.MultiplyDoublePoint(xyz)
      
      xyz = [xyz[0],xyz[1],xyz[2]]  
            
    return xyz  
  
  
  @staticmethod
  def updateQuantitativeViewLayout(studies):
    
    label3D = SlicerLongitudinalPETCTModuleViewHelper.compareLabel3D()  
     
    quantView ="<layout type=\"vertical\">"
    quantView += "<item>\
    <layout type=\"horizontal\">\
    <item>\
    <view class=\"vtkMRMLChartViewNode\" singletontag=\"ChartView1\">\
    <property name=\"viewlabel\" action=\"default\">1</property>\
    </view>\
    </item>\
    </layout>\
    </item>\
    <item>\
    <layout type=\"horizontal\">"
    for i in range(studies):
      quantView = quantView+"<item>\
    <view class=\"vtkMRMLViewNode\" singletontag=\""+label3D+str(i+1)+"\">\
    <property name=\"viewlabel\" action=\"default\">D"+str(i+1)+"</property>\
    </view>\
    </item>"
    quantView = quantView+"</layout>\
    </item>\
    </layout>"
    
    layoutNodes = slicer.mrmlScene.GetNodesByClass('vtkMRMLLayoutNode')
    layoutNodes.SetReferenceCount(layoutNodes.GetReferenceCount()-1)
    layoutNodes.InitTraversal()
    layoutNode = layoutNodes.GetNextItemAsObject()
    id = 456 + studies
    layoutNode.AddLayoutDescription(id,quantView)
    layoutNode.SetViewArrangement(id)
        
      
     
  @staticmethod
  def updateQualitativeViewLayout(studies):
    
    label = SlicerLongitudinalPETCTModuleViewHelper.compareLabel()
    label3D = SlicerLongitudinalPETCTModuleViewHelper.compareLabel3D()
   
    compareView ="<layout type=\"horizontal\">"
    
    for i in range(studies):
      compareView = compareView+"<item>\
    <layout type=\"vertical\">\
    <item>\
    <view class=\"vtkMRMLViewNode\" singletontag=\""+label3D+str(i+1)+"\">\
    <property name=\"viewlabel\" action=\"default\">D"+str(i+1)+"</property>\
    </view>\
    </item>\
    <item>\
    <view class=\"vtkMRMLSliceNode\" singletontag=\""+label+str(i+1)+"_"+str(i+1)+"\">\
    <property name=\"orientation\" action=\"default\">Axial</property>\
    <property name=\"viewlabel\" action=\"default\">L"+str(i*3+1)+"</property>\
    <property name=\"viewcolor\" action=\"default\">#E17012</property>\
    <property name=\"lightboxrows\" action=\"default\">1</property>\
    <property name=\"lightboxcolumns\" action=\"default\">1</property>\
    <property name=\"lightboxrows\" action=\"relayout\">1</property>\
    <property name=\"lightboxcolumns\" action=\"relayout\">1</property>\
    </view>\
    </item>\
    <item>\
    <view class=\"vtkMRMLSliceNode\" singletontag=\""+label+str(i+1)+"_"+str(i+2)+"\">\
    <property name=\"orientation\" action=\"default\">Sagittal</property>\
    <property name=\"viewlabel\" action=\"default\">L"+str(i*3+2)+"</property>\
    <property name=\"viewcolor\" action=\"default\">#E17012</property>\
    <property name=\"lightboxrows\" action=\"default\">1</property>\
    <property name=\"lightboxcolumns\" action=\"default\">1</property>\
    <property name=\"lightboxrows\" action=\"relayout\">1</property>\
    <property name=\"lightboxcolumns\" action=\"relayout\">1</property>\
    </view>\
    </item>\
    <item>\
    <view class=\"vtkMRMLSliceNode\" singletontag=\""+label+str(i+1)+"_"+str(i+3)+"\">\
    <property name=\"orientation\" action=\"default\">Coronal</property>\
    <property name=\"viewlabel\" action=\"default\">L"+str(i*3+3)+"</property>\
    <property name=\"viewcolor\" action=\"default\">#E17012</property>\
    <property name=\"lightboxrows\" action=\"default\">1</property>\
    <property name=\"lightboxcolumns\" action=\"default\">1</property>\
    <property name=\"lightboxrows\" action=\"relayout\">1</property>\
    <property name=\"lightboxcolumns\" action=\"relayout\">1</property>\
    </view>\
    </item>\
    </layout>\
    </item>"

    compareView = compareView+"</layout>"
    
    layoutNodes = slicer.mrmlScene.GetNodesByClass('vtkMRMLLayoutNode')
    layoutNodes.SetReferenceCount(layoutNodes.GetReferenceCount()-1)
    layoutNodes.InitTraversal()
    layoutNode = layoutNodes.GetNextItemAsObject()
    id = 123 + studies
    layoutNode.AddLayoutDescription(id,compareView)
    layoutNode.SetViewArrangement(id)
    
    
    compViewNodes = SlicerLongitudinalPETCTModuleViewHelper.getCompareViewNodes()
    for vn in compViewNodes:
      if vn.GetBoxVisible != 0:
        vn.SetBoxVisible(0)
    #for i in range(sliceCompositeNodes.GetNumberOfItems()):
      #scn = sliceCompositeNodes.GetItemAsObject(i)
      #sn = sliceNodes.GetItemAsObject(i)
      #print 'Composite node: ',sn.GetName()
      #if sn.GetName() == 'Compare0':
        #compare0 = scn
      #if sn.GetName() == 'Compare1':
        #compare1 = scn    
        
    return id            
      
  @staticmethod
  def showInformationMessageBox(windowTitle, informationMessage):
    qt.QMessageBox.information(None, windowTitle, informationMessage)    
  
  @staticmethod
  def moduleDialogTitle():
    return 'Longitudinal PET/CT Analysis Module'       
       
  @staticmethod
  def findingInfoMessage():
    return 'How to create a Finding\n\n\n1. Navigate through the image slices (red, yellow or green) to a lesion.\n\n2. Hold the SHIFT key and move the mouse cursor to the center of the lesion. All slice views will be updated to this position.\n\n3. Select "Create new Finding" to create a ROI bounding box around the lesion.\n\n4. Click "Edit Segmentation" to open the Editor mode.\nPerform the segmentation of the lesion.\n\n5. Click "Apply Segmentation to Finding" to exit the Editor mode and return to the module.\n'       
  
  @staticmethod
  def reportsHelpText():
    return '<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:\'Lucida Grande\'; font-size:13pt; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600; text-decoration: underline;\">Importing PET/CT studies to a Report</span></p>\n<ol style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\"><li style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><img src=\":/Icons/SlicerLoadDICOM.png\" /> Open the Slicer DICOM Database</li>\n<li style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Browse database for patient and select entry</li>\n<li style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Select DICOM Data which has been detected by the <span style=\" font-weight:600; font-style:italic;\">Longitudinal PET/CT Analysis</span><span style=\" font-style:italic;\"> Reader</span></li>\n<li style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Click <span style=\" font-weight:600; font-style:italic;\">Load Selection to Slicer </span></li>\n<li style=\" font-weight:600; font-style:italic;\" style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><img src=\":/Icons/LongPETCT_small.png\" /> <span style=\" font-weight:400; font-style:normal;\">Reopen</span><span style=\" font-style:normal;\"> Longitudinal PET/CT Analysis</span><span style=\" font-weight:400; font-style:normal;\"> module</span></li>\n<li style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Select the new generated <span style=\" font-weight:600; font-style:italic;\">Report</span></li></ol></body></html>'
  
    
  @staticmethod
  def compareLabel():
    return 'CompareLongitudinalPETCT_'
  
  @staticmethod
  def compareLabel3D():
    return 'CompareLongitudinalPETCT_3D_' 
  
  @staticmethod
  def getStandardViewNode():
    viewNodes = slicer.mrmlScene.GetNodesByClass('vtkMRMLViewNode')
    viewNodes.SetReferenceCount(viewNodes.GetReferenceCount()-1)
    viewNodes.InitTraversal()
    viewNode = viewNodes.GetNextItemAsObject()
    
    return viewNode
  
  @staticmethod
  def getStandardChartViewNode():
    chartViewNodes = slicer.mrmlScene.GetNodesByClass('vtkMRMLChartViewNode')
    chartViewNodes.SetReferenceCount(chartViewNodes.GetReferenceCount()-1)
    chartViewNodes.InitTraversal()
    chartViewNode = chartViewNodes.GetNextItemAsObject()
    
    return chartViewNode
 
  @staticmethod
  def getROINodesCollection():
    roiNodes = slicer.mrmlScene.GetNodesByClass('vtkMRMLAnnotationROINode')
    roiNodes.SetReferenceCount(roiNodes.GetReferenceCount()-1)
    
    return roiNodes 
    
  @staticmethod
  def getCompareViewNodes():
    compareViewNodes = []
    viewNodes = slicer.mrmlScene.GetNodesByClass('vtkMRMLViewNode')
    viewNodes.SetReferenceCount(viewNodes.GetReferenceCount()-1)
    viewNodes.InitTraversal()
    vn = viewNodes.GetNextItemAsObject()
    while vn:
      if str(vn.GetSingletonTag()).find(SlicerLongitudinalPETCTModuleViewHelper.compareLabel3D()) != -1:
        pos = 0
        for cvn in compareViewNodes:
          if vn.GetID() >= cvn.GetID():
            pos = pos + 1
        
        compareViewNodes.insert(pos, vn)
      vn = viewNodes.GetNextItemAsObject()
    
        
    return compareViewNodes
  
  @staticmethod
  def getCompareSliceNodes(index):
    compareSliceNodes = []
    sliceNodes = slicer.mrmlScene.GetNodesByClass('vtkMRMLSliceNode')
    sliceNodes.SetReferenceCount(sliceNodes.GetReferenceCount()-1)
    sliceNodes.InitTraversal()
    sn = sliceNodes.GetNextItemAsObject()
    while sn:
      if str(sn.GetSingletonTag()).find(SlicerLongitudinalPETCTModuleViewHelper.compareLabel()+str(index+1)) != -1:                                              
        pos = 0
        for csn in compareSliceNodes:
          if sn.GetID() >= csn.GetID():
            pos = pos + 1
        
        compareSliceNodes.insert(pos, sn)
      sn = sliceNodes.GetNextItemAsObject()      
      
    return compareSliceNodes
  
  @staticmethod
  def getCompareSliceCompositeNodes(index):
    compareSliceCompositeNodes = []
    compositeNodes = slicer.mrmlScene.GetNodesByClass('vtkMRMLSliceCompositeNode')
    compositeNodes.SetReferenceCount(compositeNodes.GetReferenceCount()-1)
    compositeNodes.InitTraversal()
    scn = compositeNodes.GetNextItemAsObject()
    while scn:
      if str(scn.GetSingletonTag()).find(SlicerLongitudinalPETCTModuleViewHelper.compareLabel()+str(index+1)) != -1:                                              
        pos = 0
        for cscn in compareSliceCompositeNodes:
          if scn.GetID() >= cscn.GetID():
            pos = pos + 1
        
        compareSliceCompositeNodes.insert(pos, scn)
      scn = compositeNodes.GetNextItemAsObject() 
        
    return compareSliceCompositeNodes
  
  @staticmethod
  def removeObserversFromCompareViewNodes(observerIDsList):
   
    if observerIDsList:
      compareViewNodes = SlicerLongitudinalPETCTModuleViewHelper.getCompareViewNodes()
    
      for vn in compareViewNodes:
        for id in observerIDsList:
          vn.RemoveObserver(id)
  
  @staticmethod
  def spinStandardViewNode(spin):
    if spin:
      SlicerLongitudinalPETCTModuleViewHelper.getStandardViewNode().SetAnimationMode(slicer.vtkMRMLViewNode.Spin) 
    else:
      SlicerLongitudinalPETCTModuleViewHelper.getStandardViewNode().SetAnimationMode(slicer.vtkMRMLViewNode.Off)   
                                 
  @staticmethod
  def spinCompareViewNodes(spin):
    compViewNodes = SlicerLongitudinalPETCTModuleViewHelper.getCompareViewNodes()
    for vn in compViewNodes:
      if spin:
        vn.SetAnimationMode(slicer.vtkMRMLViewNode.Spin)
      else:
        vn.SetAnimationMode(slicer.vtkMRMLViewNode.Off)
  
  @staticmethod      
  def opacityPowerFunction(window, pow, points):
    opacityFunction = None
    rg = int(points)-1
    
    window = float(window)
    pow = float(pow)
    points = float(points)
          
    if window > 0:
      opacityFunction = vtk.vtkPiecewiseFunction()
      opacityFunction.AddPoint(0.,0.,0.5,0.)  
      
      for i in range(1,rg):
       
        m = float(i/points)
        opacityFunction.AddPoint((window*m),m**pow,0.5,0.)
              
      opacityFunction.AddPoint(window,1.,0.5,0.)
    
    return opacityFunction          
  
  @staticmethod
  def RGBtoHex(r,g,b, satMod = 1.0):
    
    col = qt.QColor(r,g,b)
    sat = col.saturation() * satMod
    col.setHsv(col.hue(),sat,col.value())
    
    return col.name()

  @staticmethod
  def removeModelHierarchyAndChildModelNodesFromScene(modelHierarchyNode):
    if modelHierarchyNode:
      cmn = vtk.vtkCollection()
      
      modelHierarchyNode.GetChildrenModelNodes(cmn)
      
      for i in range(cmn.GetNumberOfItems()):
        model = cmn.GetItemAsObject(i)
        
        if model.IsA('vtkMRMLModelNode'):
          slicer.mrmlScene.RemoveNode(model)
          
      slicer.mrmlScene.RemoveNode(modelHierarchyNode)       
      
      
  @staticmethod
  def insertStr(original, new, pos):
    '''Inserts new inside original at pos.'''
    return original[:pos] + new + original[pos:]                  
    
    #return '#%02X%02X%02X' % (r,g,b)
