INCLUDES=-Iinclude/ -I../kineircd/include -Isource/ -Isource/modules \
	$(LTDLINCL) \
	@PGSQL_INCDIR@ \
	@MYSQL_INCDIR@


# Work out what modules..
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
  EXORDIUM_TARGET_LOVE_MODULE=mod_love.la
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
if EXORDIUM_COMPILE_HELP_MODULE
  EXORDIUM_TARGET_HELP_MODULE=mod_help.la
endif


# Libraries we need to build
lib_LTLIBRARIES=\
	libexordium.la \
	libexordium_bank.la \
        $(EXORDIUM_TARGET_BOT_MODULE) \
        $(EXORDIUM_TARGET_CHAN_MODULE) \
        $(EXORDIUM_TARGET_LOVE_MODULE) \
        $(EXORDIUM_TARGET_NICK_MODULE) \
        $(EXORDIUM_TARGET_NOTE_MODULE) \
        $(EXORDIUM_TARGET_OPER_MODULE) \
        $(EXORDIUM_TARGET_SERV_MODULE) \
        $(EXORDIUM_TARGET_STATS_MODULE) \
        $(EXORDIUM_TARGET_VOTE_MODULE) \
        $(EXORDIUM_TARGET_HELP_MODULE) \
	mod_credits.la \
	mod_game.la \
	lib_cards.la \
	game_hangman.la \
	game_exordi8.la \
	game_poker.la \
	mod_exordium.la 



# ******************* libexordium ***********************
libexordium_la_INCLUDES = -Isource/
# work out what else needs compiling..
if EXORDIUM_COMPILE_DB_MYSQL
  EXORDIUM_TARGET_DB_MYSQL=source/lib/database/mysql/dbmysql.cpp \
                           source/lib/database/mysql/dbmysqlres.cpp
endif
if EXORDIUM_COMPILE_DB_PGSQL
  EXORDIUM_TARGET_DB_PGSQL=source/lib/database/postgresql/dbpgsql.cpp
endif

libexordium_la_SOURCES = \
		        include/exordium/channel.h \
			include/exordium/config.h \
			include/exordium/database/base.h \
			include/exordium/database/database.h \
			include/exordium/database/result.h \
			include/exordium/database/table.h \
			include/exordium/dchan.h \
			include/exordium/log.h \
			include/exordium/server.h \
			include/exordium/service.h \
			include/exordium/services.h \
			include/exordium/singleton.h \
			include/exordium/static.h \
			include/exordium/user.h \
			include/exordium/utils.h \
			source/lib/core_language.cpp \
			source/lib/core_language.h \
			source/lib/config.cpp \
			source/lib/console.cpp \
			source/lib/channel.cpp \
			source/lib/dchan.cpp \
			source/lib/logger.cpp \
			source/lib/logger.h \
			source/lib/modules.cpp \
			source/lib/parser.cpp \
			source/lib/server.cpp \
			source/lib/service.cpp \
			source/lib/services.cpp \
			source/lib/static.cpp \
			source/lib/user.cpp \
			source/lib/utils.cpp \
			source/lib/database/database.cpp \
			$(EXORDIUM_TARGET_DB_MYSQL) \
			$(EXORDIUM_TARGET_DB_PGSQL)

source/lib/core_language.cpp source/lib/core_language.h: source/lib/core_language.tpl $(top_builddir)/autogen/language.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T source/lib/core_language.tpl \
		$(top_builddir)/autogen/language.def

libexordium_la_LIBADD = ../kineircd/libkineircd.la $(LIBLTDL)

# **************** END of libexordium *****************
# **************** START of libexordium_bank **********
libexordium_bank_la_SOURCES=\
        source/libbank/bank.cpp \
        source/libbank/bank.h \
        source/libbank/tables.cpp \
        source/libbank/tables.h

# **************** END of libexordium_bank ************
# **************** START of mod_exordium **************
mod_exordium_la_SOURCES = source/module.cpp
mod_exordium_la_LDFLAGS = -module -avoid-version \
	@PGSQL_LDFLAGS@ \
	@MYSQL_LDFLAGS@
mod_exordium_la_LIBADD = libexordium.la
# ***************** END of mod_exordium ***************
# ***************** START of mod_bot ******************
mod_bot_la_LIBADD=\
		   libexordium.la
mod_bot_la_LDFLAGS=\
		   -avoid-version -module
mod_bot_la_SOURCES=\
		   source/modules/mod.bot/bot.cpp \
		   source/modules/mod.bot/bot.h
