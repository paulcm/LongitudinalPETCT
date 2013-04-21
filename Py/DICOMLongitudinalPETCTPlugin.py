import os
import sys as SYS
from __main__ import vtk, qt, ctk, slicer
from DICOMLib import DICOMPlugin
from DICOMLib import DICOMLoadable


import DICOMLib

import math

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
    self.tags['patientHeight'] = "0010,1020"
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
    self.tags['seriesInstanceUID'] = "0020,000E"
    self.tags['sopInstanceUID'] = "0008,0018"
  
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

    self.scalarVolumePlugin = slicer.modules.dicomPlugins['DICOMScalarVolumePlugin']()
  
  
  def __getDirectoryOfImageSeries(self, sopInstanceUID):
    file = slicer.dicomDatabase.fileForInstance(sopInstanceUID)
    return os.path.dirname(file)  

  def __getSeriesInformation(self,seriesFiles,dicomTag):
    if seriesFiles:
      return  slicer.dicomDatabase.fileValue(seriesFiles[0],dicomTag)          



  def __scanForValidPETCTStudies(self,fileLists):
    
    petStudies = set()
    ctStudies = set()
    
    petCtStudies = []
    
    for fileList in fileLists:
    
      studyUID = self.__getSeriesInformation(fileList, self.tags['studyInstanceUID'])
      modality = self.__getSeriesInformation(fileList, self.tags['seriesModality'])  
    
      if modality == self.petTerm:
        petStudies.add(studyUID)    
      
      if modality == self.ctTerm:
        ctStudies.add(studyUID)
    
    for stdUID in petStudies:
      if stdUID in ctStudies:
        petCtStudies.append(stdUID)       
    
    
    return petCtStudies
          

  def examine(self,fileLists):
    """ Returns a list of DICOMLoadable instances
    corresponding to ways of interpreting the
    fileLists parameter.
    """
    petCtStudies = self.__scanForValidPETCTStudies(fileLists)
    
    mainLoadable = DICOMLib.DICOMLoadable()
    mainLoadable.tooltip = "PET/CT Studies for Longitudinal PET/CT Report"
    
    studyplrlsing = "Studies" if len(petCtStudies) != 1 else "Study" 
    mainLoadable.name = str(len(petCtStudies))+" PET/CT "+ studyplrlsing
    
    mainLoadable.selected = True
    mainLoadable.confidence = 1.0                
    
    petImageSeries = 0
    ctImageSeries = 0
    
    for fileList in fileLists:
      
      petSeries = self.__getSeriesInformation(fileList, self.tags['seriesModality']) == self.petTerm
      ctSeries = self.__getSeriesInformation(fileList, self.tags['seriesModality']) == self.ctTerm
      fromPetCtStudy = self.__getSeriesInformation(fileList, self.tags['studyInstanceUID']) in petCtStudies
      
      if (petSeries | ctSeries) & fromPetCtStudy:
        
        loadables = self.getCachedLoadables(fileList)
        
        if not loadables:
          loadables = self.scalarVolumePlugin.examineFiles(fileList)
          self.cacheLoadables(fileList, loadables)
        
        for ldbl in loadables:
          if ldbl.selected:
            type = ""  
            if petSeries:
              petImageSeries += 1
              type = "PT"
            elif ctSeries:
              ctImageSeries += 1
              type = "CT"
            
            mainLoadable.files += ldbl.files
            
            ldbl.name = type +"_"+self.__getSeriesInformation(ldbl.files, self.tags['studyDate'])+"_"+self.__getSeriesInformation(ldbl.files, self.tags['seriesTime'])
            self.cacheLoadables(ldbl.files, [ldbl])   
            
            break # break for loop because only one loadable needed  
                                   
    mainLoadable.name = mainLoadable.name + " containing " + str(petImageSeries) + " PET and " + str(ctImageSeries) + " CT image series"
                  
    return [mainLoadable]

                

           

  def __seperateFilesListIntoImageSeries(self, files):
    
    imageSeries = {}
    
    for file in files:
      seriesUID = self.__getSeriesInformation([file], self.tags['seriesInstanceUID'])
      
      if (seriesUID in imageSeries) == False:
        imageSeries[seriesUID] = [] 
            
      imageSeries[seriesUID].append(file)  
      
    return imageSeries


  def __seperateImageSeriesAndFilesIntoStudies(self, imageSeriesAndFiles):
    
    studies = {}
    
    for seriesUID in imageSeriesAndFiles.keys():
      
      if len(imageSeriesAndFiles[seriesUID]) == 0:
        continue    
      
      studyUID = self.__getSeriesInformation(imageSeriesAndFiles[seriesUID], self.tags['studyInstanceUID'])   
      
      if (studyUID in studies) == False:
        studies[studyUID] = []
      
      studies[studyUID].append(seriesUID)
    
      
    return studies
    
    
  def __extractSpecificModalitySeriesForStudies(self, studiesAndImageSeries, imageSeriesAndFiles, modality):
    
    seriesForStudies = {}
    
    for studyUID in studiesAndImageSeries.keys():
      
      seriesForStudies[studyUID] = []
        
      seriesList = studiesAndImageSeries[studyUID] 
      
      for seriesUID in seriesList:
        
        if seriesUID in imageSeriesAndFiles.keys():
        
          if len(imageSeriesAndFiles[seriesUID]) == 0:
            continue
        
          if self.__getSeriesInformation(imageSeriesAndFiles[seriesUID], self.tags['seriesModality']) == modality:   
            seriesForStudies[studyUID].append(seriesUID)      

    return seriesForStudies         
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
  
  def __getImageSeriesDescription(self,files):
    
    rows = self.__getSeriesInformation(files, self.tags['rows'])
    columns = self.__getSeriesInformation(files, self.tags['columns'])
    slices = len(files)
    
    spacingRows = self.__getSeriesInformation(files, self.tags['spacing']).split('\\')[0]
    spacingCols = self.__getSeriesInformation(files, self.tags['spacing']).split('\\')[1]
    
    width = int(columns) * float(spacingCols)
    height = int(rows) * float(spacingRows)
    
    seriesTime = self.__getSeriesInformation(files, self.tags['seriesTime'])          
    seriesTime = seriesTime[:2]+":"+seriesTime[2:4]+":"+seriesTime[4:6]
    return "Series Time: "+seriesTime+ " | Width: "+str(width)+"mm | Height: "+str(height)+"mm | Slices: "+str(slices)

  
  def load(self,loadable):
    
    imageSeriesAndFiles = self.__seperateFilesListIntoImageSeries(loadable.files) 
    studiesAndImageSeries = self.__seperateImageSeriesAndFilesIntoStudies(imageSeriesAndFiles)
    
    petImageSeriesInStudies = self.__extractSpecificModalitySeriesForStudies(studiesAndImageSeries, imageSeriesAndFiles, self.petTerm)
    ctImageSeriesInStudies = self.__extractSpecificModalitySeriesForStudies(studiesAndImageSeries, imageSeriesAndFiles, self.ctTerm)
    
    # create Report node            
    reportNode = slicer.mrmlScene.CreateNodeByClass('vtkMRMLLongitudinalPETCTReportNode')
    reportNode.SetReferenceCount(reportNode.GetReferenceCount()-1)
    slicer.mrmlScene.AddNode(reportNode)   

    # setup Report's default color node and table for Finding types
    colorLogic = slicer.modules.colors.logic()
    defaultColorNodeID = colorLogic.GetDefaultEditorColorNodeID()
    colorNode = slicer.mrmlScene.GetNodeByID(defaultColorNodeID)

    reportNode.SetColorNodeID(colorNode.GetID())
    logic = slicer.modules.longitudinalpetct.logic()

    colorTable = logic.GetDefaultFindingTypesColorTable(colorNode)
    colorTable.SetReferenceCount(colorTable.GetReferenceCount()-1)
    reportNode.SetAndObserveFindingTypesColorTableNodeID(colorTable.GetID())
      
    patientName = None
    patientBirthDate = None
    patientSex = None
    patientHeight = None  
    
    for studyUID in studiesAndImageSeries.keys():
      petDescriptions = [] #petImageSeriesInStudies[studyUID]
      ctDescriptions = []
      studyDescription = "Multiple PET and/or CT image series have been found within a Study. Please change the selection if a different pair of image series should be loaded." 
      
      
      for petUID in petImageSeriesInStudies[studyUID]:
        petDescriptions.append(self.__getImageSeriesDescription(imageSeriesAndFiles[petUID]))    
      
      for ctUID in ctImageSeriesInStudies[studyUID]:
        ctDescriptions.append(self.__getImageSeriesDescription(imageSeriesAndFiles[ctUID]))     
            
      
      selectedIndexes = [0,0]           
     
      if (len(petDescriptions) > 1) | (len(ctDescriptions) > 1):
        dialog = PETCTSeriesSelectorDialog(None,studyDescription,petDescriptions, ctDescriptions, 0, 0)
        dialog.parent.exec_()
        selectedIndexes = dialog.getSelectedSeries()
      
      # UID of current PET series
      petImageSeriesUID = petImageSeriesInStudies[studyUID][selectedIndexes[0]]
      # UID of current CT series 
      ctImageSeriesUID = ctImageSeriesInStudies[studyUID][selectedIndexes[1]]
      
      
      # gathering patient information for Report node
      if not patientName:
        name = self.__getSeriesInformation(imageSeriesAndFiles[petImageSeriesUID], self.tags['patientName'])
        reportNode.SetAttribute('DICOM.PatientName', name)
        reportNode.SetName("Report for "+name)
      if not patientBirthDate:
        reportNode.SetAttribute('DICOM.PatientBirthDate', self.__getSeriesInformation(imageSeriesAndFiles[petImageSeriesUID], self.tags['patientBirthDate']))
      if not patientSex:
         reportNode.SetAttribute('DICOM.PatientSex', self.__getSeriesInformation(imageSeriesAndFiles[petImageSeriesUID], self.tags['patientSex']))
      if not patientHeight:
        reportNode.SetAttribute('DICOM.PatientHeight', self.__getSeriesInformation(imageSeriesAndFiles[petImageSeriesUID], self.tags['patientHeight']))
                        
      # create PET SUV volume node
      petVolumeNode = self.scalarVolumePlugin.load(self.getCachedLoadables(imageSeriesAndFiles[petImageSeriesUID])[0])
      petDir = self.__getDirectoryOfImageSeries(self.__getSeriesInformation(imageSeriesAndFiles[petImageSeriesUID], self.tags['sopInstanceUID']))
      
      parameters = {}
      parameters["PETVolume"] = petVolumeNode.GetID()
      parameters["PETDICOMPath"] = petDir
      parameters["SUVVolume"] = petVolumeNode.GetID()

      cliNode = None
      cliNode = slicer.cli.run(slicer.modules.petsuvimagemaker, cliNode, parameters) 
      
      # create PET label volume node
      volLogic = slicer.modules.volumes.logic()
      petLabelVolumeNode = volLogic.CreateAndAddLabelVolume(slicer.mrmlScene,petVolumeNode,petVolumeNode.GetName()+"_LabelVolume")
      
      
      # create CT volume node
      ctVolumeNode = self.scalarVolumePlugin.load(self.getCachedLoadables(imageSeriesAndFiles[ctImageSeriesUID])[0])
      
      # create Study node
      studyNode = slicer.mrmlScene.CreateNodeByClass('vtkMRMLLongitudinalPETCTStudyNode')
      studyNode.SetReferenceCount(studyNode.GetReferenceCount()-1)
      
      studyNode.SetName("Study_"+ self.__getSeriesInformation(imageSeriesAndFiles[petImageSeriesUID], self.tags['studyDate']))

      studyNode.SetAttribute('DICOM.StudyID',self.__getSeriesInformation(imageSeriesAndFiles[petImageSeriesUID], self.tags['studyID']))
      studyNode.SetAttribute('DICOM.StudyInstanceUID',self.__getSeriesInformation(imageSeriesAndFiles[petImageSeriesUID], self.tags['studyInstanceUID']))
      studyNode.SetAttribute('DICOM.StudyDate',self.__getSeriesInformation(imageSeriesAndFiles[petImageSeriesUID], self.tags['studyDate']))
      studyNode.SetAttribute('DICOM.StudyTime',self.__getSeriesInformation(imageSeriesAndFiles[petImageSeriesUID], self.tags['studyTime']))
      studyNode.SetAttribute('DICOM.RadioPharmaconStartTime',self.__getSeriesInformation(imageSeriesAndFiles[petImageSeriesUID], self.tags['radioPharmaconStartTime']))
      studyNode.SetAttribute('DICOM.DecayFactor',self.__getSeriesInformation(imageSeriesAndFiles[petImageSeriesUID], self.tags['decayCorrection']))
      studyNode.SetAttribute('DICOM.DecayCorrection',self.__getSeriesInformation(imageSeriesAndFiles[petImageSeriesUID], self.tags['decayFactor']))
      studyNode.SetAttribute('DICOM.FrameReferenceTime',self.__getSeriesInformation(imageSeriesAndFiles[petImageSeriesUID], self.tags['frameRefTime']))
      studyNode.SetAttribute('DICOM.RadionuclideHalfLife',self.__getSeriesInformation(imageSeriesAndFiles[petImageSeriesUID], self.tags['radionuclideHalfLife']))
      studyNode.SetAttribute('DICOM.SeriesTime',self.__getSeriesInformation(imageSeriesAndFiles[petImageSeriesUID], self.tags['seriesTime']))
      studyNode.SetAttribute('DICOM.PatientWeight',self.__getSeriesInformation(imageSeriesAndFiles[petImageSeriesUID], self.tags['patientWeight']))
      
      studyNode.SetAndObservePETVolumeNodeID(petVolumeNode.GetID())
      studyNode.SetAndObserveCTVolumeNodeID(ctVolumeNode.GetID())
      studyNode.SetAndObservePETLabelVolumeNodeID(petLabelVolumeNode.GetID())
          
      slicer.mrmlScene.AddNode(studyNode) 
      reportNode.AddStudyNodeID(studyNode.GetID())
                    
    return reportNode              
      

