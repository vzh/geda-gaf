/* gEDA - GPL Electronic Design Automation
 * gschem - gEDA Schematic Capture
 * Copyright (C) 2015 gEDA Contributors (see ChangeLog for details)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* This file hosts the back annotation code; it is handling patches
coming from external tools. */

/* TODO: run indent(1) to get indentation compatible with geda policy */

#include "gschem.h"

static void patch_list_free(GList *list);

#define error(...) \
do { \
	fprintf(stderr, __VA_ARGS__); \
	fprintf(stderr, " in line %d\n", lineno); \
	patch_list_free(st->lines); \
	free(word); \
	return -1; \
} while(0)


#define append(c) \
do { \
	if (used >= alloced) { \
		alloced += 64; \
		word = realloc(word, alloced); \
	} \
	word[used] = c; \
	used++; \
} while(0)

#define reset_current() \
do { \
	used = 0; \
	memset(&current, 0, sizeof(current)); \
} while(0)

#define reset_word() \
do { \
	used = 0; \
} while(0)

#define restart(c) \
do { \
	reset_word(); \
	append(c); \
} while(0)

#define END_OP() \
do { \
	append('\0'); \
	if (strcmp(word, "add_conn") == 0) current.op = GSCHEM_PATCH_ADD_CONN; \
	else if (strcmp(word, "del_conn") == 0) current.op = GSCHEM_PATCH_DEL_CONN; \
	else if (strcmp(word, "change_attrib") == 0) current.op = GSCHEM_PATCH_CHANGE_ATTRIB; \
	else if (strcmp(word, "net_info") == 0) current.op = GSCHEM_PATCH_NET_INFO; \
	else \
		error("Syntax error: unknown opcode %s\n", word); \
	reset_word(); \
} while(0)

#define END_STR() \
do { \
	append('\0'); \
	if (*word != '\0') {\
		switch(current.op) { \
			case GSCHEM_PATCH_DEL_CONN: \
			case GSCHEM_PATCH_ADD_CONN: \
				if (current.id == NULL) current.id = strdup(word); \
				else if (current.arg1.net_name == NULL) current.arg1.net_name = strdup(word); \
				else error("Need two arguments for the connection: netname and pinname"); \
				break; \
			case GSCHEM_PATCH_CHANGE_ATTRIB: \
				if (current.id == NULL) current.id = strdup(word); \
				else if (current.arg1.attrib_name == NULL) current.arg1.attrib_name = strdup(word); \
				else if (current.arg2.attrib_val == NULL) current.arg2.attrib_val = strdup(word); \
				else error("Need three arguments for an attrib change: id attr_name attr_val"); \
				break; \
			case GSCHEM_PATCH_NET_INFO: \
				if (current.id == NULL) current.id = strdup(word); \
				else current.arg1.ids = g_list_prepend(current.arg1.ids, strdup(word)); \
		} \
	} \
	reset_word(); \
} while(0)

#define require(s) \
do { \
	if (s == NULL) \
		error("Not enough arguments"); \
} while(0)

#define END_LINE() \
do { \
	gschem_patch_line_t *n; \
	switch(current.op) { \
		case GSCHEM_PATCH_DEL_CONN: \
		case GSCHEM_PATCH_ADD_CONN: \
			require(current.id); \
			require(current.arg1.net_name); \
			break; \
		case GSCHEM_PATCH_CHANGE_ATTRIB: \
			require(current.id); \
			require(current.arg1.attrib_name); \
			require(current.arg2.attrib_val); \
			break; \
		case GSCHEM_PATCH_NET_INFO: \
			require(current.id); \
			break; \
	} \
	n = malloc(sizeof(gschem_patch_line_t)); \
	memcpy(n, &current, sizeof(gschem_patch_line_t)); \
	st->lines = g_list_prepend(st->lines, n); \
	reset_current(); \
} while(0)

