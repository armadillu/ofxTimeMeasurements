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

#define TIME_MEASUREMENTS_LINE_HEIGHT		14
#define TIME_MEASUREMENTS_EDGE_GAP_H		5
#define TIME_MEASUREMENTS_EDGE_GAP_V		5
#define TIME_MEASUREMENTS_LINE_H_MULT		0.25

#define TIME_MEASUREMENTS_UPDATE_KEY		"update()"
#define TIME_MEASUREMENTS_DRAW_KEY			"draw()"

#define TIME_MEASUREMENTS_INTERACT_KEY		'T'
#define TIME_MEASUREMENTS_SETTINGS_FILENAME	"ofxTimeMeasurements.settings"


#define TIME_SAMPLE_SET_FRAMERATE(x)	(ofxTimeMeasurements::instance()->setDesiredFrameRate(x))
#define TIME_SAMPLE_START(x)			(ofxTimeMeasurements::instance()->startMeasuring(x))
#define TIME_SAMPLE_STOP(x)				(ofxTimeMeasurements::instance()->stopMeasuring(x))
#define TIME_SAMPLE_SET_DRAW_LOCATION(x,...)(ofxTimeMeasurements::instance()->setDrawLocation(x,##__VA_ARGS__))
#define TIME_SAMPLE_GET_ENABLED()		(ofxTimeMeasurements::instance()->getEnabled())
#define TIME_SAMPLE_SET_ENABLED(e)		(ofxTimeMeasurements::instance()->setEnabled(e))
#define TIME_SAMPLE_ENABLE()			(ofxTimeMeasurements::instance()->setEnabled(true))
#define TIME_SAMPLE_DISABLE()			(ofxTimeMeasurements::instance()->setEnabled(false))
#define TIME_SAMPLE_SET_AVERAGE_RATE(x)	(ofxTimeMeasurements::instance()->setTimeAveragePercent(x)) /* 1.0 means no averaging, 0.01 means each new sample only effects 1% on previous sample */
#define TIME_SAMPLE_DISABLE_AVERAGE()	(ofxTimeMeasurements::instance()->setTimeAveragePercent(1))
#define TIME_SAMPLE_SET_PRECISION(x)	(ofxTimeMeasurements::instance()->setMsPrecision(x)) /* how many precion digits to show on time measurements */
#define TIME_SAMPLE_GET_LAST_DURATION(x)(ofxTimeMeasurements::instance()->getLastDurationFor(x)) /* ms it took for last frame*/
#define TIME_SAMPLE_GET_AVG_DURATION(x)	(ofxTimeMeasurements::instance()->getAvgDurationFor(x)) /* ms it took for last frame avgd*/


enum ofxTMDrawLocation{	TIME_MEASUREMENTS_TOP_LEFT,
	TIME_MEASUREMENTS_TOP_RIGHT,
	TIME_MEASUREMENTS_BOTTOM_LEFT,
	TIME_MEASUREMENTS_BOTTOM_RIGHT,
	TIME_MEASUREMENTS_CUSTOM_LOCATION
};

class ofxTimeMeasurements: public ofBaseDraws {

	public :

		static ofxTimeMeasurements* instance();
	
		void setDesiredFrameRate(float fr);	//forced to do this as I can't access desiredFrameRate once set with ofSetFrameRate
											//affects the % busy indicator
		void startMeasuring(string ID);
		float stopMeasuring(string ID);
		void setEnabled( bool enable );
		bool getEnabled();
		void setDrawLocation(ofxTMDrawLocation loc, ofVec2f p = ofVec2f()); //p only relevant if using TIME_MEASUREMENTS_CUSTOM_LOCATION
		void setMsPrecision(int digits);		//how many decimals for the ms units
		void setTimeAveragePercent(float p);	//[0..1] >> if set to 1.0, 100% of every new sample contributes to the average.
												//if set to 0.1, a new sample contributes 10% to the average
		unsigned long durationForID( string ID);
		void setBgColor(ofColor c){bgColor = c;}
		void setHighlightColor(ofColor c){hiColor = c;}
		void setTextColor(ofColor c){textColor = c;}

		void setUIActivationKey(char k){activateKey = k;}
		void setEnableDisableKey(char k){activateKey = k;}

	
		virtual float getWidth(){ return (maxW + 1) * 8; }
		virtual float getHeight(){ return ( 1.2 + numVisible + 1 ) * TIME_MEASUREMENTS_LINE_HEIGHT; };

		float getLastDurationFor(string ID); //ms
		float getAvgDurationFor(string ID); //ms

	private:

		ofxTimeMeasurements(); // use ofxTimeMeasurements::instance() instead!

		struct TimeMeasurement{
			unsigned long microsecondsStart;
			unsigned long microsecondsStop;
			unsigned long duration;
			float avgDuration;
			bool measuring;
			bool error;
			bool updatedLastFrame;
			int level; //for nested measurements
			string nextKey;
			bool visible;
			TimeMeasurement(){
				level = 0;
				visible = true;
			}
		};

		void _beforeUpdate(ofEventArgs &d){startMeasuring(TIME_MEASUREMENTS_UPDATE_KEY);};
		void _afterUpdate(ofEventArgs &d){stopMeasuring(TIME_MEASUREMENTS_UPDATE_KEY);};
		void _beforeDraw(ofEventArgs &d){startMeasuring(TIME_MEASUREMENTS_DRAW_KEY);};
		void _afterDraw(ofEventArgs &d){stopMeasuring(TIME_MEASUREMENTS_DRAW_KEY); autoDraw(); };

		void _appExited(ofEventArgs &e);
		void _keyPressed(ofKeyEventArgs &e);

		void draw(float x, float y);
		void draw(float x, float y, float w , float h){ ofLogError() << "ofxTimeMeasurements: ignoring draw() call"; } //w and h ignored! just here to comply with ofBaseDraws

		void autoDraw();
		void collapseExpand(int sel, bool colapse);
		void updateNumVisible();
		void updateLongestLabel();
		void loadSettings();

		static ofxTimeMeasurements*		singleton;
		float							desiredFrameRate;
		bool							enabled;

		map<string, TimeMeasurement>	times;
		map<int, string>				keyOrder;
		map<string, bool>				settings; //visible/not at startup

		int								stackLevel; //for Nested measurements
		string							lastKey;

		float							timeAveragePercent;
		int								msPrecision;

		ofxTMDrawLocation				drawLocation;
		ofVec2f							loc;
		int								maxW; //for a text line
		int								longestLabel; //

		ofColor							bgColor;
		ofColor							hiColor;
		ofColor							textColor;
		ofColor							selectionColor;

		int								selection;
		int								numVisible;

		unsigned int					enableKey;
		unsigned int					activateKey;
		bool							menuActive;

		//float							internalTimeSample; //to measure time spent drawing ofxTimeSample
};

