#version 330

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;

smooth out vec3 vNormal;
smooth out vec4 vEyeSpacePos;

varying vec2 uv_splat;
varying vec2 uv_scale;
uniform vec2 uv_size;


void main()
{	
	gl_Position = projectionMatrix*modelViewMatrix*vec4(inPosition, 1.0);
  	vEyeSpacePos = modelViewMatrix*vec4(inPosition, 1.0);
  
  	vNormal = inNormal;

	uv_scale = inPosition.xz /uv_size;
	uv_splat = uv_scale;
  	  
}