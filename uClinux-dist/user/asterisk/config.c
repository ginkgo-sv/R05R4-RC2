/*
 * Asterisk -- A telephony toolkit for Linux.
 *
 * Configuration File Parser
 * 
 * Copyright (C) 1999, Mark Spencer
 *
 * Mark Spencer <markster@linux-support.net>
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <asterisk/config.h>
#include <asterisk/config_pvt.h>
#include <asterisk/cli.h>
#include <asterisk/lock.h>
#include <asterisk/options.h>
#include <asterisk/logger.h>
#include <asterisk/utils.h>
#include "asterisk.h"
#include "astconf.h"


static int ast_cust_config=0;
struct ast_config *(*global_load_func)(char *, struct ast_config *,struct ast_category **,struct ast_variable **,int
#ifdef PRESERVE_COMMENTS                                                                                                                                     
,struct ast_comment_struct *
#endif
);

AST_MUTEX_DEFINE_STATIC(ast_cust_config_lock);
static struct ast_config_reg *ast_cust_config_list;
static char *config_conf_file = "extconfig.conf";

static char *strip(char *buf)
{
	char *start;
	/* Strip off trailing whitespace, returns, etc */
	while (!ast_strlen_zero(buf) && (buf[strlen(buf)-1]<33))
		buf[strlen(buf)-1] = '\0';
	start = buf;
	/* Strip off leading whitespace, returns, etc */
	while (*start && (*start < 33))
		*start++ = '\0';
	return start;
}

#ifdef PRESERVE_COMMENTS
static void free_comments(struct ast_comment *com)
{
	struct ast_comment *l;
	while (com) {
		l = com;
		com = com->next;
		free(l);
	}
}
#endif

void ast_destroy(struct ast_config *ast)
{
	struct ast_category *cat, *catn;
	struct ast_variable *v, *vn;

	if (!ast)
		return;

	cat = ast->root;
	while(cat) {
		v = cat->root;
		while(v) {
			vn = v;
			free(v->name);
			free(v->value);
#ifdef PRESERVE_COMMENTS
			free_comments(v->precomments);
			free_comments(v->sameline);
#endif			
			v = v->next;
			free(vn);
		}
		catn = cat;
#ifdef PRESERVE_COMMENTS
		free_comments(cat->precomments);
		free_comments(cat->sameline);
#endif		
		cat = cat->next;
		free(catn);
	}
#ifdef PRESERVE_COMMENTS
	free_comments(ast->trailingcomments);
#endif	
	free(ast);
}

int ast_true(char *s)
{
	if (!s)
		return 0;
	/* Determine if this is a true value */
	if (!strcasecmp(s, "yes") ||
	    !strcasecmp(s, "true") ||
		!strcasecmp(s, "y") ||
		!strcasecmp(s, "t") ||
		!strcasecmp(s, "1") ||
		!strcasecmp(s, "on"))
			return -1;
	return 0;
}

int ast_false(char *s)
{
	if (!s)
		return 0;
	/* Determine if this is a false value */
	if (!strcasecmp(s, "no") ||
	    !strcasecmp(s, "false") ||
		!strcasecmp(s, "n") ||
		!strcasecmp(s, "f") ||
		!strcasecmp(s, "0") ||
		!strcasecmp(s, "off"))
			return -1;
	return 0;
}

struct ast_variable *ast_variable_browse(struct ast_config *config, char *category)
{
	struct ast_category *cat;
	cat = config->root;
	while(cat) {
		if (cat->name == category)
			return cat->root;
		cat = cat->next;
	}
	cat = config->root;
	while(cat) {
		if (!strcasecmp(cat->name, category))
			return cat->root;
		cat = cat->next;
	}
	return NULL;
}

