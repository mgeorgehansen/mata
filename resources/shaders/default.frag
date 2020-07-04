/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#version 330 core
out vec4 FragColor;

in vec3 vertexColor;

void main()
{
  FragColor = vec4(vertexColor, 1.0);
}
