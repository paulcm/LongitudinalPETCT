project(qSlicer${MODULE_NAME}ModuleWidgets)

set(KIT ${PROJECT_NAME})

set(${KIT}_EXPORT_DIRECTIVE "Q_SLICER_${MODULE_NAME_UPPER}_MODULE_WIDGETS_EXPORT")

set(${KIT}_INCLUDE_DIRECTORIES
  )

set(${KIT}_SRCS
  qMRMLLongitudinalPETCTAnalysisWidget.cxx
  qMRMLLongitudinalPETCTAnalysisWidget.h
  qMRMLLongitudinalPETCTFindingWidget.cxx
  qMRMLLongitudinalPETCTFindingWidget.h
  qMRMLLongitudinalPETCTFindingSettingsDialog.cxx
  qMRMLLongitudinalPETCTFindingSettingsDialog.h
  qMRMLLongitudinalPETCTModuleSettingsWidget.cxx
  qMRMLLongitudinalPETCTModuleSettingsWidget.h
  qMRMLLongitudinalPETCTReportWidget.cxx
  qMRMLLongitudinalPETCTReportWidget.h
  qMRMLLongitudinalPETCTReportOverviewWidget.cxx
  qMRMLLongitudinalPETCTReportOverviewWidget.h
  qMRMLLongitudinalPETCTStudyTableWidget.cxx
  qMRMLLongitudinalPETCTStudyTableWidget.h
  qMRMLLongitudinalPETCTStudyWidget.cxx
  qMRMLLongitudinalPETCTStudyWidget.h
  qMRMLLongitudinalPETCTDialog.cxx
  qMRMLLongitudinalPETCTDialog.h
  qMRMLLongitudinalPETCTWidget.cxx
  qMRMLLongitudinalPETCTWidget.h
  )

set(${KIT}_MOC_SRCS
  qMRMLLongitudinalPETCTAnalysisWidget.h
  qMRMLLongitudinalPETCTFindingWidget.h
  qMRMLLongitudinalPETCTFindingSettingsDialog.h
  qMRMLLongitudinalPETCTModuleSettingsWidget.h
  qMRMLLongitudinalPETCTReportWidget.h
  qMRMLLongitudinalPETCTReportOverviewWidget.h
  qMRMLLongitudinalPETCTStudyTableWidget.h
  qMRMLLongitudinalPETCTStudyWidget.h
  qMRMLLongitudinalPETCTDialog.h
  qMRMLLongitudinalPETCTWidget.h
  )

# UI files
set(${KIT}_UI_SRCS
  ../Resources/UI/qMRMLLongitudinalPETCTAnalysisWidget.ui
  ../Resources/UI/qMRMLLongitudinalPETCTFindingWidget.ui
  ../Resources/UI/qMRMLLongitudinalPETCTFindingSettingsDialog.ui
  ../Resources/UI/qMRMLLongitudinalPETCTModuleSettingsWidget.ui
  ../Resources/UI/qMRMLLongitudinalPETCTReportWidget.ui
  ../Resources/UI/qMRMLLongitudinalPETCTReportOverviewWidget.ui
  ../Resources/UI/qMRMLLongitudinalPETCTStudyTableWidget.ui
  ../Resources/UI/qMRMLLongitudinalPETCTStudyWidget.ui
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

