#!/bin/bash

# SPDX-License-Identifier: GPL-2.0-or-later

# Copyright (C) 2020  Patryk Obara <patryk.obara@gmail.com>

# This script exists only to easily generate/update gettext translation
# template file (dosbox.pot).

cd "$(git rev-parse --show-toplevel)"

xgettext \
	--verbose \
	--keyword=_ \
	--add-comments=l10n \
	--package-name=dosbox-staging \
	--files-from=po/sources.txt \
	-o po/dosbox.pot
