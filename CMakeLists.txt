cmake_policy (SET CMP0048 NEW)

project (filefind VERSION 1.2.1 LANGUAGES CXX)
cmake_minimum_required (VERSION 3.7)
set (CMAKE_CXX_STANDARD_REQUIRED 11)
set (CMAKE_CXX_STANDARD 11)

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
    regex.C
    search.C
)

if (WIN32)
	add_definitions (-D_WIN32)
    list (APPEND HDRS search_win32.H)
	list (APPEND SRCS search_win32.C)
elseif (UNIX)
	add_definitions (-D_UNIX)
    list (APPEND HDRS search_unix.H)
	list (APPEND SRCS search_unix.C)
endif ()
add_definitions (-DPACKAGE_STRING="${PROJECT_NAME} ${filefind_VERSION}")

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
target_link_libraries (${PROJECT_NAME} ${LIBS})
install (TARGETS ${PROJECT_NAME} DESTINATION bin)
