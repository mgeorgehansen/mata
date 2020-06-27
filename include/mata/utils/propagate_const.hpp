/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

/**
 * \def PROPAGATE_CONST(TYPE)
 *
 * Declare a pointer type on a member variable that transitively propagates
 * const to the objects it points.
 *
 * This wraps the pointer type in std::experimental::propagate_const<> on
 * platforms which support it, and does nothing on platforms which do not
 * support std::experimental::propagate_const<>.
 */
#ifdef _MSC_VER
#define PROPAGATE_CONST(TYPE) TYPE
#else
#include <experimental/propagate_const>
#define PROPAGATE_CONST(TYPE) std::experimental::propagate_const<TYPE>
#endif
