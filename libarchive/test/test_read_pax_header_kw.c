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

DEFINE_TEST(test_pax_kw_shily_xattr)
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
