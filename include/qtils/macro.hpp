/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define IDENTITY(a) a

#define CONCAT__(a, b, c) a##b##c
#define CONCAT_(a, b, c) CONCAT__(a, b, c)
#define CONCAT(a, b, c) CONCAT_(IDENTITY(a), IDENTITY(b), IDENTITY(c))

#define UNIQUE_NAME(prefix) CONCAT(prefix, __func__, __LINE__)
