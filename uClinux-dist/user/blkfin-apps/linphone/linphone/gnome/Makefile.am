## Process this file with automake to produce Makefile.in

linphone_common_sources=\
	linphone.c linphone.h \
	gui_utils.c gui_utils.h \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	presence.c presence.h \
	propertybox.c propertybox.h \
	addressbook.c addressbook.h \
	friends.c friends.h


if BUILD_GNOME

INCLUDES = \
	-I$(top_srcdir)\
	-I$(top_srcdir)/intl \
	-I$(top_srcdir)/oRTP/src \
	-I$(top_srcdir)/oRTP/ \
	-I$(top_srcdir)/mediastreamer \
	-I$(top_srcdir)/speex/libspeex \
	-I$(top_srcdir)/coreapi \
	-I$(top_srcdir)/osipua/ \
	-I$(top_srcdir)/osipua/src \
	-I$(osip_prefix)/include/ 



bin_PROGRAMS = linphone

linphone_SOURCES = \
	main.c \
	$(linphone_common_sources)

linphone_LDADD = $(LIBGNOMEUI_LIBS) $(INTLLIBS) \
				$(LIBGNOME_LIBS) \
				$(top_builddir)/coreapi/liblinphone.la 							

endif

AM_CFLAGS=$(STRICT_OPTIONS) $(LIBGNOMEUI_CFLAGS) $(LIBGNOME_CFLAGS) $(GNOME_APPLETS_CFLAGS) $(IPV6_CFLAGS)	

if BUILD_GNOME_APPLET

gnome_appletdir=$(libexecdir)

gnome_applet_PROGRAMS = linphone_applet

linphone_applet_SOURCES = \
	$(linphone_common_sources)\
	applet.c
	
linphone_applet_CFLAGS=$(AM_CFLAGS) -DLINPHONE_APPLET

linphone_applet_LDADD = \
				$(GNOME_APPLETS_LIBS) \
				$(top_srcdir)/coreapi/liblinphone.la

#uidir		= $(datadir)/gnome-2.0/ui
#ui_DATA		= GNOME_ModemlightsApplet.xml

serverdir       = $(libdir)/bonobo/servers
server_in_files = GNOME_LinphoneApplet.server.in
server_DATA     = $(server_in_files:.server.in=.server)

$(server_in_files): $(server_in_files:.server.in=.server.in.in)
	sed -e "s|\@LIBEXECDIR\@|$(libexecdir)|" $< > $@

@INTLTOOL_SERVER_RULE@

uidir   = $(datadir)/gnome-2.0/ui
ui_DATA = GNOME_LinphoneApplet.xml

endif			

EXTRA_DIST = 						\
	GNOME_LinphoneApplet.server.in.in	\
	applet.c \
	$(ui_DATA)
