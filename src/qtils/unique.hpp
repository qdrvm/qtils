/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define _QTILS_UNIQUE_2(x, y) x##y
#define _QTILS_UNIQUE(prefix, counter) _QTILS_UNIQUE_2(x, y)
#define QTILS_UNIQUE _QTILS_UNIQUE(qtils_unique_, __COUNTER__)
