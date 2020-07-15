/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <chrono>

namespace mata {

using fmilliseconds = std::chrono::duration<float, std::milli>;
using fseconds = std::chrono::duration<float>;

constexpr fmilliseconds operator"" _fms(long double value) {
  return fmilliseconds(value);
}
constexpr fmilliseconds operator"" _fms(unsigned long long int value) {
  return fmilliseconds(value);
}
constexpr fseconds operator"" _fs(long double value) { return fseconds(value); }
constexpr fseconds operator"" _fs(unsigned long long int value) {
  return fseconds(value);
}

} // namespace mata
