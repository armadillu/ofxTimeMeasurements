#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){

	ofEnableSmoothing();
	ofEnableAlphaBlending();
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(22, 22, 22, 255);

	TIME_SAMPLE_SET_FRAMERATE( 60.0f ); 
}


//--------------------------------------------------------------
void testApp::update(){

	TIME_SAMPLE_START("update"); //////////////////////////////////////////////  START MEASURING ///
	
		//Put your update code here
	
	TIME_SAMPLE_STOP("update"); ///////////////////////////////////////////////  STOP MEASURING  ///
}


//--------------------------------------------------------------
void testApp::draw(){

	TIME_SAMPLE_START("draw"); ////////////////////////////////////////////////  START MEASURING ///
	
		//let's draw lots of stuff
		for(int i = 0; i < ofGetMouseX() * 40; i++){
			ofSetColor( ofRandom(96) );
			ofRect( ofRandom( ofGetWidth()), ofRandom( ofGetHeight()), 4, 4);
		}
		ofSetColor(255);
		ofDrawBitmapString("move mouse to the right to incrase draw complexity",  10, ofGetHeight() - 10);

	TIME_SAMPLE_STOP("draw"); /////////////////////////////////////////////////  STOP MEASURING  ///
	
	TIME_SAMPLE_DRAW( 10, 10); 	//finally draw our time measurements
}

