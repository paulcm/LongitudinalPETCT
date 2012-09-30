# slicer imports
from __main__ import vtk, slicer, tcl, qt

# python includes
import sys
import time

class SlicerLongPETCTModuleViewHelper( object ):
  '''
  classdocs
  '''
  
  # TODO: add a capability to control the level of messages
  @staticmethod
  def Info( message ):
    print("[LongPETCT " + time.strftime( "%m/%d/%Y %H:%M:%S" ) + "]: INFO: " + str( message ))
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
  def SetCompareBgFgLblVolumes(selStudyID, bgID, fgID = None, lblID = None, fit = False):
    
    label = SlicerLongPETCTModuleViewHelper.compareLabel()
     
    compositeNodes = slicer.util.getNodes('vtkMRMLSliceCompositeNode'+label+str(selStudyID)+'*')
    for cnode in compositeNodes.values():
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
        print compositeNode.GetID()
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
  def pasteFromCroppedToMainLabelVolume(croppedLblVolume, mainLblVolume, colorID):
    
    pasted = False
    
    if (croppedLblVolume != None) & (mainLblVolume != None):
      croppedImgData = croppedLblVolume.GetImageData()
      mainImageData = mainLblVolume.GetImageData()
      
      if (croppedImgData != None) & (mainImageData != None):
      
        rastoijk = vtk.vtkMatrix4x4()
        mainLblVolume.GetRASToIJKMatrix(rastoijk)
      
        croppedLblRASOrigin = croppedLblVolume.GetOrigin()
        croppedLblRASOrigin = [croppedLblRASOrigin[0],croppedLblRASOrigin[1],croppedLblRASOrigin[2],1.0]
    
        croppedLblIJKShiftedOrigin = rastoijk.MultiplyDoublePoint(croppedLblRASOrigin)
        croppedLblIJKShiftedOrigin = [ int(croppedLblIJKShiftedOrigin[0] + 0.5), int(croppedLblIJKShiftedOrigin[1] + 0.5), int(croppedLblIJKShiftedOrigin[2] + 0.5)]
        
        dims = croppedImgData.GetDimensions()
        
        for x in range(0,dims[0],1):
          for y in range(0,dims[1],1):
            for z in range(0,dims[2],1):
              p = croppedImgData.GetScalarComponentAsDouble(x,y,z,0)
              #if p == colorID:
              mainImageData.SetScalarComponentFromDouble(x+croppedLblIJKShiftedOrigin[0],y+croppedLblIJKShiftedOrigin[1],z+croppedLblIJKShiftedOrigin[2],0,p)
              if pasted == False:
                pasted = True    
              
    return pasted                
                
  @staticmethod
  def pasteFromMainToCroppedLabelVolume(mainLblVolume, croppedLblVolume, colorID):
    
    pasted = False
    
    if (mainLblVolume != None) & (croppedLblVolume != None):
      mainImageData = mainLblVolume.GetImageData()
      croppedImgData = croppedLblVolume.GetImageData()
      
      
      if (mainImageData != None) & (croppedImgData != None):
      
        rastoijk = vtk.vtkMatrix4x4()
        mainLblVolume.GetRASToIJKMatrix(rastoijk)
      
        croppedLblRASOrigin = croppedLblVolume.GetOrigin()
        croppedLblRASOrigin = [croppedLblRASOrigin[0],croppedLblRASOrigin[1],croppedLblRASOrigin[2],1.0]
    
        croppedLblIJKShiftedOrigin = rastoijk.MultiplyDoublePoint(croppedLblRASOrigin)
        croppedLblIJKShiftedOrigin = [ int(croppedLblIJKShiftedOrigin[0] + 0.5), int(croppedLblIJKShiftedOrigin[1] + 0.5), int(croppedLblIJKShiftedOrigin[2] + 0.5)]
        
        dims = croppedImgData.GetDimensions()
        
        for x in range(0,dims[0],1):
          for y in range(0,dims[1],1):
            for z in range(0,dims[2],1):
              p = mainImageData.GetScalarComponentAsDouble(x+croppedLblIJKShiftedOrigin[0],y+croppedLblIJKShiftedOrigin[1],z+croppedLblIJKShiftedOrigin[2],0)                             
              if p == colorID:
                croppedImgData.SetScalarComponentFromDouble(x,y,z,0,p)
                if pasted == False:
                  pasted = True
                  
    return pasted 
  
  
  @staticmethod
  def containsSegmentation(lblVolume, colorID):
    
    contains = False
    
    if lblVolume:
      imgData = lblVolume.GetImageData()
      
      if imgData:
        dims = imgData.GetDimensions()
        
        for x in range(0,dims[0],1):
          for y in range(0,dims[1],1):
            for z in range(0,dims[2],1):
              p = imgData.GetScalarComponentAsDouble(x,y,z,0)                             
              if p == colorID:
                contains = True
                break
            if contains:
              break
          if contains:
            break
          
    return contains          
  
  @staticmethod
  def removeSegmentationFromVolume(lblVolume, colorID):
      
    if lblVolume:
      imgData = lblVolume.GetImageData()
      
      if imgData:
        dims = imgData.GetDimensions()
        
        for x in range(dims[0]):
          for y in range(dims[1]):
            for z in range(dims[2]):
              p = imgData.GetScalarComponentAsDouble(x,y,z,0)                             
              if p == colorID:
                imgData.SetScalarComponentFromDouble(x,y,z,0,0.)      
        
        imgData.Modified()            
  
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
  def updateQualitativeViewLayout(studies):
    
    label = SlicerLongPETCTModuleViewHelper.compareLabel()
    label3D = SlicerLongPETCTModuleViewHelper.compareLabel3D()
   
    # add custom layout for comparing two pairs of volumes
    compareViewTwoRows ="<layout type=\"horizontal\">"
    
    for i in range(studies):
      print "Constructing ROWS"
      compareViewTwoRows = compareViewTwoRows+"   <item>\
    <layout type=\"vertical\">\
    <item>\
    <view class=\"vtkMRMLViewNode\" singletontag=\""+label3D+str(i+1)+"\">\
    <property name=\"viewlabel\" action=\"default\">D"+str(i+1)+"</property>\
    </view>\
    </item>\
    <item>\
    <view class=\"vtkMRMLSliceNode\" singletontag=\""+label+str(i)+"_"+str(i+1)+"\">\
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
    <view class=\"vtkMRMLSliceNode\" singletontag=\""+label+str(i)+"_"+str(i+2)+"\">\
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
    <view class=\"vtkMRMLSliceNode\" singletontag=\""+label+str(i)+"_"+str(i+3)+"\">\
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

    compareViewTwoRows = compareViewTwoRows+"</layout>"
    
    layoutNode = slicer.mrmlScene.GetNodesByClass('vtkMRMLLayoutNode').GetItemAsObject(0)
    id = 123 + studies
    layoutNode.AddLayoutDescription(id,compareViewTwoRows)
    layoutNode.SetViewArrangement(id)
    
    
    compViewNodes = SlicerLongPETCTModuleViewHelper.getCompareViewNodes()
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
    return 'CompareLongPETCT_'
  
  @staticmethod
  def compareLabel3D():
    return 'CompareLongPETCT_3D_' 
  
  @staticmethod
  def getStandardViewNode():
    viewNode = slicer.util.getNode('vtkMRMLViewNode1')
    return viewNode
    
  @staticmethod
  def getCompareViewNodes():
    compareViewNodes = []
    viewNodes = slicer.util.getNodes('vtkMRMLViewNode*')
    for vn in viewNodes.values():
      if str(vn.GetSingletonTag()).find(SlicerLongPETCTModuleViewHelper.compareLabel3D()) != -1:
        compareViewNodes.append(vn)
        
    return compareViewNodes
  
  @staticmethod
  def removeObserversFromCompareViewNodes(observerIDsList):
   
    if observerIDsList:
      compareViewNodes = SlicerLongPETCTModuleViewHelper.getCompareViewNodes()
    
      for vn in compareViewNodes:
        for id in observerIDsList:
          vn.RemoveObserver(id)
  
  @staticmethod
  def spinStandardViewNode(spin):
    if spin:
      SlicerLongPETCTModuleViewHelper.getStandardViewNode().SetAnimationMode(slicer.vtkMRMLViewNode.Spin) 
    else:
      SlicerLongPETCTModuleViewHelper.getStandardViewNode().SetAnimationMode(slicer.vtkMRMLViewNode.Off)   
           
                         
  @staticmethod
  def spinCompareViewNodes(spin):
    compViewNodes = SlicerLongPETCTModuleViewHelper.getCompareViewNodes()
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