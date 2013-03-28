#version 150

uniform sampler2D src_tex_unit0;
uniform vec4 color = vec4(1.0);
uniform float useTexture = 0.0;
uniform float useColors = 1.0;
uniform vec4 lightPositionInCameraSpace;

uniform vec4 Ka = vec4(0.4, 0.4, 0.4, 1.0); // ambient coefficient
uniform vec4 Kd = vec4(40.5, 40.5, 40.5, 1.0); // diffuse coefficient
uniform vec4 Ks = vec4(150.3, 50.3, 50.3, 1.0); // specular coefficient
uniform float Ns = 64.0; // specular exponent
uniform vec4 Ld = vec4(0.2, 0.2, 0.6, 1.0); // diffuse light colour


in vec4 vpeye;	// varying position eye space
in vec4 vneye;	// varying normal eye space

in vec4 colorVarying;
in vec2 texCoordVarying;


out	vec4 fragColor;

void main()
{

	vec4 n_eye = normalize(vneye); // normalise just to be on the safe side
	vec4 s_eye = normalize(lightPositionInCameraSpace - vpeye); // get direction from surface fragment to light
	vec4 v_eye = -normalize(vpeye); // get direction from surface fragment to camera
	vec4 h_eye = normalize(v_eye + s_eye); // Blinn's half-way vector
										   //vec4 r = reflect(-s, vneye); // Phong's full reflection (could use instead of h)
	
	vec4 Ia = vec4(0.2,0.2,0.6,1) * Ka; // ambient light has a hard-coded colour here, but we could load an La value
	vec4 Id = Ld * Kd * max(dot(s_eye, n_eye), 0.0); // max() is a safety catch to make sure we never get negative colours
	vec4 Is = vec4(1.0) * Ks * pow(max(dot(h_eye, n_eye), 0), Ns); // my ambient light colour is hard coded white, but could load Ls
	
	// 	fragColor = vec4((h_eye.xyz + vec3(1.0, 1.0, 1.0)) / 2.0,1.0);
	fragColor = (Ia + Id + Is);

	// 	fragColor = vec4(vec3(1.0) * texCoordVarying.y, 1.0);
	// fragColor = texture(src_tex_unit0, texCoordVarying.xy);
	
	//  * vec4((normalVarying.xyz + vec3(1.0, 1.0, 1.0)) / 2.0,1.0)
}
