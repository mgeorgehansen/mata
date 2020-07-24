/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <GLFW/glfw3.h>
#include <chrono>
#include <exception>
#include <fmt/format.h>
#include <glbinding/glbinding.h>
#include <glm/vec2.hpp>
#include <stdexcept>
#include <string>

#include <mata/core/time.hpp>
#include <mata/platform/filesystem.hpp>
#include <mata/platform/platform.hpp>
#include <mata/renderer/camera.hpp>
#include <mata/renderer/renderer.hpp>
#include <mata/renderer/tile_layer.hpp>
#include <mata/renderer/window.hpp>

#include "mata/app.hpp"

namespace mata {

using namespace mata::core::units;

inline std::shared_ptr<mata::platform::VirtualFileSystem>
initVirtualFilesystem(const AppParams &params) {
  const auto resourcesPath =
      params.resourcesPath.value_or(mata::platform::execDir() / "resources");
  return std::make_shared<mata::platform::VirtualFileSystem>(resourcesPath);
}

static constexpr auto SCROLL_SPEED = 2.0f;

class App::Impl final {
private:
  static constexpr auto SIMULATION_UPDATE_FREQ = 10_fms;

  std::shared_ptr<mata::platform::VirtualFileSystem> m_pVfs;
  mata::renderer::Window m_window;
  mata::renderer::Renderer m_renderer;

  mata::renderer::Camera m_camera{};
  bool m_closeRequested = false;
  float m_cameraHorizontalAxis = 0.0f;
  float m_cameraVerticalAxis = 0.0f;

  void initScene() {
    const auto imageBytes = m_pVfs->readFile("tilesets/terrain.png");
    const auto tilesetTexture = mata::renderer::Texture::fromPng(imageBytes);
    const auto tileset =
        mata::renderer::Tileset{{32, 32}, {2, 2}, tilesetTexture};
    const auto layer = mata::renderer::TileLayer{{4, 4},
                                                 tileset,
                                                 {
                                                     {0, 0},
                                                     {1, 0},
                                                     {1, 2},
                                                     {1, 2},
                                                     {0, 0},
                                                     {1, 0},
                                                     {1, 2},
                                                     {1, 2},
                                                     {0, 0},
                                                     {1, 0},
                                                     {1, 2},
                                                     {1, 2},
                                                     {0, 0},
                                                     {1, 0},
                                                     {1, 2},
                                                     {1, 2},
                                                 }};
    m_renderer.setLayer(0, layer);
  }

  void updateCamera(const fmilliseconds dt) {
    const auto secs = dt.count() / 1000.0f;
    m_camera.translateBy({secs * -SCROLL_SPEED * m_cameraHorizontalAxis,
                          secs * -SCROLL_SPEED * m_cameraVerticalAxis});
  }

public:
  Impl(const AppParams &params)
      : m_pVfs(initVirtualFilesystem(params)), m_window(params.headless),
        m_renderer(m_window, m_pVfs) {
    m_window.onResize([this](const int width, const int height) {
      m_renderer.resize(width, height);
    });
    m_window.onWindowCloseRequested(
        [this]() { this->m_closeRequested = true; });
    m_window.onKeyEvent([this](const int key, const int action) {
      if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        m_renderer.toggleWireframeMode();
      }

      if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        m_closeRequested = true;
      }
      if ((key == GLFW_KEY_UP && action == GLFW_PRESS) ||
          (key == GLFW_KEY_DOWN && action == GLFW_RELEASE)) {
        m_cameraVerticalAxis += 1.0f;
      }
      if ((key == GLFW_KEY_RIGHT && action == GLFW_PRESS) ||
          (key == GLFW_KEY_LEFT && action == GLFW_RELEASE)) {
        m_cameraHorizontalAxis += 1.0f;
      }
      if ((key == GLFW_KEY_DOWN && action == GLFW_PRESS) ||
          (key == GLFW_KEY_UP && action == GLFW_RELEASE)) {
        m_cameraVerticalAxis -= 1.0f;
      }
      if ((key == GLFW_KEY_LEFT && action == GLFW_PRESS) ||
          (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE)) {
        m_cameraHorizontalAxis -= 1.0f;
      }
    });
    initScene();
  }

  void stepSimulation(const fmilliseconds dt) { updateCamera(dt); }

  void render() {
    m_renderer.updateViewMatrix(m_camera.viewMatrix());
    m_renderer.drawFrame();
    m_window.update();
  }

  void stepFrame() {
    this->stepSimulation(SIMULATION_UPDATE_FREQ);
    this->render();
  }

  void run() {
    // Based on https://gafferongames.com/post/fix_your_timestep/.
    auto lastFrameEndedAt = std::chrono::high_resolution_clock::now();
    auto simulationTimeLeft = 0_fms;
    while (!m_closeRequested) {
      const auto currentFrameStartedAt =
          std::chrono::high_resolution_clock::now();
      const auto lastFrameDuration = currentFrameStartedAt - lastFrameEndedAt;

      simulationTimeLeft += lastFrameDuration;
      while (simulationTimeLeft >= SIMULATION_UPDATE_FREQ) {
        simulationTimeLeft -= SIMULATION_UPDATE_FREQ;
        this->stepSimulation(SIMULATION_UPDATE_FREQ);
      }

      this->render();

      lastFrameEndedAt = currentFrameStartedAt;
    }
  }
};

App::App(const AppParams &params) : m_pImpl(std::make_unique<Impl>(params)) {}

App::~App() noexcept = default;

void App::stepFrame() { m_pImpl->stepFrame(); }

void App::run() { m_pImpl->run(); }

} // namespace mata
