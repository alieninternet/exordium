#SUBDIRS=credits
INCLUDES=-I../../../include -I../../../../kineircd/include -I../../
lib_LTLIBRARIES= mod_credits.la
mod_credits_la_SOURCES = \
	                 language.cpp \
			 language.h \
			 credits.cpp

language.cpp language.h: language.tpl $(top_builddir)/autogen/language.def
	     $(AUTOGEN) \
	     		-b $(basename $@) \
			-T language.tpl \
			$(top_builddir)/autogen/language.def
     
mod_credits_la_LIBADD = ../../libbank/libexordium_bank.la \
                        ../../libexordium.la
mod_credits_la_LDFLAGS = -avoid-version -module


CPPFLAGS=
LDFLAGS=


