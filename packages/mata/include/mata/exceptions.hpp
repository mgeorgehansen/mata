/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <exception>
#include <string>

namespace mata {

std::string format_exception(const std::exception &error,
                             const std::size_t level = 0);

} // namespace mata
