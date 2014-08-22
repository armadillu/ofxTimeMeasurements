#pragma once

#include "ofMain.h"
#include "ofxTimeMeasurements.h"

static int tcount = 1;

class MyThread: public ofThread{

	void threadedFunction(){

		string ID = ofToString(tcount);
		tcount++;
		getPocoThread().setName("MyThread_" + ID);
		TS_START("task_" + ID);
		ofSleepMillis(1000 * 5); //sleep for a bit
		TS_STOP("task_" + ID);
	}
};

class testApp : public ofBaseApp,  ofThread{

	public:
		void setup();
		void update();
		void draw();
		void exit(){stopThread();}

		void keyPressed( ofKeyEventArgs & key );

		void threadedFunction();
		vector<MyThread*> myThreads;

};
