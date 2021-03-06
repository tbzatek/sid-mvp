/*
 * This file is part of SID.
 *
 * Copyright (C) 2001-2004 Sistina Software, Inc. All rights reserved.
 * Copyright (C) 2004-2018 Red Hat, Inc. All rights reserved.
 *
 * SID is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * SID is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SID.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mem.h"
#include "hash.h"

#include <stdlib.h>
#include <string.h>

struct hash_node {
	struct hash_node *next;
	void *data;
	unsigned data_len;
	unsigned keylen;
	char key[0];
};

struct hash_table {
	unsigned num_nodes;
	unsigned num_slots;
	struct hash_node **slots;
};

/* Permutation of the Integers 0 through 255 */
static unsigned char _nums[] = {
	1, 14, 110, 25, 97, 174, 132, 119, 138, 170, 125, 118, 27, 233, 140, 51,
	87, 197, 177, 107, 234, 169, 56, 68, 30, 7, 173, 73, 188, 40, 36, 65,
	49, 213, 104, 190, 57, 211, 148, 223, 48, 115, 15, 2, 67, 186, 210, 28,
	12, 181, 103, 70, 22, 58, 75, 78, 183, 167, 238, 157, 124, 147, 172,
	144,
	176, 161, 141, 86, 60, 66, 128, 83, 156, 241, 79, 46, 168, 198, 41, 254,
	178, 85, 253, 237, 250, 154, 133, 88, 35, 206, 95, 116, 252, 192, 54,
	221,
	102, 218, 255, 240, 82, 106, 158, 201, 61, 3, 89, 9, 42, 155, 159, 93,
	166, 80, 50, 34, 175, 195, 100, 99, 26, 150, 16, 145, 4, 33, 8, 189,
	121, 64, 77, 72, 208, 245, 130, 122, 143, 55, 105, 134, 29, 164, 185,
	194,
	193, 239, 101, 242, 5, 171, 126, 11, 74, 59, 137, 228, 108, 191, 232,
	139,
	6, 24, 81, 20, 127, 17, 91, 92, 251, 151, 225, 207, 21, 98, 113, 112,
	84, 226, 18, 214, 199, 187, 13, 32, 94, 220, 224, 212, 247, 204, 196,
	43,
	249, 236, 45, 244, 111, 182, 153, 136, 129, 90, 217, 202, 19, 165, 231,
	71,
	230, 142, 96, 227, 62, 179, 246, 114, 162, 53, 160, 215, 205, 180, 47,
	109,
	44, 38, 31, 149, 135, 0, 216, 52, 63, 23, 37, 69, 39, 117, 146, 184,
	163, 200, 222, 235, 248, 243, 219, 10, 152, 131, 123, 229, 203, 76, 120,
	209
};

static struct hash_node *_create_node(const char *str, unsigned len)
{
	struct hash_node *n = malloc(sizeof(*n) + len);

	if (n) {
		memcpy(n->key, str, len);
		n->keylen = len;
	}

	return n;
}

static unsigned long _hash(const char *str, unsigned len)
{
	unsigned long h = 0, g;
	unsigned i;

	for (i = 0; i < len; i++) {
		h <<= 4;
		h += _nums[(unsigned char) *str++];
		g = h & ((unsigned long) 0xf << 16u);
		if (g) {
			h ^= g >> 16u;
			h ^= g >> 5u;
		}
	}

	return h;
}

struct hash_table *hash_create(unsigned size_hint)
{
	size_t len;
	unsigned new_size = 16u;
	struct hash_table *hc = zalloc(sizeof(*hc));

	if (!hc)
		return NULL;

	/* round size hint up to a power of two */
	while (new_size < size_hint)
		new_size = new_size << 1;

	hc->num_slots = new_size;
	len = sizeof(*(hc->slots)) * new_size;
	if (!(hc->slots = zalloc(len)))
		goto bad;

	return hc;
bad:
	free(hc->slots);
	free(hc);
	return NULL;
}

static void _free_nodes(struct hash_table *t)
{
	struct hash_node *c, *n;
	unsigned i;

	for (i = 0; i < t->num_slots; i++)
		for (c = t->slots[i]; c; c = n) {
			n = c->next;
			free(c);
		}
}

void hash_destroy(struct hash_table *t)
{
	_free_nodes(t);
	free(t->slots);
	free(t);
}

static struct hash_node **_find(struct hash_table *t, const void *key, uint32_t len)
{
	unsigned h = _hash(key, len) & (t->num_slots - 1);
	struct hash_node **c;

	for (c = &t->slots[h]; *c; c = &((*c)->next)) {
		if ((*c)->keylen != len)
			continue;

		if (!memcmp(key, (*c)->key, len))
			break;
	}

	return c;
}

void *hash_lookup_binary(struct hash_table *t, const void *key, uint32_t len)
{
	struct hash_node **c = _find(t, key, len);

	return *c ? (*c)->data : NULL;
}

static int _do_hash_insert_binary(struct hash_table *t, struct hash_node **c, const void *key, uint32_t len, void *data)
{
	struct hash_node *n = _create_node(key, len);

	if (!n)
		return -1;

	n->data = data;
	n->next = 0;
	*c = n;
	t->num_nodes++;

	return 0;
}

int hash_insert_binary(struct hash_table *t, const void *key, uint32_t len, void *data)
{
	struct hash_node **c = _find(t, key, len);

	if (*c)
		(*c)->data = data;
	else
		return _do_hash_insert_binary(t, c, key, len, data);

	return 0;
}

void hash_remove_binary(struct hash_table *t, const void *key,
                        uint32_t len)
{
	struct hash_node **c = _find(t, key, len);

	if (*c) {
		struct hash_node *old = *c;
		*c = (*c)->next;
		free(old);
		t->num_nodes--;
	}
}

