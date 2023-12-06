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
layout(location = 4) in vec3 normal;
// ***** FRAGMENT SHADER HELPER FUNCTIONS *****


// ***** FRAGMENT SHADER MAIN FUNCTION *****
void main() {
  vec3 lightDir = vec3(0,0.5,0);
  float intensity;
  vec3 colorCel;
  vec4 tempColor;
  if(isEye.x==0){
    fragColorOut = vec4(color,1.0);
  }else{
    tempColor = texture(diffuseMap, texCoord); //* vec4(color, 1.0);
    intensity = dot(lightDir, normal);
    if (intensity > 0.95) colorCel = vec3(215.0/255.0, 220.0/255.0, 1.0);
    else if (intensity > 0.75) colorCel = vec3(215.0/255.0, 220.0/255.0, 1.0) * 0.75;
    else if (intensity > 0.5) colorCel = vec3(215.0/255.0, 220.0/255.0, 1.0) * 0.5;
    else colorCel = vec3(215.0/255.0, 220.0/255.0, 1.0) * 0.2;
    fragColorOut = tempColor * vec4(colorCel, 1.0);
  }

}