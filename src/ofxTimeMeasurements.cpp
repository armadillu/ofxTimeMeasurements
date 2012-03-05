/*
 *  ofxTimeMeasurements.cpp
 *  emptyExample
 *
 *  Created by Oriol Ferrer Mesià on 28/01/11.
 *  Copyright 2011 uri.cat. All rights reserved.
 *
 */

#include "ofxTimeMeasurements.h"
#include <float.h>

ofxTimeMeasurements::ofxTimeMeasurements(){

	
}


void ofxTimeMeasurements::startMeasuring(string ID){

	TimeMeasurement t;
	t.measuring = true;
	t.microsecondsStart = ofGetElapsedTimeMicros();
	t.microsecondsStop = 0;
	t.duration = 0;
	t.error = true;
	times[ID] = t;
}


void ofxTimeMeasurements::stopMeasuring(string ID){
	
	map<string,TimeMeasurement>::iterator it;
	
	it = times.find(ID);
	
	if ( it == times.end() ){	//not found!
		
		ofLog( OF_LOG_WARNING, "ID (%s)not found at stopMeasuring(). Make sure you called startMeasuring with that ID first.", ID.c_str());
		
	}else{
		
		if ( times[ID].measuring ){

			TimeMeasurement t;
			t.measuring = false;
			t.error = false;
			t.microsecondsStop = ofGetElapsedTimeMicros();
			t.microsecondsStart = times[ID].microsecondsStart;
			t.duration = t.microsecondsStop - t.microsecondsStart;
			times[ID] = t;

		}else{	//wrong use, start first, then stop
			
			ofLog( OF_LOG_WARNING, "Can't stopMeasuring(%s). Make sure you called startMeasuring with that ID first.", ID.c_str());				
		}		
	}
}


void ofxTimeMeasurements::draw(int x, int y){

	int c = 1;

	ofDrawBitmapString( "-- Measured Times --", x, y + c * TIME_MEASUREMENTS_LINE_HEIGHT );
	
	for( map<string,TimeMeasurement>::iterator ii = times.begin(); ii != times.end(); ++ii ){

		c++;
		string key = (*ii).first;
		TimeMeasurement t = (*ii).second;
		if ( t.error == false ){
			//ofDrawBitmapString( " " + key + " = " + ofToString(t.duration / 1000.0f, 2) + "ms (" + ofToString(t.duration) + ")" , x, y + c * TIME_MEASUREMENTS_LINE_HEIGHT );
			ofDrawBitmapString( " " + key + " = " + ofToString(t.duration / 1000.0f, 2)  + "ms" , x, y + c * TIME_MEASUREMENTS_LINE_HEIGHT );
		}else{
			ofDrawBitmapString( " " + key + " = Usage Error! see log...", x, y + c * TIME_MEASUREMENTS_LINE_HEIGHT );
		}
	}
	c++;
	ofDrawBitmapString( "-------------------", x, y + c * TIME_MEASUREMENTS_LINE_HEIGHT );
}


unsigned long ofxTimeMeasurements::durationForID( string ID){

	map<string,TimeMeasurement>::iterator it;
	it = times.find(ID);
	
	if ( it == times.end() ){	//not found!
		
		if ( times[ID].error ){
			return times[ID].duration;
		}else{
			return 0;
		}
	}
}

