#version 150

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform vec4 uColor;


in vec4  position;
out vec4 vertColor;

void main()
{
	vertColor = uColor;
	gl_Position = projectionMatrix * modelViewMatrix * position;
}