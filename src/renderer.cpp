/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <algorithm>
#include <exception>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <glbinding/Binding.h>
#include <glbinding/gl33core/gl.h>
#include <glbinding/glbinding.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <vector>

#include <mata/concepts/noncopyable.hpp>
#include <mata/geometry.hpp>
#include <mata/renderer.hpp>
#include <mata/tile_layer.hpp>
#include <mata/virtual-file-system.hpp>

using namespace gl;

namespace mata {

typedef GLuint shaderprogram_h;
typedef GLuint shader_h;
typedef GLuint buffer_h;
typedef GLuint texture_h;

inline std::string readableErrorCode(const gl::GLenum errorCode) {
  if (GL_INVALID_ENUM == errorCode) {
    return "GL_INVALID_ENUM";
  }
  if (gl::GL_INVALID_VALUE == errorCode) {
    return "GL_INVALID_VALUE";
  }
  if (gl::GL_INVALID_OPERATION == errorCode) {
    return "GL_INVALID_OPERATION";
  }
  if (gl::GL_INVALID_FRAMEBUFFER_OPERATION == errorCode) {
    return "GL_INVALID_FRAMEBUFFER_OPERATION";
  }
  return "Unknown Error";
}

struct Vertex {
  using PositionType = glm::vec2;
  using TextureCoordsType = glm::vec2;

  PositionType position;
  TextureCoordsType textureCoords;
};

class TileLayerMesh {
private:
  std::vector<Vertex> m_vertices = {};
  std::vector<int> m_tileIndices = {};

public:
  TileLayerMesh(const TileLayer &layer) noexcept {
    const auto dimensions = layer.dimensions();
    const auto tileset = layer.tileset();
    for (auto j = 0; j < dimensions.nRows; j++) {
      for (auto i = 0; i < dimensions.nColumns; i++) {
        const auto tileIdx =
            index2dTo1d(layer.tileAt({i, j}), tileset.dimensions());

        // TODO: use indices to share vertices b and c.

        // Each quad has normalized texture coordinates mapped to the
        // corresponding corner of the tile image:
        //
        //                 , (0,0)-------(1,0)
        // a-------c <- '     |     _     |
        // |  / \ *|          |   /   \ * |
        // |  \ /  |          |   \   /   |
        // | \-+-/ |          |  \--+--/  |
        // b-------d <- ,     |   \   /   |
        //                ` (0,1)-------(1,1)
        //
        // We must also split up the tile into triangles with counter-clockwise
        // winding:
        //
        // a-------c    t1 = [a, b, c]
        // | t1  / |    t2 = [b, d, c]
        // |   /   |
        // | /  t2 |
        // b-------d

        // t1.a
        m_vertices.push_back(
            {{static_cast<float>(i), static_cast<float>(j)}, {0.0f, 0.0f}});
        m_tileIndices.push_back(tileIdx);

        // t1.b
        m_vertices.push_back(
            {{static_cast<float>(i), static_cast<float>(j + 1)}, {0.0f, 1.0f}});
        m_tileIndices.push_back(tileIdx);

        // t1.c
        m_vertices.push_back(
            {{static_cast<float>(i + 1), static_cast<float>(j)}, {1.0f, 0.0f}});
        m_tileIndices.push_back(tileIdx);

        // t2.b
        m_vertices.push_back(
            {{static_cast<float>(i), static_cast<float>(j + 1)}, {0.0f, 1.0f}});
        m_tileIndices.push_back(tileIdx);

        // t2.d
        m_vertices.push_back(
            {{static_cast<float>(i + 1), static_cast<float>(j + 1)},
             {1.0f, 1.0f}});
        m_tileIndices.push_back(tileIdx);

        // t2.c
        m_vertices.push_back(
            {{static_cast<float>(i + 1), static_cast<float>(j)}, {1.0f, 0.0f}});
        m_tileIndices.push_back(tileIdx);
      }
    }
  }

  const std::vector<Vertex> &vertices() const noexcept { return m_vertices; }

  const std::vector<int> &tileIndices() const noexcept { return m_tileIndices; }

  int nIndices() const noexcept { return static_cast<int>(m_vertices.size()); }
};

struct LayerH {
  buffer_h vao;
  int nIndices;
  texture_h texture;
};

class [[nodiscard]] Renderer::Impl final : private noncopyable {
  std::shared_ptr<VirtualFileSystem> m_pVfs;
  shaderprogram_h m_hShaderProgram{0};
  bool m_wireframeModeEnabled = false;
  std::vector<LayerH> m_layers{};

