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

#include "string_utils.h"

#include <cstring>

char *safe_strcpy(char *dst, size_t dst_size, const char *src, size_t n) noexcept
{
	assert(dst != nullptr);
	assert(src != nullptr);
	if (n < dst_size) {
		strncpy(dst, src, n);
		dst[n] = '\0';
	} else {
		strncpy(dst, src, dst_size - 1);
		dst[dst_size - 1] = '\0';
	}
	return dst;
}
