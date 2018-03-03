#version 330 core

in vec3 vColor;
in vec2 texCoords;
in float mixA;

out vec4 color;

uniform sampler2D curTexture1;
uniform sampler2D curTexture2;

void main()
{
	color = mix(texture(curTexture1, texCoords), texture(curTexture2, texCoords), mixA) * vec4(vColor, 1.0);
}
