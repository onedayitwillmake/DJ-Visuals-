#version 110

uniform float aColor;
uniform vec2 windowSize;
uniform vec2 sampleOffset;

void main() {
	
	float steps = 5.0;
	float ix = floor(gl_TexCoord[0].s*steps) / steps;
	float iy = floor(gl_TexCoord[0].t*steps) / steps;
	// vec3 sum = vec3( 0.0, 0.0, 0.0 );
	// gl_FragColor.rgb = texture2D( abc, gl_TexCoord[0].st ).rgb;
	gl_FragColor.r = ix;//float( int( gl_TexCoord[0].s*windowSize.x ) ) / windowSize.x;
	gl_FragColor.g = iy;//float( int( gl_TexCoord[0].t*windowSize.y ) ) / windowSize.y;//sampleOffset.s;//float( int( gl_TexCoord[0].t * windowSize.y ) );
	gl_FragColor.b = 0.0;
	// gl_FragColor.rgb = sum;
	gl_FragColor.a = 1.0;
}
