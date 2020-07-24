#-------------------------------------------------------
# Support macro to use a precompiled header
# Usage:
#   use_precompiled_header(TARGET HEADER_FILE SRC_FILE)
#-------------------------------------------------------

macro(use_precompiled_header TARGET HEADER_FILE SRC_FILE)
  get_filename_component(HEADER ${HEADER_FILE} NAME)
  MESSAGE(STATUS "get_filename_component22222, " ${HEADER_FILE}) 
  # Use MSVC_IDE to exclude NMake from using PCHs
  if (MSVC AND NOT NMAKE AND NOT OGRE_UNITY_BUILD)
	add_definitions(/Yu"${HEADER}")
	MESSAGE(STATUS "add_definitions, " ${HEADER}) 
	MESSAGE(STATUS "SRC_FILE, " ${SRC_FILE}) 
    set_source_files_properties(${SRC_FILE}
      PPROPERTIES COMPILE_FLAGS /Yc"${HEADER}"
	)
    
  elseif (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
  MESSAGE(STATUS "add_definitions, " ${HEADER}) 
  endif ()
endmacro(use_precompiled_header)
