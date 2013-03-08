#version 150

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

in vec4	position;
in vec4	normal;
in vec4	color;
in vec2	texcoord;

out vec4 normalVaryingX;
out vec2 texCoordVarying;
flat out vec4 colorVarying;

void main()
{
	// colorVarying = vec4((normal.xyz + vec3(1.0, 1.0, 1.0)) / 2.0,1.0);
	// //	vertColor = vec4(1.0);
	// colorVarying.a = 1.0;
	texCoordVarying = texcoord;
	mat4 normalMatrix;
	normalMatrix = transpose(inverse(modelViewMatrix));

	// colorVarying = color;
	normalVaryingX = normalMatrix * normal;
	colorVarying = vec4((normal.xyz + vec3(1.0, 1.0, 1.0)) / 2.0,1.0);
	gl_Position = projectionMatrix * modelViewMatrix * position;
}

