SUBDIRS=cards
INCLUDES=-I../../../include -I../../../../kineircd/include
lib_LTLIBRARIES= mod_game.la
mod_game_la_SOURCES = channelgame.cpp \
		      exordi8.cpp \
		      game.cpp
mod_game_la_LIBADD = cards/lib_cards.la ../../libservices.la
mod_game_la_LDFLAGS = -avoid-version -module
