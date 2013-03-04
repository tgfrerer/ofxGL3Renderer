#version 150

out vec4 fragColor;
flat in  vec4 vertColor;

void main()
{
    fragColor = vertColor;
}
