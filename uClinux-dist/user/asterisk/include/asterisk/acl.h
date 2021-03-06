/*
 * Asterisk -- A telephony toolkit for Linux.
 *
 * Access Control of various sorts
 * 
 * Copyright (C) 1999, Mark Spencer
 *
 * Mark Spencer <markster@linux-support.net>
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License
 */

#ifndef _ASTERISK_ACL_H
#define _ASTERISK_ACL_H

#define AST_SENSE_DENY                  0
#define AST_SENSE_ALLOW                 1


#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#include <netinet/in.h>

/* Host based access control */

struct ast_ha;

extern void ast_free_ha(struct ast_ha *ha);
extern struct ast_ha *ast_append_ha(char *sense, char *stuff, struct ast_ha *path);
extern int ast_apply_ha(struct ast_ha *ha, struct sockaddr_in *sin);
extern int ast_get_ip(struct sockaddr_in *sin, char *value);
extern int ast_ouraddrfor(struct in_addr *them, struct in_addr *us);
extern int ast_lookup_iface(char *iface, struct in_addr *address);
extern struct ast_ha *ast_duplicate_ha_list(struct ast_ha *original);
extern int ast_find_ourip(struct in_addr *ourip, struct sockaddr_in bindaddr);

//! Compares the source address and port of two sockaddr_in
static inline int inaddrcmp(struct sockaddr_in *sin1, struct sockaddr_in *sin2)
{
	return ((sin1->sin_addr.s_addr != sin2->sin_addr.s_addr ) 
		|| (sin1->sin_port != sin2->sin_port));
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