char *ast_variable_retrieve(struct ast_config *config, char *category, char *value)
{
	struct ast_variable *v;
	if (category) {
		v = ast_variable_browse(config, category);
		while (v) {
			if (value == v->name)
				return v->value;
			v=v->next;
		}
		v = ast_variable_browse(config, category);
		while (v) {
			if (!strcasecmp(value, v->name))
				return v->value;
			v=v->next;
		}
	} else {
		struct ast_category *cat;
		cat = config->root;
		while(cat) {
			v = cat->root;
			while (v) {
				if (!strcasecmp(value, v->name))
					return v->value;
				v=v->next;
			}
			cat = cat->next;
		}
	}
	return NULL;
}

#ifdef PRESERVE_COMMENTS
int ast_variable_delete(struct ast_config *cfg, char *category, char *variable, char *value)
{
	struct ast_variable *v, *pv, *bv, *bpv;
	struct ast_category *cat;
	cat = cfg->root;
	while(cat) {
		if (cat->name == category) {
			break;
		}
		cat = cat->next;
	}
	if (!cat) {
		cat = cfg->root;
		while(cat) {
			if (!strcasecmp(cat->name, category)) {
				break;
			}
			cat = cat->next;
		}
	}
	if (!cat)
		return -1;
	v = cat->root;
	pv = NULL;
	while (v) {
		if ((variable == v->name) && (!value || !strcmp(v->value, value)))
			break;
		pv = v;
		v=v->next;
	}
	if (!v) {
		/* Get the last one that looks like it */
		bv = NULL;
		bpv = NULL;
		v = cat->root;
		pv = NULL;
		while (v) {
			if (!strcasecmp(variable, v->name) && (!value || !strcmp(v->value, value))) {
				bv = v;
				bpv = pv;
			}
			pv = v;
			v=v->next;
		}
		v = bv;
	}

	if (v) {
		/* Unlink from original position */
		if (pv) 
			pv->next = v->next;
		else
			cat->root = v->next;
		v->next = NULL;
		free(v->name);
		if (v->value)
			free(v->value);
		free_comments(v->sameline);
		free_comments(v->precomments);
		return 0;
	}
	return -1;
}

int ast_category_delete(struct ast_config *cfg, char *category)
{
	struct ast_variable *v, *pv;
	struct ast_category *cat, *cprev;
	cat = cfg->root;
	cprev = NULL;
	while(cat) {
		if (cat->name == category) {
			break;
		}
		cprev = cat;
		cat = cat->next;
	}
	if (!cat) {
		cat = cfg->root;
		cprev = NULL;
		while(cat) {
			if (!strcasecmp(cat->name, category)) {
				break;
			}
			cprev = cat;
			cat = cat->next;
		}
	}
	if (!cat)
		return -1;
	/* Unlink it */
	if (cprev)
		cprev->next = cat->next;
	else
		cfg->root = cat->next;
	v = cat->root;
	while (v) {
		pv = v;
		v=v->next;
		if (pv->value)
			free(pv->value);
		if (pv->name)
			free(pv->name);
		free_comments(pv->sameline);
		free_comments(pv->precomments);
		free(pv);
	}
	free_comments(cat->sameline);
	free_comments(cat->precomments);
	free(cat);
	return 0;
}