static int patch_parse(gschem_patch_state_t *st, FILE *f)
{
	char *word = NULL;
	int alloced = 0, used;
	int c, lineno;
	gschem_patch_line_t current;
	enum {
		ST_INIT,
		ST_COMMENT,
		ST_OP,
		ST_PRE_STR,
		ST_STR
	} state = ST_INIT;

	st->lines = NULL;
	lineno = 1;
	reset_current();
	do {
		c = fgetc(f);
		switch(state) {
			case ST_INIT:
				switch(c) {
					case '#': state = ST_COMMENT; break;
					case '\r':
					case '\n':
					case EOF:
					case ' ':
					case '\t':
						break;
					default:
						restart(c);
						state = ST_OP;
				}
				break;
			case ST_OP:
				switch(c) {
					case '#': END_OP(); END_LINE(); state = ST_COMMENT; break;
					case ' ':
					case '\t':
						END_OP();
						state = ST_PRE_STR;
						break;
					case '\r':
					case '\n':
					case EOF:
						END_OP();
						END_LINE();
						state = ST_INIT;
						break;
					default:
						append(c);
						break;
				}
				break;
			case ST_PRE_STR:
				switch(c) {
					case '#': END_LINE(); state = ST_COMMENT; break;
					case ' ':
					case '\t':
						break;
					case '\r':
					case '\n':
					case EOF:
						END_LINE();
						state = ST_INIT;
						break;
					default:
						restart(c);
						state = ST_STR;
						break;
				}
				break;
			case ST_STR:
				switch(c) {
					case '#': END_STR(); END_LINE(); state = ST_COMMENT; break;
					case ' ':
					case '\t':
						END_STR();
						state = ST_PRE_STR;
						break;
					case '\r':
					case '\n':
					case EOF:
						END_STR();
						END_LINE();
						state = ST_INIT;
						break;
					default:
						append(c);
						break;
				}
				break;
			case ST_COMMENT:
				switch(c) {
					case '\r':
					case '\n':
					case EOF:
						state = ST_INIT;
						break;
				}
				break;
		}
		if (c == '\n')
			lineno++;
	} while(c != EOF);
	st->lines = g_list_reverse(st->lines);
	return 0;
}

static void patch_list_free(GList *list)
{
	
}

static char *op_names[] = {
	"disconnect",
	"connect",
	"chnage attribute",
	"net_info"
};


#if DEBUG
static void patch_list_print(gschem_patch_state_t *st)
{
	GList *i;
	for (i = st->lines; i != NULL; i = g_list_next (i)) {
		gschem_patch_line_t *l = i->data;
		GList *p;
		if (l == NULL) {
			fprintf(stderr, "NULL data on list\n");
			continue;
		}
		switch(l->op) {
			case GSCHEM_PATCH_DEL_CONN:
			case GSCHEM_PATCH_ADD_CONN:
				fprintf(stderr, "%s %s %s\n", op_names[l->op], l->id, l->arg1.net_name);
				break;
			case GSCHEM_PATCH_CHANGE_ATTRIB:
				fprintf(stderr, "%s %s %s=%s\n", op_names[l->op], l->id, l->arg1.attrib_name, l->arg2.attrib_val);
				break;
			case GSCHEM_PATCH_NET_INFO:
				fprintf(stderr, "%s %s", op_names[l->op], l->id);
				for (p = l->arg1.ids; p != NULL; p = g_list_next (p))
					fprintf(stderr, " %s", p->data);
				fprintf(stderr, "\n");
				break;
		}
	}
}
#endif

int gschem_patch_state_init(gschem_patch_state_t *st, const char *fn)
{
	FILE *f;
	int res;

	f = fopen(fn, "r");
	if (f == NULL)
		return -1;

	res = patch_parse(st, f);

#if DEBUG
	patch_list_print(st);
#endif

	if (res == 0) {
		GList *i;

		/* Create hashes for faster lookups avoiding O(objects*patches) */
		st->pins  = g_hash_table_new (g_str_hash, g_str_equal);
		st->comps = g_hash_table_new (g_str_hash, g_str_equal);
		st->nets  = g_hash_table_new (g_str_hash, g_str_equal);
		for (i = st->lines; i != NULL; i = g_list_next (i)) {
			gschem_patch_line_t *l = i->data;
			if (l->op == GSCHEM_PATCH_NET_INFO)
				g_hash_table_insert(st->nets, l->id, l->arg1.ids);
		}
	}

	fclose(f);
	return res;
}

