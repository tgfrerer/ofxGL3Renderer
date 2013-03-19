#version 150

uniform sampler2D src_tex_unit0;

flat	in  vec4 vertColor;
		in  vec2 texCoordVarying;

		out vec4 fragColor;

void main()
{
	fragColor = vec4(texCoordVarying.x/1024.0, texCoordVarying.y/768.0, 1.0,1.0);
}
