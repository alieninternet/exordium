INCLUDES=-I../include -I../../kineircd/include -Imodules/include \
         @PGSQL_INCDIR@ \
         @MYSQL_INCDIR@
lib_LTLIBRARIES = mod_exordium.la libexordium.la
mod_exordium_la_SOURCES = module.cpp
mod_exordium_la_LDFLAGS = -module -avoid-version \
                          @PGSQL_LDFLAGS@ \
                          @MYSQL_LDFLAGS@
mod_exordium_la_LIBADD = libexordium.la
libexordium_la_LIBADD = ../../kineircd/src/libkineircd.la

# work out what else needs compiling..
if EXORDIUM_COMPILE_DB_MYSQL
  EXORDIUM_TARGET_DB_MYSQL=lib/database/mysql/dbmysql.cpp
endif
if EXORDIUM_COMPILE_DB_PGSQL
  EXORDIUM_TARGET_DB_PGSQL=lib/database/postgresql/dbpgsql.cpp
endif

libexordium_la_SOURCES = lib/config.cpp \
			 lib/console.cpp \
		       	 lib/channel.cpp \
			 lib/dchan.cpp \
			 lib/modules.cpp \
			 lib/parser.cpp \
			 lib/service.cpp \
			 lib/services.cpp \
			 lib/user.cpp \
			 lib/database/database.cpp \
			 $(EXORDIUM_TARGET_DB_MYSQL) \
			 $(EXORDIUM_TARGET_DB_PGSQL)

			 
SUBDIRS=. libbank modules