/* insert obj in a hash table of slists */
static void build_insert_hash_list(GHashTable *hash, char *full_name, OBJECT *obj)
{
	GSList *lst;
	
	lst = g_hash_table_lookup(hash, full_name);
	if (lst != NULL) /* key already exists, the new one won't end up in the hash and won't get free'd on hash destroy */
		g_free(full_name);
	lst = g_slist_prepend(lst, obj);
	g_hash_table_insert(hash, full_name, lst);
}

int gschem_patch_state_build(gschem_patch_state_t *st, OBJECT *o)
{
	GList *i;
	gchar *refdes, *pin;
	int refdes_len, pin_len;

	switch(o->type) {
		case OBJ_COMPLEX:
			refdes = o_attrib_search_object_attribs_by_name (o, "refdes", 0);
			if (refdes == NULL)
				break;

			build_insert_hash_list(st->comps, g_strdup(refdes), o);

			refdes_len = strlen(refdes);
			for(i = o->complex->prim_objs; i != NULL; i = g_list_next(i)) {
				OBJECT *sub = i->data;
				switch(sub->type) {
					case OBJ_PIN:
						pin = o_attrib_search_object_attribs_by_name (sub, "pinnumber", 0);
						if (pin != NULL) {
							char *full_name;
							pin_len = strlen(pin);
							full_name = g_malloc(refdes_len + pin_len + 2);
							sprintf(full_name, "%s-%s", refdes, pin);
/*						printf("add: '%s' -> '%p' o=%p at=%p p=%p\n", full_name, sub, o, sub->attached_to, sub->parent);
						fflush(stdout);*/
							build_insert_hash_list(st->pins, full_name, sub);
							g_free(pin);
						}
						break;
				}
			}
			g_free(refdes);
			break;

		case OBJ_NET: /* what to do with nets? */
/*			printf("type: '%c'\n", o->type);*/
			break;

		/* ignore floating pins */
		case OBJ_PIN:
			break;

		/* ignore all graphical objects */
		case OBJ_TEXT:
		case OBJ_LINE:
		case OBJ_PATH:
		case OBJ_BOX:
		case OBJ_CIRCLE:
		case OBJ_PICTURE:
		case OBJ_BUS:
		case OBJ_ARC:
			break;
	}
	return 0;
}

static gboolean free_key(gpointer key, gpointer value, gpointer user_data)
{
	free(key);
	return TRUE;
}

static gboolean free_key_list(gpointer key, gpointer value, gpointer user_data)
{
	GSList *lst = value;
	g_slist_free(lst);
	g_free(key);
	return TRUE;
}

void gschem_patch_state_destroy(gschem_patch_state_t *st)
{
	g_hash_table_foreach_remove(st->pins, free_key_list, NULL);
	g_hash_table_foreach_remove(st->comps, free_key_list, NULL);
	g_hash_table_destroy(st->nets);
	g_hash_table_destroy(st->pins);
	g_hash_table_destroy(st->comps);
	patch_list_free(st->lines);
}

static GSList *add_hit(GSList *hits, OBJECT *obj, char *text)
{
	gschem_patch_hit_t *hit;
	hit = calloc(sizeof(gschem_patch_hit_t), 1);
	hit->object = obj;
	hit->text = text;
	return g_slist_prepend(hits, hit);
}

/*! \brief get a list of all objects connected to this one (recursively)
 *
 *  \par Function Description
 *  This function gets an open list of objects to be checked and maps all connections
 *  of all objects on the list. The resulting new open list is empty, while the
 *  found hash is non-empty. For each new object on the found hash, the value
 *  is determined by calling the user provided hashval() callback.
 *
 *  \param [in/out] found    (OBJECT*) -> (value) hash of all objects found
 *  \param [in] open         GList of OBJECT's to start the sarch from
 *  \param [in] hashval()    a callback that generates the value of the object; all object values are NULL if hashval() is NULL
 *  \param [in] user_ctx     user context pointer for hashval()
 *  \return the new open list (empty list)
 *
 *  \warning
 *  Caller must g_list_free returned GList pointer.
 *  Also free the found hash.
 */