struct ast_variable *ast_variable_append_modify(struct ast_config *config, char *category, char *variable, char *value, int newcat, int newvar, int move)
{
	struct ast_variable *v, *pv=NULL, *bv, *bpv;
	struct ast_category *cat, *pcat;
	cat = config->root;
	if (!newcat) {
		while(cat) {
			if (cat->name == category) {
				break;
			}
			cat = cat->next;
		}
		if (!cat) {
			cat = config->root;
			while(cat) {
				if (!strcasecmp(cat->name, category)) {
					break;
				}
				cat = cat->next;
			}
		}
	}
	if (!cat) {
		cat = malloc(sizeof(struct ast_category));
		if (!cat)
			return NULL;
		memset(cat, 0, sizeof(struct ast_category));
		strncpy(cat->name, category, sizeof(cat->name) - 1);
		if (config->root) {
			/* Put us at the end */
			pcat = config->root;
			while(pcat->next)
				pcat = pcat->next;
			pcat->next = cat;
		} else {
			/* We're the first one */
			config->root = cat;
		}
			
	}
	if (!newvar) {
		v = cat->root;
		pv = NULL;
		while (v) {
			if (variable == v->name)
				break;
			pv = v;
			v=v->next;
		}
		if (!v) {
			/* Get the last one that looks like it */
			bv = NULL;
			bpv = NULL;
			v = cat->root;
			pv = NULL;
			while (v) {
				if (!strcasecmp(variable, v->name)) {
					bv = v;
					bpv = pv;
				}
				pv = v;
				v=v->next;
			}
			v = bv;
		}
	} else v = NULL;
	if (v && move) {
		/* Unlink from original position */
		if (pv) 
			pv->next = v->next;
		else
			cat->root = v->next;
		v->next = NULL;
	}
	if (!v) {
		v = malloc(sizeof(struct ast_variable));
		if (!v)
			return NULL;
		memset(v, 0, sizeof(struct ast_variable));
		v->name = strdup(variable);
		move = 1;
	}
	if (v->value)
		free(v->value);
	if (value)
		v->value = strdup(value);
	else
		v->value = strdup("");
	if (move) {
		if (cat->root) {
			pv = cat->root;
			while (pv->next) 
				pv = pv->next;
			pv->next = v;
		} else {
			cat->root = v;
		}
	}
	return v;
}
#endif		

int ast_category_exist(struct ast_config *config, char *category_name)
{
	struct ast_category *category = NULL;

	category = config->root;

	while(category) {
		if (!strcasecmp(category->name,category_name)) 
			return 1;
		category = category->next;
	} 

	return 0;
}

#ifdef PRESERVE_COMMENTS
static struct ast_comment *build_comment(char *cmt)
{
	struct ast_comment *c;
	int len = strlen(cmt) + 1;
	c = malloc(sizeof(struct ast_comment) + len);
	if (c) {
		/* Memset the header */
		memset(c, 0, sizeof(struct ast_comment));
		/* Copy the rest */
		strcpy(c->cmt, cmt);
	}
	return c;
}
#endif

static struct ast_config *__ast_load(char *configfile, struct ast_config *tmp, struct ast_category **_tmpc, struct ast_variable **_last, int includelevel
#ifdef PRESERVE_COMMENTS
, struct ast_comment_struct *acs
#endif
);

