AC_INIT([filefind], [1.2.2])

AC_CONFIG_SRCDIR([search_unix.C])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIRS([m4])

AM_INIT_AUTOMAKE([-Wall foreign])

AC_PROG_CXX
AC_LANG(C++)
AX_CHECK_COMPILE_FLAG([-std=c++11], [
	CXXFLAGS="$CXXFLAGS -std=c++11"
], [
	echo "C++ compiler cannot compile C++11 code"
	exit -1
])

AC_CONFIG_FILES([Makefile])

AC_CONFIG_HEADERS(conf.h)

AC_OUTPUT

