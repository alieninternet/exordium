
INCLUDES= -I../../include -I../../../kineircd/include -Iinclude/
lib_LTLIBRARIES = mod_bot.la \
		  mod_chan.la \
		  mod_nick.la \
		  mod_note.la \
		  mod_oper.la \
		  mod_serv.la \
		  mod_stats.la \
		  mod_vote.la
CPPFLAGS=
LDFLAGS=
mod_bot_la_SOURCES = mod.bot/bot.cpp
mod_bot_la_LIBADD = ../libexordium.la
mod_bot_la_LDFLAGS = -avoid-version -module
mod_chan_la_SOURCES = mod.chan/chan.cpp
mod_chan_la_LIBADD = ../libexordium.la
mod_chan_la_LDFLAGS = -avoid-version -module
mod_nick_la_SOURCES = mod.nick/nick.cpp
mod_nick_la_LIBADD = ../libexordium.la
mod_nick_la_LDFLAGS = -avoid-version -module
mod_note_la_SOURCES = mod.note/note.cpp
mod_note_la_LIBADD = ../libexordium.la
mod_note_la_LDFLAGS = -avoid-version -module
mod_oper_la_SOURCES = mod.oper/oper.cpp
mod_oper_la_LIBADD = ../libexordium.la
mod_oper_la_LDFLAGS = -avoid-version -module
mod_serv_la_SOURCES = mod.serv/serv.cpp
mod_serv_la_LIBADD = ../libexordium.la
mod_serv_la_LDFLAGS = -avoid-version -module
mod_stats_la_SOURCES = mod.stats/stats.cpp
mod_stats_la_LIBADD = ../libexordium.la
mod_stats_la_LDFLAGS = -avoid-version -module
mod_vote_la_SOURCES = mod.vote/vote.cpp
mod_vote_la_LIBADD = ../libexordium.la
mod_vote_la_LDFLAGS = -avoid-version -module
SUBDIRS=mod.game mod.love mod.credits
