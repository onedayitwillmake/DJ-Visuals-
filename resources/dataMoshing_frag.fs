#version 110

#define kWINDOW_DIMENSIONS_WIDTH 1200
#define kWINDOW_DIMENSIONS_HEIGHT 800
#define kDATAMOSH_SPACING 45

#define kROW_COUNT kWINDOW_DIMENSIONS_HEIGHT / kDATAMOSH_SPACING
#define kCOLUMN_COUNT kWINDOW_DIMENSIONS_WIDTH / kDATAMOSH_SPACING
#define kCOLOR_SIZE kCOLUMN_COUNT * kROW_COUNT 
 
uniform vec3 colors[ kCOLOR_SIZE ];
uniform vec3 oldColors[ kCOLOR_SIZE ];
uniform float requiredDelta;
uniform float colorSize;

uniform vec2 windowSize;
uniform sampler2D tex0;

void main() {
	int ix = int( gl_TexCoord[0].t * float(kCOLUMN_COUNT) );
	int iy = int( gl_TexCoord[0].s * float(kROW_COUNT) );
	int index = int( ix * kROW_COUNT + iy );
	vec3 colorRGB = colors[ index ];

	float oldColor1D = oldColors[ index ].r + oldColors[ index ].g + oldColors[ index ].b;
	float newColor1D = colors[ index ].r + colors[ index ].g + colors[ index ].b;

	if( abs(newColor1D - oldColor1D) < requiredDelta*2.0 ) gl_FragColor.rgb = oldColors[ index ].rgb;
	else gl_FragColor.rgb = texture2D( tex0, gl_TexCoord[0].st  ).rgb;
	gl_FragColor.a = 1.0;
}
