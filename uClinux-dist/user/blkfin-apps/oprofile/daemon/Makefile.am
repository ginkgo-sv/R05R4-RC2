SUBDIRS = liblegacy .

oprofiled_SOURCES = \
	init.c \
	oprofiled.c \
	oprofiled.h \
	opd_stats.c \
	opd_sfile.c \
	opd_sfile.h \
	opd_kernel.c \
	opd_kernel.h \
	opd_trans.c \
	opd_trans.h \
	opd_printf.h \
	opd_stats.h \
	opd_cookie.c \
	opd_cookie.h \
	opd_events.c \
	opd_events.h \
	opd_interface.h \
	opd_mangling.c \
	opd_mangling.h \
	opd_perfmon.h \
	opd_perfmon.c \
	opd_anon.h \
	opd_anon.c

LIBS=@POPT_LIBS@ @LIBERTY_LIBS@

AM_CPPFLAGS = \
	-I ${top_srcdir}/libabi \
	-I ${top_srcdir}/libutil \
	-I ${top_srcdir}/libop \
	-I ${top_srcdir}/libdb

# -fno-omit-frame-pointer needed for daemon build: see ChangeLog 2004-02-23
AM_CFLAGS = @OP_CFLAGS@ -fno-omit-frame-pointer

bin_PROGRAMS = oprofiled

if enable_abi
oprofiled_LDADD = \
	liblegacy/liblegacy.a \
	../libabi/libabi.a \
	../libdb/libodb.a \
	../libop/libop.a \
	../libutil/libutil.a
LINKER=$(CXX)
else
oprofiled_LDADD = \
	liblegacy/liblegacy.a \
	../libdb/libodb.a \
	../libop/libop.a \
	../libutil/libutil.a
LINKER=$(CC)
endif

oprofiled_LINK = $(LINKER) $(AM_CFLAGS) $(CFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@