# ***************** END of mod_bot ********************
# ***************** START of mod_chan *****************
mod_chan_la_LIBADD=\
		   libexordium.la
mod_chan_la_LDFLAGS=\
		   -avoid-version -module
mod_chan_la_SOURCES=\
		   source/modules/mod.chan/language.cpp \
		   source/modules/mod.chan/language.h \
		   source/modules/mod.chan/chan.cpp \
		   source/modules/mod.chan/chan.h \
		   source/modules/mod.chan/tables.cpp \
		   source/modules/mod.chan/tables.h

source/modules/mod.chan/language.cpp source/modules/mod.chan/language.h: source/modules/mod.chan/language.tpl $(top_builddir)/autogen/language.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T source/modules/mod.chan/language.tpl \
		$(top_builddir)/autogen/language.def
				 
		    
# ***************** END of mod_chan *******************
# ***************** START of mod_love *****************
mod_love_la_LIBADD=\
		   libexordium.la
mod_love_la_LDFLAGS=\
		   -avoid-version -module
mod_love_la_SOURCES=\
		   source/modules/mod.love/language.cpp \
		   source/modules/mod.love/language.h \
		   source/modules/mod.love/love.cpp \
		   source/modules/mod.love/love.h \
		   source/modules/mod.love/tables.cpp \
		   source/modules/mod.love/tables.h

source/modules/mod.love/language.cpp source/modules/mod.love/language.h: source/modules/mod.love/language.tpl $(top_builddir)/autogen/language.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T source/modules/mod.love/language.tpl \
		$(top_builddir)/autogen/language.def

# ***************** END of mod_love *******************
# ***************** START of mod_nick *****************
mod_nick_la_LIBADD=\
		    libexordium.la
mod_nick_la_LDFLAGS=\
		    -avoid-version -module
mod_nick_la_SOURCES=\
		    source/modules/mod.nick/language.cpp \
		    source/modules/mod.nick/language.h \
		    source/modules/mod.nick/commands.cpp \
		    source/modules/mod.nick/commands.h \
		    source/modules/mod.nick/nick.cpp \
		    source/modules/mod.nick/nick.h \
		    source/modules/mod.nick/tables.cpp \
		    source/modules/mod.nick/tables.h

source/modules/mod.nick/commands.cpp source/modules/mod.nick/commands.h: source/modules/mod.nick/commands.tpl source/modules/mod.nick/commands.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T source/modules/mod.nick/commands.tpl \
		source/modules/mod.nick/commands.def

source/modules/mod.nick/language.cpp source/modules/mod.nick/language.h: source/modules/mod.nick/language.tpl $(top_builddir)/autogen/language.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T source/modules/mod.nick/language.tpl \
		$(top_builddir)/autogen/language.def

# ***************** END of mod_nick *******************
# ***************** START of mod_note *****************
mod_note_la_LIBADD=\
		    libexordium.la
mod_note_la_LDFLAGS=\
		    -avoid-version -module
mod_note_la_SOURCES=\
		    source/modules/mod.note/language.cpp \
		    source/modules/mod.note/language.h \
		    source/modules/mod.note/note.cpp \
		    source/modules/mod.note/note.h \
		    source/modules/mod.note/tables.cpp \
		    source/modules/mod.note/tables.h

source/modules/mod.note/language.cpp source/modules/mod.note/language.h: source/modules/mod.note/language.tpl $(top_builddir)/autogen/language.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T source/modules/mod.note/language.tpl \
		$(top_builddir)/autogen/language.def

# ***************** END of mod_note *******************
# ***************** START of mod_oper *****************
mod_oper_la_LIBADD=\
		   libexordium.la
mod_oper_la_LDFLAGS=\
		   -avoid-version -module
mod_oper_la_SOURCES=\
		   source/modules/mod.oper/language.cpp \
		   source/modules/mod.oper/language.h \
		   source/modules/mod.oper/oper.cpp \
		   source/modules/mod.oper/oper.h

source/modules/mod.oper/language.cpp source/modules/mod.oper/language.h: source/modules/mod.oper/language.tpl $(top_builddir)/autogen/language.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T source/modules/mod.oper/language.tpl \
		$(top_builddir)/autogen/language.def

# ***************** END of mod_oper *******************
# ***************** START of mod_serv *****************
mod_serv_la_LIBADD=\
		     libexordium.la
mod_serv_la_LDFLAGS=\
		     -avoid-version -module
