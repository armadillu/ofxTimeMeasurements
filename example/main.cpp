#include "ofMain.h"
#include "testApp.h"
#include "ofAppGLFWWindow.h"
#include "ofxTimeMeasurements.h"

int main( ){
	ofAppGLFWWindow win;
	win.setNumSamples(8);
	//win.setMultiDisplayFullscreen(true);

	ofSetupOpenGL(&win, 800, 600, OF_WINDOW);	// <-------- setup the GL context

	TIME_SAMPLE_SET_FRAMERATE(60);

	ofRunApp(new testApp());
}
