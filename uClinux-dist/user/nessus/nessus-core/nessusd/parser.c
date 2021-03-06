/* Nessus
 * Copyright (C) 1998-2001 Renaud Deraison
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2,
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * In addition, as a special exception, Renaud Deraison
 * gives permission to link the code of this program with any
 * version of the OpenSSL library which is distributed under a
 * license identical to that listed in the included COPYING.OpenSSL
 * file, and distribute linked combinations including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * this file, you may extend this exception to your version of the
 * file, but you are not obligated to do so.  If you do not wish to
 * do so, delete this exception statement from your version.
 */

#include <includes.h>

/* 
 * This function extracts the string after the 
 * ' <|> ' symbol.
 *
 */
char * 
parse_symbol(str)
	char * str;
{
 char * s = str;
 
 while(s)
 {
 s = strchr(s, '|');
 if(!s)return(NULL);
 if((s[1]=='>')&&(s-1)[0]=='<')return(s+3);
 s++;
 }
 return(NULL);
}

char * parse_separator(str)
	char * str;
{
 char * s_1, *s_2;
 char * ret;
 
 s_1 = parse_symbol(str);
 if(!s_1)return(NULL);
 s_2 = parse_symbol(s_1);
 if(!s_2)
  {
  ret = emalloc(strlen(s_1));
  strncpy(ret, s_1, strlen(s_1)-1);
  }
 else
 {
 int c;
 s_2 = s_2 - 4;
 c = s_2[0];
 s_2[0] = 0;
 ret = emalloc(strlen(s_1));
 strncpy(ret, s_1, strlen(s_1)-1);
 s_2[0]=c;
 }
#ifdef DEBUGMORE
 fprintf(stderr, "%s:%d got %s returning \"%s\"\n", __FILE__, __LINE__, str, ret);
#endif
  return(ret);
}
 
 
