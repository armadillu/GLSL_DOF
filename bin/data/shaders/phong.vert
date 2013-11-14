#extension GL_EXT_gpu_shader4 : require

varying vec3 normal, lightDir, eyeVec;
varying vec2 vTexCoord;

void main( void ){

	vTexCoord = gl_MultiTexCoord0.xy; 
	normal = gl_NormalMatrix * gl_Normal;	
	vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);
	
	lightDir = vec3(gl_LightSource[0].position.xyz - vVertex);
	eyeVec = -vVertex;
	
	gl_Position = ftransform();
}