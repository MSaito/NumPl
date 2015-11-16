#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([NumberPlaceGenerator], [1.1.1], [saito@manieth.com])
AC_CONFIG_SRCDIR([src/analyze.c])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_MACRO_DIRS([m4])
# doxygen before AM_INIT_AUTOMAKE
DX_DOXYGEN_FEATURE(ON)
DX_DOT_FEATURE(OFF)
DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN(NumberPlaceGenerator, doxygen.cfg)
# after doxygen
AM_INIT_AUTOMAKE
#AM_EXTRA_RECURSIVE_TARGETS([foo])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([inttypes.h limits.h stdint.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T


# Checks for library functions.
AC_CHECK_FUNCS([memset strtoul strtoull])
# Checks for builtin functions.
AX_GCC_BUILTIN(__builtin_popcount)

AC_CONFIG_FILES([Makefile src/Makefile src/6x6/Makefile])
AC_OUTPUT
