#version 150

uniform sampler2DRect src_tex_unit0;
uniform vec4 color = vec4(1.0);
uniform float useTexture = 0.0;
uniform float useColors = 1.0;

flat in	vec4 colorVarying;
in	vec2 texCoordVarying;
in	vec4 normalVaryingX;

out	vec4 fragColor;

void main()
{
	vec4 col = color;
	vec4 tex = vec4(1.0);

	if (useColors > 0.5){
		// use per-vertex color
		fragColor = colorVarying;
	} else {
		// use uniform color
		fragColor = color;
	}
	if (useTexture > 0.5){
		fragColor *= texture(src_tex_unit0, texCoordVarying);
	} else {
		fragColor *= vec4(1.0);
	}

	fragColor = colorVarying;
	// fragColor = color * tex;
}
