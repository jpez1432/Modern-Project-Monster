#version 330

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

out vec3 Normal;
out vec2 TexCoord;
out vec4 Position;
out vec4 vEyeSpacePos;

void main()
{
	gl_Position = projectionMatrix*modelViewMatrix*vec4(inPosition, 1.0);
	vEyeSpacePos = modelViewMatrix*vec4(inPosition, 1.0);

	TexCoord = inCoord;
	Normal = inNormal;
	Position = vec4(inPosition, 1.0);
}