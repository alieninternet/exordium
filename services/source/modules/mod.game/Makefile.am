SUBDIRS=cards
INCLUDES=-I../../../include -I../../../../kineircd/include @PGSQL_INCDIR@
lib_LTLIBRARIES= mod_game.la
mod_game_la_SOURCES = channelgame.cpp \
		      config.cpp \
		      exordi8.cpp \
                      hangman.cpp \
                      poker.cpp \
		      game.cpp
mod_game_la_LIBADD = cards/lib_cards.la ../../libexordium.la
mod_game_la_LDFLAGS = -avoid-version -module
CPPFLAGS=
LDFLAGS=


