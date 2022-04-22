# cmake/Findre2.cmake

#[=======================================================================[.rst:
Findre2
----------

Find the RE2, a regular expression library.

IMPORTED Targets
^^^^^^^^^^^^^^^^

The following :prop_tgt:`IMPORTED` targets may be defined

``re2::re2``
  If the RE2 library has been found

Result variables
^^^^^^^^^^^^^^^^

This module will set the following variables in your project:

``re2_FOUND``
  true if RE2 headers and libraries were found

Cache variables
^^^^^^^^^^^^^^^

Control variables
^^^^^^^^^^^^^^^^^

#]=======================================================================]

# Try the original cmake package first
find_package(re2 QUIET CONFIG)
if(re2_FOUND)
    message(STATUS "Found RE2 via cmake")
    return()
endif()

# Then try pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(_pc_re2 QUIET IMPORTED_TARGET re2)
if(_pc_re2_FOUND)
    set(re2_FOUND "${_pc_re2_FOUND}")
    add_library(re2::re2 INTERFACE IMPORTED)
    if(_pc_re2_INCLUDE_DIRS)
        set_property(TARGET re2::re2 PROPERTY
            INTERFACE_INCLUDE_DIRECTORIES "${_pc_re2_INCLUDE_DIRS}")
    endif()
    if(_pc_re2_CFLAGS_OTHER)
        list(FILTER _pc_re2_CFLAGS_OTHER EXCLUDE REGEX "^-std=")
        set_property(TARGET re2::re2 PROPERTY
            INTERFACE_COMPILE_OPTIONS "${_pc_re2_CFLAGS_OTHER}")
    endif()
    if(_pc_re2_LDFLAGS)
        set_property(TARGET re2::re2 PROPERTY
            INTERFACE_LINK_LIBRARIES "${_pc_re2_LDFLAGS}")
    endif()
    message(STATUS "Found RE2 via pkg-config")
    return()
endif()

# Fall-back to system directories
find_path(re2_INCLUDE_DIR NAMES re2/re2.h)
find_library(re2_LIBRARY NAMES re2)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(re2
    REQUIRED_VARS re2_LIBRARY re2_INCLUDE_DIR)

if (re2_FOUND)
    add_library(re2::re2 INTERFACE IMPORTED)
    set_property(TARGET re2::re2 PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES "${re2_INCLUDE_DIR}")
    set_property(TARGET re2::re2 PROPERTY
        INTERFACE_LINK_LIBRARIES "${re2_LIBRARY}")
endif()
