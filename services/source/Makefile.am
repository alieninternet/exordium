INCLUDES= -I../include -I../../kineircd/include -Imodules/include
lib_LTLIBRARIES = lib_mod_exordium.la libservices.la lib_bot.la \
		  lib_chan.la lib_nick.la lib_note.la lib_cards.la \
		  lib_game.la lib_love.la 
lib_mod_exordium_la_SOURCES = module.cpp
lib_mod_exordium_la_LDFLAGS = -module 
lib_mod_exordium_la_LIBADD = libservices.la
libservices_la_LIBADD = -lkineircd -lmysqlclient \
		        -L../../kineircd/src/lib/
libservices_la_SOURCES = lib/channel.cpp \
		         lib/conf.cpp \
			 lib/ircdome.cpp \
			 lib/log.cpp \
			 lib/modules.cpp \
			 lib/parser.cpp \
			 lib/services.cpp \
			 lib/sql.cpp \
			 lib/user.cpp
lib_bot_la_SOURCES = modules/bot.cpp
lib_bot_la_LIBADD = libservices.la
lib_chan_la_SOURCES = modules/chan.cpp
lib_chan_la_LIBADD = libservices.la
lib_nick_la_SOURCES = modules/nick.cpp
lib_nick_la_LIBADD = libservices.la
lib_note_la_SOURCES = modules/note.cpp
lib_note_la_LIBADD = libservices.la
lib_cards_la_SOURCES = modules/game/cards/card.cpp \
		       modules/game/cards/hand.cpp \
		       modules/game/cards/pack.cpp 
lib_game_la_SOURCES = modules/game/channelgame.cpp \
		      modules/game/exordi8.cpp \
		      modules/game/game.cpp
lib_game_la_LIBADD = lib_cards.la libservices.la
lib_love_la_SOURCES = modules/love/love.cpp 
lib_love_la_LIBAIDD = libservices.la

