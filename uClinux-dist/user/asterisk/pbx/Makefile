#
# Asterisk -- A telephony toolkit for Linux.
# 
# Makefile for PBX frontends (dynamically loaded)
#
# Copyright (C) 1999, Mark Spencer
#
# Mark Spencer <markster@linux-support.net>
#
# This program is free software, distributed under the terms of
# the GNU General Public License
#



PBX_LIBS=pbx_config.so pbx_wilcalu.so pbx_spool.so     # pbx_gtkconsole.so pbx_kdeconsole.so

# Add GTK console if appropriate
PBX_LIBS+=$(shell gtk-config --cflags >/dev/null 2>/dev/null && echo "pbx_gtkconsole.so")
# Add KDE Console if appropriate
#PBX_LIBS+=$(shell [ "$$QTDIR" != "" ] && echo "pbx_kdeconsole.so")


GTK_FLAGS=`gtk-config --cflags gthread`
GTK_LIBS=`gtk-config --libs gthread`
#CXX=egcs
MOC=$(QTDIR)/bin/moc
KDE_FLAGS=-I$(KDEDIR)/include -I$(KDEDIR)/include/kde -I$(QTDIR)/include
KDE_LIBS=-L$(KDEDIR)/lib -L$(QTDIR)/lib -lqt -lkdecore -lkdeui
CFLAGS+=-fPIC

KDE_CONSOLE_OBJS=pbx_kdeconsole_main.o pbx_kdeconsole.o 

all: depend $(PBX_LIBS)

clean:
	rm -f *.so *.o .depend

pbx_gtkconsole.o: pbx_gtkconsole.c
	$(CC) $(CFLAGS) $(GTK_FLAGS) -c -o $@ $<

pbx_gtkconsole.so: pbx_gtkconsole.o
	$(CC) $(SOLINK) -o $@ $< $(GTK_LIBS)

pbx_kdeconsole.o: pbx_kdeconsole.cc pbx_kdeconsole.moc
	$(CXX) $(CFLAGS) $(KDE_FLAGS) -c -o $@ $<

pbx_kdeconsole_main.o: pbx_kdeconsole_main.cc pbx_kdeconsole.h
	$(CXX) $(CFLAGS) $(KDE_FLAGS) -c -o $@ $<

pbx_kdeconsole.so: $(KDE_CONSOLE_OBJS)
	$(CC) $(SOLINK) -o $@ $(KDE_CONSOLE_OBJS) $(KDE_LIBS)

%.moc : %.h
	$(MOC) $< -o $@

%.so : %.o
	$(CC) $(SOLINK) -o $@ $<

ifneq ($(wildcard .depend),)
include .depend
endif

install: all
	for x in $(PBX_LIBS); do $(INSTALL) -m 755 $$x $(DESTDIR)$(MODULES_DIR) ; done

depend: .depend

.depend:
	../mkdep $(CFLAGS) `ls *.c`
