/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */

/* $Header: /cvsroot/uclinux533/uClinux-dist/user/blkfin-apps/linphone/linphone/gsmlib/debug.c,v 1.2 2005/11/09 11:26:26 hjuergen Exp $ */

#include "private.h"

#ifndef	NDEBUG

/* If NDEBUG _is_ defined and no debugging should be performed,
 * calls to functions in this module are #defined to nothing
 * in private.h.
 */

#include <stdio.h>
#include "proto.h"

void gsm_debug_words P4( (name, from, to, ptr), 
	char 	      * name,
	int		from,
	int		to,
	word		* ptr)
{
	int 	nprinted = 0;

	fprintf( stderr, "%s [%d .. %d]: ", name, from, to );
	while (from <= to) {
		fprintf(stderr, "%d ", ptr[ from ] );
		from++;
		if (nprinted++ >= 7) {
			nprinted = 0;
			if (from < to) putc('\n', stderr);
		}
	}
	putc('\n', stderr);
}

void gsm_debug_longwords P4( (name, from, to, ptr),
	char 	      * name,
	int		from,
	int		to,
	longword      * ptr)
{
	int 	nprinted = 0;

	fprintf( stderr, "%s [%d .. %d]: ", name, from, to );
	while (from <= to) {

		fprintf(stderr, "%d ", ptr[ from ] );
		from++;
		if (nprinted++ >= 7) {
			nprinted = 0;
			if (from < to) putc('\n', stderr);
		}
	}
	putc('\n', stderr);
}

void gsm_debug_longword P2(  (name, value),
	char		* name,
	longword	  value	)
{
	fprintf(stderr, "%s: %d\n", name, (long)value );
}

void gsm_debug_word P2(  (name, value),
	char	* name,
	word	  value	)
{
	fprintf(stderr, "%s: %d\n", name, (long)value);
}

#endif
