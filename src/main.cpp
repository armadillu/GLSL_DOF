#include "ofMain.h"
#include "testApp.h"
#include "ofAppGLFWWindow.h"


int main( ){

	//ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);

	ofAppGLFWWindow win;
	win.setNumSamples(4);
	win.setMultiDisplayFullscreen(true);

	ofSetupOpenGL(&win, 640,480, OF_WINDOW);
	//ofSetupOpenGL(&win, 1024,690, OF_FULLSCREEN);

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new testApp());

}
