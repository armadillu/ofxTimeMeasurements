#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){

	ofEnableSmoothing();
	ofEnableAlphaBlending();
	ofSetVerticalSync(true);
	ofSetFrameRate(63);
	ofBackground(22, 22, 22, 255);

	TIME_SAMPLE_SET_FRAMERATE( 60.0f ); //set the app's target framerate (MANDATORY)

	//draw on a pre-specified corner, or supply a custom onscreen location:
	TIME_SAMPLE_SET_DRAW_LOCATION( TIME_MEASUREMENTS_TOP_RIGHT ); //specify a drawing location (OPTIONAL)
	//TIME_SAMPLE_SET_DRAW_LOCATION( TIME_MEASUREMENTS_CUSTOM_LOCATION, ofVec2f(40,40) ); //custom location allowed

	TIME_SAMPLE_SET_AVERAGE_RATE(0.07); //averaging samples, [0..1], 1 gets you no averaging at all
										//use lower values to get steadier readings
}


//--------------------------------------------------------------
void testApp::update(){
}


//--------------------------------------------------------------
void testApp::draw(){

	TIME_SAMPLE_START("draw dots");	///////////////////////////////  START MEASURING ///
	for(int i = 0; i < ofGetMouseX() * 5; i++){
		ofSetColor( ofRandom(96) );
		ofRect( ofRandom( ofGetWidth()), ofRandom( ofGetHeight()), 4, 4);
	}
	TIME_SAMPLE_STOP("draw dots");	///////////////////////////////  STOP MEASURING  ///


	//testing nested samples
	TIME_SAMPLE_START("Nested Test");	///////////////////////////////  START MEASURING ///
		TIME_SAMPLE_START("Nested Test 2");
			TIME_SAMPLE_START("Nested Test 3");
				ofSleepMillis(1);
			TIME_SAMPLE_STOP("Nested Test 3");
		TIME_SAMPLE_STOP("Nested Test 2");
	TIME_SAMPLE_STOP("Nested Test");	///////////////////////////////  STOP MEASURING  ///

	ofSetColor(255);
	ofDrawBitmapString("Move mouse to the right to incrase draw complexity.\n"
					   "Press 'Right Shift' key to toggle samping\n"
					   "Press arrows keys to select/collapse",
					   10, ofGetHeight() - 40);
}

