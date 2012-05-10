#version 110

uniform vec3 colors[35];
uniform float colorSize;

uniform vec2 windowSize;
uniform vec2 sampleOffset;

void main() {
	float t = 10.0;
	float ix = floor(gl_TexCoord[0].s*colorSize);
	float iy = floor(gl_TexCoord[0].t*windowSize.y) / windowSize.y;
	// vec3 colorRGB = colors[ int( ix * 34.0 ) ];
	vec3 colorRGB = colors[  int( ix ) ];
	// vec3 sum = vec3( 0.0, 0.0, 0.0 );
	
	// gl_FragColor.rgb = texture2D( abc, gl_TexCoord[0].st ).rgb;
	// gl_FragColor.rgb = colorRGB;//float( int( gl_TexCoord[0].s*windowSize.x ) ) / windowSize.x;
	// gl_FragColor.g = ix;
	// gl_FragColor.b = iy;
	gl_FragColor.rgb = colorRGB;
	gl_FragColor.a = 1.0;
}