static int cfg_process(struct ast_config *tmp, struct ast_category **_tmpc, struct ast_variable **_last, char *buf, int lineno, char *configfile, int includelevel 
#ifdef PRESERVE_COMMENTS
,struct ast_comment_struct *acs
#endif
)
{
	char *c;
	char *cur;
	char *arg=NULL;
	struct ast_config_reg *reg=NULL;
	struct ast_config *(*load_func)(char *, struct ast_config *,struct ast_category **,struct ast_variable **,int
#ifdef PRESERVE_COMMENTS
,struct ast_comment_struct *
#endif
	);
	struct ast_variable *v;
#ifdef PRESERVE_COMMENTS
	struct ast_comment *com = NULL;
#endif	
	int object;
	/* Strip off lines using ; as comment */
	c = strchr(buf, ';');
	while (c) {
		if ((c == buf) || (*(c-1) != '\\')) {
			*c = '\0';
#ifdef PRESERVE_COMMENTS
			c++;
			if (*c != '!')
				com = build_comment(c);
#endif			
		} else {
			*(c-1) = ';';
			memmove(c, c + 1, strlen(c + 1));
		}
		c = strchr(c + 1, ';');
	}
	cur = strip(buf);
	if (!ast_strlen_zero(cur)) {
		/* Actually parse the entry */
		if (cur[0] == '[') {
			/* A category header */
			c = strchr(cur, ']');
			if (c) {
				*c = 0;
				*_tmpc = malloc(sizeof(struct ast_category));
				if (!*_tmpc) {
					ast_destroy(tmp);
					ast_log(LOG_WARNING,
						"Out of memory, line %d\n", lineno);
					return -1;
				}
				memset(*_tmpc, 0, sizeof(struct ast_category));
				strncpy((*_tmpc)->name, cur+1, sizeof((*_tmpc)->name) - 1);
				(*_tmpc)->root =  NULL;
#ifdef PRESERVE_COMMENTS
				(*_tmpc)->precomments = acs->root;
				(*_tmpc)->sameline = com;
#endif				
				if (!tmp->prev)
					tmp->root = *_tmpc;
				else
					tmp->prev->next = *_tmpc;

				tmp->prev = *_tmpc;
#ifdef PRESERVE_COMMENTS
				acs->root = NULL;
				acs->prev = NULL;
#endif				
				*_last =  NULL;
			} else {
				ast_log(LOG_WARNING, 
					"parse error: no closing ']', line %d of %s\n", lineno, configfile);
			}
		} else if (cur[0] == '#') {
			/* A directive */
			cur++;
			c = cur;
			while(*c && (*c > 32)) c++;
			if (*c) {
				*c = '\0';
				c++;
				/* Find real argument */
				while(*c  && (*c < 33)) c++;
				if (!*c)
					c = NULL;
			} else 
				c = NULL;
			if (!strcasecmp(cur, "include")) {
				/* A #include */
				if (c) {
					while((*c == '<') || (*c == '>') || (*c == '\"')) c++;
					/* Get rid of leading mess */
					cur = c;
					while (!ast_strlen_zero(cur)) {
						c = cur + strlen(cur) - 1;
						if ((*c == '>') || (*c == '<') || (*c == '\"'))
							*c = '\0';
						else
							break;
					}
					
					if((c = strchr(cur,':'))) {
						*c = '\0';
						c++;
						arg = c;
					}
					
					if (includelevel < MAX_INCLUDE_LEVEL) {
						if(arg && cur) {
							load_func = NULL;
							if(ast_cust_config_list)
								reg = get_ast_cust_config(cur);
							if(reg && reg->func)
								load_func = reg->func;
							if(load_func) { 
								ast_log(LOG_NOTICE,"External Include '%s' via '%s' config engine\n",arg,cur);
								load_func(arg,tmp, _tmpc, _last, includelevel
#ifdef PRESERVE_COMMENTS
										  ,&acs
#endif
										  );
							}
							else 
								ast_log(LOG_WARNING,"Cant Find Registered Config Engine [%s] for [%s]\n",cur,arg);
						}
						else {
							__ast_load(cur, tmp, _tmpc, _last, includelevel + 1
#ifdef PRESERVE_COMMENTS
									   ,acs
#endif
									   );
						}
					} else
						ast_log(LOG_WARNING, "Maximum Include level (%d) exceeded\n", includelevel);
				} else
					ast_log(LOG_WARNING, "Directive '#include' needs an argument (filename) at line %d of %s\n", lineno, configfile);
				/* Strip off leading and trailing "'s and <>'s */
			}
			else 
				ast_log(LOG_WARNING, "Unknown directive '%s' at line %d of %s\n", cur, lineno, configfile);
		} else {
			/* Just a line (variable = value) */
			if (!*_tmpc) {
				ast_log(LOG_WARNING,
					"parse error: No category context for line %d of %s\n", lineno, configfile);
				ast_destroy(tmp);
				return -1;
			}
			c = strchr(cur, '=');
			if (c) {
				*c = 0;
				c++;
				/* Ignore > in => */
				if (*c== '>') {
					object = 1;
					c++;
				} else
					object = 0;
				v = malloc(sizeof(struct ast_variable));
				if (v) {
					memset(v, 0, sizeof(struct ast_variable));
					v->next = NULL;
					v->name = strdup(strip(cur));
					v->value = strdup(strip(c));
					v->lineno = lineno;
					v->object = object;
					/* Put and reset comments */
#ifdef PRESERVE_COMMENTS
					v->precomments = acs->root;
					v->sameline = com;
					acs->prev = NULL;
					acs->root = NULL;
#endif					
					v->blanklines = 0;
					if (*_last)
						(*_last)->next = v;
					else
						(*_tmpc)->root = v;
					*_last = v;
				} else {
					ast_destroy(tmp);
					ast_log(LOG_WARNING, "Out of memory, line %d\n", lineno);
					return -1;
				}
			} else {
				ast_log(LOG_WARNING, "No '=' (equal sign) in line %d of %s\n", lineno, configfile);
			}
														
		}
	} else {
		/* store any comments if there are any */
#ifdef PRESERVE_COMMENTS
		if (com) {
			if (acs->prev)
				acs->prev->next = com;
			else
				acs->root = com;
			acs->prev = com;
		} else {
			if (*_last) 
				(*_last)->blanklines++;
		}
#endif
	}
	return 0;
}

