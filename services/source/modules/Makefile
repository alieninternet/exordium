# $Id$
DEFINES=-DHAVE_CONFIG_H
RM=rm -f
CXX=g++
#CXXFLAGS=-g -Wall -Winline -Werror -pg -O0 -fmemoize-lookups
CXXFLAGS=-g -Wall -Werror -O0 -fpic -ggdb
INCDIRS=-I../../include -I../../../kineircd/src/lib -I../../../kineircd/include -I/usr/local/mysql/include
LDFLAGS=
LIBS=-lmysqlclient

INCLUDES=/../include

SOURCES=chan.cpp \
	nick.cpp \
	serv.cpp \
	love.cpp \
	note.cpp \
	game.cpp \
	bot.cpp
	
OBJECTS=$(SOURCES:.cpp=.so)

all: $(OBJECTS)

%.so: %.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(DEFS) $(INCDIRS) -shared $< -o $@

clean:
	$(RM) *.so modules

distclean:
	$(RM) *.so modules

