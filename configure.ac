#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT(LCUIFightGame, 1.1.0, lc-soft@live.cn)

# Check system type
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE

AM_SILENT_RULES([yes])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

# Checks for header files.
AC_CHECK_HEADERS([stdint.h stdio.h malloc.h memory.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

# Checks for libraries.
AC_CHECK_LIB([LCUI], [LCUI_Init],[],[
	echo "Error! the LCUI library has not been installed!"
	exit -1
] )


##输出文件
AC_OUTPUT([ Makefile src/Makefile ])