#ifdef PRESERVE_COMMENTS
static void dump_comments(FILE *f, struct ast_comment *comment)
{
	while (comment) {
		fprintf(f, ";%s", comment->cmt);
		comment = comment->next;
	}
}
#endif

int ast_save(char *configfile, struct ast_config *cfg, char *generator)
{
	FILE *f;
	char fn[256];
	char date[256]="";
	time_t t;
	struct ast_variable *var;
	struct ast_category *cat;
	int blanklines = 0;
	if (configfile[0] == '/') {
		strncpy(fn, configfile, sizeof(fn)-1);
	} else {
		snprintf(fn, sizeof(fn), "%s/%s", AST_CONFIG_DIR, configfile);
	}
	time(&t);
	strncpy(date, ctime(&t), sizeof(date) - 1);
	if ((f = fopen(fn, "w"))) {
		if ((option_verbose > 1) && !option_debug)
			ast_verbose(  VERBOSE_PREFIX_2 "Saving '%s': ", fn);
		fprintf(f, ";!\n");
		fprintf(f, ";! Automatically generated configuration file\n");
		fprintf(f, ";! Filename: %s (%s)\n", configfile, fn);
		fprintf(f, ";! Generator: %s\n", generator);
		fprintf(f, ";! Creation Date: %s", date);
		fprintf(f, ";!\n");
		cat = cfg->root;
		while(cat) {
#ifdef PRESERVE_COMMENTS
			/* Dump any precomments */
			dump_comments(f, cat->precomments);
#endif
			/* Dump section with any appropriate comment */
#ifdef PRESERVE_COMMENTS
			if (cat->sameline) 
				fprintf(f, "[%s]  ; %s\n", cat->name, cat->sameline->cmt);
			else
#endif
				fprintf(f, "[%s]\n", cat->name);
			var = cat->root;
			while(var) {
#ifdef PRESERVE_COMMENTS
				dump_comments(f, var->precomments);
#endif				
				if (var->sameline) 
					fprintf(f, "%s %s %s  ; %s\n", var->name, (var->object ? "=>" : "="), var->value, var->sameline->cmt);
				else	
					fprintf(f, "%s %s %s\n", var->name, (var->object ? "=>" : "="), var->value);
				if (var->blanklines) {
					blanklines = var->blanklines;
					while (blanklines) {
						fprintf(f, "\n");
						blanklines--;
					}
				}
					
				var = var->next;
			}
#if 0
			/* Put an empty line */
			fprintf(f, "\n");
#endif
			cat = cat->next;
		}
#ifdef PRESERVE_COMMENTS
		dump_comments(f, cfg->trailingcomments);
#endif		
	} else {
		if (option_debug)
			printf("Unable to open for writing: %s\n", fn);
		else if (option_verbose > 1)
			printf( "Unable to write (%s)", strerror(errno));
		return -1;
	}
	fclose(f);
	return 0;
}

