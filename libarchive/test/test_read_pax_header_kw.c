/*-
 * Copyright (c) 2017 Versity Software, Inc.
 * Copyright (c) 2003-2007 Tim Kientzle
 *
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
 *
 * This test case's code has been derived from test_entry.c
 */

#include "test.h"
// /app/bin/libarchive_test -vv -r /app/libarchive/test test_write_pax_kw

static char buff2[64];

/* TODO:
 * add write support, with flag from set options
 * test write support
 * test read and write from python
 * test read/write from Panorama
 * fork repo(s)
 * push commits
 * RFC PRs
 * reviews from Versity team
 * push upstream - libarchive first
 *  on accept, python-libarchive-c
 */

DEFINE_TEST(test_read_pax_kw)
{
	struct archive *a;
	struct archive_entry *ae;
	const char *refname = "test_read_pax_schily_xattr.tar";
	const char *key;
	const void *value;
	size_t value_len;

	assert((a = archive_read_new()) != NULL);
	assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
	assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));

	extract_reference_file(refname);
	assertEqualIntA(a, ARCHIVE_OK,
	    archive_read_open_filename(a, refname, 10240));

	assertEqualInt(ARCHIVE_OK, archive_read_next_header(a, &ae));

	/* make sure there are the xattr entries we expect first */
	assertEqualInt(2, archive_entry_xattr_count(ae));
	assertEqualInt(2, archive_entry_xattr_reset(ae));

	/* start checking generic pax headers - should be 4:
	 * two xattr, then ctime & atime for generic PAX header bits */
	assertEqualInt(4, archive_entry_pax_kw_count(ae));
	assertEqualInt(4, archive_entry_pax_kw_reset(ae));

	/* not checking values, they are binary and checked in other tests already */
	assertEqualInt(0, archive_entry_pax_kw_next(ae, &key, &value, &value_len));
	assertEqualString(key, "SCHILY.xattr.security.selinux");

	assertEqualInt(0, archive_entry_pax_kw_next(ae, &key, &value, &value_len));
	assertEqualString(key, "SCHILY.xattr.security.ima");

	assertEqualInt(0, archive_entry_pax_kw_next(ae, &key, &value, &value_len));
	assertEqualString(key, "ctime");

	assertEqualInt(0, archive_entry_pax_kw_next(ae, &key, &value, &value_len));
	assertEqualString(key, "atime");

	/* Close the archive. */
	assertEqualInt(ARCHIVE_OK, archive_read_close(a));
	assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}

DEFINE_TEST(test_write_pax_kw)
{
	size_t buffsize = 1000000;
	char *buff;
	struct archive_entry *ae;
	struct archive *a;
	size_t used;
	const char *kw_name = "VENDOR.item";
	const char *kw_value = "avalue";
	const char *key;
	const void *value;
	size_t value_len;

	buff = malloc(buffsize); /* million bytes of work area */
	assert(buff != NULL);

	/* Create a new archive in memory. */
	assert((a = archive_write_new()) != NULL);
	assertA(0 == archive_write_set_format_pax(a));
	assertA(0 == archive_write_set_options(a, "pax:generic_kw=1"));
	assertA(0 == archive_write_add_filter_none(a));
	assertA(0 == archive_write_open_memory(a, buff, buffsize, &used));

	/*
	 * "file" has a bunch of attributes and 8 bytes of data.
	 */
	assert((ae = archive_entry_new()) != NULL);
	archive_entry_set_atime(ae, 2, 20);
	archive_entry_set_birthtime(ae, 3, 30);
	archive_entry_set_ctime(ae, 4, 40);
	archive_entry_set_mtime(ae, 5, 50);
	archive_entry_copy_pathname(ae, "file");
	archive_entry_set_mode(ae, S_IFREG | 0755);
	archive_entry_set_size(ae, 8);
	archive_entry_pax_kw_add_entry(ae, kw_name, kw_value, strlen(kw_value) + 1);
	assertEqualIntA(a, ARCHIVE_OK, archive_write_header(a, ae));
	archive_entry_free(ae);
	assertEqualIntA(a, 8, archive_write_data(a, "12345678", 9));

	/* Close out the archive. */
	assertEqualIntA(a, ARCHIVE_OK, archive_write_close(a));
	assertEqualIntA(a, ARCHIVE_OK, archive_write_free(a));

	/*
	 *
	 * Now, read the data back.
	 *
	 */
	assert((a = archive_read_new()) != NULL);
	assertEqualIntA(a, 0, archive_read_support_format_all(a));
	assertEqualIntA(a, 0, archive_read_support_filter_all(a));
	assertEqualIntA(a, 0, archive_read_open_memory(a, buff, used));

	/*
	 * Read "file"
	 */
	assertEqualIntA(a, 0, archive_read_next_header(a, &ae));
	assertEqualInt(2, archive_entry_atime(ae));
	assertEqualInt(20, archive_entry_atime_nsec(ae));
	assertEqualInt(3, archive_entry_birthtime(ae));
	assertEqualInt(30, archive_entry_birthtime_nsec(ae));
	assertEqualInt(4, archive_entry_ctime(ae));
	assertEqualInt(40, archive_entry_ctime_nsec(ae));
	assertEqualInt(5, archive_entry_mtime(ae));
	assertEqualInt(50, archive_entry_mtime_nsec(ae));
	assertEqualString("file", archive_entry_pathname(ae));
	assert((S_IFREG | 0755) == archive_entry_mode(ae));
	assertEqualInt(8, archive_entry_size(ae));

	assertEqualInt(5, archive_entry_pax_kw_count(ae));
	assertEqualInt(5, archive_entry_pax_kw_reset(ae));

	/* Validate our attribute of interest -- it got recorded first */
	assertEqualInt(0, archive_entry_pax_kw_next(ae, &key, &value, &value_len));
	assertEqualString(key, kw_name);
	assertEqualString(value, kw_value);
	/* API is binary, add null termination */
	assertEqualInt((int)value_len, strlen(value) + 1);

	/* Validate names are still OK, above validated any values we needed to check. */
	assertEqualInt(0, archive_entry_pax_kw_next(ae, &key, &value, &value_len));
	assertEqualString(key, "mtime");

	assertEqualInt(0, archive_entry_pax_kw_next(ae, &key, &value, &value_len));
	assertEqualString(key, "LIBARCHIVE.creationtime");

	assertEqualInt(0, archive_entry_pax_kw_next(ae, &key, &value, &value_len));
	assertEqualString(key, "atime");

	assertEqualInt(0, archive_entry_pax_kw_next(ae, &key, &value, &value_len));
	assertEqualString(key, "ctime");

	assertEqualIntA(a, 8, archive_read_data(a, buff2, 10));
	assertEqualMem(buff2, "12345678", 8);

	/*
	 * Verify the end of the archive.
	 */
	assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
	assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
	assertEqualIntA(a, ARCHIVE_OK, archive_read_free(a));

	free(buff);
}
