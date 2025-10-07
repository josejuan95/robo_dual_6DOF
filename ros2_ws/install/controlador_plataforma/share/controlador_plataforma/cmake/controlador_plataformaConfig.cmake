# generated from ament/cmake/core/templates/nameConfig.cmake.in

# prevent multiple inclusion
if(_controlador_plataforma_CONFIG_INCLUDED)
  # ensure to keep the found flag the same
  if(NOT DEFINED controlador_plataforma_FOUND)
    # explicitly set it to FALSE, otherwise CMake will set it to TRUE
    set(controlador_plataforma_FOUND FALSE)
  elseif(NOT controlador_plataforma_FOUND)
    # use separate condition to avoid uninitialized variable warning
    set(controlador_plataforma_FOUND FALSE)
  endif()
  return()
endif()
set(_controlador_plataforma_CONFIG_INCLUDED TRUE)

# output package information
if(NOT controlador_plataforma_FIND_QUIETLY)
  message(STATUS "Found controlador_plataforma: 0.0.0 (${controlador_plataforma_DIR})")
endif()

# warn when using a deprecated package
if(NOT "" STREQUAL "")
  set(_msg "Package 'controlador_plataforma' is deprecated")
  # append custom deprecation text if available
  if(NOT "" STREQUAL "TRUE")
    set(_msg "${_msg} ()")
  endif()
  # optionally quiet the deprecation message
  if(NOT controlador_plataforma_DEPRECATED_QUIET)
    message(DEPRECATION "${_msg}")
  endif()
endif()

# flag package as ament-based to distinguish it after being find_package()-ed
set(controlador_plataforma_FOUND_AMENT_PACKAGE TRUE)

# include all config extra files
set(_extras "")
foreach(_extra ${_extras})
  include("${controlador_plataforma_DIR}/${_extra}")
endforeach()
