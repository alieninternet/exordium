SUBDIRS = . source

# libexordium headers
include_HEADERS=\
	include/exordium/database/base.h \
	include/exordium/database/database.h \
	include/exordium/database/table.h \
	include/exordium/channel.h \
	include/exordium/config.h \
	include/exordium/dchan.h \
	include/exordium/log.h \
	include/exordium/service.h \
	include/exordium/services.h \
	include/exordium/user.h

# Stuff we need to make, but automake is too dumb to realise
BUILT_SOURCES=\
	install/.lang 

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
