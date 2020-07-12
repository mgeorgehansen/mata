/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#version 330 core
layout (location = 0) in vec2 aPos;

uniform mat4 viewMatrix;

void main()
{
  gl_Position = viewMatrix * vec4(aPos, 1.0, 1.0);
}
