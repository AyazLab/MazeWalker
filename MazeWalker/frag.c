uniform sampler2D baseMap;
 
 varying vec3 N;
 varying vec3 v;
 varying vec2 varTexCoord;
 
 
 vec4 lightSrc (in int lightNum)
 {
 vec4 color;
 
 
 vec3 L = gl_LightSource[lightNum].position.xyz - v; 
 //vec3 E = normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0)
 //vec3 R = normalize(-reflect(L,N)); 
 
 float dist = length(L);
 L = normalize(L);
 
 float atten = (gl_LightSource[lightNum].constantAttenuation +
               gl_LightSource[lightNum].linearAttenuation * dist +
               gl_LightSource[lightNum].quadraticAttenuation * dist * dist);
 
 //calculate Ambient Term:
 //vec4 Iamb = gl_LightSource[lightNum].ambient*gl_FrontMaterial.ambient;
 
 //calculate Diffuse Term:
 float dotted = dot(L,N);
if(dotted/(dist*length(N))<0.0)
{
	dotted=-dotted;
}
 //vec4 Idiff = (gl_LightSource[lightNum].diffuse * max(dot(N,L), 0.0));
vec4 Idiff = (gl_LightSource[lightNum].diffuse * dotted);
 
 // calculate Specular Term:
 //vec4 Ispec = gl_FrontMaterial.specular * pow(max(dot(R,E),0.0),0.3*gl_FrontMaterial.shininess);
 
 color = (Idiff)/atten;
 return color;
 }
 

 void main (void)
 {
 // write Total Color:
 vec4 buff=((lightSrc(0)+lightSrc(1)+lightSrc(2)+lightSrc(3)+lightSrc(4)+lightSrc(5))*gl_FrontMaterial.diffuse+(gl_LightModel.ambient*gl_FrontMaterial.ambient));
 if(texture2D(baseMap, varTexCoord)[3]>0.0)
	 gl_FragColor =buff*texture2D(baseMap, varTexCoord);
 else
	gl_FragColor=buff;
	
 }
