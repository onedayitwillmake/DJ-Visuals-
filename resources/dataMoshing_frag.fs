#version 110

#define kWINDOW_DIMENSIONS_WIDTH 1200
#define kWINDOW_DIMENSIONS_HEIGHT 800
#define kDATAMOSH_SPACING 100

#define kROW_COUNT kWINDOW_DIMENSIONS_HEIGHT / kDATAMOSH_SPACING
#define kCOLUMN_COUNT kWINDOW_DIMENSIONS_WIDTH / kDATAMOSH_SPACING
#define kCOLOR_SIZE kCOLUMN_COUNT * kROW_COUNT 
 
uniform vec3 colors[ kCOLOR_SIZE ];
uniform float colorSize;

uniform vec2 windowSize;

void main() {
	int ix = int( gl_TexCoord[0].t * float(kCOLUMN_COUNT) );
	int iy = int( gl_TexCoord[0].s * float(kROW_COUNT) );
	int index = int( ix * kROW_COUNT + iy );
	vec3 colorRGB = colors[  index ];
	gl_FragColor.rgb = colorRGB;
	gl_FragColor.a = 1.0;
}
