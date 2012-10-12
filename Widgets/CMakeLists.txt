project(qSlicer${MODULE_NAME}ModuleWidgets)

set(KIT ${PROJECT_NAME})

set(${KIT}_EXPORT_DIRECTIVE "Q_SLICER_${MODULE_NAME_UPPER}_MODULE_WIDGETS_EXPORT")

set(${KIT}_INCLUDE_DIRECTORIES
  )

set(${KIT}_SRCS
  qSlicerLongitudinalPETCTAnalysisSettingsWidget.cxx
  qSlicerLongitudinalPETCTAnalysisSettingsWidget.h
  qSlicerLongitudinalPETCTColorSelectionDialog.cxx
  qSlicerLongitudinalPETCTColorSelectionDialog.h
  qSlicerLongitudinalPETCTFindingSelectionWidget.cxx
  qSlicerLongitudinalPETCTFindingSelectionWidget.h
  qSlicerLongitudinalPETCTFindingSettingsDialog.cxx
  qSlicerLongitudinalPETCTFindingSettingsDialog.h
  qSlicerLongitudinalPETCTReportSelectionWidget.cxx
  qSlicerLongitudinalPETCTReportSelectionWidget.h
  qSlicerLongitudinalPETCTReportTableWidget.cxx
  qSlicerLongitudinalPETCTReportTableWidget.h
  qSlicerLongitudinalPETCTStudySelectionTableWidget.cxx
  qSlicerLongitudinalPETCTStudySelectionTableWidget.h
  qSlicerLongitudinalPETCTStudySelectionWidget.cxx
  qSlicerLongitudinalPETCTStudySelectionWidget.h
  )

set(${KIT}_MOC_SRCS
  qSlicerLongitudinalPETCTAnalysisSettingsWidget.h
  qSlicerLongitudinalPETCTColorSelectionDialog.h
  qSlicerLongitudinalPETCTFindingSelectionWidget.h
  qSlicerLongitudinalPETCTFindingSettingsDialog.h
  qSlicerLongitudinalPETCTReportSelectionWidget.h
  qSlicerLongitudinalPETCTReportTableWidget.h
  qSlicerLongitudinalPETCTStudySelectionTableWidget.h
  qSlicerLongitudinalPETCTStudySelectionWidget.h
  )

# UI files
set(${KIT}_UI_SRCS
  ../Resources/UI/qSlicerLongitudinalPETCTAnalysisSettingsWidget.ui
  ../Resources/UI/qSlicerLongitudinalPETCTColorSelectionDialog.ui
  ../Resources/UI/qSlicerLongitudinalPETCTFindingSelectionWidget.ui
  ../Resources/UI/qSlicerLongitudinalPETCTFindingSettingsDialog.ui
  ../Resources/UI/qSlicerLongitudinalPETCTReportSelectionWidget.ui
  ../Resources/UI/qSlicerLongitudinalPETCTReportTableWidget.ui
  ../Resources/UI/qSlicerLongitudinalPETCTStudySelectionTableWidget.ui
  ../Resources/UI/qSlicerLongitudinalPETCTStudySelectionWidget.ui
  )
  
set(${KIT}_RESOURCES
  ../Resources/qSlicerLongitudinalPETCTModule.qrc
  )  
  
set(${KIT}_TARGET_LIBRARIES
  vtkSlicer${MODULE_NAME}ModuleLogic
  )
  
# Python wrap
#if(Slicer_USE_PYTHONQT)
#  list(APPEND KIT_target_libraries
#    CTKScriptingPythonWidgets
#    )
#endif()

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

