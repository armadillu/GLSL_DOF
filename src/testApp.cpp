#include "testApp.h"
#include <GLUT/GLUT.h>

//--------------------------------------------------------------
void testApp::setup(){

	ofSetVerticalSync(true);
	ofSetFrameRate(65);
	ofEnableAlphaBlending();

	ofDisableArbTex();
	tex.loadImage("tex.png");
	tex2.loadImage("lavaNormal.png");
	//ofEnableArbTex();

	ofSetWindowPosition(10, 10);

	isShaderDirty = true;

	cam.setFov(60);
	cam.setDistance(1000);

	light.enable();
	glEnable(GL_DEPTH_TEST);

	OFX_REMOTEUI_SERVER_SETUP(); 	//start server

	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_GROUP("SHADER");
	OFX_REMOTEUI_SERVER_SHARE_PARAM(doShader);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(doDOFShader);

	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(diffuseGain,0,2);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(diffusePow,0,2.0);

	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(specularGain,0,2);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(specularClamp,0,1);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(specularPow,0,2);

	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_GROUP("LIGHT");
	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(timeSpeed, 0, 1);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(lightSpeed,0,10);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(lightH,-100,100);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(lightDist,100,600);

	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_GROUP("CAMERA");
	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(focusPoint,0,1);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(blurSize,0,0.02);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(blurGain,0.0,10.0);

	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(depthDiffPow,0,3.0);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(focusRange,0,0.6);

	OFX_REMOTEUI_SERVER_SHARE_PARAM(showDOF);

	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(nearClip,10,1000);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(farClip,10,1000);

	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(animateLight);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(animateCam);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(drawAxes);

	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_GROUP("COLORS");
	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(matAmbient);
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(matDiffuse);
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(matSpecular);
	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(lightAmbient);
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(lightDiffuse);
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(lightSpecular);

	OFX_REMOTEUI_SERVER_LOAD_FROM_XML();

	quad.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
	quad.addVertex(ofVec2f(0,0));
	quad.addVertex(ofVec2f(ofGetWidth(),0));
	quad.addVertex(ofVec2f(ofGetWidth(),ofGetHeight()));
	quad.addVertex(ofVec2f(0,ofGetHeight()));
	quad.addTexCoord(ofVec2f(0,0));
	quad.addTexCoord(ofVec2f(1,0));
	quad.addTexCoord(ofVec2f(1,1));
	quad.addTexCoord(ofVec2f(0,1));

	ofFbo::Settings s;
	s.width = ofGetWidth();
	s.height = ofGetHeight();
	s.internalformat = GL_RGBA;
	//s.textureTarget = GL_TEXTURE_RECTANGLE_ARB;
	s.maxFilter = GL_LINEAR; GL_NEAREST;
	s.numSamples = 0;
	s.numColorbuffers = 3;
	s.useDepth = true;
	s.useStencil = false;
	s.depthStencilAsTexture = true;

	fbo.allocate(s);
	ping.allocate(s);
	pong.allocate(s);
}

//--------------------------------------------------------------
void testApp::update(){

	OFX_REMOTEUI_SERVER_UPDATE(0.01666);

	if (isShaderDirty){
		
		GLuint err = glGetError();	// we need this to clear out the error buffer.
		
		if (shader != NULL ) delete shader;
		shader = new ofxAutoReloadedShader();
		shader->setMillisBetweenFileCheck(100);
		shader->load("shaders/phong");
		err = glGetError();	// we need this to clear out the error buffer.
		ofLogNotice() << "Loaded Shader: " << err;

		if (dofShaderH != NULL ) delete dofShaderH;
		dofShaderH = new ofxAutoReloadedShader();
		dofShaderH->setMillisBetweenFileCheck(100);
		dofShaderH->load("shaders/dofH");
		err = glGetError();	// we need this to clear out the error buffer.
		ofLogNotice() << "Loaded Shader: " << err;

		if (dofShaderV != NULL ) delete dofShaderV;
		dofShaderV = new ofxAutoReloadedShader();
		dofShaderV->setMillisBetweenFileCheck(100);
		dofShaderV->load("shaders/dofV");
		err = glGetError();	// we need this to clear out the error buffer.
		ofLogNotice() << "Loaded Shader: " << err;

		isShaderDirty = false;
	}

	mMatMainMaterial.setAmbientColor( matAmbient);
	mMatMainMaterial.setDiffuseColor(matDiffuse);
	mMatMainMaterial.setSpecularColor(matSpecular);
	mMatMainMaterial.setShininess(25.0f);

	light.setAmbientColor(lightAmbient);
	light.setDiffuseColor(lightDiffuse);
	light.setSpecularColor(lightSpecular);

	if(animateLight || ofGetFrameNum() < 2){
		float r = lightDist;
		lightPos = ofVec3f(r * (sin(lightSpeed * ofGetElapsedTimef())),
						   lightH + 50,
						   r * (cos(lightSpeed * ofGetElapsedTimef()) ) );
	}
	light.setGlobalPosition(lightPos);

	cam.setNearClip(nearClip);
	cam.setFarClip(farClip);

	if(animateCam || ofGetFrameNum() < 2){
		float r = lightDist;
		ofVec3f camPos = ofVec3f(
								 r * sin(lightSpeed * ofGetElapsedTimef() * 1.2),
								 lightH,
								 r * cos(lightSpeed * ofGetElapsedTimef() * 1.2)
								 );

		cam.setGlobalPosition( camPos );
		cam.setTarget(ofVec3f());
	}

	if (ofGetFrameNum()%15 == 1) ofSetWindowTitle( ofToString( ofGetFrameRate()) );
}

