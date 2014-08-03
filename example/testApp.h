#pragma once

#include "ofMain.h"
#include "ofxTimeMeasurements.h"

class testApp : public ofBaseApp,  ofThread{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed( ofKeyEventArgs & key );

		void threadedFunction();

};
