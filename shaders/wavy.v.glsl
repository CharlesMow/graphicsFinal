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
uniform float time;
// ***** VERTEX SHADER INPUT *****
layout(location = 0) in vec3 vPos;

// ***** VERTEX SHADER HELPER FUNCTIONS *****
float x_amp = 1.0;
float z_amp = 0.1;
float x_freq = 5.0;
float z_freq = 0.3;

// ***** VERTEX SHADER MAIN FUNCTION *****
void main() {
  vec3 newPos = vec3(vPos.x+sin(vPos.z*x_freq*time)*x_amp, vPos.y, vPos.z+sin(vPos.x*z_freq*time)*z_amp);
  gl_Position = mvpMatrix * vec4( newPos, 1.0 );
}
