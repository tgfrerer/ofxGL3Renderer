#version 150

uniform mat4 project;
uniform mat4 modelview;
in vec4 vVertex;

void main()
{
   gl_Position = project * modelview * vVertex;
}