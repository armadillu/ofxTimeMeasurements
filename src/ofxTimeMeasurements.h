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

/*
if you want better resolution on windows, the use of ofxMsaTimer is recommended.
Just include it in your project, and set USE_MSA_TIMER to TRUE.
*/
#define USE_MSA_TIMER FALSE
#if USE_MSA_TIMER 
	#include "ofxMSATimer.h"
	#define TM_GET_MICROS() timer.getElapsedMicros()
#else
	#define TM_GET_MICROS() ofGetElapsedTimeMicros()
#endif




#define TIME_MEASUREMENTS_LINE_HEIGHT		14
#define TIME_MEASUREMENTS_EDGE_GAP_H		5
#define TIME_MEASUREMENTS_EDGE_GAP_V		5
#define TIME_MEASUREMENTS_LINE_H_MULT		0.25

#define TIME_MEASUREMENTS_SETUP_KEY			"setup()"
#define TIME_MEASUREMENTS_UPDATE_KEY		"update()"
#define TIME_MEASUREMENTS_DRAW_KEY			"draw()"

#define TIME_MEASUREMENTS_GLOBAL_TOGGLE_KEY		(OF_KEY_PAGE_DOWN)
#define TIME_MEASUREMENTS_INTERACT_KEY			'T'
#define TIME_MEASUREMENTS_TOGGLE_SAMPLE_KEY		OF_KEY_RETURN

#define TIME_MEASUREMENTS_SETTINGS_FILENAME	"ofxTimeMeasurements.settings"

