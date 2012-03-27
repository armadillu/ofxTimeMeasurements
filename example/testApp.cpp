#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){

	ofEnableSmoothing();
	ofEnableAlphaBlending();
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(22, 22, 22, 255);

	chrono = ofxTimeMeasurements::instance();
	chrono->setDesiredFrameRate(60.0);

}

//--------------------------------------------------------------
void testApp::update(){

	chrono->startMeasuring("update"); //////////////////////////////////////////////  START MEASURING ///
	
	//Put your update code here
	
	chrono->stopMeasuring("update"); ///////////////////////////////////////////////  STOP MEASURING  ///
}

//--------------------------------------------------------------
void testApp::draw(){

	chrono->startMeasuring("draw"); ////////////////////////////////////////////////  START MEASURING ///
	
	//let's draw lots of stuff
	for(int i = 0; i < ofGetMouseX() * 40; i++){
		ofSetColor( ofRandom(96) );
		ofRect( ofRandom( ofGetWidth()), ofRandom( ofGetHeight()), 4, 4);		
	}
	ofSetColor(255);
	ofDrawBitmapString("move mouse to the right to incrase draw complexity, see draw time increase",  100, ofGetHeight() - 20);

	chrono->stopMeasuring("draw"); /////////////////////////////////////////////////  STOP MEASURING  ///

	chrono->draw( ofGetWidth() - chrono->getWidth() - 10, 10); 	//finally draw our time measurements
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}