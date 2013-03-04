#version 150

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform vec4 uColor;


in vec4  position;
in vec4  normal;
in vec4  texCoord;
in vec4  color;

flat out vec4 vertColor;

void main()
{
	vertColor = uColor;

	vertColor = vec4((normal.xyz + vec3(1.0, 1.0, 1.0)) / 2.0,1.0);

	vertColor.a = 1.0;
	gl_Position = projectionMatrix * modelViewMatrix * position;
}