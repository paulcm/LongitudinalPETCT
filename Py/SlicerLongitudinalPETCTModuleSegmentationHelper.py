# slicer imports
from __main__ import vtk, slicer, ctk, qt

# python includes
import sys



class SlicerLongitudinalPETCTModuleSegmentationHelper( object ):
  '''
  classdocs
  '''
 
    
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
              posInMain = [x+croppedLblIJKShiftedOrigin[0],y+croppedLblIJKShiftedOrigin[1],z+croppedLblIJKShiftedOrigin[2]]
              d = mainImageData.GetScalarComponentAsDouble(posInMain[0],posInMain[1],posInMain[2],0)
              if (d == 0.) & (p == colorID):
                mainImageData.SetScalarComponentFromDouble(posInMain[0],posInMain[1],posInMain[2],0,p)
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
        
        change = slicer.vtkImageLabelChange()
        change.SetInput(imgData)
        change.SetInputLabel(colorID)
        change.SetOutputLabel(0)
        
        change.Update()
        
        output = change.GetOutput()
        
        if output:
          lblVolume.SetAndObserveImageData(output)
          output.Modified()
          lblVolume.Modified()
                  
            