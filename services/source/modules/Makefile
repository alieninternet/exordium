# $Id$
DEFINES=-DHAVE_CONFIG_H
RM=rm -f
CXX=g++
#CXXFLAGS=-g -Wall -Winline -Werror -pg -O0 -fmemoize-lookups
CXXFLAGS=-g -Wall -Werror -O0 -fpic -ggdb
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

TARGET=modules

all: $(TARGET)

$(TARGET):
	$(CXX) $(CXXFLAGS) -I../../include -I../../../kineircd/src/lib -I../../../kineircd/include -I/usr/local/mysql/include -shared -o love.so love.cpp
	$(CXX) $(CXXFLAGS) -I../../include -I../../../kineircd/src/lib -I../../../kineircd/include -I/usr/local/mysql/include -shared -o nick.so nick.cpp
	$(CXX) $(CXXFLAGS) -I../../include -I../../../kineircd/src/lib -I../../../kineircd/include -I/usr/local/mysql/include -shared -o chan.so chan.cpp
	$(CXX) $(CXXFLAGS) -I../../include -I../../../kineircd/src/lib -I../../../kineircd/include -I/usr/local/mysql/include -shared -o serv.so serv.cpp
	$(CXX) $(CXXFLAGS) -I../../include -I../../../kineircd/src/lib -I../../../kineircd/include -I/usr/local/mysql/include -shared -o note.so note.cpp
	$(CXX) $(CXXFLAGS) -I../../include -I../../../kineircd/src/lib -I../../../kineircd/include -I/usr/local/mysql/include -shared -o game.so game.cpp
	$(CXX) $(CXXFLAGS) -I../../include -I../../../kineircd/src/lib -I../../../kineircd/include -I/usr/local/mysql/include -shared -o bot.so bot.cpp

clean:
	$(RM) *.so modules

distclean:
	$(RM) *.so modules

