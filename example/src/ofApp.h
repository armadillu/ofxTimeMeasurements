#pragma once

#include "ofMain.h"
#include "ofxTimeMeasurements.h"

static int tcount = 1;

class MyThread: public ofThread{

	void threadedFunction(){

		string ID = ofToString(tcount);
		tcount++;
		TS_START("task_" + ID);
		ofSleepMillis(ofRandom(1,500)); //sleep for a bit
		TS_STOP("task_" + ID);
	}
};


class ofApp : public ofBaseApp,  ofThread{

	public:
		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed( ofKeyEventArgs & key );

		void threadedFunction();
		vector<MyThread*> myThreads;
};
