#version 330 core

layout(location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 inTexCoords;

out vec3 vColor;
out vec2 texCoords;
out float mixA;

uniform vec3 shift;

void main()
{
	gl_Position = vec4(pos.x + shift.x, pos.y + shift.y, pos.z + shift.z, 1.0);
	vColor = color;
	texCoords = inTexCoords;
	mixA = shift.x + 0.5;
}
