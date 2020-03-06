varying vec2 varTexCoord;
 
 void main(void)
 {
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    
    varTexCoord = vec2(gl_MultiTexCoord0);
 }
