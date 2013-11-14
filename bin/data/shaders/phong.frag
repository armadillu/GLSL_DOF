#extension GL_EXT_gpu_shader4 : require

varying vec3 normal, lightDir, eyeVec;
varying vec2 vTexCoord;

uniform sampler2D tex;
uniform sampler2D depthTex;
uniform float time;

uniform float specularGain;
uniform float specularClamp;
uniform float specularPow;
uniform float diffuseGain;
uniform float diffusePow;
uniform float width;
uniform float height;

void main( void ){

	// TEX /////////////////////////////////////
	vec4 texPix = texture2D(tex, vTexCoord);
	float x = gl_FragCoord.x / width;
	float y = gl_FragCoord.y / height;
	float depth = texture2D( depthTex, vec2(x,y) ).r;

	vec3 L = normalize(lightDir);
	vec3 N = normalize(normal) ;
	
	vec4 ambientV = vec4(0.0);
	vec4 diffuseV = vec4(0.0);
	vec4 specularV = vec4(0.0);
	
	// AMBIENT /////////////////////////////////
	ambientV = 	(gl_FrontLightModelProduct.sceneColor * gl_FrontMaterial.ambient) 
				+
		        (gl_LightSource[0].ambient * gl_FrontMaterial.ambient);
	
	// DIFFUSE /////////////////////////////////
	
	float lambertTerm = dot(N, L);
	
	if(lambertTerm  > 0.0){

		vec3 E = normalize(eyeVec);
		vec3 R = reflect(-L, N);

		diffuseV += diffuseGain * gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * pow(lambertTerm , diffusePow );

		// SPECULAR /////////////////////////////////
		float specular = specularGain * clamp( pow( max(dot(R,E), 0.0), gl_FrontMaterial.shininess * specularPow ), 0.0, specularClamp);
		specularV = gl_LightSource[0].specular * gl_FrontMaterial.specular * specular ;	
	}
	
	gl_FragColor = ambientV + diffuseV + specularV;
	//gl_FragColor = texPix;
	//float test = depth;
	//gl_FragColor = vec4(test, test, test, 1.0);

}

