project(qSlicer${MODULE_NAME}ModuleWidgets)

set(KIT ${PROJECT_NAME})

set(${KIT}_EXPORT_DIRECTIVE "Q_SLICER_MODULE_${MODULE_NAME_UPPER}_WIDGETS_EXPORT")

set(${KIT}_INCLUDE_DIRECTORIES
  )

set(${KIT}_SRCS
  qSlicerLongPETCTReportTree.cxx
  qSlicerLongPETCTReportTree.h
  qSlicerLongPETCTReportSelectionWidget.cxx
  qSlicerLongPETCTReportSelectionWidget.h
  qSlicerLongPETCTStudySelectionWidget.cxx
  qSlicerLongPETCTStudySelectionWidget.h
  qSlicerLongPETCTStudySliderWidget.cxx
  qSlicerLongPETCTStudySliderWidget.h
  )

set(${KIT}_MOC_SRCS
  qSlicerLongPETCTReportTree.h
  qSlicerLongPETCTReportSelectionWidget.h
  qSlicerLongPETCTStudySelectionWidget.h
  qSlicerLongPETCTStudySliderWidget.h  
  )

# UI files
set(${KIT}_UI_SRCS
  )

# Resources
set(${KIT}_RESOURCES
  )
  

set(${KIT}_TARGET_LIBRARIES
  vtkSlicer${MODULE_NAME}ModuleLogic
  )

#-----------------------------------------------------------------------------
SlicerMacroBuildModuleWidgets(
  NAME ${KIT}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${${KIT}_INCLUDE_DIRECTORIES}
  SRCS ${${KIT}_SRCS}
  MOC_SRCS ${${KIT}_MOC_SRCS}
  UI_SRCS ${${KIT}_UI_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  RESOURCES ${${KIT}_RESOURCES}
  WRAP_PYTHONQT
  )

