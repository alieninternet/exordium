INCLUDES= -I../include -I../../kineircd/include -Imodules/include
lib_LTLIBRARIES = lib_mod_exordium.la libservices.la
lib_mod_exordium_la_SOURCES = module.cpp
lib_mod_exordium_la_LDFLAGS = -module 
lib_mod_exordium_la_LIBADD = libservices.la
libservices_la_LIBADD = -lmysqlclient \
		        ../../kineircd/src/lib/libkineircd.la
libservices_la_SOURCES = lib/channel.cpp \
		         lib/conf.cpp \
			 lib/ircdome.cpp \
			 lib/log.cpp \
			 lib/modules.cpp \
			 lib/parser.cpp \
			 lib/service.cpp \
			 lib/services.cpp \
			 lib/sql.cpp \
			 lib/user.cpp
SUBDIRS=. modules