  void clearScreen() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(ClearBufferMask::GL_COLOR_BUFFER_BIT);
  }

  [[nodiscard]] shaderprogram_h initShaderProgram() {
    const auto hVertexShader =
        this->loadShader("default.vert", GL_VERTEX_SHADER);
    const auto hFragmentShader =
        this->loadShader("default.frag", GL_FRAGMENT_SHADER);
    const auto hShaderProgram = glCreateProgram();
    glAttachShader(hShaderProgram, hVertexShader);
    glAttachShader(hShaderProgram, hFragmentShader);
    glLinkProgram(hShaderProgram);
    int success;
    glGetProgramiv(hShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
      char infoLog[512];
      glGetProgramInfoLog(hShaderProgram, 512, nullptr, infoLog);
      throw std::runtime_error(fmt::format("Failed to link shader program: {0}",
                                           std::string_view(infoLog)));
    }
    glDeleteShader(hFragmentShader);
    glDeleteShader(hVertexShader);
    return hShaderProgram;
  }

  [[nodiscard]] shader_h loadShader(const std::filesystem::path &shaderPath,
                                    const GLenum shaderType) {
    const shader_h hShader = glCreateShader(shaderType);
    if (hShader == 0) {
      throw std::runtime_error("Failed to create shader object");
    }

    const std::string shaderSrc =
        this->m_pVfs->readTextFile("shaders" / shaderPath);
    const char *shaderSrcCstr = shaderSrc.c_str();
    glShaderSource(hShader, 1, &shaderSrcCstr, nullptr);
    glCompileShader(hShader);
    int success;
    glGetShaderiv(hShader, GL_COMPILE_STATUS, &success);
    if (!success) {
      char infoLog[512];
      glGetShaderInfoLog(hShader, 512, nullptr, infoLog);
      throw std::runtime_error(fmt::format("Error compiling shader {0}: {1}",
                                           shaderPath.string(),
                                           std::string_view(infoLog)));
    }
    return hShader;
  }

  [[nodiscard]] buffer_h createVertexBuffers(const TileLayerMesh &mesh) {
    buffer_h vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // =========================================================================
    // Vertex Buffer
    //
    // We have one vertex buffer object (vbo) for all of the static,
    // unchanging vertex attributes, including position and texture coords.
    buffer_h vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    const auto vertices = mesh.vertices();
    const auto nVertexCoords = vertices.size();
    glBufferData(GL_ARRAY_BUFFER, nVertexCoords * sizeof(Vertex),
                 vertices.data(), GL_STATIC_DRAW);

    static const auto vertexPosAttribSize = sizeof(Vertex::PositionType);
    static const auto textureCoordAttribSize =
        sizeof(Vertex::TextureCoordsType);
    static const auto vertexStride =
        vertexPosAttribSize + textureCoordAttribSize;

    // Add position vertex attribute.
    static const auto vertexPosAttrib = 0;
    glVertexAttribPointer(vertexPosAttrib, 2, GL_FLOAT, GL_FALSE, vertexStride,
                          nullptr);
    glEnableVertexAttribArray(vertexPosAttrib);

    // Map the textureCoords vertex attribute to the tbo.
    static const auto textureCoordsAttrib = 1;
    glVertexAttribPointer(textureCoordsAttrib, 2, GL_FLOAT, GL_FALSE,
                          vertexStride, nullptr);
    glEnableVertexAttribArray(textureCoordsAttrib);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // =========================================================================
    // Tile Index Buffer
    //
    // The tile index buffer (tibo) holds the dynamic id of the tile to render
    // for a particular triangle/quad.
    buffer_h tibo;
    glGenBuffers(1, &tibo);
    glBindBuffer(GL_ARRAY_BUFFER, tibo);
    const auto tileIndexes = mesh.tileIndices();
    const auto nTileIndexes = vertices.size();
    glBufferData(GL_ARRAY_BUFFER, nTileIndexes * sizeof(int),
                 tileIndexes.data(), GL_DYNAMIC_DRAW);

    static const auto tileIndexAttribSize = sizeof(int);
    static const auto tiboStride = tileIndexAttribSize;

    // Map the textureCoords vertex attribute to the tbo.
    static const auto tileIndexAttrib = 2;
    glVertexAttribPointer(tileIndexAttrib, 1, GL_FLOAT, GL_FALSE, tiboStride,
                          nullptr);
    glEnableVertexAttribArray(tileIndexAttrib);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Finish working on the vertex array.
    glBindVertexArray(0);

    return vao;
  }