#define TIME_SAMPLE_SET_FRAMERATE(x)	(ofxTimeMeasurements::instance()->setDesiredFrameRate(x))
#define TIME_SAMPLE_START(x)			(ofxTimeMeasurements::instance()->startMeasuring(x))
#define TIME_SAMPLE_STOP(x)				(ofxTimeMeasurements::instance()->stopMeasuring(x))
#define TIME_SAMPLE_SET_DRAW_LOCATION(x,...)(ofxTimeMeasurements::instance()->setDrawLocation(x,##__VA_ARGS__))
#define TIME_SAMPLE_GET_ENABLED()		(ofxTimeMeasurements::instance()->getEnabled())
#define TIME_SAMPLE_SET_ENABLED(e)		(ofxTimeMeasurements::instance()->setEnabled(e))
#define TIME_SAMPLE_ENABLE()			(ofxTimeMeasurements::instance()->setEnabled(true))
#define TIME_SAMPLE_DISABLE()			(ofxTimeMeasurements::instance()->setEnabled(false))
#define TIME_SAMPLE_SET_AVERAGE_RATE(x)	(ofxTimeMeasurements::instance()->setTimeAveragePercent(x)) /* 1.0 means no averaging, 0.01 means each new sample only affects 1% on previous sample */
#define TIME_SAMPLE_DISABLE_AVERAGE()	(ofxTimeMeasurements::instance()->setTimeAveragePercent(1))
#define TIME_SAMPLE_SET_PRECISION(x)	(ofxTimeMeasurements::instance()->setMsPrecision(x)) /* how many precion digits to show on time measurements */
#define TIME_SAMPLE_GET_LAST_DURATION(x)(ofxTimeMeasurements::instance()->getLastDurationFor(x)) /* ms it took for last frame*/
#define TIME_SAMPLE_GET_AVG_DURATION(x)	(ofxTimeMeasurements::instance()->getAvgDurationFor(x)) /* ms it took for last frame avgd*/
#define TIME_SAMPLE_GET_INSTANCE()		(ofxTimeMeasurements::instance())


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
		bool startMeasuring(string ID);
		float stopMeasuring(string ID);
		void setEnabled( bool enable );
		bool getEnabled();
		void setDrawLocation(ofxTMDrawLocation loc, ofVec2f p = ofVec2f()); //p only relevant if using TIME_MEASUREMENTS_CUSTOM_LOCATION
		void setMsPrecision(int digits);		//how many decimals for the ms units
		void setTimeAveragePercent(float p);	//[0..1] >> if set to 1.0, 100% of every new sample contributes to the average.
												//if set to 0.1, a new sample contributes 10% to the average
		float durationForID( string ID);
		void setBgColor(ofColor c){bgColor = c;}
		void setHighlightColor(ofColor c){hilightColor = c;}
		void setTextColor(ofColor c){textColor = c;}

		void setUIActivationKey(unsigned int k){activateKey = k;}
		void setGlobalEnableDisableKey(unsigned int k){enableKey = k;}
		void setEnableDisableSectionKey(unsigned int k){toggleSampleKey = k;}

		virtual float getWidth(){ return (maxW + 1) * 8; }
		virtual float getHeight(){ return ( 1.2 + numVisible + 1 ) * TIME_MEASUREMENTS_LINE_HEIGHT; };

		float getLastDurationFor(string ID); //ms
		float getAvgDurationFor(string ID); //ms

	private:

		ofxTimeMeasurements(); // use ofxTimeMeasurements::instance() instead!

	struct TimeMeasurementSettings{
		bool visible;
		bool enabled;
	};
		struct TimeMeasurement{
			uint64_t microsecondsStart;
			uint64_t microsecondsStop;
			uint64_t duration;
			double avgDuration;
			bool measuring;
			bool error;
			bool updatedLastFrame;
			int level; //for nested measurements
			string nextKey;
			bool visible;
			bool enabled; //
			TimeMeasurement(){
				level = 0;
				visible = true;
				enabled = true;
				intensity = 0.0f;
				duration = 0;
				avgDuration = 0.0;
			}
			float intensity;
		};

		void _beforeSetup(ofEventArgs &d){startMeasuring(TIME_MEASUREMENTS_SETUP_KEY);};
		void _afterSetup(ofEventArgs &d){stopMeasuring(TIME_MEASUREMENTS_SETUP_KEY);};
		void _beforeUpdate(ofEventArgs &d){startMeasuring(TIME_MEASUREMENTS_UPDATE_KEY);};
		void _afterUpdate(ofEventArgs &d){stopMeasuring(TIME_MEASUREMENTS_UPDATE_KEY);};
		void _beforeDraw(ofEventArgs &d){startMeasuring(TIME_MEASUREMENTS_DRAW_KEY);};
		void _afterDraw(ofEventArgs &d){stopMeasuring(TIME_MEASUREMENTS_DRAW_KEY); autoDraw(); };

		void _appExited(ofEventArgs &e);
		void _keyPressed(ofKeyEventArgs &e);

		void draw(float x, float y);
		void draw(float x, float y, float w , float h){ ofLogError() << "ofxTimeMeasurements: ignoring draw() call"; } //w and h ignored! just here to comply with ofBaseDraws

		void autoDraw();
		void collapseExpand(string sel, bool colapse);
		void updateNumVisible();
		void updateLongestLabel();
		void loadSettings();
		void saveSettings();

		map<int, string>::iterator getIndexForOrderedKey(string key);

		static ofxTimeMeasurements*		singleton;
		float							desiredFrameRate;
		bool							enabled;

		map<string, TimeMeasurement>			times;
		map<int, string>						keyOrder;
		map<string, TimeMeasurementSettings>	settings; //visible/not at startup

		int								stackLevel; //for Nested measurements
		string							lastKey;

		float							timeAveragePercent;
		int								msPrecision;

		ofxTMDrawLocation				drawLocation;
		ofVec2f							loc;
		int								maxW; //for a text line
		int								longestLabel; //

		ofColor							bgColor;
		ofColor							hilightColor;
		ofColor							textColor;
		ofColor							selectionColor;
		ofColor							disabledTextColor;


		string							selection;
		int								numVisible;

		unsigned int					enableKey; //the whole addon
		unsigned int					activateKey;
		unsigned int					toggleSampleKey;  //selected time sample

		bool							menuActive;
		//float							internalTimeSample; //to measure time spent drawing ofxTimeSample
		#if USE_MSA_TIMER
		ofxMSATimer						timer;
		#endif
};

