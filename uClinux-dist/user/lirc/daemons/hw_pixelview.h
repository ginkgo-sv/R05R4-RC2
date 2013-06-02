/*      $Id: hw_pixelview.h,v 1.2 2005/07/25 03:57:11 magicyang Exp $      */

/****************************************************************************
 ** hw_pixelview.h **********************************************************
 ****************************************************************************
 *
 * routines for PixelView Play TV receiver
 * 
 * Copyright (C) 1999 Christoph Bartelmus <lirc@bartelmus.de>
 *
 */

#ifndef _HW_PIXELVIEW_H
#define _HW_PIXELVIEW_H

#include <linux/lirc.h>

int pixelview_decode(struct ir_remote *remote,
		  ir_code *prep,ir_code *codep,ir_code *postp,
		  int *repeat_flagp,lirc_t *remaining_gapp);
int pixelview_init(void);
int pixelview_deinit(void);
char *pixelview_rec(struct ir_remote *remotes);

#endif
