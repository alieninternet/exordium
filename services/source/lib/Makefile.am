lib_LTLIBRARIES = libservices.la
INCLUDES= -I../../include -I../../../kineircd/include
libservices_la_LIBADD = -lkineircd -lmysqlclient -ldl \
		        -L../../../kineircd/src/lib/
libservices_la_SOURCES = channel.cpp \
		         conf.cpp \
			 ircdome.cpp \
			 log.cpp \
			 modules.cpp \
			 parser.cpp \
			 services.cpp \
			 sql.cpp \
			 user.cpp 
			
