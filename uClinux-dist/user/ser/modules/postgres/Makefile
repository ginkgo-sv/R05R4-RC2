# $Id: Makefile,v 1.1 2005/07/25 05:52:06 magicyang Exp $
#
# WARNING: do not run this directly, it should be run by the master Makefile

include ../../Makefile.defs
auto_gen=
NAME=postgres.so

# libpq-fe.h locations
DEFS +=-I/usr/local/pgsql/include -I/usr/include/postgresql
LIBS=-L$(LOCALBASE)/pgsql/lib -L$(LOCALBASE)/lib/pgsql -L/usr/pkg/lib \
		-L/usr/pkg/lib/pgsql -lpq

include ../../Makefile.modules
