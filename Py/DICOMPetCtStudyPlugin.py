import os
from __main__ import vtk, qt, ctk, slicer
from DICOMLib import DICOMPlugin
from DICOMLib import DICOMLoadable
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
    self.loadType = "PET/CT Study"
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
  
    self.tags['studyDate'] = "0008,0020"
    self.tags['studyTime'] = "0008,0030"
    
    self.tags['smallestPixelVal'] = "0028,0108"
    self.tags['largestPixelVal'] = "0028,0109"
  
    #self.tags['seriesTime'] = "0008,0031"
    

  def examine(self,fileLists):
    """ Returns a list of DICOMLoadable instances
    corresponding to ways of interpreting the
    fileLists parameter.
    """
    
    print "FileLists Size: " + str(len(fileLists))
    loadables = []
    
    
    for files in fileLists:
      if self.isSpecificSeries(files, 'PT'):
          print "IS PET"
      elif self.isSpecificSeries(files, 'CT'):
          print "IS CT"
      else:
          print "IS NEITHER PET NOR CT"
          break
          
      loadables += self.examineFiles(files)
    return loadables

#TODO PixelData and Orientation

  def isSpecificSeries(self,files,type):
      """ ___ """
      isSpecificSeries = True
      
      smallestVal = 999999999
      
      for file in files:
        if slicer.dicomDatabase.fileValue(file,self.tags['seriesModality']) != type:
          isSpecificSeries = False
        
        val = slicer.dicomDatabase.fileValue(file,self.tags['smallestPixelVal'])
        if val != "":
          tempVal = Decimal(val)
          if tempVal < smallestVal:
            smallestVal = tempVal
          
      
      print "Smallest Pixel Value: " + str(smallestVal)
          
      return isSpecificSeries
  
  

  def examineFiles(self,files):
    """ Returns a list of DICOMLoadable instances
    corresponding to ways of interpreting the
    files parameter.
    Process is to look for 'known' private tags corresponding
    to the types of pet/ct datasets that the DicomToNrrd utility
    should be able to process. Only need to look at one header
    in the series since all should be the same with respect
    to this check.
    """
    
    patients = slicer.dicomDatabase.patients()
    # get the series description to use as base for volume name

    name = slicer.dicomDatabase.fileValue(files[0], self.tags['seriesDescription'])
    if name == "":
      name = "Unknown"

    isPET = False
    hasRelated = False
    
    modality = slicer.dicomDatabase.fileValue(files[0], self.tags['seriesModality'])
    related = slicer.dicomDatabase.fileValue(files[0], self.tags['relatedSeriesSequence'])
    isPET = modality == "PT"
    hasRelated = related != ""

    loadables = []
    
    if isPET:
      loadable = DICOMLib.DICOMLoadable()
      loadable.files = files
      loadable.name = name + ' ' + slicer.dicomDatabase.fileValue(files[0],self.tags['studyDate'])
      loadable.selected = False
      loadable.tooltip = "Appears to be PET"
      loadables = [loadable] 
          
               
    return loadables
      

  def loadFilesWithArchetype(self,files,name):
    """Load files in the traditional Slicer manner
    using the volume logic helper class
    and the vtkITK archetype helper code
    """
    fileList = vtk.vtkStringArray()
    for f in files:
      fileList.InsertNextValue(f)
    volumesLogic = slicer.modules.volumes.logic()
    return (volumesLogic.AddArchetypeVolume( files[0], name, 0, fileList ))


  def load(self,loadable):
    """Load the select as a scalar volume
    """
    
    
    volumeNode = self.loadFilesWithArchetype(loadable.files, loadable.name)
    if volumeNode:
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
      volumeNode.SetAttribute("DICOM.instanceUIDs", instanceUIDs)

      #
      # automatically select the volume to display
      #
      appLogic = slicer.app.applicationLogic()
      selNode = appLogic.GetSelectionNode()
      selNode.SetReferenceActiveVolumeID(volumeNode.GetID())
      appLogic.PropagateVolumeSelection()
    return volumeNode


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


