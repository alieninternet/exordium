SUBDIRS=cards
INCLUDES=-I../../../include -I../../../../kineircd/include
lib_LTLIBRARIES= mod_game.la game_hangman.la game_exordi8.la game_poker.la
mod_game_la_SOURCES = channelgame.cpp \
		      config.cpp \
		      game.cpp
mod_game_la_LIBADD = ../../libexordium.la
mod_game_la_LDFLAGS = -avoid-version -module
game_hangman_la_SOURCES = hangman.cpp 
game_hangman_la_LIBADD = ../../libexordium.la
game_hangman_la_LDFLAGS = -avoid-version -module
game_exordi8_la_SOURCES = exordi8.cpp 
game_exordi8_la_LIBADD = cards/lib_cards.la ../../libexordium.la
game_exordi8_la_LDFLAGS = -avoid-version -module
game_poker_la_SOURCES = poker.cpp 
game_poker_la_LIBADD = cards/lib_cards.la ../../libexordium.la
game_poker_la_LDFLAGS = -avoid-version -module
CPPFLAGS=
LDFLAGS=
