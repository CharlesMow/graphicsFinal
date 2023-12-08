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
struct PointLight{
  vec3 position;

  float constant;
  float linear;
  float quadratic;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};
// ***** FRAGMENT SHADER OUTPUT *****
out vec4 fragColorOut;
uniform vec3 color;
uniform vec3 viewPos;
uniform vec3 pLightPos;
uniform sampler2D diffuseMap;
layout(location = 0) in vec2 texCoord;
layout(location = 3) in vec2 isEye;
layout(location = 4) in vec3 normal;
layout(location = 5) in vec3 FragPos;
// ***** FRAGMENT SHADER HELPER FUNCTIONS *****
vec3 CaclPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
// ***** FRAGMENT SHADER MAIN FUNCTION *****
void main() {
  vec3 lightDir = vec3(0,0.5,0);
  vec3 toonShadeColor = vec3(215.0/255.0, 220.0/255.0, 1.0);
  float intensity;
  vec3 colorCel;
  vec4 texColor;
  if(isEye.x==0){
    fragColorOut = vec4(color,1.0);
  }else{
    texColor = texture(diffuseMap, texCoord); //* vec4(color, 1.0);
    intensity = dot(lightDir, normal);
    if (intensity > 0.95) colorCel = toonShadeColor;
    else if (intensity > 0.75) colorCel = toonShadeColor * 0.75;
    else if (intensity > 0.5) colorCel = toonShadeColor * 0.5;
    else colorCel = toonShadeColor * 0.2;
    fragColorOut = texColor * vec4(colorCel, 1.0);
  }

}
// TODO find some way to calculate fragment position relative to light position
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
  return vec3(0,0,0);
}