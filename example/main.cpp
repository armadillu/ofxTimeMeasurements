#include "ofMain.h"
#include "testApp.h"
#include "ofAppGLFWWindow.h"
#include "ofxTimeMeasurements.h"

int main( ){
	ofAppGLFWWindow win;
	win.setNumSamples(8);
	//win.setMultiDisplayFullscreen(true);

	ofSetupOpenGL(&win, 800, 600, OF_WINDOW);	// <-------- setup the GL context

	TIME_SAMPLE_SET_FRAMERATE(60);	//if you want setup() to get automaticall measured, you need to create the instance once;
									//just call any TIME_SAMPLE_* method to do so

	ofRunApp(new testApp());
}
