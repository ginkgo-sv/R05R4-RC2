/****************************************************************************
 * Copyright (c) 1998 Free Software Foundation, Inc.                        *
 *                                                                          *
 * Permission is hereby granted, free of charge, to any person obtaining a  *
 * copy of this software and associated documentation files (the            *
 * "Software"), to deal in the Software without restriction, including      *
 * without limitation the rights to use, copy, modify, merge, publish,      *
 * distribute, distribute with modifications, sublicense, and/or sell       *
 * copies of the Software, and to permit persons to whom the Software is    *
 * furnished to do so, subject to the following conditions:                 *
 *                                                                          *
 * The above copyright notice and this permission notice shall be included  *
 * in all copies or substantial portions of the Software.                   *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  *
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF               *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   *
 * IN NO EVENT SHALL THE ABOVE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR    *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR    *
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.                               *
 *                                                                          *
 * Except as contained in this notice, the name(s) of the above copyright   *
 * holders shall not be used in advertising or otherwise to promote the     *
 * sale, use or other dealings in this Software without prior written       *
 * authorization.                                                           *
 ****************************************************************************/

/****************************************************************************
 *  Author: Zeyd M. Ben-Halim <zmbenhal@netcom.com> 1992,1995               *
 *     and: Eric S. Raymond <esr@snark.thyrsus.com>                         *
 ****************************************************************************/

#include <curses.priv.h>

MODULE_ID("$Id: lib_bkgd.c,v 1.1.1.1 2004/07/19 09:47:07 lgsoft Exp $")

void wbkgdset(WINDOW *win, chtype ch)
{
  T((T_CALLED("wbkgdset(%p,%s)"), win, _tracechtype(ch)));
    
  if (win) {
    chtype off = AttrOf(win->_bkgd);
    chtype on  = AttrOf(ch);
    
    toggle_attr_off(win->_attrs,off);
    toggle_attr_on (win->_attrs,on);
    
    if (TextOf(ch)==0)
      ch |= BLANK;
    win->_bkgd = ch;
  }
  returnVoid;
}

int wbkgd(WINDOW *win, const chtype ch)
{
  int code = ERR;
  int x, y;
  chtype new_bkgd = ch;

  T((T_CALLED("wbkgd(%p,%s)"), win, _tracechtype(new_bkgd)));

  if (win) {
    chtype old_bkgd = getbkgd(win);

    wbkgdset(win, new_bkgd);
    wattrset(win, AttrOf(win->_bkgd));
    
    for (y = 0; y <= win->_maxy; y++) {
      for (x = 0; x <= win->_maxx; x++) {
	if (win->_line[y].text[x] == old_bkgd)
	  win->_line[y].text[x] = win->_bkgd;
	else 
	  win->_line[y].text[x] =
	    _nc_render(win,(A_ALTCHARSET & 
			    AttrOf(win->_line[y].text[x])) 
		       | TextOf(win->_line[y].text[x]));
      }
    }
    touchwin(win);
    _nc_synchook(win);
    code = OK;
  }
  returnCode(code);
}
