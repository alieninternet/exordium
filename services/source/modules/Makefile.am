
# work out which modules to compile
if EXORDIUM_COMPILE_BOT_MODULE
  EXORDIUM_TARGET_BOT_MODULE=mod_bot.la
endif
if EXORDIUM_COMPILE_CHAN_MODULE
  EXORDIUM_TARGET_CHAN_MODULE=mod_chan.la
endif
if EXORDIUM_COMPILE_CREDITS_MODULE
  EXORDIUM_TARGET_CREDITS_MODULE=mod.credits
endif
if EXORDIUM_COMPILE_GAME_MODULE
  EXORDIUM_TARGET_GAME_MODULE=mod.game
endif
if EXORDIUM_COMPILE_LOVE_MODULE
  EXORDIUM_TARGET_LOVE_MODULE=mod.love
endif
if EXORDIUM_COMPILE_NICK_MODULE
  EXORDIUM_TARGET_NICK_MODULE=mod_nick.la
endif
if EXORDIUM_COMPILE_NOTE_MODULE
  EXORDIUM_TARGET_NOTE_MODULE=mod_note.la
endif
if EXORDIUM_COMPILE_OPER_MODULE
  EXORDIUM_TARGET_OPER_MODULE=mod_oper.la
endif
if EXORDIUM_COMPILE_SERV_MODULE
  EXORDIUM_TARGET_SERV_MODULE=mod_serv.la
endif
if EXORDIUM_COMPILE_STATS_MODULE
  EXORDIUM_TARGET_STATS_MODULE=mod_stats.la
endif
if EXORDIUM_COMPILE_VOTE_MODULE
  EXORDIUM_TARGET_VOTE_MODULE=mod_vote.la
endif
  
INCLUDES= -I../../include -I../../../kineircd/include -Iinclude/
lib_LTLIBRARIES=\
	$(EXORDIUM_TARGET_BOT_MODULE) \
	$(EXORDIUM_TARGET_CHAN_MODULE) \
	$(EXORDIUM_TARGET_NICK_MODULE) \
	$(EXORDIUM_TARGET_NOTE_MODULE) \
	$(EXORDIUM_TARGET_OPER_MODULE) \
	$(EXORDIUM_TARGET_SERV_MODULE) \
	$(EXORDIUM_TARGET_STATS_MODULE) \
	$(EXORDIUM_TARGET_VOTE_MODULE)

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

mod_note_la_LIBADD=\
	../libexordium.la
mod_note_la_LDFLAGS=\
	-avoid-version -module
mod_note_la_SOURCES=\
	mod.note/note.cpp \
	mod.note/tables.cpp

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

SUBDIRS=\
	$(EXORDIUM_TARGET_CREDITS_MODULE) \
	$(EXORDIUM_TARGET_GAME_MODULE) \
	$(EXORDIUM_TARGET_LOVE_MODULE)
