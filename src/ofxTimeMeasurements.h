/*
 *  ofxTimeMeasurements.h
 *  emptyExample
 *
 *  Created by Oriol Ferrer Mesià on 28/01/11.
 *  Copyright 2011 uri.cat. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include <map>

#define TIME_MEASUREMENTS_LINE_HEIGHT 14
#define SEPARATOR "---------------------------"

typedef struct TimeMeasurement{
	unsigned long microsecondsStart;
	unsigned long microsecondsStop;
	unsigned long duration;
	bool measuring;
	bool error;
};


class ofxTimeMeasurements: public ofBaseDraws {

	public :

		ofxTimeMeasurements();
		static ofxTimeMeasurements* instance();
	
		void setDesiredFrameRate(float fr);	//forced to do this as I can't access desiredFrameRate once set with ofSetFrameRate
		void startMeasuring(string ID);
		void stopMeasuring(string ID);
		void setEnabled( bool enable );
		bool getEnabled();
		unsigned long durationForID( string ID);
	
		void draw(float x, float y);
		void draw(float x, float y, float w , float h ){ draw(x,y); } //w and h ignored! just here to comply with ofBaseDraws
		virtual float getWidth(){ return ((string)(SEPARATOR)).length() * 8; }
		virtual float getHeight(){ return ( 4 + times.size() ) * TIME_MEASUREMENTS_LINE_HEIGHT; };
	
	private:

		static ofxTimeMeasurements* singleton;
		float desiredFrameRate;
		bool enabled;
		map<string, TimeMeasurement> times;

};

