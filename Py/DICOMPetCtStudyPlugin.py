import os
import sys as SYS
from __main__ import vtk, qt, ctk, slicer
from DICOMLib import DICOMPlugin
from DICOMLib import DICOMLoadable

from datetime import datetime

from decimal import *


import DICOMLib

#
# This is the plugin to handle translation of diffusion volumes
# from DICOM files into MRML nodes.  It follows the DICOM module's
# plugin architecture.
#

class DICOMPetCtStudyPluginClass(DICOMPlugin):
  """ PetCtStudy specific interpretation code
  """

  def __init__(self):
    super(DICOMPetCtStudyPluginClass,self).__init__()
    self.loadType = "Longitudinal PET/CT Analysis"
    #self.tags['patientName'] = "0010,0010"
    #self.tags['patientBirthDate'] = "0010,0030"
    #self.tags['patientSex'] = "0010,0040"
    #self.tags['patientSize'] = "0010,1020"
    #self.tags['patientWeight'] = "0010,1030"
    
    self.tags['relatedSeriesSequence'] = "0008,1250"
    
    #self.tags['radioPharmStartTime'] = "0018,1072"
    #self.tags['decayCorrection'] = "0054,1102"
    #self.tags['decayFactor'] = "0054,1321"
    #self.tags['frameRefTime'] = "0054,1300"
    #self.tags['radionuclidenHL'] = "0018,1075"
     
    #self.tags['studyInstanceUID'] = "0020,000D"    
    
    #self.tags['seriesInstanceUID'] = "0020,000E"
    self.tags['seriesDescription'] = "0008,103e"
    self.tags['seriesModality'] = "0008,0060"
    self.tags['instanceUID'] = "0008,0018"
  
    self.tags['studyInstanceUID'] = "0020,000D"
    self.tags['studyDate'] = "0008,0020"
    self.tags['studyTime'] = "0008,0030"
    
    self.tags['rows'] = "0028,0010"
    self.tags['columns'] = "0028,0011"
    self.tags['spacing'] = "0028,0030"
    self.tags['orientation'] = "0020,0037"
    self.tags['pixelData'] = "7fe0,0010"
  
    #self.tags['seriesTime'] = "0008,0031"
    
    self.ctTag = "CT"
    self.petTag = "PT"
    

  def examine(self,fileLists):
    """ Returns a list of DICOMLoadable instances
    corresponding to ways of interpreting the
    fileLists parameter.
    """
    time = datetime.now()
        
    loadables = []
    
    self.allSeriesLoadables = []
    
    studyCounter = 0
   
    petFileLists = []
    ctFileLists = []
   
    studyUIDs = []
    # Filling lists of pet and ct filelists        
    for files in fileLists:
      
      if self.isSpecificSeries(files, self.petTag):
        petFileLists.append(files)
        tempStudyUID = self.studyInstanceUID(files)
        if (tempStudyUID in studyUIDs) == False:
          studyUIDs.append(tempStudyUID) 
      
      elif self.isSpecificSeries(files, self.ctTag):
          ctFileLists.append(files)

    print "TIME TO LOAD PET AND CT's: " + str(datetime.now()-time) + " ms"
    time = datetime.now()
     
    #cleaning ct list
    i = 0
    while i < len(ctFileLists):
      if (self.studyInstanceUID(ctFileLists[i]) in studyUIDs) == False:
        ctFileLists.pop(i)
      else:
        i += 1  
        
    print "TIME TO CLEAR CT's: " + str(datetime.now()-time) + " ms"
    time = datetime.now() 
    
    for petFiles in petFileLists:
      loadablePT = self.examineFiles(petFiles,self.petTag)
      if(loadablePT):
        studyUID = self.studyInstanceUID(petFiles) 
        
        i = 0
        sim = -1
        mostSimilarIdx = -1
                
        for ctFiles in ctFileLists:          
          
          if self.studyInstanceUID(ctFiles) == studyUID:
             ptSurface = self.surface(petFiles)
             ctSurface = self.surface(ctFiles)
                
             diff = min(ptSurface,ctSurface) / max(ptSurface,ctSurface)
             if diff > sim:
               sim = diff
               mostSimilarIdx = i
          i += 1
        
        if mostSimilarIdx != -1:
          loadableCT = self.examineFiles(ctFileLists[mostSimilarIdx], self.ctTag)
          
          if loadableCT:
            self.allSeriesLoadables += loadablePT
            self.allSeriesLoadables += loadableCT  
            studyCounter += 1 
    
    
    print "TIME TO GET ALL STUDIES: " + str(datetime.now()-time) + " ms"
    time = datetime.now() 
    
    loadable = DICOMLib.DICOMLoadable()
    
    allFiles = []
    warning = ""

    for petCtLoadable in self.allSeriesLoadables:
      allFiles += petCtLoadable.files
      if petCtLoadable.warning:
        warning += petCtLoadable.name + ": "+petCtLoadable.warning+" "
    
    print "FILES SIZE: "+ str(len(allFiles))
    if allFiles:
    
      print "CAME HERE"
    
      loadable.files = allFiles
      loadable.name = "PET/CT: "+str(studyCounter) + " "
      if(studyCounter == 1):
        loadable.name += "study "
      else:
        loadable.name += "studies " 
    
      loadable.name += self.studiesTimeframeStr()  
      loadable.warning = warning
       
      loadable.selected = True
      loadable.confidence = 1.0
    
      loadables = [loadable]
    
    print "TIME TO CREATE LOADABLE: " + str(datetime.now()-time) + " ms"
    time = datetime.now() 
          
    return loadables