static GList *s_conn_find_all(GHashTable *found, GList *open, void *(*hashval)(void *user_ctx, OBJECT *o), void *user_ctx)
{
	GList *i;

	/* iterate by consuming the first element of the list */
	for(i = open; i != NULL; i = open) {
		OBJECT *o = i->data;

		open = g_list_remove(open, o);

		if (g_hash_table_lookup(found, o) == NULL) { /* ... check if it's not yet found */
			void *val;
			if (hashval != NULL)
				val = hashval(user_ctx, o);
			else
				val = NULL;
			g_hash_table_insert(found, o, val);
			open = s_conn_return_others(open, o);
		}
	}
	return open;
}

/* return the name of the object and add relevant objects to a name->obj hash in user_ctx */
static void *exec_check_conn_hashval(void *user_ctx, OBJECT *o)
{
	gchar *name = NULL, *tmp;
	GHashTable *name2obj = user_ctx;

	switch(o->type) {
		case OBJ_NET:
			tmp = o_attrib_search_object_attribs_by_name (o, "netname", 0);
			if (tmp != NULL) {
				int len = strlen(tmp);
				name = g_malloc(len+2);
				*name = OBJ_NET;
				memcpy(name+1, tmp, len+1);
				g_free(tmp);
				g_hash_table_insert(name2obj, name, o);
			}
			else
				name = " "; /* anon net segments are not interesting at all; should be a static string as it doesn't end up on name2obj where we free these strings */
			break;
		case OBJ_PIN: 
			if (o->parent != NULL) {
				gchar *oname, *pname;

				oname = o_attrib_search_object_attribs_by_name (o->parent, "refdes", 0);
				pname = o_attrib_search_object_attribs_by_name (o, "pinnumber", 0);
				name = g_malloc(strlen(oname) + strlen(pname) + 3);
				sprintf(name, "%c%s-%s", OBJ_PIN, (char *)oname, (char *)pname);
				g_free(oname);
				g_free(pname);
				g_hash_table_insert(name2obj, name, o);
			}
	}
	return name;
}

/* Build a name->object hash of everything connected to a pin */
static GHashTable *exec_list_conns(OBJECT *pin)
{
	GHashTable *found, *connections;
	GList *open = NULL;

	connections = g_hash_table_new(g_str_hash, g_str_equal);
	found = g_hash_table_new(g_direct_hash, NULL);
	open = g_list_prepend(open, pin);
	open = s_conn_find_all(found, open, exec_check_conn_hashval, connections);

	g_hash_table_destroy(found);
	g_list_free(open);
	return connections;
}

static void exec_free_conns(GHashTable *connections)
{
	g_hash_table_foreach_remove(connections, free_key, NULL);
	g_hash_table_destroy(connections);
}

#define DEBUG
#ifdef DEBUG
static void exec_print_conns(GHashTable *connections)
{
	gpointer key, val;
	GHashTableIter cni;

	for(g_hash_table_iter_init(&cni, connections); g_hash_table_iter_next(&cni, &key, &val);) {
		printf(" cn=%s %p\n", (char *)key, val);
	}
}
#endif

