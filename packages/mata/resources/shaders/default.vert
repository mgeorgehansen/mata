/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#version 330 core
layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec2 inTextureCoords;
layout (location = 2) in int  inTileIndex;

uniform mat4 viewMatrix;

out VertexData {
  vec3 tileCoords;
} o;

void main() {
  // Flip the y-coord so that we can use the convention that UV coords are from
  // top-to-bottom, instead of bottom-to-top which requires flipping textures.
  o.tileCoords = vec3(inTextureCoords, inTileIndex);
  gl_Position = viewMatrix * vec4(inPosition.x, -inPosition.y, 1.0, 1.0);
}
