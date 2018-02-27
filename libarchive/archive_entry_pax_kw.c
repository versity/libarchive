/*-
 * Copyright (c) 2017 Versity Software, Inc.
 * Copyright (c) 2003-2007 Tim Kientzle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "archive_platform.h"
__FBSDID("$FreeBSD$");

#include <stddef.h>
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_WCHAR_H
#include <wchar.h>
#endif

#include "archive.h"
#include "archive_entry.h"
#include "archive_private.h"
#include "archive_entry_private.h"

/*
 * PAX arbitrary key=value attribute handling
 */

void
archive_entry_pax_kw_clear(struct archive_entry *entry)
{
	struct ae_pax_kw	*pk;

	while (entry->pax_kw_head != NULL) {
		pk = entry->pax_kw_head->next;
		free(entry->pax_kw_head->name);
		free(entry->pax_kw_head->value);
		free(entry->pax_kw_head);
		entry->pax_kw_head = pk;
	}

	entry->pax_kw_head = NULL;
}

void
archive_entry_pax_kw_add_entry(struct archive_entry *entry,
	const char *name, const void *value, size_t value_len)
{
	struct ae_pax_kw	*pk;

	if ((pk = (struct ae_pax_kw *)malloc(sizeof(struct ae_pax_kw))) == NULL)
		__archive_errx(1, "Out of memory");

	if ((pk->name = strdup(name)) == NULL)
		__archive_errx(1, "Out of memory");

	if ((pk->value = malloc(value_len)) != NULL) {
		memcpy(pk->value, value, value_len);
		pk->value_len = value_len;
	} else
		pk->value_len = 0;

	pk->next = entry->pax_kw_head;
	entry->pax_kw_head = pk;
}


/*
 * returns number of the extended attribute entries
 */
int
archive_entry_pax_kw_count(struct archive_entry *entry)
{
	struct ae_pax_kw *pk;
	int count = 0;

	for (pk = entry->pax_kw_head; pk != NULL; pk = pk->next)
		count++;

	return count;
}

int
archive_entry_pax_kw_reset(struct archive_entry * entry)
{
	entry->pax_kw_p = entry->pax_kw_head;

	return archive_entry_pax_kw_count(entry);
}

int
archive_entry_pax_kw_next(struct archive_entry * entry,
	const char **name, const void **value, size_t *value_len)
{
	if (entry->pax_kw_p) {
		*name = entry->pax_kw_p->name;
		*value = entry->pax_kw_p->value;
		*value_len = entry->pax_kw_p->value_len;

		entry->pax_kw_p = entry->pax_kw_p->next;

		return (ARCHIVE_OK);
	} else {
		*name = NULL;
		*value = NULL;
		*value_len = (size_t)0;
		return (ARCHIVE_WARN);
	}
}

/*
 * end of pax_kw handling
 */
