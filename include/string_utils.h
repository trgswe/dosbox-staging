/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *  Copyright (C) 2019-2020  The dosbox-staging team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef DOSBOX_STRING_UTILS_H
#define DOSBOX_STRING_UTILS_H

#include <cstdio>
#include <cstring>

#include <strings.h>

/* Compare two C strings ignoring case
 *
 * Normally we use POSIX functions, but MSVC complains about ISO compatibility.
 */

#ifdef _MSC_VER
#define strcasecmp(a, b)     _stricmp(a, b)
#define strncasecmp(a, b, n) _strnicmp(a, b, n)
#endif

/* Safely copy a string into fixed-size char buffer
 *
 * This function copies string pointed by src to fixed-size buffer dst.
 * At most N bytes from src are copied, where N is size of dst.
 * If exactly N bytes are copied, then terminating null byte is put
 * into buffer, thus buffer overrun is prevented.
 *
 * Function returns pointer to buffer to be compatible with strcpy.
 *
 * Example usage:
 *
 *     char buffer[2];
 *     safe_strcpy(buffer, "abc");
 *     // buffer is filled with "a"
 */

template <size_t N>
char *safe_strcpy(char (&dst)[N], const char *src) noexcept
{
	snprintf(dst, N, "%s", src);
	return &dst[0];
}

/* FIXME
 *
 * was:
#define safe_strncpy(a, b, n)                                                  \
	do {                                                                   \
		strncpy((a), (b), (n)-1);                                      \
		(a)[(n)-1] = 0;                                                \
	} while (0)
*/

inline char *safe_strncpy(char *dst, const char *src, size_t n) noexcept
{
	strncpy(dst, src, n - 1);
	dst[n - 1] = '\0';
	return dst;
}

/* Safely append a string to the content of fixed-size char buffer
 *
 * TODO document
 *
 */

template <size_t N>
char *safe_strcat(char (&dst)[N], const char *src) noexcept
{
	strncat(dst, src, N - strnlen(dst, N) - 1);
	return &dst[0];
}

#endif