  [[nodiscard]] texture_h uploadTileset(const Tileset &tileset) {
    texture_h glTexture;
    glGenTextures(1, &glTexture);

    glBindTexture(GL_TEXTURE_2D_ARRAY, glTexture);

    const auto tilesetDims = tileset.dimensions();
    const auto nTiles = tilesetDims.nColumns * tilesetDims.nRows;
    const auto tileSize = tileset.tileSize();

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, tileSize.nColumns,
                 tileSize.nRows, nTiles, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 tileset.asLinearBytes().data());

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    return glTexture;
  }

public:
  Impl(const Window &window, const std::shared_ptr<VirtualFileSystem> _pVfs)
      : m_pVfs(_pVfs) {
    glbinding::initialize(window.glProcAddressFunc());
    glbinding::setAfterCallback(
        []([[maybe_unused]] const glbinding::FunctionCall &functionCall) {
          const auto errorCode = glbinding::Binding::GetError.directCall();

          if (errorCode == gl::GL_NO_ERROR) {
            return;
          }

          if (errorCode == GL_OUT_OF_MEMORY) {
            throw std::runtime_error("OpenGL driver reported out of memory");
          }

#ifndef NDEBUG
          const auto message =
              fmt::format("{0}: {1}", functionCall.function->name(),
                          readableErrorCode(errorCode));
          std::cerr << message << "\n";
          std::terminate();
#endif
        });
    glbinding::setCallbackMaskExcept(glbinding::CallbackMask::After,
                                     {"glGetError"});

    this->m_hShaderProgram = this->initShaderProgram();
    glUseProgram(this->m_hShaderProgram);
  }

  ~Impl() {
    glbinding::removeCallbackMaskExcept(glbinding::CallbackMask::After,
                                        {"glGetError"});
  }

  void setLayer(const LayerIdx layerN, const TileLayer &layer) {
    const auto mesh = TileLayerMesh(layer);
    const auto vao = createVertexBuffers(mesh);
    const auto textureHandle = uploadTileset(layer.tileset());

    const auto layerIter = this->m_layers.begin();
    this->m_layers.insert(layerIter + layerN, {
                                                  vao,
                                                  mesh.nIndices(),
                                                  textureHandle,
                                              });
  }

  void updateViewMatrix(const glm::mat4 &viewMatrix) const {
    const auto transformLoc =
        glGetUniformLocation(this->m_hShaderProgram, "viewMatrix");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
  }

  void toggleWireframeMode() {
    m_wireframeModeEnabled = !m_wireframeModeEnabled;
    if (m_wireframeModeEnabled) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
  }

  void drawFrame() {
    this->clearScreen();

    glActiveTexture(GL_TEXTURE0);
    for (const auto &layer : this->m_layers) {
      glBindVertexArray(layer.vao);
      glBindTexture(GL_TEXTURE_2D_ARRAY, layer.texture);
      glDrawArrays(GL_TRIANGLES, 0, layer.nIndices);
    }
    // Ensure that we keep the vertex array unbound just to keep global state
    // cleaned up.
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    glBindVertexArray(0);
  }

  void resize(const int width, const int height) {
    glViewport(0, 0, width, height);
  }
}; // namespace mata

Renderer::Renderer(const Window &window,
                   const std::shared_ptr<VirtualFileSystem> _pVfs)
    : m_pImpl(std::make_unique<Impl>(window, _pVfs)) {}

Renderer::~Renderer() noexcept = default;

void Renderer::updateViewMatrix(const glm::mat4 &viewMatrix) {
  m_pImpl->updateViewMatrix(viewMatrix);
}

void Renderer::setLayer(const LayerIdx layerN, const TileLayer &layer) {
  m_pImpl->setLayer(layerN, layer);
}

void Renderer::toggleWireframeMode() { m_pImpl->toggleWireframeMode(); }

void Renderer::drawFrame() { m_pImpl->drawFrame(); }

void Renderer::resize(const int width, const int height) {
  m_pImpl->resize(width, height);
}

} // namespace mata
