#extension GL_EXT_gpu_shader4 : require
varying vec2 vTexCoord;

void main( void ){

	vTexCoord = gl_MultiTexCoord0.xy;  
	gl_Position = ftransform();
}