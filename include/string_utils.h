/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *  Copyright (C) 2020-2020  The dosbox-staging team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef DOSBOX_STRING_UTILS_H
#define DOSBOX_STRING_UTILS_H

#include <cassert>
#include <cstdlib>
#include <cstdio>

/* Copy a string into C array (when buffer size is known at compile time).
 *
 * This function copies string pointed by src to a fixed-size buffer dst.
 * At most N bytes from src are copied, where N is size of dst. If exactly
 * N bytes are copied, then terminating null byte is put into dst, thus
 * buffer overrun is prevented.
 *
 * Function returns pointer to buffer to be compatible with strcpy.
 *
 * This is a safer drop-in replacement for strcpy function (when used to fill
 * buffers, whose size is known at compilation time), however some caveats
 * still apply:
 *
 * - src cannot be null, otherwise the behaviour is undefined
 * - dst and src strings must not overlap, otherwise the behaviour is undefined
 * - src string must be null-terminated, otherwise the behaviour is undefined
 *
 * Usage:
 *
 *     char buffer[2];
 *     safe_strcpy(buffer, "abc");
 *     // buffer is filled with "a"
 */
template <size_t N>
char *safe_strcpy(char (&dst)[N], const char *src) noexcept
{
	assert(src != nullptr);
	assert(src < &dst[0] || src > &dst[N - 1]);
	snprintf(dst, N, "%s", src);
	return &dst[0];
}

/* Copy a number of characters into a buffer
 *
 * This function copies n first characters out of src to dst, and then appends a
 * terminating null byte.  If dst_size is not big enough to store the resulting
 * string, then trimming occurs to avoid buffer overflow.
 *
 * Usage:
 *
 *     char buffer[4];
 *
 *     safe_strcpy(buffer, 4, "abc", 2);
 *     // buffer contains "ab"
 *
 *     safe_strcpy(buffer, 4, "abc", strlen("abc"));
 *     // buffer contains "abc"
 *
 *     safe_strcpy(buffer, 4, "abcd", strlen("abcd"));
 *     // buffer contains "abc"
 *
 */
char *safe_strcpy(char *dst, size_t dst_size, const char *src, size_t n) noexcept;

/* safe_strncpy macro is "somewhat safer" semi-replacement for strncpy.
 * It prevents buffer overflow and ensure dst buffer is null-terminated
 * (but only if user passes the 3rd parameter correctly).
 *
 * Use safe_strcpy or std::string assignment instead of this macro whenever
 * possible.
 *
 * BEWARE: Sometimes user might want to pass count of characters to copy
 * as 3rd parameter (because that's how strncpy is normally used), but
 * this will result in overwriting the last copied character (hence len(src)+1
 * characters need to be copied, when strncpy call would work with len(src)).
 *
 * This macro is kept for compatibility with new code arriving from upstream.
 */
#define safe_strncpy(dst, src, dst_size) \
	do { \
		static_assert(!std::is_array<decltype(dst)>::value, \
		              "dst is an array with size known at compile time; " \
		              "use safe_strcpy(dst, src) instead"); \
		strncpy((dst), (src), (dst_size)-1); \
		(dst)[(dst_size)-1] = '\0'; \
	} while (0)

#endif