static struct ast_config *__ast_load(char *configfile, struct ast_config *tmp, struct ast_category **_tmpc, struct ast_variable **_last, int includelevel
#ifdef PRESERVE_COMMENTS
, struct ast_comment_struct *acs
#endif
)
{
	char fn[256];
	char buf[8192];
	FILE *f;
	int lineno=0;
	int master=0;
	struct ast_config_reg *reg=NULL;
	struct ast_config *(*load_func)(char *, struct ast_config *,struct ast_category **,struct ast_variable **,int
#ifdef PRESERVE_COMMENTS
,struct ast_comment_struct *
#endif
);

	load_func=NULL;
	if (strcmp(configfile,config_conf_file) && strcmp(configfile,"asterisk.conf") && ast_cust_config_list) {
		if (global_load_func) {
			load_func = global_load_func;
		} else {
			reg = get_ast_cust_config_keyword(configfile);
			if (reg && reg->func) {
				load_func = reg->func;
			} else {
				reg = get_ast_cust_config_keyword("global");
				if (reg && reg->func)
					global_load_func = load_func = reg->func;
			}
		}

		if (load_func) {
			ast_log(LOG_NOTICE,"Loading Config %s via %s engine\n",configfile,reg && reg->name ? reg->name : "global");
			tmp = load_func(configfile,tmp, _tmpc, _last, includelevel
#ifdef PRESERVE_COMMENTS
,&acs
#endif
);
	    
			if (tmp)
				return tmp;
		}
	}

	if (configfile[0] == '/') {
		strncpy(fn, configfile, sizeof(fn)-1);
	} else {
		snprintf(fn, sizeof(fn), "%s/%s", (char *)ast_config_AST_CONFIG_DIR, configfile);
	}
	if ((option_verbose > 1) && !option_debug) {
		ast_verbose(  VERBOSE_PREFIX_2 "Parsing '%s': ", fn);
		fflush(stdout);
	}
	if ((f = fopen(fn, "r"))) {
		if (option_debug)
			ast_log(LOG_DEBUG, "Parsing %s\n", fn);
		else if (option_verbose > 1)
			ast_verbose( "Found\n");
		if (!tmp) {
			tmp = malloc(sizeof(struct ast_config));
			if (tmp)
				memset(tmp, 0, sizeof(struct ast_config));

			master = 1;
		}
		if (!tmp) {
			ast_log(LOG_WARNING, "Out of memory\n");
			fclose(f);
			return NULL;
		}
		while(!feof(f)) {
			lineno++;
			if (fgets(buf, sizeof(buf), f)) {
				if (cfg_process(tmp, _tmpc, _last, buf, lineno, configfile, includelevel
#ifdef PRESERVE_COMMENTS
				, acs
#endif
				)) {
					fclose(f);
					return NULL;
				}
			}
		}
		fclose(f);		
	} else {
		if (option_debug)
			ast_log(LOG_DEBUG, "No file to parse: %s\n", fn);
		else if (option_verbose > 1)
			ast_verbose( "Not found (%s)\n", strerror(errno));
	}
#ifdef PRESERVE_COMMENTS
	if (master) {
		/* Keep trailing comments */
		tmp->trailingcomments = acs->root;
		acs->root = NULL;
		acs->prev = NULL;
	}
#endif
	return tmp;
}

struct ast_config_reg *get_ast_cust_config_keyword(char *name) 
{
	struct ast_config_reg *reg,*ret=NULL;
	int x=0;
	ast_mutex_lock(&ast_cust_config_lock);
	for (reg=ast_cust_config_list;reg && !ret;reg=reg->next) {
		for (x=0;x<reg->keycount && !ret ;x++) {
			if (!strcmp(reg->keywords[x],name))
				ret=reg;
		}
	}
	ast_mutex_unlock(&ast_cust_config_lock);
	return ret;
}

