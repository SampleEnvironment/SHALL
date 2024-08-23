# SPDX-License-Identifier: LGPL-3.0-or-later
# Copyright (c) 2017-2019 Helmholtz-Zentrum Berlin fuer Materialien und Energie GmbH <https://www.helmholtz-berlin.de>
TEMPLATE =	subdirs
CONFIG  +=	ordered

# Export gtest directory path
GOOGLETEST_DIR = "D:\shall-repos\googletest"
export(GOOGLETEST_DIR)



SUBDIRS +=	SHALL-VariantLib \
		SHALL-ServerLib \
		SHALL-ServerDummy \
		SHALL-ClientLib \
		SHALL-ClientDummy \
                gtest-serverlib



