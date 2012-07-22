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

#define TIME_SAMPLE_SET_FRAMERATE(x)	(ofxTimeMeasurements::instance()->setDesiredFrameRate(x))
#define TIME_SAMPLE_START(x)			(ofxTimeMeasurements::instance()->startMeasuring(x))
#define TIME_SAMPLE_STOP(x)				(ofxTimeMeasurements::instance()->stopMeasuring(x))
#define TIME_SAMPLE_DRAW(x,y)			(ofxTimeMeasurements::instance()->draw(x,y))
#define TIME_SAMPLE_ENABLE				(ofxTimeMeasurements::instance()->setEnabled(true))
#define TIME_SAMPLE_DISABLE				(ofxTimeMeasurements::instance()->setEnabled(false))
#define TIME_SAMPLE_SET_AVERAGE_RATE(x)	(ofxTimeMeasurements::instance()->setTimeAveragePercent(x))

class ofxTimeMeasurements: public ofBaseDraws {

	public :

		ofxTimeMeasurements();
		static ofxTimeMeasurements* instance();
	
		void setDesiredFrameRate(float fr);	//forced to do this as I can't access desiredFrameRate once set with ofSetFrameRate
											//affects the % busy indicator
		void startMeasuring(string ID);
		void stopMeasuring(string ID);
		void setEnabled( bool enable );
		bool getEnabled();
		void setTimeAveragePercent(float p); //[0..1] >> if set to 1.0, 100% of every new sample contributes to the average.
											 //if set to 0.1, a new sample contributes 10% to the average
		unsigned long durationForID( string ID);
	
		void draw(float x, float y);
		void draw(float x, float y, float w , float h ){ draw(x,y); } //w and h ignored! just here to comply with ofBaseDraws
		virtual float getWidth(){ return ((string)(SEPARATOR)).length() * 8; }
		virtual float getHeight(){ return ( 4 + times.size() ) * TIME_MEASUREMENTS_LINE_HEIGHT; };
	
	private:
	
		typedef struct TimeMeasurement{
			unsigned long microsecondsStart;
			unsigned long microsecondsStop;
			unsigned long duration;
			float avgDuration;
			bool measuring;
			bool error;
		};

		static ofxTimeMeasurements* singleton;
		float desiredFrameRate;
		bool enabled;
		map<string, TimeMeasurement> times;
		float timeAveragePercent;

};