#TODO PixelData and Orientation

  def studiesTimeframeStr(self):
    
    studydates = self.allSelectedStudyDates()
    resultStr = ""
    
    if studydates:
      resultStr = "from "+studydates[0]+" "
      
      length = len(studydates)
      
      if length > 1:
        resultStr += "to "+studydates[length-1]
        
    return resultStr
  

  def allSelectedStudyDates(self):
    
    dates = []
    
    for loadablePetCt in self.allSeriesLoadables:
      studyDate = self.studyDate(loadablePetCt.files)
      
      if (studyDate in dates) == False:
        dates.append(studyDate)
    
    return sorted(dates)
      

  def studyDate(self,files):
    """___"""
    studyDate = slicer.dicomDatabase.fileValue(files[0], self.tags['studyDate'])
    
    for file in files:
      tempStudyDate = slicer.dicomDatabase.fileValue(file, self.tags['studyDate'])
      
      if tempStudyDate != studyDate:
        studyDate = ""
    
    return studyDate
  
  
  def studyInstanceUID(self,files):
    """___"""
    studyInstanceUID = slicer.dicomDatabase.fileValue(files[0], self.tags['studyInstanceUID'])
    
    for file in files:
      tempStudyInstanceUID = slicer.dicomDatabase.fileValue(file, self.tags['studyInstanceUID'])
      
      if tempStudyInstanceUID != studyInstanceUID:
        studyInstanceUID = ""
    
    return studyInstanceUID 
  

  def isSpecificSeries(self,files,type):
    """ ___ """
    isSpecificSeries = True
      
    for file in files:
      if slicer.dicomDatabase.fileValue(file,self.tags['seriesModality']) != type:
        isSpecificSeries = False 
                            
    return isSpecificSeries
  
  
  def surface(self,files):
    """___"""
    maxDim = self.maxDimensions(files)
    
    return maxDim[0]*maxDim[1]  
  
  def maxDimensions(self,files):
    """ ___ """
    global SYS
    maxW = SYS.float_info.min
    maxH = SYS.float_info.min
      
    for file in files:
      
      width = self.dimensions(file)[0]
      height = self.dimensions(file)[1]
        
      if width > maxW:
        maxW = width
      if height > maxH:
        maxH = height
        
    result = []
    result.append(maxW)
    result.append(maxH)
      
    return result
    
  
  def dimensions(self,file):
        
    rows = Decimal(slicer.dicomDatabase.fileValue(file,self.tags['rows']))
    cols = Decimal(slicer.dicomDatabase.fileValue(file,self.tags['columns']))
    spacingRows = Decimal(slicer.dicomDatabase.fileValue(file,self.tags['spacing']).partition('\\')[0])
    spacingCols = Decimal(slicer.dicomDatabase.fileValue(file,self.tags['spacing']).partition('\\')[2])
      
    width = cols * spacingCols
    height = rows * spacingRows
    
    result = []
    
    result.append(width)
    result.append(height)
    
    return result
            

  def examineFiles(self,files,type):
    """ Returns a list of DICOMLoadable instances
    corresponding to ways of interpreting the
    files parameter.
    Process is to look for 'known' private tags corresponding
    to the types of pet/ct datasets that the DicomToNrrd utility
    should be able to process. Only need to look at one header
    in the series since all should be the same with respect
    to this check.
    """

    # get the series description to use as base for volume name
    loadables = []
    name = type

    scalarVolumePlugin = slicer.modules.dicomPlugins['DICOMScalarVolumePlugin']()
    loadables = scalarVolumePlugin.examineFiles(files)

    for loadable in loadables:
      if loadable.selected:
        loadable.name = name + ' ' + self.studyDate(files)
        loadable.selected = True
        loadable.tooltip = "Appears to be " + type
        loadable.confidence = .1
        loadables = [loadable]   
        break 
                
    return loadables
      

  def load(self,loadable):
    """ ___ """
    a = datetime.now()
    
    vaStorageNode = slicer.vtkMRMLVolumeArchetypeStorageNode()
    
    for petCtLoadable in self.allSeriesLoadables:
      vaStorageNode.SetFileName(petCtLoadable.files[0])
      vaStorageNode.ResetFileNameList()
    
      for file in petCtLoadable.files:
        vaStorageNode.AddFileName(file)
    
      vaStorageNode.SetSingleFile(0)
    
      svNode = slicer.vtkMRMLScalarVolumeNode()
      svNode.SetScene(slicer.mrmlScene)
    
      vaStorageNode.ReadData(svNode)  
    
      if svNode:
        svNode.SetName(petCtLoadable.name)
        #
        # add list of DICOM instance UIDs to the volume node
        # corresponding to the loaded files
        #
        instanceUIDs = ""
        for file in loadable.files:
          uid = slicer.dicomDatabase.fileValue(file,self.tags['instanceUID'])
          if uid == "":
            uid = "Unknown"
          instanceUIDs += uid + " "
        instanceUIDs = instanceUIDs[:-1]  # strip last space
        svNode.SetAttribute("DICOM.instanceUIDs", instanceUIDs)
      
        slicer.mrmlScene.AddNode(svNode)
      
        #
        # automatically select the volume to display
        #
        #appLogic = slicer.app.applicationLogic()
        #selNode = appLogic.GetSelectionNode()
        #selNode.SetReferenceActiveVolumeID(svNode.GetID())
        #appLogic.PropagateVolumeSelection()
    
    
    b = datetime.now()
        
    print "TIME TO LOAD: " + str(b-a) + " ms"
    
    return svNode
  


