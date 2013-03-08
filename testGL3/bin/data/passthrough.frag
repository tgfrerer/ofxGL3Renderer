#version 150

uniform sampler2D src_tex_unit0;

flat	in  vec4 vertColor;
		in  vec2 texCoordVarying;

		out vec4 fragColor;

void main()
{

	//	fragColor = vertColor;
//	fragColor = vec4(texCoordVarying,texCoordVarying);
  fragColor = mix(vertColor,texture(src_tex_unit0, texCoordVarying),0.5);
}
