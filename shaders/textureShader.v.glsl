/*
 *  CSCI 444, Advanced Computer Graphics, Fall 2021
 *
 *  Project: lab1
 *  File: textureShader.v.glsl
 *
 *  Description:
 *      Pass Through Vertex Shader
 *
 *  Author:
 *      Dr. Jeffrey Paone, Colorado School of Mines
 *
 *  Notes:
 *
 */

// we are using OpenGL 4.1 Core profile
#version 410 core

// ***** VERTEX SHADER UNIFORMS *****
uniform mat4 mvpMatrix;

// ***** VERTEX SHADER INPUT *****
layout(location = 0) in vec3 vPos;
layout(location = 2) in vec2 vTexCoord;

// ***** VERTEX SHADER OUTPUT *****
layout(location = 0) out vec2 texCoord;

// ***** VERTEX SHADER HELPER FUNCTIONS *****


// ***** VERTEX SHADER MAIN FUNCTION *****
void main() {
  gl_Position = mvpMatrix * vec4( vPos, 1.0 );
  texCoord = vTexCoord;
}