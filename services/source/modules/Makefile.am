INCLUDES= -I../../include -I../../../kineircd/include -Iinclude/
lib_LTLIBRARIES = mod_bot.la \
		  mod_chan.la \
		  mod_nick.la \
		  mod_note.la \
		  mod_oper.la \
		  mod_serv.la
CPPFLAGS=
LDFLAGS=
mod_bot_la_SOURCES = bot.cpp
mod_bot_la_LIBADD = ../libservices.la
mod_bot_la_LDFLAGS = -avoid-version -module
mod_chan_la_SOURCES = chan.cpp
mod_chan_la_LIBADD = ../libservices.la
mod_chan_la_LDFLAGS = -avoid-version -module
mod_nick_la_SOURCES = nick.cpp
mod_nick_la_LIBADD = ../libservices.la
mod_nick_la_LDFLAGS = -avoid-version -module
mod_note_la_SOURCES = note.cpp
mod_note_la_LIBADD = ../libservices.la
mod_note_la_LDFLAGS = -avoid-version -module
mod_serv_la_SOURCES = serv.cpp
mod_serv_la_LIBADD = ../libservices.la
mod_serv_la_LDFLAGS = -avoid-version -module
mod_oper_la_SOURCES = oper.cpp
mod_oper_la_LIBADD = ../libservices.la
mod_oper_la_LDFLAGS = -avoid-version -module
SUBDIRS=game love
