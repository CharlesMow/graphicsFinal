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
struct DirLight {
  vec3 direction;

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
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
// ***** FRAGMENT SHADER MAIN FUNCTION *****
void main() {
  //*********** Point Light Stuff *************
  PointLight pLight;
  pLight.position = pLightPos;

  pLight.constant = 1;
  pLight.linear = 0.09;
  pLight.quadratic = 0.032;

  pLight.ambient = vec3(0.3,0.3,0.3);
  pLight.diffuse = vec3(0.8,0.8,0.8);
  pLight.specular = vec3(1.0,1.0,1.0);

  DirLight dLight;
  dLight.direction = vec3(0.1,-1.0,0.1);

  dLight.ambient = vec3(0.2,0.2,0.05);
  dLight.diffuse = vec3(0.4,0.4,0.1);
  dLight.specular = vec3(0.5,0.5,0.125);

  //*************************
  vec3 lightDir = vec3(0,0.5,0);
  vec3 toonShadeColor = vec3(215.0/255.0, 220.0/255.0, 1.0);
  float intensity;
  vec3 colorCel;
  vec4 texColor;
  if(isEye.x==0){
    fragColorOut = vec4(color,1.0);
  }else{
    texColor = texture(diffuseMap, texCoord); //* vec4(color, 1.0);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 pLightResult = CalcPointLight(pLight,normal,FragPos,viewDir);
    vec3 dLightResult = CalcDirLight(dLight,normal,viewDir);
    vec4 result = vec4(dLightResult+pLightResult,1.0);

    intensity = (result.x*0.21 + result.y*0.72 + result.z*0.07);
    if (intensity > 0.95) colorCel = toonShadeColor*0.9;
    else if (intensity > 0.75) colorCel = toonShadeColor * 0.75;
    else if (intensity > 0.5) colorCel = toonShadeColor * 0.6;
    else colorCel = toonShadeColor * 0.4;
    fragColorOut = texColor*vec4(colorCel, 1.0);
  }

}
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
  vec3 lightDir = normalize(-light.direction);
  // diffuse shading
  float diff = max(dot(normal, lightDir), 0.0);
  // specular shading
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 0.032);
  // combine results
  vec3 ambient = light.ambient;
  vec3 diffuse = light.diffuse * diff;
  vec3 specular = light.specular * spec;
  return (ambient + diffuse + specular);
}
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
  vec3 lightDir = normalize(light.position - fragPos);
  // diffuse shading
  float diff = max(dot(normal, lightDir), 0.0);
  // specular shading
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 0.032);
  // attenuation
  float distance = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
  // combine results
  vec3 ambient = light.ambient;
  vec3 diffuse = light.diffuse * diff;
  vec3 specular = light.specular * spec;
  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;
  return (ambient + diffuse + specular);
}