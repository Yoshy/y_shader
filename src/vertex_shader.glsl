#version 330 core

layout(location = 0) in vec3 pos;
layout (location = 1) in vec3 color;

out vec3 vColor;

uniform vec3 shift;

void main()
{
	gl_Position = vec4(pos.x + shift.x, pos.y + shift.y, pos.z + shift.z, 1.0);
	vColor = color;
}
