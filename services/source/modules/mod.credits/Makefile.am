#SUBDIRS=credits
INCLUDES=-I../../../include -I../../../../kineircd/include -I../../
lib_LTLIBRARIES= mod_credits.la
mod_credits_la_SOURCES = credits.cpp
mod_credits_la_LIBADD = ../../libbank/libexordium_bank.la \
                        ../../libexordium.la
mod_credits_la_LDFLAGS = -avoid-version -module
CPPFLAGS=
LDFLAGS=


