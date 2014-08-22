#include "testApp.h"


void testApp::setup(){

	ofBackground(22);

	TIME_SAMPLE_SET_FRAMERATE( 60.0f ); //set the app's target framerate (MANDATORY)

	//specify where the widget is to be drawn
	TIME_SAMPLE_SET_DRAW_LOCATION( TIME_MEASUREMENTS_TOP_RIGHT ); //specify a drawing location (OPTIONAL)


	TIME_SAMPLE_SET_AVERAGE_RATE(0.1);	//averaging samples, (0..1],
										//1.0 gets you no averaging at all
										//use lower values to get steadier readings
	TIME_SAMPLE_DISABLE_AVERAGE();	//disable averaging

	startThread();
}

void testApp::threadedFunction(){

	getPocoThread().setName("MyLoopingThread");

	while(isThreadRunning()){
		TS_START("task");
			ofSleepMillis(30);
			TS_START("subtask1");
			ofSleepMillis(2030);
			TS_STOP("subtask1");

		TS_STOP("task");
		ofSleepMillis(100);
	}
}


void testApp::update(){

	if(TS_START("some common method")){
		ofSleepMillis(1);
	}TS_STOP("some common method");


	if (ofGetFrameNum()%60 == 1){
		TS_START("sample across frames");
	}

	if (ofGetFrameNum()%60 == 3){
		TS_STOP("sample across frames");
	}

	if (ofGetFrameNum()%60 == 30 || ofGetFrameNum()%60 == 1){
		if (TS_START("some uncommon method")){
			ofSleepMillis(ofRandom(3));
		}TS_STOP("some uncommon method");
	}

	//test accumulation time sampling
	for(int i = 0; i < 3; i++){
		TS_START_ACC("accum test");
		ofSleepMillis(1);
		TS_STOP_ACC("accum test");
	}

	for(int i = myThreads.size() - 1; i >= 0 ; i--){
		if (!myThreads[i]->isThreadRunning()){
			delete myThreads[i];
			myThreads.erase(myThreads.begin() + i);
		}
	}
}


void testApp::draw(){

	if( TS_START("draw dots") ){	///////////////////////////////  START MEASURING ///
		for(int i = 0; i < ofGetMouseX() * 5; i++){
			ofSetColor( ofRandom(96) );
			ofRect( ofRandom( ofGetWidth()), ofRandom( ofGetHeight()), 4, 4);
		}
	}TS_STOP("draw dots");			///////////////////////////////  STOP MEASURING  ///


	//testing late samples
	if (ofGetFrameNum() > 10){
		TS_START("Nested Test L2");
		TS_STOP("Nested Test L2");
	}

	ofSetColor(255);
	ofDrawBitmapString("Move mouse to the right to incrase draw complexity\n"
					   "Notice how drawing more cubes takes longer\n"
					   "Press 'PAGE_DOWN' key to toggle time sampling\n"
					   "Press 'T' key to toggle interactive mode\n"
					   "When in interactive mode:\n"
					   "    press up/down arrows keys to select\n"
					   "    press left/right arrow keys to collapse/expand\n"
					   "    press 'RETURN' key to toggle the execution of the selected section\n",
					   10,
					   ofGetHeight() - 104);
}


void testApp::keyPressed( ofKeyEventArgs & key ){

	TS_START("keyDown");
	if(myThreads.size() < 6){
		MyThread *t = new MyThread();
		t->startThread();
		myThreads.push_back(t);
	}
	TS_STOP("keyDown");
}
