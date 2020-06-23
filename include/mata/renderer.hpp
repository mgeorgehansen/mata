#pragma once

#include <experimental/propagate_const>
#include <memory>

#include "concepts/noncopyable.hpp"
#include "virtual-file-system.hpp"

namespace mata {

class [[nodiscard]] Renderer final : private noncopyable {
  class Impl;
  std::experimental::propagate_const<std::unique_ptr<Impl>> m_pImpl;

public:
  Renderer(const std::shared_ptr<VirtualFileSystem>);
  ~Renderer() noexcept;

  void drawFrame() const;
  void resize(const int width, const int height) const;
};

} // namespace mata
