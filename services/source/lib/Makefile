DEFINES=
RM=rm -f
CXX=g++
#CXXFLAGS=-g -Wall -Winline -Werror -pg -O0 -fmemoize-lookups
CXXFLAGS=-O0 -g -nostdlib
LDFLAGS=
LIBS=-lmysqlclient -ldl 

SOURCES=sql.cpp \
	nickname.cpp \
	channel.cpp \
	ircdome.cpp \
	log.cpp \
	parser.cpp \
	services.cpp 
INCLUDES=../../include

OBJECTS=$(SOURCES:.cpp=.o)

TARGET=libservices.so

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) -shared -ldl -W1,-soname=libservices.so -o libservices.so $(OBJECTS) ../../../kineircd/src/lib/.libs/libkineircd.so

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDES) -I/usr/local/mysql/include/ -I../../../kineircd/include -I../../../kineircd/src/lib -I../include/  -c $< 


clean:
	$(RM) *.o *~ libservices.so

distclean:
	$(RM) *.o *~ libservices.so

pretty:
	indent *.cpp
	indent *.h
	$(RM) *~

modules:
	$(CXX) -I../include -I../../kineircd/include -I/usr/local/mysql/include -shared -o love.so love.cpp


