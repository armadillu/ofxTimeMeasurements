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
#define SEPARATOR "---------------------------------"

typedef struct TimeMeasurement{
	unsigned long microsecondsStart;
	unsigned long microsecondsStop;
	unsigned long duration;
	bool measuring;
	bool error;
};


class ofxTimeMeasurements {

	public :

		
		static ofxTimeMeasurements* instance();
		void startMeasuring(string ID);
		void stopMeasuring(string ID);

		void draw(int x, int y);
		unsigned long durationForID( string ID);
	
	private:

		ofxTimeMeasurements(){};  // Private so that it can  not be called
		static ofxTimeMeasurements* singleton;

		map<string, TimeMeasurement> times;
};

