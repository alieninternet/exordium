# This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                             
# $Author$        
# $Date$                      
# $Id$                                                                        

AC_INIT([Exordium], [0.1-alpha], [coding@ircdome.org])
AM_INIT_AUTOMAKE([1.6.1 foreign no-installinfo no-installman])
AC_PROG_LIBTOOL
AC_PREREQ(2.54)
AC_COPYRIGHT(Exordium Network Services are (c) 2002 IRCDome Development Team)
AC_COPYRIGHT(Please read the LICENSE file for more information)
AC_CONFIG_SRCDIR([include/exordium/config.h.in])
AM_CONFIG_HEADER([include/exordium/config.h])
AC_MSG_CHECKING([what to set CXXFLAGS to])
if test "$CXXFLAGS" = ""; then
	CXXFLAGS="-O0 -g"
	AC_MSG_RESULT(['$CXXFLAGS' (default)])
else
	AC_MSG_RESULT(['$CXXFLAGS' (given)])
fi

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_CHECK_PROG(RM, rm, rm -f, del)
AC_CHECK_PROG(LD, ld, ld)

# Checks for libraries+functions
AC_CHECK_LIB(resolv, res_init)
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_FUNC_FNMATCH
AC_FUNC_VPRINTF
dnl AC_FUNC_GETLOADAVG
AC_FUNC_STRFTIME
AC_CHECK_FUNCS([gethostbyname gettimeofday memset select socket strchr])
AC_CHECK_LIB([dl], [dlopen], [], [AC_MSG_ERROR([I cannot find a working dlopen! :(])])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h netdb.h netinet/in.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

AC_MSG_CHECKING([Whether to enable Debug mode])
AC_ARG_ENABLE(debug-mode,[
	--enable-debug-mode		(Default is disabled)
	This enables (very) verbose output from services, primarily for
	tracking down that annoying little bug :-).
	],[
	if test "$enableval" = "no"; then
     		AC_MSG_RESULT(no)
   	else
     		AC_MSG_RESULT(yes)
     		AC_DEFINE(DEBUG)
   fi
 ],[
   AC_MSG_RESULT([no (default)])
])
AC_MSG_CHECKING([for what language to use as default])
AC_ARG_ENABLE(language,
[
	--enable-language=language 	Default language for services
					Default: English
					Look in doc/languages for more.],
[
	if test "$enableval"; then
		LANGUAGE=$enableval
		AC_MSG_RESULT([$enableval])
	fi
],[LANGUAGE="English"])
AC_MSG_RESULT([English (default)])
AC_SUBST(LANGUAGE)

AC_ARG_ENABLE(mysql_dir,
[
	--enable-mysql-path=<path> 	Path where mysql_config resides.],
[
        if test "$enableval"; then
                mysql_config_prog="$enableval/mysql_config"
                AC_MSG_RESULT([$enableval])
        fi
],[mysql_config_prog="mysql_config"])

AC_CHECK_PROG(mysql_config, $mysql_config_prog, [yes])
if test "$mysql_config" ; then
        CPPFLAGS="$CPPFLAGS `$mysql_config_prog --cflags`"
        LDFLAGS="$LDFLAGS `$mysql_config_prog --libs`"
        AC_CHECK_LIB([mysqlclient], [main], [
        AC_DEFINE(HAVE_MYSQL)
	exordium_test_compile_db_mysql=yes
        AC_MSG_RESULT(Building with MySQL support)], [AC_MSG_RESULT([Building without MySQL support])])
fi
AM_CONDITIONAL(EXORDIUM_COMPILE_DB_MYSQL,
               [test x$exordium_test_compile_db_mysql = xyes])

AC_CHECK_LIB([pq], [main], [

CPPFLAGS="$CPPFLAGS -I`pg_config --includedir`"

AC_CHECK_HEADERS([`pg_config --includedir`/libpq-fe.h],
  [
#    CPPFLAGS="$CPPFLAGS -DHAVE_LIBPQ_FE_H"
	AC_DEFINE(HAVE_LIBPQ_FE_H)
  ],
  [
    AC_CHECK_HEADERS([`pg_config --includedir`/libpq-int.h], 
    [
      #CPPFLAGS="$CPPFLAGS -DHAVE_LIBPQ_INT_H"
      AC_DEFINE(HAVE_LIBPQ_INT_H)
    ],
    [
      AC_MSG_ERROR([libpq headers not found!])
    ])
  ])

#CPPFLAGS="$CPPFLAGS -DHAVE_PGSQL"
AC_DEFINE(HAVE_PGSQL)
exordium_test_compile_db_pgsql=yes
LDFLAGS="$LDFLAGS -L `pg_config --libdir`"
LDFLAGS="$LDFLAGS  -lpq"
AC_MSG_RESULT(Building with PostgreSQL support)], [AC_MSG_RESULT([Building without PosgreSQL support])
])
AM_CONDITIONAL(EXORDIUM_COMPILE_DB_PGSQL,
               [test x$exordium_test_compile_db_pgsql = xyes])

AC_CONFIG_FILES([source/Makefile
		 source/modules/Makefile
		 source/modules/mod.game/Makefile
		 source/modules/mod.game/cards/Makefile
		 source/modules/mod.love/Makefile
		 Makefile
		 ])
AC_OUTPUT
