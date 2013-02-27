#version 150

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;


in vec4 vVertex;

void main()
{
   gl_Position = projectionMatrix * modelViewMatrix * vVertex;
}