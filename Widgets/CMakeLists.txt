project(qSlicer${MODULE_NAME}ModuleWidgets)

set(KIT ${PROJECT_NAME})

set(${KIT}_EXPORT_DIRECTIVE "Q_SLICER_${MODULE_NAME_UPPER}_MODULE_WIDGETS_EXPORT")

set(${KIT}_INCLUDE_DIRECTORIES
  )

set(${KIT}_SRCS
  qSlicerLongPETCTAnalysisSettingsWidget.cxx
  qSlicerLongPETCTAnalysisSettingsWidget.h
  qSlicerLongPETCTColorSelectionDialog.cxx
  qSlicerLongPETCTColorSelectionDialog.h
  qSlicerLongPETCTFindingSelectionWidget.cxx
  qSlicerLongPETCTFindingSelectionWidget.h
  qSlicerLongPETCTFindingSettingsDialog.cxx
  qSlicerLongPETCTFindingSettingsDialog.h
  qSlicerLongPETCTReportSelectionWidget.cxx
  qSlicerLongPETCTReportSelectionWidget.h
  qSlicerLongPETCTReportTableWidget.cxx
  qSlicerLongPETCTReportTableWidget.h
  qSlicerLongPETCTSegmentationTableCellWidget.cxx
  qSlicerLongPETCTSegmentationTableCellWidget.h
  qSlicerLongPETCTStudySelectionWidget.cxx
  qSlicerLongPETCTStudySelectionWidget.h
  qSlicerLongPETCTStudySliderWidget.cxx
  qSlicerLongPETCTStudySliderWidget.h
  )

set(${KIT}_MOC_SRCS
  qSlicerLongPETCTAnalysisSettingsWidget.h
  qSlicerLongPETCTColorSelectionDialog.h
  qSlicerLongPETCTFindingSelectionWidget.h
  qSlicerLongPETCTFindingSettingsDialog.h
  qSlicerLongPETCTReportSelectionWidget.h
  qSlicerLongPETCTReportTableWidget.h
  qSlicerLongPETCTSegmentationTableCellWidget.h
  qSlicerLongPETCTStudySelectionWidget.h
  qSlicerLongPETCTStudySliderWidget.h  
  )

# UI files
set(${KIT}_UI_SRCS
  ../Resources/UI/qSlicerLongPETCTAnalysisSettingsWidget.ui
  ../Resources/UI/qSlicerLongPETCTColorSelectionDialog.ui
  ../Resources/UI/qSlicerLongPETCTFindingSelectionWidget.ui
  ../Resources/UI/qSlicerLongPETCTFindingSettingsDialog.ui
  ../Resources/UI/qSlicerLongPETCTReportSelectionWidget.ui
  ../Resources/UI/qSlicerLongPETCTSegmentationTableCellWidget.ui
  ../Resources/UI/qSlicerLongPETCTStudySelectionWidget.ui
  ../Resources/UI/qSlicerLongPETCTStudySliderWidget.ui
  )
  
set(${KIT}_RESOURCES
  ../Resources/qSlicerLongPETCTModule.qrc
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