struct ast_config_reg *get_ast_cust_config(char *name) 
{
	struct ast_config_reg *ptr=NULL;
	ast_mutex_lock(&ast_cust_config_lock);
	for (ptr=ast_cust_config_list;ptr;ptr=ptr->next) {
		if (!strcmp(name,ptr->name))
			break;
	}
	ast_mutex_unlock(&ast_cust_config_lock);
	return ptr;
}

void ast_config_destroy_all(void) 
{
	struct ast_config_reg *key;
	ast_mutex_lock(&ast_cust_config_lock);
	for (key=ast_cust_config_list;key;key=key->next) {
		ast_config_deregister(key);
	}
	ast_cust_config_list = NULL;
	ast_mutex_unlock(&ast_cust_config_lock);
}

struct ast_config_reg *get_config_registrations(void) 
{
	return ast_cust_config_list;
}

int ast_config_register(struct ast_config_reg *new) 
{
	struct ast_config_reg *ptr;
	ast_mutex_lock(&ast_cust_config_lock);
	new->keycount = 0;
	if (!ast_cust_config_list) {
		ast_cust_config_list = new;
	} else {
		for(ptr=ast_cust_config_list;ptr->next;ptr=ptr->next);
			ptr->next = new;
	}
	ast_mutex_unlock(&ast_cust_config_lock);
	ast_log(LOG_NOTICE,"Registered Config Engine %s\n",new->name);
	return 1;
}

int ast_config_deregister(struct ast_config_reg *del) 
{
	struct ast_config_reg *ptr=NULL,*last=NULL;
	ast_mutex_lock(&ast_cust_config_lock);
	for (ptr=ast_cust_config_list;ptr;ptr=ptr->next) {
		if (ptr == del) {
			if (last && ptr->next) {
				last->next = ptr->next;
			} else if (last && ! ptr->next) {
				last->next = NULL;
			} else if (!last && ptr->next) {
				ast_cust_config_list = ptr->next;
			} else if (!last && !ptr->next) {
				ast_cust_config_list = NULL;
			}
		}
		last = ptr;
	}
	ast_mutex_unlock(&ast_cust_config_lock);
	return 0;
}

int ast_cust_config_active(void) {
	return (ast_cust_config >0) ? 1 : 0;
}

struct ast_config *ast_load(char *configfile)
{
	struct ast_category *tmpc=NULL;
	struct ast_variable *last = NULL;


#ifdef PRESERVE_COMMENTS
	struct ast_comment_struct acs = { NULL, NULL };
#endif	


	return __ast_load(configfile, NULL, &tmpc, &last, 0 
#ifdef PRESERVE_COMMENTS
	,&acs
#endif
	);
}

char *ast_category_browse(struct ast_config *config, char *prev)
{	
	struct ast_category *cat;
	if (!prev) {
		if (config->root)
			return config->root->name;
		else
			return NULL;
	}
	cat = config->root;
	while(cat) {
		if (cat->name == prev) {
			if (cat->next)
				return cat->next->name;
			else
				return NULL;
		}
		cat = cat->next;
	}
	cat = config->root;
	while(cat) {
		if (!strcasecmp(cat->name, prev)) {
			if (cat->next)
				return cat->next->name;
			else
				return NULL;
		}
		cat = cat->next;
	}
	return NULL;
}


struct ast_config *ast_new_config(void) 
{
	struct ast_config *config;
	config = malloc(sizeof(struct ast_config));
	memset(config,0,sizeof(struct ast_config));
	return config;
}



struct ast_category *ast_new_category(char *name) 
{
	struct ast_category *category;
	category = malloc(sizeof(struct ast_category));
	if (category) {
		memset(category,0,sizeof(struct ast_category));
		strncpy(category->name,name,sizeof(category->name) - 1);
	}
	return category;
}


