project (filefind CXX)
cmake_minimum_required (VERSION 3.7)
set (CMAKE_CXX_STANDARD_REQUIRED 11)
set (CMAKE_CXX_STANDARD 11)

set (HDRS
    args.H
    cmdline.H
    config.H
    error.H
    filter.H
    regex.H
)
set (SRCS
    args.C
    cmdline.C
    config.C
    filter.C
    main.C
    regex.C
)

add_executable (${PROJECT_NAME} ${HDRS} ${SRCS})
if (AIX)
    target_compile_options (${PROJECT_NAME} PRIVATE -maix64 -pthread)
    set_target_properties (${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-brtl -Wl,-bsvr4 -Wl,-R/QOpenSys/pkgs/lib/gcc/powerpc-ibm-aix6.1.0.0/6.3.0/pthread/ppc64 -Wl,-R/QOpenSys/pkgs/lib/gcc/powerpc-ibm-aix6.1.0.0/6.3.0 -Wl,-R/QOpenSys/pkgs/lib/gcc -Wl,-R/QOpenSys/pkgs/lib")
endif ()
install (TARGETS ${PROJECT_NAME} DESTINATION bin)
