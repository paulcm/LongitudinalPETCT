project(vtkSlicer${MODULE_NAME}ModuleMRML)

set(KIT ${PROJECT_NAME})

set(${KIT}_EXPORT_DIRECTIVE "VTK_SLICER_${MODULE_NAME_UPPER}_MODULE_MRML_EXPORT")

set(${KIT}_INCLUDE_DIRECTORIES
	${CMAKE_BIN_DIR}/MRML
  )

set(${KIT}_SRCS
  vtkMRML${MODULE_NAME}StudyNode.cxx
  vtkMRML${MODULE_NAME}StudyNode.h
  vtkMRML${MODULE_NAME}FindingNode.cxx
  vtkMRML${MODULE_NAME}FindingNode.h
  vtkMRML${MODULE_NAME}ReportNode.cxx
  vtkMRML${MODULE_NAME}ReportNode.h
  )

set(${KIT}_TARGET_LIBRARIES
  ${MRML_LIBRARIES}
  )

#-----------------------------------------------------------------------------
SlicerMacroBuildModuleMRML(
  NAME ${KIT}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${${KIT}_INCLUDE_DIRECTORIES}
  SRCS ${${KIT}_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  )

