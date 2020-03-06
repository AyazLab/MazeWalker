uniform sampler2D baseMap;
 
 varying vec2 varTexCoord;
 
 
 void main (void)
 {
 // write Total Color:
	 gl_FragColor =texture2D(baseMap, varTexCoord);

	
 }