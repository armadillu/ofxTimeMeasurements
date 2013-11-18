#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){

	ofEnableSmoothing();
	ofEnableAlphaBlending();
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(22, 22, 22, 255);

	TIME_SAMPLE_SET_FRAMERATE( 60.0f ); //set the app's target framerate (MANDATORY)

	//draw on a pre-specified corner, or supply a custom onscreen location:
	TIME_SAMPLE_SET_DRAW_LOCATION( TIME_MEASUREMENTS_TOP_RIGHT ); //specify a drawing location (OPTIONAL)
	//TIME_SAMPLE_SET_DRAW_LOCATION( TIME_MEASUREMENTS_CUSTOM_LOCATION, ofVec2f(40,40) );
}


//--------------------------------------------------------------
void testApp::update(){

}


//--------------------------------------------------------------
void testApp::draw(){

	//let's draw lots of stuff
	for(int i = 0; i < ofGetMouseX() * 100; i++){
		ofSetColor( ofRandom(96) );
		ofRect( ofRandom( ofGetWidth()), ofRandom( ofGetHeight()), 4, 4);
	}

	//measure time of a particular bit of code
	TIME_SAMPLE_START("test");	///////////////////////////////  START MEASURING ///

	ofSleepMillis(5);

	TIME_SAMPLE_STOP("test");	///////////////////////////////  STOP MEASURING  ///


	ofSetColor(255);
	ofDrawBitmapString("move mouse to the right to incrase draw complexity",  10, ofGetHeight() - 10);
}

