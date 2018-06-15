#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"
#include "ofxTimeMeasurements.h"

int main( ){

	ofGLFWWindowSettings settings;
	#ifdef NANOVG_GL2_IMPLEMENTATION
	settings.setGLVersion(2, 1);  // Fixed pipeline
	#endif
	#ifdef NANOVG_GL3_IMPLEMENTATION
	settings.setGLVersion(3, 2);  // Programmable pipeline >> you need to define GL_VERSION_3 in you pre-processor macros!
	#endif
	settings.stencilBits = 0;
	int w = 800;
	int h = 600;
	#if OF_VERSION_MINOR < 10
	settings.width = w;
	settings.height = w;
	#else
	settings.setSize(w, h);
	#endif

	ofCreateWindow(settings);

	TIME_SAMPLE_ADD_SETUP_HOOKS();
	TIME_SAMPLE_SET_FRAMERATE(60);	//if you want setup() to get automaticall measured,
	//you need to create the instance once before setup() is called;
	//just call any TIME_SAMPLE_* method to do so

	ofRunApp(new ofApp());
}

