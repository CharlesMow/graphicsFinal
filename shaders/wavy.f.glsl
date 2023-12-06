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

// ***** FRAGMENT SHADER OUTPUT *****
out vec4 fragColorOut;
uniform vec3 color;
uniform sampler2D diffuseMap;
layout(location = 0) in vec2 texCoord;
layout(location = 3) in vec2 isEye;
// ***** FRAGMENT SHADER HELPER FUNCTIONS *****


// ***** FRAGMENT SHADER MAIN FUNCTION *****
void main() {
  if(isEye.x==0){
    fragColorOut = vec4(color,1.0);
  }else{
    fragColorOut = texture(diffuseMap, texCoord); //* vec4(color, 1.0);
  }

}