#
# DICOMPetCtStudyPlugin
#

class DICOMPetCtStudyPlugin:
  """
  This class is the 'hook' for slicer to detect and recognize the plugin
  as a loadable scripted module
  """
  def __init__(self, parent):
    parent.title = "DICOM Diffusion Volume Plugin"
    parent.categories = ["Developer Tools.DICOM Plugins"]
    parent.contributors = ["Steve Pieper (Isomics Inc.)"]
    parent.helpText = """
    Plugin to the DICOM Module to parse and load diffusion volumes
    from DICOM files.
    No module interface here, only in the DICOM module
    """
    parent.acknowledgementText = """
    This DICOM Plugin was developed by
    Steve Pieper, Isomics, Inc.
    and was partially funded by NIH grant 3P41RR013218.
    """

    # don't show this module - it only appears in the DICOM module
    parent.hidden = True

    # Add this extension to the DICOM module's list for discovery when the module
    # is created.  Since this module may be discovered before DICOM itself,
    # create the list if it doesn't already exist.
    try:
      slicer.modules.dicomPlugins
    except AttributeError:
      slicer.modules.dicomPlugins = {}
    slicer.modules.dicomPlugins['DICOMPetCtStudyPlugin'] = DICOMPetCtStudyPluginClass

#
# DICOMPetCtStudyWidget
#

class DICOMPetCtStudyWidget:
  def __init__(self, parent = None):
    self.parent = parent

  def setup(self):
    # don't display anything for this widget - it will be hidden anyway
    pass

  def enter(self):
    pass

  def exit(self):
    pass


