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

ofxTimeMeasurements* ofxTimeMeasurements::singleton = NULL; 

ofxTimeMeasurements::ofxTimeMeasurements(){
	desiredFrameRate = 60.0f;
	enabled = true;
	timeAveragePercent = 0.1; 
}

ofxTimeMeasurements* ofxTimeMeasurements::instance(){	
	if (!singleton){   // Only allow one instance of class to be generated.
		singleton = new ofxTimeMeasurements();
	}
	return singleton;
}

void ofxTimeMeasurements::startMeasuring(string ID){

	if (!enabled) return;
	
	TimeMeasurement t;
	t.measuring = true;
	t.microsecondsStart = ofGetElapsedTimeMicros();
	t.microsecondsStop = 0;
	t.duration = 0;
	t.avgDuration = times[ID].avgDuration;
	t.error = true;
	times[ID] = t;
}


void ofxTimeMeasurements::stopMeasuring(string ID){

	if (!enabled) return;
	
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
			t.avgDuration = (1.0f - timeAveragePercent) * times[ID].avgDuration + t.duration * timeAveragePercent;
			times[ID] = t;

		}else{	//wrong use, start first, then stop
			
			ofLog( OF_LOG_WARNING, "Can't stopMeasuring(%s). Make sure you called startMeasuring with that ID first.", ID.c_str());				
		}		
	}
}


void ofxTimeMeasurements::draw(float x, float y){

	if (!enabled) return;
	static char msChar[50];
	static char percentChar[50];
	static char msg[100];
	
	int c = 1;
	float timePerFrame = 1000.0f / desiredFrameRate;
	
	ofDrawBitmapString( SEPARATOR, x, y + c * TIME_MEASUREMENTS_LINE_HEIGHT );
	float percentTotal = 0.0f;
	for( map<string,TimeMeasurement>::iterator ii = times.begin(); ii != times.end(); ++ii ){

		c++;
		string key = (*ii).first;
		TimeMeasurement t = (*ii).second;
		float ms = t.avgDuration / 1000.0f;	
		float percent = 100.0f * ms / timePerFrame;
		
		if ( t.error == false ){
			sprintf(msChar, "%*.2f", 4, ms );			
			sprintf(percentChar, "%*.1f", 2, percent );
			ofDrawBitmapString( " " + key + " = " + msChar + "ms (" + percentChar+ "\%)" , x, y + c * TIME_MEASUREMENTS_LINE_HEIGHT );
		}else{
			ofDrawBitmapString( " " + key + " = Usage Error! see log...", x, y + c * TIME_MEASUREMENTS_LINE_HEIGHT );
		}
		percentTotal += percent;
	}
		
	bool missingFrames = ( ofGetFrameRate() < desiredFrameRate - 1.0 ); // tolerance of 1 fps
	
	c++;
	ofDrawBitmapString( SEPARATOR, x, y + c * TIME_MEASUREMENTS_LINE_HEIGHT );
		
	if ( missingFrames ) {
		ofPushStyle();
		ofSetColor(255, 0, 0);
	}

	sprintf(msg, " App fps %*.1f (%*.1f%% busy)", 4, ofGetFrameRate(), 3, percentTotal );
	c++;
	ofDrawBitmapString( msg, x, y + c * TIME_MEASUREMENTS_LINE_HEIGHT );
	
	if ( missingFrames ) {
		ofPopStyle();
	}
	
	c++;
	ofDrawBitmapString( SEPARATOR, x, y + c * TIME_MEASUREMENTS_LINE_HEIGHT );
}


unsigned long ofxTimeMeasurements::durationForID( string ID){

	map<string,TimeMeasurement>::iterator it;
	it = times.find(ID);
	
	if ( it == times.end() ){	//not found!
		
		if ( times[ID].error ){
			return times[ID].duration;
		}
	}
	return 0;
}

void ofxTimeMeasurements::setTimeAveragePercent(float p){
	timeAveragePercent = ofClamp(p, 0, 1);
}

void ofxTimeMeasurements::setDesiredFrameRate(float fr){
	desiredFrameRate = fr;
}

void ofxTimeMeasurements::setEnabled(bool ena){
	enabled = ena;
}

bool ofxTimeMeasurements::getEnabled(){
	return enabled;
}
