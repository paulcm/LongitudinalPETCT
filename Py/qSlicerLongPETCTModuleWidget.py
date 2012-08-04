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
    self.layout.addWidget(self.reloadButton)
    self.reloadButton.connect('clicked()', self.onReload)


    # Collapsible button
    reportsCollapsibleButton = ctk.ctkCollapsibleButton()
    reportsCollapsibleButton.text = "Report Selection"
    self.layout.addWidget(reportsCollapsibleButton)

    # Layout within the dummy collapsible button
    reportsLayout = qt.QVBoxLayout(reportsCollapsibleButton)

    self.reportSelector = slicer.qMRMLNodeComboBox()
    self.reportSelector.nodeTypes = ['vtkMRMLLongPETCTReportNode']
    self.reportSelector.setMRMLScene(slicer.mrmlScene)
    self.reportSelector.addEnabled = 1

    self.reportSelectionWidget = slicer.modulewidget.qSlicerLongPETCTReportSelectionWidget(self.reportSelector)    
    
    reportsLayout.addWidget(self.reportSelectionWidget)
    

    #helloWorldButton.connect('clicked(bool)', self.onHelloWorldButtonClicked)

    # Add vertical spacer
    self.layout.addStretch()
    
    
    # Add Study Slider
    self.studySlider = slicer.modulewidget.qSlicerLongPETCTStudySliderWidget()
    self.layout.addWidget(self.studySlider)

  def onHelloWorldButtonClicked(self):
    print "Hello World !"

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
