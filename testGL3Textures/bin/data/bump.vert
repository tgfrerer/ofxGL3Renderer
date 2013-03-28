#version 150

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform vec4 lightPositionInCameraSpace;

in vec4	position;
in vec4	normal;
in vec4	color;
in vec2	texcoord;

out vec4 vpeye;	// varying position eye space
out vec4 vneye;	// varying normal eye space

out vec4 colorVarying;
out vec2 texCoordVarying;

void main()
{
	// we do all lighting calculations in camera space, which is also called eye space.
	// this means we have to apply the modelviewmatrix to any vertices which originate
	// in world space.
	
	// the lightPosition arrives to this shader already in camera space (at least that's
	// what we expect).
	
	texCoordVarying = texcoord;

	mat4 normalMatrix = transpose( inverse ( modelViewMatrix) );

	vneye = normalMatrix * vec4(normal.xyz,0.0);
	vpeye = modelViewMatrix * vec4(position.xyz,1.0);
	
	colorVarying = vec4((normal.xyz + vec3(1.0, 1.0, 1.0)) / 2.0,1.0);
	
	gl_Position = projectionMatrix * modelViewMatrix * position;
}

