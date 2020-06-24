/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

namespace mata {

class noncopyable {
protected:
  constexpr noncopyable() = default;
  ~noncopyable() = default;

  noncopyable(const noncopyable &) = delete;
  noncopyable &operator=(const noncopyable &) = delete;
};

} // namespace mata
