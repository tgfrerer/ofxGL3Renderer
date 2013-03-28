#version 150

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform vec4 uColor;


in vec4  position;
in vec4  normal;
in vec4  color;
in vec2  texcoord;

flat out vec4 vertColor;
out vec2 texCoordVarying;

void main()
{
	vertColor = uColor;

	vec4 tNormal = transpose(inverse(modelViewMatrix)) * normal;
	
	vertColor = vec4((tNormal.xyz + vec3(1.0, 1.0, 1.0)) / 2.0,1.0);
//	vertColor = vec4(1.0);
	vertColor.a = 1.0;
	texCoordVarying = texcoord;
	gl_Position = projectionMatrix * modelViewMatrix * position;
}