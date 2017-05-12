AC_INIT([liblog], [1.0], [dfgs83@gmail.com])

AC_CHECK_LIB(stringutil,nowToString, ,[AC_MSG_ERROR([Couldn't find libstringutil...] )])
AC_CHECK_HEADER(stringutil.h,, [AC_MSG_ERROR([Couldn't find stringutil.h...] )])


AM_INIT_AUTOMAKE([foreign])
#AM_PROG_AR
AC_PROG_CC
AC_PROG_RANLIB
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

