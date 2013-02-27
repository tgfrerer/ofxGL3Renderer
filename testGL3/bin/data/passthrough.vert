#version 150

uniform mat4 project;
uniform mat4 modelview;
in float x;
in float y;
in float z;

void main()
{
   gl_Position = project * modelview * vec4(x, y, z, 1.0);
}