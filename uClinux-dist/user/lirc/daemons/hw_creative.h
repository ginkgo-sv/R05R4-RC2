/*      $Id: hw_creative.h,v 1.2 2005/07/25 03:57:11 magicyang Exp $      */

/****************************************************************************
 ** hw_creative.h **********************************************************
 ****************************************************************************
 *
 * routines for Creative receiver 
 * 
 * Copyright (C) 1999 Christoph Bartelmus <lirc@bartelmus.de>
 *	modified for creative receiver by Isaac Lauer <inl101@alumni.psu.edu>
 */

#ifndef _HW_CREATIVE_H
#define _HW_CREATIVE_H

#include <linux/lirc.h>

int creative_decode(struct ir_remote *remote,
		  ir_code *prep,ir_code *codep,ir_code *postp,
		  int *repeat_flagp,lirc_t *remaining_gapp);
int creative_init(void);
int creative_deinit(void);
char *creative_rec(struct ir_remote *remotes);

#endif
