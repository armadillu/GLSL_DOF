#extension GL_EXT_gpu_shader4 : require

uniform sampler2D tex;
uniform sampler2D depthTex;
uniform float focusPoint;
uniform float blurSize;
uniform float blurGain;
uniform float depthDiffPow;
uniform float farClip;
uniform float nearClip;
uniform float showDOF;
uniform float width;
uniform float height;
uniform float focusRange;


varying vec2 vTexCoord;

float LinearizeDepth(float zoverw){
    return (2.0 * nearClip) / (farClip + nearClip - zoverw * (farClip - nearClip));  
} 

void main( void ){

	float focus =  focusPoint;
	
	// TEX /////////////////////////////////////
	vec4 texPix = texture2D(tex, vTexCoord);  
	
	// DEPTH /////////////////////////////////////
	float x = gl_FragCoord.x / width;
	float y = 1.0 - gl_FragCoord.y / height;
	vec4 depthPix = texture2D(depthTex, vec2(x,y) );  
	float depth = depthPix.r;	
    depth = LinearizeDepth(depth); 
        
	vec4 sum = vec4(0.0);
	vec2 dLoc;
	float d;
	vec2 loc;
	
	dLoc = vec2(x, y - 4.0 * blurSize);
	d = pow( max( -focusRange + abs(focus - texture2D(depthTex, dLoc).r), 0.0 ) , depthDiffPow);
	loc = vec2(x , y - 4.0 * blurSize * d); // tex neigtbor sample
	sum += blurGain * texture2D(tex, loc) * 0.05;
	
	dLoc = vec2(x, y  - 3.0 * blurSize);
	d = pow( max( -focusRange + abs(focus - texture2D(depthTex, dLoc).r), 0.0 ) , depthDiffPow);
	loc = vec2(x , y - 3.0 * blurSize * d); // tex neigtbor sample
	sum += blurGain * texture2D(tex, loc) * 0.09;
	
	dLoc = vec2(x , y - 2.0 * blurSize);
	d = pow( max( -focusRange + abs(focus - texture2D(depthTex, dLoc).r), 0.0 ) , depthDiffPow);
	loc = vec2(x, y  - 2.0 * blurSize * d);
	sum += blurGain * texture2D(tex, loc ) * 0.12;
	
	dLoc = vec2(x , y - blurSize);
	d = pow( max( -focusRange + abs(focus - texture2D(depthTex, dLoc).r), 0.0 ) , depthDiffPow);
	loc = vec2(x, y  - blurSize * d);
	sum += blurGain * texture2D(tex, loc ) * 0.15;
	
	/////////////////////////////////////////////////////////////////////////
	loc = vec2(x, y);
	sum += texture2D(tex, loc) ;
	/////////////////////////////////////////////////////////////////////////
	
	dLoc = vec2(x , y + blurSize);
	d = pow( max( -focusRange + abs(focus - texture2D(depthTex, dLoc).r), 0.0 ) , depthDiffPow);
	loc = vec2(x , y + blurSize * d);
	sum += blurGain * texture2D(tex, loc) * 0.15;
	
	dLoc = vec2(x , y + 2.0 * blurSize);
	d = pow( max( -focusRange + abs(focus - texture2D(depthTex, dLoc).r), 0.0 ) , depthDiffPow);
	loc = vec2(x, y  + 2.0 * blurSize * d);
	sum += blurGain * texture2D(tex, loc) * 0.12;
	
	dLoc = vec2(x, y  + 3.0 * blurSize);
	d = pow( max( -focusRange + abs(focus - texture2D(depthTex, dLoc).r), 0.0 ) , depthDiffPow);
	loc = vec2(x , y + 3.0 * blurSize * d);
	sum += blurGain * texture2D(tex, loc) * 0.09;
	
	dLoc = vec2(x, y  + 4.0 * blurSize);
	d = pow( max( -focusRange + abs(focus - texture2D(depthTex, dLoc).r), 0.0 ) , depthDiffPow);
	loc = vec2(x , y + 4.0 * blurSize * d);
	sum += blurGain * texture2D(tex, loc) * 0.05;

    sum.a = 1.0;

	//gl_FragColor =  vec4(1.0 - depth,texPix.g, 0.0, 1.0);
	if (showDOF == 1.0){
		gl_FragColor =  vec4(d, d,  d, 1.0);
	}else{
		gl_FragColor = sum;
	}
}