mod_serv_la_SOURCES=\
		     source/modules/mod.serv/language.cpp \
		     source/modules/mod.serv/language.h \
		     source/modules/mod.serv/serv.cpp \
		     source/modules/mod.serv/serv.h \
		     source/modules/mod.serv/tables.cpp \
		     source/modules/mod.serv/tables.h 

source/modules/mod.serv/language.cpp source/modules/mod.serv/language.h: source/modules/mod.serv/language.tpl $(top_builddir)/autogen/language.def
		      $(AUTOGEN) \
		      		 -b $(basename $@) \
				 -T source/modules/mod.serv/language.tpl \
				 $(top_builddir)/autogen/language.def

# ***************** END of mod_serv *******************
# ***************** START of mod_stats ****************
mod_stats_la_LIBADD=\
		     libexordium.la
mod_stats_la_LDFLAGS=\
		     -avoid-version -module
mod_stats_la_SOURCES=\
		     source/modules/mod.stats/stats.cpp \
		     source/modules/mod.stats/stats.h \
		     source/modules/mod.stats/tables.cpp \
		     source/modules/mod.stats/tables.h
# ***************** END of mod_stats ******************
# ***************** START of mod_vote *****************
mod_vote_la_LIBADD=\
		    libexordium.la
mod_vote_la_LDFLAGS=\
		    -avoid-version -module
mod_vote_la_SOURCES=\
		     source/modules/mod.vote/vote.cpp \
		     source/modules/mod.vote/vote.h

# ***************** END of mod_vote *******************
# ***************** START of mod_help *****************
mod_help_la_LIBADD=\
		    libexordium.la
mod_help_la_LDFLAGS=\
		    -avoid-version -module
mod_help_la_SOURCES=\
		    source/modules/mod.help/help.cpp \
		    source/modules/mod.help/help.h

# ***************** END of mod_help *******************
# ***************** START of mod_credits **************
mod_credits_la_SOURCES=\
			source/modules/mod.credits/language.cpp \
			source/modules/mod.credits/language.h \
			source/modules/mod.credits/credits.cpp

mod_credits_la_LIBADD=\
		       libexordium_bank.la \
		       libexordium.la

mod_credits_la_LDFLAGS=\
			-avoid-version -module

source/modules/mod.credits/language.cpp source/modules/mod.credits/language.h: source/modules/mod.credits/language.tpl $(top_builddir)/autogen/language.def
	$(AUTOGEN) \
		-b $(basename $@) \
		-T source/modules/mod.credits/language.tpl \
		$(top_builddir)/autogen/language.def

# ***************** END of mod_credits ****************
# ***************** START of mod_game *****************
mod_game_la_SOURCES = \
		    source/modules/mod.game/channelgame.cpp \
		    source/modules/mod.game/config.cpp \
		    source/modules/mod.game/game.cpp 

mod_game_la_LIBADD = \
		    libexordium.la
 
mod_game_la_LDFLAGS = \
		    -avoid-version -module

lib_cards_la_SOURCES = \
		    source/modules/mod.game/cards/card.cpp \
		    source/modules/mod.game/cards/hand.cpp

game_hangman_la_SOURCES = \
		    source/modules/mod.game/hangman.cpp

game_hangman_la_LIBADD = \
		    libexordium.la

game_hangman_la_LDFLAGS = \
		    -avoid-version -module
		    
game_exordi8_la_SOURCES = \
		    source/modules/mod.game/exordi8.cpp

game_exordi8_la_LIBADD = \
		    lib_cards.la \
		    libexordium.la

game_exordi8_la_LDFLAGS = \
		    -avoid-version -module

game_poker_la_SOURCES = \
		    source/modules/mod.game/poker.cpp

game_poker_la_LIBADD = \
		    lib_cards.la \
		    libexordium.la

game_poker_la_LDFLAGS = \
		    -avoid-version -module

# ***************** END of mod_game *******************
# Stuff we need to make, but automake is too dumb to realise
BUILT_SOURCES=\
	install/.lang \
	include/exordium/database/table.h

# rule to create the exordium/database/table.h file
include/exordium/database/table.h: include/exordium/database/table.h.tpl autogen/fieldtypes.def
	$(AUTOGEN) -b include/exordium/database/table \
		-T include/exordium/database/table.h.tpl \
		autogen/fieldtypes.def

# rule to create the language data files
install/.lang: install/language.tpl autogen/language.def
	$(AUTOGEN) -b install/ \
		-T install/language.tpl \
		autogen/language.def
