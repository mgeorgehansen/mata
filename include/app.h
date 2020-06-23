#pragma once

#include <experimental/propagate_const>
#include <memory>

#include "concepts/noncopyable.h"

class [[nodiscard]] App final : private noncopyable {
  class Impl;
  std::experimental::propagate_const<std::unique_ptr<Impl>> m_pImpl;

public:
  App();
  ~App() noexcept;

  void run() const;
};
