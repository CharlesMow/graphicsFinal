/*
 *  CSCI 444, Advanced Computer Graphics, Fall 2021
 *
 *  Project: lab11
 *  File: textureShader.f.glsl
 *
 *  Description:
 *      Fragment Shader that applies color as modualted diffuse texture
 *
 *  Author:
 *      Dr. Jeffrey Paone, Colorado School of Mines
 *
 *  Notes:
 *
 */

// we are using OpenGL 4.1 Core profile
#version 410 core

// ***** FRAGMENT SHADER UNIFORMS *****
uniform sampler2D diffuseMap;
uniform vec3 colorTint;

// ***** FRAGMENT SHADER INPUT *****
layout(location = 0) in vec2 texCoord;

// ***** FRAGMENT SHADER OUTPUT *****
out vec4 fragColorOut;

// ***** FRAGMENT SHADER HELPER FUNCTIONS *****


// ***** FRAGMENT SHADER MAIN FUNCTION *****
void main() {
  // set the output color to the diffuse color modulated by the color tint
  fragColorOut = texture(diffuseMap, texCoord) * vec4(colorTint, 1.0);

  // if viewing the backside of the fragment,
  // reverse the colors as a visual cue
  if( !gl_FrontFacing ) {
    fragColorOut.rgb = fragColorOut.bgr;
  }
}