void *hash_lookup(struct hash_table *t, const char *key)
{
	return hash_lookup_binary(t, key, strlen(key) + 1);
}

int hash_insert(struct hash_table *t, const char *key, void *data)
{
	return hash_insert_binary(t, key, strlen(key) + 1, data);
}

void hash_remove(struct hash_table *t, const char *key)
{
	hash_remove_binary(t, key, strlen(key) + 1);
}

static struct hash_node **_find_str_with_val(struct hash_table *t, const void *key, const void *val, uint32_t len, uint32_t val_len)
{
	struct hash_node **c;
	unsigned h;

	h = _hash(key, len) & (t->num_slots - 1);

	for (c = &t->slots[h]; *c; c = &((*c)->next)) {
		if ((*c)->keylen != len)
			continue;

		if (!memcmp(key, (*c)->key, len) && (*c)->data) {
			if (((*c)->data_len == val_len) &&
			    !memcmp(val, (*c)->data, val_len))
				return c;
		}
	}

	return NULL;
}

int hash_insert_allow_multiple(struct hash_table *t, const char *key, const void *val, uint32_t val_len)
{
	struct hash_node *n;
	struct hash_node *first;
	int len = strlen(key) + 1;
	unsigned h;

	n = _create_node(key, len);
	if (!n)
		return -1;

	n->data = (void *)val;
	n->data_len = val_len;

	h = _hash(key, len) & (t->num_slots - 1);

	first = t->slots[h];

	if (first)
		n->next = first;
	else
		n->next = 0;
	t->slots[h] = n;

	t->num_nodes++;
	return 0;
}

/*
 * Look through multiple entries with the same key for one that has a
 * matching val and return that.  If none have maching val, return NULL.
 */
void *hash_lookup_with_val(struct hash_table *t, const char *key, const void *val, uint32_t val_len)
{
	struct hash_node **c;

	c = _find_str_with_val(t, key, val, strlen(key) + 1, val_len);

	return (c && *c) ? (*c)->data : NULL;
}

/*
 * Look through multiple entries with the same key for one that has a
 * matching val and remove that.
 */
void hash_remove_with_val(struct hash_table *t, const char *key, const void *val, uint32_t val_len)
{
	struct hash_node **c;

	c = _find_str_with_val(t, key, val, strlen(key) + 1, val_len);

	if (c && *c) {
		struct hash_node *old = *c;
		*c = (*c)->next;
		free(old);
		t->num_nodes--;
	}
}

/*
 * Look up the value for a key and count how many
 * entries have the same key.
 *
 * If no entries have key, return NULL and set count to 0.
 *
 * If one entry has the key, the function returns the val,
 * and sets count to 1.
 *
 * If N entries have the key, the function returns the val
 * from the first entry, and sets count to N.
 */
void *hash_lookup_with_count(struct hash_table *t, const char *key, int *count)
{
	struct hash_node **c;
	struct hash_node **c1 = NULL;
	uint32_t len = strlen(key) + 1;
	unsigned h;

	*count = 0;

	h = _hash(key, len) & (t->num_slots - 1);

	for (c = &t->slots[h]; *c; c = &((*c)->next)) {
		if ((*c)->keylen != len)
			continue;

		if (!memcmp(key, (*c)->key, len)) {
			(*count)++;
			if (!c1)
				c1 = c;
		}
	}

	if (!c1)
		return NULL;
	else
		return *c1 ? (*c1)->data : NULL;
}

unsigned hash_get_num_entries(struct hash_table *t)
{
	return t->num_nodes;
}

void hash_iter(struct hash_table *t, hash_iterate_fn f)
{
	struct hash_node *c, *n;
	unsigned i;

	for (i = 0; i < t->num_slots; i++)
		for (c = t->slots[i]; c; c = n) {
			n = c->next;
			f(c->data);
		}
}

void hash_wipe(struct hash_table *t)
{
	_free_nodes(t);
	memset(t->slots, 0, sizeof(struct hash_node *) * t->num_slots);
	t->num_nodes = 0u;
}

char *hash_get_key(struct hash_table *t __attribute__((unused)), struct hash_node *n)
{
	return n->key;
}

void *hash_get_data(struct hash_table *t __attribute__((unused)),
                    struct hash_node *n)
{
	return n->data;
}

static struct hash_node *_next_slot(struct hash_table *t, unsigned s)
{
	struct hash_node *c = NULL;
	unsigned i;

	for (i = s; i < t->num_slots && !c; i++)
		c = t->slots[i];

	return c;
}

struct hash_node *hash_get_first(struct hash_table *t)
{
	return _next_slot(t, 0);
}

struct hash_node *hash_get_next(struct hash_table *t, struct hash_node *n)
{
	unsigned h = _hash(n->key, n->keylen) & (t->num_slots - 1);

	return n->next ? n->next : _next_slot(t, h + 1);
}


/*
 * THE FUNCTIONS BELOW ARE EXTRA TO ORIGINAL CODE TAKEN FROM LVM2 SOURCETREE AND ITS dm_hash_table IMPLEMENTATION.
 */

int hash_update_binary(struct hash_table *t, const void *key, uint32_t len, void **data,
                       hash_update_fn_t hash_update_fn, void *hash_update_fn_arg)
{
	struct hash_node **c = _find(t, key, len);

	if (*c) {
		if (!hash_update_fn || hash_update_fn(key, len, (*c)->data, data, hash_update_fn_arg))
			(*c)->data = data ? *data : NULL;
		return 0;
	} else {
		if (!hash_update_fn || hash_update_fn(key, len, NULL, data, hash_update_fn_arg))
			return _do_hash_insert_binary(t, c, key, len, *data);
	}

	return 0;
}
