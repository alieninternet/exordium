# This file is a part of Exordium Network Services - (c) 2002 IRCDome Development Team                             
# $Author$        
# $Date$                      
# $Id$                                                                        

AC_INIT([Exordium], [0.1-alpha], [coding@ircdome.org])
AM_INIT_AUTOMAKE([1.6.1 foreign no-installinfo no-installman subdir-objects])
AC_PROG_LIBTOOL
AC_PREREQ(2.54)
AC_COPYRIGHT(Exordium Network Services are (c) 2002 IRCDome Development Team)
AC_COPYRIGHT(Please read the LICENSE file for more information)
AC_CONFIG_SRCDIR([include/autoconf.h.in])
AM_CONFIG_HEADER([include/autoconf.h])
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
AC_CHECK_PROG(AUTOGEN, autogen, autogen, :)

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
AC_CHECK_HEADERS([\
	dlfcn.h \
	netdb.h \
	unistd.h \
	arpa/inet.h \
	netinet/in.h \
	sys/time.h \
	sys/types.h \
		])

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
     		AC_DEFINE(DEBUG, 1, [Define if you want debugging compiled in])
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

AC_ARG_ENABLE(mysql-path,
[
	--enable-mysql-path=<path> 	Path where mysql_config resides.],
[
        if test "$enableval"; then
#                mysql_config_prog="$enableval/mysql_config"
                mysql_config_path=$enableval
                AC_MSG_RESULT([$enableval])
        fi
],[])

AC_PATH_PROG([MYSQL_CONFIG], [mysql_config], [no], [$PATH:$mysql_config_path])
if test "$MYSQL_CONFIG" != "no" ; then
        MYSQL_INCDIR=`$MYSQL_CONFIG --cflags`
        MYSQL_LDFLAGS=`$MYSQL_CONFIG --libs`
	
        AC_CHECK_LIB([mysqlclient], [main], [
         AC_DEFINE(HAVE_MYSQL, 1, [Define if you want MySQL support compiled])
 	 exordium_test_compile_db_mysql=yes
         AC_SUBST(MYSQL_INCDIR)
         AC_SUBST(MYSQL_LDFLAGS)
         AC_MSG_RESULT(Building with MySQL support)], 
                       [AC_MSG_RESULT([Building without MySQL support])],
	[$MYSQL_LDFLAGS])
fi
AM_CONDITIONAL(EXORDIUM_COMPILE_DB_MYSQL,
               [test x$exordium_test_compile_db_mysql = xyes])

AC_CHECK_LIB([pq], [main], [

AC_CHECK_HEADERS([`pg_config --includedir`/libpq-fe.h],
  [
#    CPPFLAGS="$CPPFLAGS -DHAVE_LIBPQ_FE_H"
	AC_DEFINE(HAVE_LIBPQ_FE_H, 1, [Define if your PostgreSQL has libpq-fe.h])
  ],
  [
    AC_CHECK_HEADERS([`pg_config --includedir`/libpq-int.h], 
    [
      #CPPFLAGS="$CPPFLAGS -DHAVE_LIBPQ_INT_H"
      AC_DEFINE(HAVE_LIBPQ_INT_H, 1, [Define if your PostgreSQL has libpq-int.h])
    ],
    [
      AC_MSG_ERROR([libpq headers not found!])
    ])
  ])

AC_DEFINE(HAVE_PGSQL, 1, [Define if you want PostgreSQL support compiled in])
exordium_test_compile_db_pgsql=yes
PGSQL_INCDIR="-I`pg_config --includedir`"
PGSQL_LDFLAGS="-L`pg_config --libdir` -lpq"
AC_MSG_RESULT(Building with PostgreSQL support)], [AC_MSG_RESULT([Building without PosgreSQL support])
])
AM_CONDITIONAL(EXORDIUM_COMPILE_DB_PGSQL,
               [test x$exordium_test_compile_db_pgsql = xyes])
AC_SUBST(PGSQL_INCDIR)
AC_SUBST(PGSQL_LDFLAGS)


dnl Work out which modules we want..
AC_MSG_CHECKING([which modules to compile])

dnl A macro, based on the one I wrote for kine's configure script (- pickle)
dnl use: AC_EXORDIUM_MODULE_CHECK(name, [the name of the module])
AC_DEFUN([AC_EXORDIUM_MODULE_CHECK],[
  AC_ARG_ENABLE($1-module,
                AC_HELP_STRING([--disable-$1-module],
		               [Disable $2]),
   [
    if test "$enableval" \!= "no"; then
      echo -n "$1 "
      AS_TR_SH(exordium_test_compile_$1_module)=yes
    fi
   ],[
    echo -n "$1 "
    AS_TR_SH(exordium_test_compile_$1_module)=yes
   ])
  AM_CONDITIONAL(AS_TR_CPP(EXORDIUM_COMPILE_$1_MODULE),
                 [test x$AS_TR_SH(exordium_test_compile_$1_module) = xyes])
 ])
 
AC_EXORDIUM_MODULE_CHECK([bot], [the bot assistance module])
AC_EXORDIUM_MODULE_CHECK([chan], [the channel service module])
AC_EXORDIUM_MODULE_CHECK([credits], [the credits module])
AC_EXORDIUM_MODULE_CHECK([game], [the games module])
AC_EXORDIUM_MODULE_CHECK([love], [the love/cupid module])
AC_EXORDIUM_MODULE_CHECK([nick], [the nickname service module])
AC_EXORDIUM_MODULE_CHECK([note], [the note service module])
AC_EXORDIUM_MODULE_CHECK([oper], [the IRC operator module])
AC_EXORDIUM_MODULE_CHECK([serv], [the service service module :)])
AC_EXORDIUM_MODULE_CHECK([stats], [the statistics module])
AC_EXORDIUM_MODULE_CHECK([vote], [the voting module])

dnl finish off the line
echo


AC_CONFIG_FILES([source/Makefile
		 source/libbank/Makefile
		 source/modules/Makefile
		 source/modules/mod.credits/Makefile
		 source/modules/mod.game/Makefile
		 source/modules/mod.game/cards/Makefile
		 Makefile
		 ])
AC_OUTPUT