#define enlarge(to) \
do { \
	if (to > alloced) { \
		alloced = to+256; \
		free(buff); \
		buff = malloc(to); \
	} \
} while(0)
static GSList *exec_check_conn(GSList *diffs, gschem_patch_line_t *patch, OBJECT *pin, GList *net, int del)
{
	GList *np;
	GHashTable *connections;
	int len, pin_hdr;
	char *buff = NULL;
	int alloced = 0;
	GString *msg = NULL;

	printf("exec %d:\n", del);

	connections = exec_list_conns(pin);
	exec_print_conns(connections);

	/* check if we are connected to the network */
	len = strlen(patch->arg1.net_name);
	enlarge(len+2);
	*buff = OBJ_NET;
	memcpy(buff+1, patch->arg1.net_name, len+1);
	if (g_hash_table_lookup(connections, buff) != NULL) {
		if (del) {
			msg = g_string_new(": disconnect from net ");
			g_string_append(msg, buff+1);
		}
	}
	else {
		if (!del) {
			msg = g_string_new(": connect to net ");
			g_string_append(msg, buff+1);
		}
	}

	/* check if we still have a connection to any of the pins */
	pin_hdr = 0;
	for(np = net; np != NULL; np = g_list_next(np)) {
		const char *action = NULL;
		OBJECT *target;
		len = strlen(np->data);
		enlarge(len+2);
		*buff = OBJ_PIN;
		memcpy(buff+1, np->data, len+1);
		target = g_hash_table_lookup(connections, buff);
		if (target == pin)
			continue;
		if (target != NULL) {
			if (del)
				action = "disconnect from pin ";
		}
		else {
			if (!del)
				action = "connect to pin ";
		}
		if (action != NULL) {
			if (!pin_hdr) {
				if (msg == NULL)
					msg = g_string_new(": ");
				else
					g_string_append(msg, "; ");
				g_string_append(msg, action);
				pin_hdr = 1;
			}
			else
				g_string_append(msg, ", ");
			g_string_append(msg, buff+1);
		}
	}

	if (buff != NULL)
		free(buff);
	exec_free_conns(connections);

	if (msg != NULL) {
		g_string_prepend(msg, patch->id);
		return add_hit(diffs, pin->parent, g_string_free(msg, FALSE));
	}

	return diffs;
}
#undef enlarge

static GSList *exec_check_attrib(GSList *diffs, gschem_patch_line_t *patch, OBJECT *comp)
{
	gchar *attr_val;
	attr_val = o_attrib_search_object_attribs_by_name (comp, patch->arg1.attrib_name, 0);
	if (attr_val == NULL)
		return diffs;
	if (strcmp(attr_val, patch->arg2.attrib_val) != 0) {
		gchar *msg = g_strdup_printf("%s: change attribute %s from %s to %s", patch->id, patch->arg1.attrib_name, attr_val, patch->arg2.attrib_val);
		diffs = add_hit(diffs, comp, msg);
	}
	g_free(attr_val);
	return diffs;
}


GSList *gschem_patch_state_execute(gschem_patch_state_t *st, GSList *diffs)
{
	GList *i, *net;
	GSList *pins, *comps;
	int found;

	for (i = st->lines; i != NULL; i = g_list_next (i)) {
		gschem_patch_line_t *l = i->data;
		if (l == NULL) {
			fprintf(stderr, "NULL data on list\n");
			continue;
		}
		switch(l->op) {
			case GSCHEM_PATCH_DEL_CONN:
			case GSCHEM_PATCH_ADD_CONN:
				pins = g_hash_table_lookup(st->pins, l->id);
				if (pins == NULL) {
					fprintf(stderr, "Patch references to non-existing pin %s\n", l->id);
					break;
				}
				net = g_hash_table_lookup(st->nets, l->arg1.net_name);
				if (net == NULL) {
					fprintf(stderr, "NULL net\n");
					break;
				}
				for(;pins != NULL; pins = g_slist_next(pins))
					diffs = exec_check_conn(diffs, l, (OBJECT *)pins->data, net, (l->op == GSCHEM_PATCH_DEL_CONN));
				break;
			case GSCHEM_PATCH_CHANGE_ATTRIB:
				comps = g_hash_table_lookup(st->comps, l->id);
				for(found = 0;comps != NULL; comps = g_slist_next(comps)) {
					diffs = exec_check_attrib(diffs, l, (OBJECT *)comps->data);
					found++;
				}
				if (found == NULL) {
					gchar *msg = g_strdup_printf("%s (NOT FOUND): change attribute %s to %s", l->id, l->arg1.attrib_name, l->arg2.attrib_val);
					diffs = add_hit(diffs, NULL, msg);
				}
				break;
			case GSCHEM_PATCH_NET_INFO:
				/* just ignore them, we've already built data structs while parsing */
				break;
		}
	}
	return diffs;
}
