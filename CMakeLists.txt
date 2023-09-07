cmake_minimum_required (VERSION 3.7...3.27)

project (filefind VERSION 1.3.1 LANGUAGES CXX)

set (CMAKE_CXX_STANDARD_REQUIRED 11)
set (CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Find the optional re2 library
find_package(re2 QUIET)

set (HDRS
    utils.H
    args.H
    cmdline.H
    config.H
    error.H
    filter.H
    regex.H
	search.H
)
set (SRCS
    utils.C
    args.C
    cmdline.C
    config.C
    filter.C
    main.C
    search.C
)

if (re2_FOUND)
    list (APPEND HDRS re2_regex.H)
    list (APPEND SRCS re2_regex.C)
else ()
    list (APPEND HDRS std_regex.H)
    list (APPEND SRCS std_regex.C)
endif()

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

add_executable (${PROJECT_NAME} ${HDRS} ${SRCS})
if (AIX)
    target_compile_options (${PROJECT_NAME} PRIVATE -maix64 -pthread)
    set_target_properties (${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-brtl -Wl,-bsvr4 -Wl,-R/QOpenSys/pkgs/lib/gcc/powerpc-ibm-aix6.1.0.0/6.3.0/pthread/ppc64 -Wl,-R/QOpenSys/pkgs/lib/gcc/powerpc-ibm-aix6.1.0.0/6.3.0 -Wl,-R/QOpenSys/pkgs/lib/gcc -Wl,-R/QOpenSys/pkgs/lib")
endif ()

target_compile_definitions(${PROJECT_NAME} PUBLIC "PACKAGE_STRING=\"${PROJECT_NAME} ${filefind_VERSION}\"")
if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC "_WIN32")
elseif (UNIX)
    target_compile_definitions(${PROJECT_NAME} PUBLIC "_UNIX")
endif()
if (re2_FOUND)
    target_compile_definitions(${PROJECT_NAME} PUBLIC "RE2_FOUND")
    list (APPEND LIBS re2::re2)
endif()

target_link_libraries (${PROJECT_NAME} PUBLIC ${LIBS})
install (TARGETS ${PROJECT_NAME} DESTINATION bin)
