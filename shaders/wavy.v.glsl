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
uniform bool shouldMove;
uniform int typeBodyPart;
layout(location = 2) in vec2 vTexCoord;
layout(location = 0) out vec2 texCoord;
/*  Body Part Mapping
0 == eyes
1 == fin1
2 == fin2
3 == body1
4 == body2
5 == body3
6 == body4
*/
// ***** VERTEX SHADER INPUT *****
layout(location = 0) in vec3 vPos;

// ***** VERTEX SHADER HELPER FUNCTIONS *****
float x_amp = 1.0;
float z_amp = 0.3;
float x_freq = 5.0;
float z_freq = 0.3;
float fin_freq = 0.5;
float fin_amp = 0.1;
float body_freq = 8;
float body_amp = 0.1;
// ***** VERTEX SHADER MAIN FUNCTION *****
void main() {
  //sin(vPos.z*x_freq*time)*x_amp
  if(shouldMove){
    vec3 newPos = vec3(vPos.x, vPos.y, vPos.z);
    if(typeBodyPart == 0){
      // Do nothing
    }else if(typeBodyPart ==1 || typeBodyPart ==2){
      newPos = vec3(vPos.x+sin(vPos.y*fin_freq*time)*fin_amp, vPos.y, vPos.z+sin(vPos.y*fin_freq*time)*fin_amp);
    }else if(typeBodyPart ==3|| typeBodyPart == 5){
      // Center Ring  +z = down   +y = right  +x = forward
      newPos = vec3(vPos.x, vPos.y+cos(-vPos.z*body_freq*time)*body_amp,vPos.z);
    }else if(typeBodyPart ==4 || typeBodyPart == 6){
      // LHS Disk     +z = right  +y = up     +x = forward
      newPos = vec3(vPos.x, vPos.y,vPos.z+cos(vPos.y*body_freq*time)*body_amp);
    }else if(typeBodyPart ==6){
      // RHS Disk
      newPos = vec3(vPos.x, vPos.y,vPos.z+5);
    }
    gl_Position = mvpMatrix * vec4(newPos, 1.0);
  }else{
    gl_Position = mvpMatrix * vec4(vPos, 1.0);
  }
  texCoord = vTexCoord;
}