//--------------------------------------------------------------
void testApp::draw(){

	ofClear(0, 0, 0, 0);

	ofBackgroundGradient(ofColor::fromHsb(0, 0, 120), ofColor::fromHsb(0, 0, 0));

	fbo.begin();
		ofClear(0,0,0,0);
		cam.begin();
			ofSetColor(255);

			ofEnableDepthTest();
			ofEnableLighting();
			light.enable();
			light.draw();
			glDisable(GL_COLOR_MATERIAL);
			mMatMainMaterial.begin();

			if(doShader){
				shader->begin();
				shader->setUniformTexture("tex", tex, 0);
				shader->setUniformTexture("depthTex", fbo.getDepthTexture(), 1);
				shader->setUniform1f("time", ofGetElapsedTimef() * timeSpeed);

				shader->setUniform1f("width", fbo.getWidth());
				shader->setUniform1f("height", fbo.getHeight());

				shader->setUniform1f("diffuseGain", diffuseGain);
				shader->setUniform1f("diffusePow", diffusePow);

				shader->setUniform1f("specularGain", specularGain);
				shader->setUniform1f("specularClamp", specularClamp);
				shader->setUniform1f("specularPow", specularPow);
			}

			glutSolidTeapot(80);

			ofSeedRandom(50);
			for(int i = 0; i < 40; i++){
				ofDrawCylinder(ofRandom(-350,350), 0, ofRandom(-350, 350), 10, 200);
			}

			if(doShader){
				shader->end();
			}

			mMatMainMaterial.end();
			ofDisableLighting();

			ofSetColor(255);
			if(drawAxes)ofDrawAxis(100);

		cam.end();
	fbo.end();

	ofDisableDepthTest();
	ofEnableNormalizedTexCoords();


	if(doDOFShader){
		ping.begin();
		ofClear(0, 0);

		dofShaderH->begin();
		dofShaderH->setUniformTexture("tex", fbo.getTextureReference(), 0);
		dofShaderH->setUniformTexture("depthTex", fbo.getDepthTexture(), 1);
		dofShaderH->setUniform1f("focusPoint", focusPoint);
		dofShaderH->setUniform1f("width", fbo.getWidth());
		dofShaderH->setUniform1f("height", fbo.getHeight());
		dofShaderH->setUniform1f("blurSize", blurSize);
		dofShaderH->setUniform1f("blurGain", blurGain);
		dofShaderH->setUniform1f("depthDiffPow", depthDiffPow);
		dofShaderH->setUniform1f("nearClip", nearClip);
		dofShaderH->setUniform1f("farClip", farClip);
		dofShaderH->setUniform1f("showDOF", showDOF ? 1.0f : 0.0f);
		dofShaderH->setUniform1f("focusRange", focusRange);

		//ofRect(0, 0, ofGetWidth(), ofGetHeight()); // << no tex coords on an ofRect! damn!
		quad.draw();

		dofShaderH->end();

		ping.end();

		pong.begin();
		ofClear(0,0);
		dofShaderV->begin();
		dofShaderV->setUniformTexture("tex", ping.getTextureReference(), 0);
		dofShaderV->setUniformTexture("depthTex", fbo.getDepthTexture(), 1);
		dofShaderV->setUniform1f("focusPoint", focusPoint);
		dofShaderV->setUniform1f("width", fbo.getWidth());
		dofShaderV->setUniform1f("height", fbo.getHeight());
		dofShaderV->setUniform1f("blurSize", blurSize);
		dofShaderV->setUniform1f("blurGain", blurGain);
		dofShaderV->setUniform1f("depthDiffPow", depthDiffPow);
		dofShaderV->setUniform1f("nearClip", nearClip);
		dofShaderV->setUniform1f("farClip", farClip);
		dofShaderV->setUniform1f("showDOF", showDOF ? 1.0f : 0.0f);
		dofShaderV->setUniform1f("focusRange", focusRange);

		//ofRect(0, 0, ofGetWidth(), ofGetHeight()); // << no tex coords on an ofRect! damn!
		quad.draw();

		dofShaderV->end();
		pong.end();

		pong.draw(0, 0);

	}else{
		fbo.draw(0,0);
	}

	float s = 0.1;
	fbo.getDepthTexture().draw(
							   ofGetWidth() * (1-s) , ofGetHeight() * (1-s),
							   ofGetWidth() * s, ofGetHeight() * s
							   );

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}


void testApp::exit(){
	OFX_REMOTEUI_SERVER_CLOSE();
	OFX_REMOTEUI_SERVER_SAVE_TO_XML();
}