class PETCTSeriesSelectorDialog(object):
  
  def __init__(self, parent=None, studyDescription="",petDescriptions=[],ctDescriptions=[],petSelection=0,ctSelection=0):
    
    self.studyDescription = studyDescription
    self.petDescriptions = petDescriptions
    self.ctDescriptions = ctDescriptions  
    self.petSelection = petSelection
    self.ctSelection = ctSelection
    
    if not parent:
      self.parent = qt.QDialog()
      self.parent.setModal(True)
      self.parent.setLayout(qt.QGridLayout())
      self.layout = self.parent.layout()
      self.setup()
      self.parent.show()
      
    else:
      self.parent = parent
      self.layout = parent.layout() 
      
  
  def setup(self):
      
    self.studyLabel = qt.QLabel(self.studyDescription)
    self.studyLabel.setAlignment(qt.Qt.AlignCenter)
    self.petLabel = qt.QLabel("PET Image Series")
    self.petLabel.setAlignment(qt.Qt.AlignCenter)
    self.ctLabel = qt.QLabel("CT Image Series")
    self.ctLabel.setAlignment(qt.Qt.AlignCenter)
    self.petList = qt.QListWidget()
    self.petList.addItems(self.petDescriptions)
    self.petList.setCurrentRow(self.petSelection)
    self.ctList = qt.QListWidget()
    self.ctList.addItems(self.ctDescriptions)
    self.ctList.setCurrentRow(self.ctSelection)
    self.button = qt.QPushButton("Ok")
    
    self.layout.addWidget(self.studyLabel,0,0,1,2)
    self.layout.addWidget(self.petLabel,1,0,1,1)
    self.layout.addWidget(self.ctLabel,1,1,1,1)
    self.layout.addWidget(self.petList,2,0,1,1)
    self.layout.addWidget(self.ctList,2,1,1,1)
    self.layout.addWidget(self.button,3,0,1,2)
    
    self.button.connect('clicked()',self.parent.close)    
      

  def getSelectedSeries(self):
    return [self.petList.currentRow, self.ctList.currentRow]              
          
          
  
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


