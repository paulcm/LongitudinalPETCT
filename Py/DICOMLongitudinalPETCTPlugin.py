import os
import sys as SYS
from __main__ import vtk, qt, ctk, slicer
from DICOMLib import DICOMPlugin
from DICOMLib import DICOMLoadable


import DICOMLib

#
# This is the plugin to handle translation of diffusion volumes
# from DICOM files into MRML nodes.  It follows the DICOM module's
# plugin architecture.
#

class DICOMLongitudinalPETCTPluginClass(DICOMPlugin):
  """ PetCtStudy specific interpretation code
  """

  def __init__(self):
    super(DICOMLongitudinalPETCTPluginClass,self).__init__()
    self.loadType = "Longitudinal PET/CT Analysis"
    self.tags['patientName'] = "0010,0010"
    self.tags['patientBirthDate'] = "0010,0030"
    self.tags['patientSex'] = "0010,0040"
    self.tags['patientSize'] = "0010,1020"
    self.tags['patientWeight'] = "0010,1030"
    
    self.tags['relatedSeriesSequence'] = "0008,1250"
    
    self.tags['radioPharmaconStartTime'] = "0018,1072"
    self.tags['decayCorrection'] = "0054,1102"
    self.tags['decayFactor'] = "0054,1321"
    self.tags['frameRefTime'] = "0054,1300"
    self.tags['radionuclideHalfLife'] = "0018,1075"
    self.tags['contentTime'] = "0008,0033"
    self.tags['seriesTime'] = "0008,0031" 


    self.tags['seriesDescription'] = "0008,103e"
    self.tags['seriesModality'] = "0008,0060"
    self.tags['instanceUID'] = "0008,0018"
  
    self.tags['studyInstanceUID'] = "0020,000D"
    self.tags['studyDate'] = "0008,0020"
    self.tags['studyTime'] = "0008,0030"
    self.tags['studyID'] = "0020,0010"
    
    self.tags['rows'] = "0028,0010"
    self.tags['columns'] = "0028,0011"
    self.tags['spacing'] = "0028,0030"
    self.tags['position'] = "0020,0032"
    self.tags['orientation'] = "0020,0037"
    self.tags['pixelData'] = "7fe0,0010"
  
    
    self.fileLists = []
    self.patientName = ""
    self.patientBirthDate = ""
    self.patientSex = ""
    
    self.ctTerm = "CT"
    self.petTerm = "PT"
    


  def examine(self,fileLists):
    """ Returns a list of DICOMLoadable instances
    corresponding to ways of interpreting the
    fileLists parameter.
    """
    del self.fileLists[:]
    self.fileLists += fileLists
        
    loadables = []
    
    self.petFileLoadables = []
    self.ctFileLoadables = []
    
    validStudyCounter = 0

    petFileLists = self.findPetSeriesImageFiles()

    ctFileLists = map(self.findCtSeriesFilesForPetSeries, petFileLists)

    
    counter = 0
    
    allFiles = []
    warning = ""
    
    if len(petFileLists) == len(ctFileLists):
      i = 0
      for petFiles in petFileLists:
        if petFiles:
          if ctFileLists[i]:
            loadablePTs = self.examineFiles(petFiles, self.petTerm)
            loadableCTs = self.examineFiles(ctFileLists[i], self.ctTerm)
          
            if loadablePTs:
              loadablePT = loadablePTs[0]
              if loadableCTs:
                loadableCT = loadableCTs[0]
                self.petFileLoadables.append(loadablePT)
                allFiles += loadablePT.files
                if loadablePT.warning:
                  warning += loadablePT.name + ": "+loadablePT.warning+" "
            
                self.ctFileLoadables.append(loadableCT)
                allFiles += loadableCT.files
                if loadableCT.warning:
                  warning += loadableCT.name + ": "+loadableCT.warning+" "
            
                counter += 1
        i += 1           
              
    
    if counter > 0:
      loadable = DICOMLib.DICOMLoadable()
      loadable.files = allFiles
      loadable.name = "PET/CT: "+str(counter) + " "
      if(counter == 1):
        loadable.name += "study "
      else:
        loadable.name += "studies "
    
      loadable.name += self.studiesTimeframeStr()
      loadable.warning = warning
       
      loadable.selected = True
      loadable.confidence = 1.0
    
      loadables = [loadable]
          
    
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
    
    for loadablePet in self.petFileLoadables:
      studyDate = self.studyDate(loadablePet.files)
      
      if (studyDate in dates) == False:
        dates.append(studyDate)
    
    return sorted(dates)
      

  def studyDateImageFile(self,file):
    """___"""
    return slicer.dicomDatabase.fileValue(file, self.tags['studyDate'])
    
  def studyDate(self,files):
    """___"""
    
    studyDate = self.studyDateImageFile(files[0])
    
    #comment for performance
    
    #for file in files:
      #tempStudyDate = self.studyDateImageFile(file)
      
      #if tempStudyDate != studyDate:
        #studyDate = ""
    
    return studyDate
  
  
  
  def findPetSeriesImageFiles(self):
    """___"""
    petSeriesFiles = []
    
    for files in self.fileLists:
      if self.isSpecificSeries(files, self.petTerm):
        petSeriesFiles.append(files)

    return petSeriesFiles
  

       
  
  def findMostSimilarCtSeriesFiles(self, ctFilesList, petFiles):
    """___"""
    ctSeriesFiles = []
    
    if petFiles:
      petVolume = self.volume(petFiles)
      i = 0
      similarity = -1
      slices = -1
      mostSimilarIdx = -1
    
      for i in xrange(len(ctFilesList)):
        ctVolume = self.volume(ctFilesList[i])       
        ratio = min(petVolume,ctVolume) / max(petVolume,ctVolume)
        if ratio > similarity:
          similarity = ratio
          mostSimilarIdx = i
          slices = len(ctFilesList[i])
        elif ratio == similarity:
          if len(ctFilesList[i]) > slices:
            similarity = ratio
            mostSimilarIdx = i
    
      
      #if mostSimilarIdx >= 0 & mostSimilarIdx < len(ctFilesList):
        #ctSeriesFiles = ctFilesList[mostSimilarIdx]
    
    return mostSimilarIdx #ctSeriesFiles      
    
  
  def findCtSeriesFilesForPetSeries(self, petFiles):
    """___"""
    
    if not petFiles:
      return
    
    label = "The Longitudinal PET/CT Analysis Reader has detected multiple CT series in a study with the following PET series:\n\n"
    
    petDescription = slicer.dicomDatabase.fileValue(petFiles[0],self.tags['seriesDescription']) 
    
    dims = self.dimensions(petFiles)
    info = self.acquireImageSeriesComparisonInformation(petFiles)
    
    petDescription += "  |  Slices: "+str(len(petFiles)) + "  |  Dimensions: ("+str(int(dims[0]+.5))+"mm x "+str(int(dims[1]+.5))+"mm x "+str(int(dims[2]+.5))+"mm)  |  Content Time: "+str(info['ContentTime'])
    
    label += petDescription + "\n\n\nA CT series has been selected automatically. Please change the selection if you want to use another CT series.\n"
      
    studyUID = self.studyInstanceUID(petFiles)
    ctSeriesFilesList = []
    
    ctDescriptions = []
    
    for files in self.fileLists:
      if self.isSpecificModalityImage(files[0],self.ctTerm): # for performance: check only first file
        if self.isSpecificSeries(files, self.ctTerm):
          if self.studyInstanceUIDForImage(files[0]) == studyUID: # for performace: check only first file
            if self.studyInstanceUID(files) == studyUID:
              ctSeriesFilesList.append(files)
              dims = self.dimensions(files)
              info = self.acquireImageSeriesComparisonInformation(files)
              ctDescriptions.append(slicer.dicomDatabase.fileValue(files[0],self.tags['seriesDescription']) + "  |  Slices: "+str(len(files)) + "  |  Dimensions: ("+str(int(dims[0]+.5))+"mm x "+str(int(dims[1]+.5))+"mm x "+str(int(dims[2]+.5))+"mm)  |  Content Time: "+str(info['ContentTime'])) 
              
    index = 0
    
    if len(ctSeriesFilesList) > 1:
      msidx = self.findMostSimilarCtSeriesFiles(ctSeriesFilesList, petFiles)
      selected = qt.QInputDialog.getItem(None, 'Select matching CT Series for PET',label,ctDescriptions,msidx,False)
    
    
      try:
        index = ctDescriptions.index(str(selected))
      except IndexError:
        index = msidx 
    
    if ctSeriesFilesList:
      return ctSeriesFilesList[index] #self.findMostSimilarCtSeriesFiles(ctSeriesFilesList, petFiles)
    
    else:
      return None
    
  def studyInstanceUIDForImage(self, file):
    """___"""
    return slicer.dicomDatabase.fileValue(file, self.tags['studyInstanceUID'])
        
  
  def studyInstanceUID(self,files):
    """___"""
    studyInstanceUID = self.studyInstanceUIDForImage(files[0])
    
    # commented for performance
    
    #for file in files:
      #tempStudyInstanceUID = self.studyInstanceUIDForImage(file)
      
      #if tempStudyInstanceUID != studyInstanceUID:
       #studyInstanceUID = ""
    
    return studyInstanceUID 
  
  
  def isSpecificModalityImage(self, file, modality):
    """___"""
    return slicer.dicomDatabase.fileValue(file,self.tags['seriesModality']) == modality


  def isSpecificSeries(self,files,modality):
    """ ___ """
    isSpecificSeries = True
      
    for file in files:
      if self.isSpecificModalityImage(file, modality) == False:
        isSpecificSeries = False 
                            
    return isSpecificSeries
  
  
  def volume(self,files):
    """___"""
    dims = self.dimensions(files)
    
    return dims[0]*dims[1]*dims[2]  
  
 
  def dimensions(self,files):
    """ ___ """
    w = self.sliceSurface(files[0])[0]
    h = self.sliceSurface(files[0])[1]
    d = 0  
    
    info = self.acquireImageSeriesComparisonInformation(files)
    if info:
      d = info['ZDimension']
        
    return [w,h,d]
  


  def acquireImageSeriesComparisonInformation(self, files):
    """ ___ """
    if not files:
      return None
    
    informations = {}
    
    minZ = 0
    maxZ = 0
    minContentTime = 0
            
    for i in xrange(len(files)):
      
      z = float(slicer.dicomDatabase.fileValue(files[i],self.tags['position']).split('\\')[2])
      t = float(slicer.dicomDatabase.fileValue(files[i], self.tags['contentTime']))
      
      # initialize with values of first image file
      if i==0:
        minZ = z
        maxZ = z
        minContentTime = t
      
      else:
        
        if z < minZ:
          minZ = z
        
        if z > maxZ:
          maxZ = z
        
        if t < minContentTime:
          minContentTime = t
    
    informations['ContentTime'] = minContentTime
    informations['ZDimension'] = math.fabs(maxZ-minZ)
    
    return informations
    
  
  def sliceSurface(self,file):
        
    rows = float(slicer.dicomDatabase.fileValue(file,self.tags['rows']))
    cols = float(slicer.dicomDatabase.fileValue(file,self.tags['columns']))
    spacingRows = float(slicer.dicomDatabase.fileValue(file,self.tags['spacing']).split('\\')[0])
    spacingCols = float(slicer.dicomDatabase.fileValue(file,self.tags['spacing']).split('\\')[1])

    width = cols * spacingCols
    height = rows * spacingRows

    return [width, height]


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
  
  def createScalarVolumeNode(self, vaStorageNode, loadable):
    vaStorageNode.SetFileName(loadable.files[0])
    vaStorageNode.ResetFileNameList()
    
    for file in loadable.files:
      vaStorageNode.AddFileName(file)
    
    vaStorageNode.SetSingleFile(0)
    
    svNode = slicer.vtkMRMLScalarVolumeNode()
    svNode.SetScene(slicer.mrmlScene)
    
    vaStorageNode.ReadData(svNode)
    
    if svNode:
      svNode.SetName(loadable.name)
      
      instanceUIDs = ""
      for file in loadable.files:
        uid = slicer.dicomDatabase.fileValue(file,self.tags['instanceUID'])
        if uid == "":
          uid = "Unknown"
        instanceUIDs += uid + " "
      instanceUIDs = instanceUIDs[:-1]  # strip last space
      svNode.SetAttribute("DICOM.instanceUIDs", instanceUIDs)
      
      svNode.SetScene(slicer.mrmlScene)
      slicer.mrmlScene.AddNode(svNode)
    
    return svNode
           

  def load(self,loadable):
    """ ___ """

    reportNode = slicer.mrmlScene.CreateNodeByClass('vtkMRMLLongitudinalPETCTReportNode')
    reportNode.SetReferenceCount(reportNode.GetReferenceCount()-1) 
    slicer.mrmlScene.AddNode(reportNode)

    if self.petFileLoadables:
      files = self.petFileLoadables[0].files
        
      patientName = slicer.dicomDatabase.fileValue(files[0], self.tags['patientName'])
      patientBirthDate = slicer.dicomDatabase.fileValue(files[0], self.tags['patientBirthDate'])
      patientSex = slicer.dicomDatabase.fileValue(files[0], self.tags['patientSex'])
      patientSize = slicer.dicomDatabase.fileValue(files[0], self.tags['patientSize'])
    
      #Report Node    
      reportNode.SetName('Report for '+patientName)
      reportNode.SetAttribute('DICOM.PatientName',patientName)
      reportNode.SetAttribute('DICOM.PatientBirthDate',patientBirthDate)
      reportNode.SetAttribute('DICOM.PatientSex',patientSex)
      reportNode.SetAttribute('DICOM.PatientSize',patientSize)
      
      colorLogic = slicer.modules.colors.logic()
      defaultColorNodeID = colorLogic.GetDefaultEditorColorNodeID()
      colorNode = slicer.mrmlScene.GetNodeByID(defaultColorNodeID)

      reportNode.SetColorNodeID(colorNode.GetID())
      logic = slicer.modules.longitudinalpetct.logic()

      colorTable = logic.GetDefaultFindingTypesColorTable(colorNode)
      #colorTable.SetReferenceCount(colorTable.GetReferenceCount()-1)
      reportNode.SetAndObserveFindingTypesColorTableNodeID(colorTable.GetID())


      vaStorageNode = slicer.vtkMRMLVolumeArchetypeStorageNode()

      if len(self.petFileLoadables) == len(self.ctFileLoadables):
        i = 0
        while i < len(self.petFileLoadables):

          petScalarVolume = self.createScalarVolumeNode(vaStorageNode, self.petFileLoadables[i])
          ctScalarVolume = self.createScalarVolumeNode(vaStorageNode, self.ctFileLoadables[i])

          volLogic  = slicer.modules.volumes.logic() 
          petLabelVolume = volLogic.CreateAndAddLabelVolume(slicer.mrmlScene,petScalarVolume,self.petFileLoadables[i].name+"_LabelVolume")     

          studyID = slicer.dicomDatabase.fileValue(self.petFileLoadables[i].files[0], self.tags['studyID'])
          studyUID = self.studyInstanceUIDForImage(self.petFileLoadables[i].files[0])
          studyDate = self.studyDateImageFile(self.petFileLoadables[i].files[0])
          studyTime = slicer.dicomDatabase.fileValue(self.petFileLoadables[i].files[0], self.tags['studyTime'])

          radioPharmaconStartTime = slicer.dicomDatabase.fileValue(self.petFileLoadables[i].files[0], self.tags['radioPharmaconStartTime'])
          decayCorrection = slicer.dicomDatabase.fileValue(self.petFileLoadables[i].files[0], self.tags['decayCorrection'])
          decayFactor = slicer.dicomDatabase.fileValue(self.petFileLoadables[i].files[0], self.tags['decayFactor'])
          frameRefTime = slicer.dicomDatabase.fileValue(self.petFileLoadables[i].files[0], self.tags['frameRefTime'])
          radionuclideHalfLife = slicer.dicomDatabase.fileValue(self.petFileLoadables[i].files[0], self.tags['radionuclideHalfLife'])
          seriesTime = slicer.dicomDatabase.fileValue(self.petFileLoadables[i].files[0], self.tags['seriesTime'])
          patientWeight = slicer.dicomDatabase.fileValue(self.petFileLoadables[i].files[0], self.tags['patientWeight'])

          
          #Study Node
          studyNode = slicer.mrmlScene.CreateNodeByClass('vtkMRMLLongitudinalPETCTStudyNode')
          studyNode.SetReferenceCount(studyNode.GetReferenceCount()-1)  
          

          studyNode.SetName('Study_'+str(i))

          studyNode.SetAttribute('DICOM.StudyID',studyID)
          studyNode.SetAttribute('DICOM.StudyInstanceUID',studyUID)
          studyNode.SetAttribute('DICOM.StudyDate',studyDate)
          studyNode.SetAttribute('DICOM.StudyTime',studyTime)
          studyNode.SetAttribute('DICOM.RadioPharmaconStartTime',radioPharmaconStartTime)
          studyNode.SetAttribute('DICOM.DecayFactor',decayFactor)
          studyNode.SetAttribute('DICOM.DecayCorrection',decayCorrection)
          studyNode.SetAttribute('DICOM.FrameReferenceTime',frameRefTime)
          studyNode.SetAttribute('DICOM.RadionuclideHalfLife',radionuclideHalfLife)
          studyNode.SetAttribute('DICOM.SeriesTime',seriesTime)
          studyNode.SetAttribute('DICOM.PatientWeight',patientWeight)


          studyNode.SetAndObservePETVolumeNodeID(petScalarVolume.GetID())
          studyNode.SetAndObserveCTVolumeNodeID(ctScalarVolume.GetID())
          studyNode.SetAndObservePETLabelVolumeNodeID(petLabelVolume.GetID())
          
          slicer.mrmlScene.AddNode(studyNode) 
                  
          reportNode.AddStudyNodeID(studyNode.GetID())
          
          
          if logic:
            logic.CenterStudyVolumeNodes(studyNode,slicer.mrmlScene)
        
          i += 1 

      
    
    return reportNode
  
#
# DICOMLongitudinalPETCTPlugin
#

class DICOMLongitudinalPETCTPlugin:
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
    slicer.modules.dicomPlugins['DICOMLongitudinalPETCTPlugin'] = DICOMLongitudinalPETCTPluginClass

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


