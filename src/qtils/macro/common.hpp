/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define IDENTITY(a) a

#define CONCAT__(a, b) a##b
#define CONCAT_(a, b) CONCAT__(a, b)
#define CONCAT(a, b) CONCAT_(IDENTITY(a), IDENTITY(b))

#define QTILS_UNIQUE_NAME(prefix) CONCAT(prefix, __COUNTER__)
