#version 110

uniform float aColor;
uniform vec2 windowSize;
uniform vec2 sampleOffset;

void main() {
	// vec3 sum = vec3( 0.0, 0.0, 0.0 );
	// gl_FragColor.rgb = texture2D( abc, gl_TexCoord[0].st ).rgb;
	gl_FragColor.r = float( int( gl_TexCoord[0].s*windowSize.x ) ) / windowSize.x;
	gl_FragColor.g = float( int( gl_TexCoord[0].t*windowSize.y ) ) / windowSize.y;//sampleOffset.s;//float( int( gl_TexCoord[0].t * windowSize.y ) );
	gl_FragColor.b = 0.0;
	// gl_FragColor.rgb = sum;
	gl_FragColor.a = 1.0;
}