struct ast_variable *ast_new_variable(char *name, char *value) 
{
	struct ast_variable *variable;
	variable = malloc(sizeof(struct ast_variable));
	if (variable) {
		memset(variable,0,sizeof(struct ast_variable));
		variable->object=0;
		variable->name = malloc(strlen(name)+1);
		if (variable->name) {
			strcpy(variable->name,name);
			variable->value = malloc(strlen(value)+1);
			if (variable->value) {
				strcpy(variable->value,value);
			} else {
				free(variable->name);
				variable->name = NULL;
			}
		}
	}
	if (!variable->value) {
		free(variable);
		variable = NULL;
	}
		
	return variable;
}

int ast_cust_config_register(struct ast_config_reg *new) 
{
	ast_config_register(new);
	read_ast_cust_config();
	return 1;
}
int ast_cust_config_deregister(struct ast_config_reg *new) 
{
	ast_config_deregister(new);
	read_ast_cust_config();
	return 1;
}

static void clear_cust_keywords(void) 
{
	struct ast_config_reg *key;
	int x;
	ast_mutex_lock(&ast_cust_config_lock);
	for (key=get_config_registrations();key;key=key->next) {
		for (x=0;x<key->keycount;x++) {
			key->keywords[x][0] = '\0';
		}
		key->keycount=0;
	}
	ast_mutex_unlock(&ast_cust_config_lock);
}

static int config_command(int fd, int argc, char **argv) 
{
	struct ast_config_reg *key;
	int x;
	
	ast_cli(fd,"\n\n");
	ast_mutex_lock(&ast_cust_config_lock);
	for (key=get_config_registrations();key;key=key->next) {
		ast_cli(fd,"\nConfig Engine: %s\n",key->name);
		for (x=0;x<key->keycount;x++)
			ast_cli(fd,"===>%s\n",key->keywords[x]);
	}
	ast_mutex_unlock(&ast_cust_config_lock);
	ast_cli(fd,"\n\n");
	
	return 0;
}

static struct ast_cli_entry config_command_struct = {
  { "show","config","handles", NULL }, config_command,
  "Show Config Handles", NULL };

int register_config_cli() 
{
	return ast_cli_register(&config_command_struct);
}

int read_ast_cust_config(void) 
{
	char *cfg = config_conf_file;
	struct ast_config *config;
	struct ast_variable *v;
	struct ast_config_reg *ptr;
	struct ast_config_reg *test = NULL;

	clear_cust_keywords();
	config = ast_load(cfg);
	if (config) {
		for (v = ast_variable_browse(config,"settings");v;v=v->next) {
			
			ptr = get_ast_cust_config(v->value);
			if (ptr) {
				if (ptr->keycount >= CONFIG_KEYWORD_ARRAYLEN) {
					ast_log(LOG_WARNING,"Max Number of Bindings exceeded for %s->%s %d/%d\n",v->name,v->value,ptr->keycount,CONFIG_KEYWORD_ARRAYLEN);
				} else {
					if (strcmp(v->name,config_conf_file) && strcmp(v->name,"asterisk.conf")) {
						if (!(test = get_ast_cust_config_keyword(v->name))) {
							ast_log(LOG_NOTICE,"Binding: %s to %s\n",v->name,v->value);
							strncpy(ptr->keywords[ptr->keycount],v->name,sizeof(ptr->keywords[ptr->keycount]) - 1);
							ptr->keywords[ptr->keycount][sizeof(ptr->keywords[ptr->keycount])-1] = '\0';
							ptr->keycount++;
						}
					} else {
						ast_log(LOG_WARNING,"Cannot bind %s, Permission Denied\n",v->name);
					}
				}
			}
		}
		
		ast_destroy(config);
	}

	return 0;
}
