/*
 * $Id: checks.c,v 1.1 2005/07/25 05:52:08 magicyang Exp $
 *
 * Various URI checks
 *
 * Copyright (C) 2001-2003 Fhg Fokus
 *
 * This file is part of ser, a free SIP server.
 *
 * ser is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version
 *
 * For a license to use the ser software under conditions
 * other than those described here, or to purchase support for this
 * software, please contact iptel.org by e-mail at the following addresses:
 *    info@iptel.org
 *
 * ser is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * History:
 * --------
 * 2003-02-26: Created by janakj
 * 2004-03-20: has_totag introduced (jiri)
 */

#include <string.h>
#include "../../str.h"
#include "../../dprint.h"               /* Debugging */
#include "../../parser/digest/digest.h" /* get_authorized_cred */
#include "../../parser/parse_from.h"
#include "../../parser/parse_uri.h"
#include "../../ut.h"                   /* Handy utilities */
#include "../../db/db.h"                /* Database API */
#include "uri_mod.h"
#include "checks.h"

/*
 * Checks if From includes a To-tag -- good to identify
 * if a request creates a new dialog
 */
int has_totag(struct sip_msg* _m, char* _foo, char* _bar)
{
	str tag;

	if (!_m->to && parse_headers(_m, HDR_TO,0)==-1) {
		LOG(L_ERR, "ERROR: has_totag: To parsing failed\n");
		return -1;
	}
	if (!_m->to) {
		LOG(L_ERR, "ERROR: has_totag: no To\n");
		return -1;
	}
	tag=get_to(_m)->tag_value;
	if (tag.s==0 || tag.len==0) {
		DBG("DEBUG: has_totag: no totag\n");
		return -1;
	}
	DBG("DEBUG: has_totag: totag found\n");
	return 1;
}


/*
 * Check if the username matches the username in credentials
 */
int is_user(struct sip_msg* _m, char* _user, char* _str2)
{
	str* s;
	struct hdr_field* h;
	auth_body_t* c;

	s = (str*)_user;

	get_authorized_cred(_m->authorization, &h);
	if (!h) {
		get_authorized_cred(_m->proxy_auth, &h);
		if (!h) {
			LOG(L_ERR, "is_user(): No authorized credentials found (error in scripts)\n");
			LOG(L_ERR, "is_user(): Call {www,proxy}_authorize before calling is_user function !\n");
			return -1;
		}
	}

	c = (auth_body_t*)(h->parsed);

	if (!c->digest.username.user.len) {
		DBG("is_user(): Username not found in credentials\n");
		return -1;
	}

	if (s->len != c->digest.username.user.len) {
		DBG("is_user(): Username length does not match\n");
		return -1;
	}

	if (!memcmp(s->s, c->digest.username.user.s, s->len)) {
		DBG("is_user(): Username matches\n");
		return 1;
	} else {
		DBG("is_user(): Username differs\n");
		return -1;
	}
}


/*
 * Check if a header field contains the same username
 * as digest credentials
 */
