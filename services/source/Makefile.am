INCLUDES=-I../include -I../../kineircd/include -Imodules/include \
	 $(LTDLINCL) \
         @PGSQL_INCDIR@ \
         @MYSQL_INCDIR@
lib_LTLIBRARIES = mod_exordium.la libexordium.la
mod_exordium_la_SOURCES = module.cpp
mod_exordium_la_LDFLAGS = -module -avoid-version \
                          @PGSQL_LDFLAGS@ \
                          @MYSQL_LDFLAGS@
mod_exordium_la_LIBADD = libexordium.la
libexordium_la_LIBADD = ../../kineircd/libkineircd.la $(LIBLTDL)

# work out what else needs compiling..
if EXORDIUM_COMPILE_DB_MYSQL
  EXORDIUM_TARGET_DB_MYSQL=lib/database/mysql/dbmysql.cpp \
  			   lib/database/mysql/dbmysqlres.cpp
endif
if EXORDIUM_COMPILE_DB_PGSQL
  EXORDIUM_TARGET_DB_PGSQL=lib/database/postgresql/dbpgsql.cpp
endif

libexordium_la_SOURCES = \
			 lib/core_language.cpp \
			 lib/core_language.h \
		       	 lib/config.cpp \
			 lib/console.cpp \
		       	 lib/channel.cpp \
			 lib/dchan.cpp \
			 lib/modules.cpp \
			 lib/parser.cpp \
			 lib/service.cpp \
			 lib/services.cpp \
			 lib/user.cpp \
			 lib/utils.cpp \
			 lib/database/database.cpp \
			 $(EXORDIUM_TARGET_DB_MYSQL) \
			 $(EXORDIUM_TARGET_DB_PGSQL)


lib/core_language.h: lib/core_language.tpl $(top_builddir)/autogen/language.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T lib/core_language.tpl \
		$(top_builddir)/autogen/language.def

lib/core_language.cpp: lib/core_language.tpl $(top_builddir)/autogen/language.def
	$(AUTOGEN) \
		 -b $(basename $@) \
		 -T lib/core_language.tpl \
		 $(top_builddir)/autogen/language.def
		 

			   
	
SUBDIRS=. libbank modules

