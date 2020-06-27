/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#define MATA_OS_WINDOWS 0
#define MATA_OS_LINUX 0
#define MATA_OS_MACOS 0

#if defined(_WIN32) || defined(_WIN64)
#undef MATA_OS_WINDOWS
#define MATA_OS_WINDOWS 1
#elif defined(__LINUX__)
#undef MATA_OS_LINUX
#define MATA_OS_LINUX 1
#elif defined(__APPLE__)
#undef MATA_OS_MACOS
#define MATA_OS_MACOS 1
#endif
