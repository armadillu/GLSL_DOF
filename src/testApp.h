#pragma once

#include "ofMain.h"
#include "ofxAutoReloadedShader.h"
#include "ofxRemoteUIServer.h"


class testApp : public ofBaseApp{


	ofEasyCam	cam;
	ofxAutoReloadedShader*   shader;
	ofxAutoReloadedShader*   dofShaderH;
	ofxAutoReloadedShader*   dofShaderV;
	ofMaterial	mMatMainMaterial;

	ofFbo		fbo;
	ofFbo		ping;
	ofFbo		pong;

	ofVec3f		lightPos;
	ofLight		light;
	
	bool		isShaderDirty;

	ofColor matAmbient;
	ofColor matDiffuse;
	ofColor matSpecular;

	ofColor lightAmbient;
	ofColor lightDiffuse;
	ofColor lightSpecular;

	float nearClip, farClip;

	bool animateLight;
	bool animateCam;
	bool doShader;
	bool doDOFShader;
	bool drawAxes;

	float lightSpeed;
	float lightDist;
	float timeSpeed;
	float lightH;

	float diffuseGain, diffusePow;
	float specularGain, specularClamp, specularPow;

	float focusPoint;
	float blurSize;
	float blurGain;
	float focusRange;
	float depthDiffPow;
	bool showDOF;

	ofImage tex;
	ofImage tex2;

	public:

	void setup();
	void update();
	void draw();

	void exit();

	void keyPressed  (int key);

	ofMesh quad;
};
