cmake_minimum_required (VERSION 3.7...3.27)

project (filefind VERSION 2.0.0 LANGUAGES CXX)

set (CMAKE_CXX_STANDARD_REQUIRED 17)
set (CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Find the re2 library
find_package(re2 CONFIG REQUIRED)

# Find the libfmt library
find_package(fmt CONFIG REQUIRED)

set (HDRS
    args.H
    cmdline.H
    config.H
    error.H
    filter.H
    regex.H
	search.H
    utils.H
)
set (SRCS
    args.C
    cmdline.C
    config.C
    filter.C
    main.C
    regex.C
    search.C
    utils.C
)

if (WIN32)
    list (APPEND HDRS search_win32.H)
	list (APPEND SRCS search_win32.C)
elseif (UNIX)
    list (APPEND HDRS search_unix.H)
	list (APPEND SRCS search_unix.C)
endif ()

set (LIBS "")
if (WIN32)
	list (APPEND LIBS "Shlwapi")
elseif (AIX)
    list (APPEND LIBS pthread)
endif ()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable (${PROJECT_NAME} ${HDRS} ${SRCS})
if (AIX)
    target_compile_options (${PROJECT_NAME} PRIVATE -maix64 -pthread)
    set_target_properties (${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-brtl -Wl,-bsvr4 -Wl,-R/QOpenSys/pkgs/lib/gcc/powerpc-ibm-aix6.1.0.0/6.3.0/pthread/ppc64 -Wl,-R/QOpenSys/pkgs/lib/gcc/powerpc-ibm-aix6.1.0.0/6.3.0 -Wl,-R/QOpenSys/pkgs/lib/gcc -Wl,-R/QOpenSys/pkgs/lib")
endif ()

target_compile_definitions(${PROJECT_NAME} PUBLIC "PACKAGE_STRING=\"${PROJECT_NAME} ${filefind_VERSION}\"")
if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE "FF_WIN32")
elseif (UNIX)
    target_compile_definitions(${PROJECT_NAME} PRIVATE "FF_UNIX")
    if (APPLE)
        target_compile_definitions(${PROJECT_NAME} PRIVATE "FF_APPLE")
    elseif (AIX)
        target_compile_definitions(${PROJECT_NAME} PRIVATE "FF_AIX")
    else ()
        target_compile_definitions(${PROJECT_NAME} PRIVATE "FF_LINUX")
    endif ()
endif ()

target_link_libraries (${PROJECT_NAME} PRIVATE re2::re2 fmt::fmt ${LIBS})
install (TARGETS ${PROJECT_NAME} DESTINATION bin)