static inline int check_username(struct sip_msg* _m, str* _uri)
{
	struct hdr_field* h;
	auth_body_t* c;
	struct sip_uri puri;
	db_key_t keys[3];
	db_val_t vals[3];
	db_key_t cols[1];
	db_res_t* res;

	if (!_uri) {
		LOG(L_ERR, "check_username(): Bad parameter\n");
		return -1;
	}

	     /* Get authorized digest credentials */
	get_authorized_cred(_m->authorization, &h);
	if (!h) {
		get_authorized_cred(_m->proxy_auth, &h);
		if (!h) {
			LOG(L_ERR, "check_username(): No authorized credentials found (error in scripts)\n");
			LOG(L_ERR, "check_username(): Call {www,proxy}_authorize before calling check_* function !\n");
			return -2;
		}
	}

	c = (auth_body_t*)(h->parsed);

	     /* Parse To/From URI */
	if (parse_uri(_uri->s, _uri->len, &puri) < 0) {
		LOG(L_ERR, "check_username(): Error while parsing URI\n");
		return -3;
	}
	
	     /* Make sure that the URI contains username */
 	if (!puri.user.len) {
		LOG(L_ERR, "check_username(): Username not found in URI\n");
		return -4;
	}

	     /* If use_uri_table is set, use URI table to determine if Digest username
	      * and To/From username match. URI table is a table enumerating all allowed
	      * usernames for a single, thus a user can have several different usernames
	      * (which are different from digest username and it will still match)
	      */
	if (use_uri_table) {
		     /* Make sure that From/To URI domain and digest realm are equal
		      * FIXME: Should we move this outside this condition and make it general ?
		      */
		if (puri.host.len != c->digest.realm.len) {
			LOG(L_ERR, "check_username(): Digest realm and URI domain do not match\n");
			return -5;
		}

		if (strncasecmp(puri.host.s, c->digest.realm.s, puri.host.len) != 0) {
			DBG("check_username(): Digest realm and URI domain do not match\n");
			return -6;
		}

		if (db_use_table(db_handle, uri_table) < 0) {
			LOG(L_ERR, "check_username(): Error while trying to use uri table\n");
		}

		keys[0] = uri_user_col;
		keys[1] = uri_domain_col;
		keys[2] = uri_uriuser_col;
		cols[0] = uri_user_col;

		VAL_TYPE(vals) = VAL_TYPE(vals + 1) = VAL_TYPE(vals + 2) = DB_STR;
		VAL_NULL(vals) = VAL_NULL(vals + 1) = VAL_NULL(vals + 2) = 0;
    
		VAL_STR(vals) = c->digest.username.user;
    		VAL_STR(vals + 1) = c->digest.realm;
		VAL_STR(vals + 2) = puri.user;

		if (db_query(db_handle, keys, 0, vals, cols, 3, 1, 0, &res) < 0) {
			LOG(L_ERR, "check_username(): Error while querying database\n");
			return -7;
		}

		     /* If the previous function returns at least one row, it means
		      * there is an entry for given digest username and URI username
		      * and thus this combination is allowed and the function will match
		      */
		if (RES_ROW_N(res) == 0) {
			DBG("check_username(): From/To user '%.*s' is spoofed\n", 
			    puri.user.len, ZSW(puri.user.s));
			db_free_query(db_handle, res);
			return -8;
		} else {
			DBG("check_username(): From/To user '%.*s' and auth user match\n", 
			    puri.user.len, ZSW(puri.user.s));
			db_free_query(db_handle, res);
			return 1;
		}
	} else {
		     /* URI table not used, simply compare digest username and From/To
		      * username, the comparison is case insensitive
		      */
		if (puri.user.len == c->digest.username.user.len) {
			if (!strncasecmp(puri.user.s, c->digest.username.user.s, puri.user.len)) {
				DBG("check_username(): Digest username and URI username match\n");
				return 1;
			}
		}
	
		DBG("check_username(): Digest username and URI username do NOT match\n");
		return -9;
	}
}


/*
 * Check username part in To header field
 */
int check_to(struct sip_msg* _m, char* _s1, char* _s2)
{
	if (!_m->to && ((parse_headers(_m, HDR_TO, 0) == -1) || (!_m->to))) {
		LOG(L_ERR, "check_to(): Error while parsing To header field\n");
		return -1;
	}
	return check_username(_m, &get_to(_m)->uri);
}


/*
 * Check username part in From header field
 */
int check_from(struct sip_msg* _m, char* _s1, char* _s2)
{
	if (parse_from_header(_m) < 0) {
		LOG(L_ERR, "check_from(): Error while parsing From header field\n");
		return -1;
	}

	return check_username(_m, &get_from(_m)->uri);
}


/*
 * Check if uri belongs to a local user
 */
int does_uri_exist(struct sip_msg* _msg, char* _s1, char* _s2)
{
	db_key_t keys[2];
	db_val_t vals[2];
	db_key_t cols[1];
	db_res_t* res;

	if (parse_sip_msg_uri(_msg) < 0) {
		LOG(L_ERR, "does_uri_exist(): Error while parsing URI\n");
		return -1;
	}

	if (use_uri_table) {
		if (db_use_table(db_handle, uri_table) < 0) {
			LOG(L_ERR, "does_uri_exist(): Error while trying to use uri table\n");
		}
		keys[0] = uri_uriuser_col;
		keys[1] = uri_domain_col;
		cols[0] = uri_uriuser_col;
	} else {
		if (db_use_table(db_handle, subscriber_table) < 0) {
			LOG(L_ERR, "does_uri_exist(): Error while trying to use subscriber table\n");
		}
		keys[0] = subscriber_user_col;
		keys[1] = subscriber_domain_col;
		cols[0] = subscriber_user_col;
	}

	VAL_TYPE(vals) = VAL_TYPE(vals + 1) = DB_STR;
	VAL_NULL(vals) = VAL_NULL(vals + 1) = 0;
	VAL_STR(vals) = _msg->parsed_uri.user;
	VAL_STR(vals + 1) = _msg->parsed_uri.host;

	if (db_query(db_handle, keys, 0, vals, cols, (use_domain ? 2 : 1), 1, 0, &res) < 0) {
		LOG(L_ERR, "does_uri_exist(): Error while querying database\n");
		return -2;
	}
	
	if (RES_ROW_N(res) == 0) {
		DBG("does_uri_exit(): User in request uri does not exist\n");
		db_free_query(db_handle, res);
		return -3;
	} else {
		DBG("does_uri_exit(): User in request uri does exist\n");
		db_free_query(db_handle, res);
		return 1;
	}
}
