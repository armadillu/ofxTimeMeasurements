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
#include "../lib/tree.h"
#include <map>


#if __cplusplus>=201103L || defined(_MSC_VER)
	#include <unordered_map>
	#include <memory>
#else
	#include <tr1/unordered_map>
	using std::tr1::unordered_map;
#endif


#ifdef USE_OFX_HISTORYPLOT
	#include "ofxHistoryPlot.h"
#endif

#ifdef USE_OFX_FONTSTASH
	#include "ofxFontStash.h"
#endif


/*
if you want better resolution on windows, the use of ofxMsaTimer is recommended.
Just include it in your project, and define USE_MSA_TIMER in your project preprocessor macros.
*/

#ifdef USE_MSA_TIMER 
	#include "ofxMSATimer.h"
	#define TM_GET_MICROS() timer.getElapsedMicros()
#else
	#define TM_GET_MICROS() ofGetElapsedTimeMicros()
#endif


#define TIME_MEASUREMENTS_LINE_HEIGHT		(14)
#define TIME_MEASUREMENTS_EDGE_GAP_H		(5 * uiScale)
#define TIME_MEASUREMENTS_EDGE_GAP_V		(5 * uiScale)

#define TIME_MEASUREMENTS_SETUP_KEY			"setup()"
#define TIME_MEASUREMENTS_UPDATE_KEY		"update()"
#define TIME_MEASUREMENTS_DRAW_KEY			"draw()"

#define TIME_MEASUREMENTS_GLOBAL_TOGGLE_KEY		(OF_KEY_PAGE_DOWN)
#define TIME_MEASUREMENTS_INTERACT_KEY			'T'
#define TIME_MEASUREMENTS_TOGGLE_SAMPLE_KEY		OF_KEY_RETURN

#define TIME_MEASUREMENTS_SETTINGS_FILENAME	(configsDir + "/" + "ofxTimeMeasurements.settings")

#include "ofxTimeMeasurementsMacros.h"


enum ofxTMDrawLocation{
	TIME_MEASUREMENTS_TOP_LEFT = 0,
	TIME_MEASUREMENTS_TOP_RIGHT,
	TIME_MEASUREMENTS_BOTTOM_LEFT,
	TIME_MEASUREMENTS_BOTTOM_RIGHT,
	TIME_MEASUREMENTS_CUSTOM_LOCATION,
	TIME_MEASUREMENTS_NUM_DRAW_LOCATIONS
};


class ofxTimeMeasurements {

	public :

		static ofxTimeMeasurements* instance();

		void setConfigsDir(string d);
		void setDesiredFrameRate(float fr);	//forced to do this as I can't access desiredFrameRate once set with ofSetFrameRate
											//affects the % busy indicator
		bool startMeasuring(const string & ID, bool accumulate, const ofColor & c = ofColor(0,0,0,0));
		float stopMeasuring(const string & ID, bool accumulate);
		void setEnabled( bool enable );
		bool getEnabled();
		void setDrawLocation(ofxTMDrawLocation loc, ofVec2f p = ofVec2f()); //p only relevant if using TIME_MEASUREMENTS_CUSTOM_LOCATION
		void setMsPrecision(int digits);		//how many decimals for the ms units
		void setTimeAveragePercent(double p);	//[0..1] >> if set to 1.0, 100% of every new sample contributes to the average.
												//if set to 0.1, a new sample contributes 10% to the average
		float durationForID(const string & ID);
		void setBgColor(ofColor c){bgColor = c;}
		void setHighlightColor(ofColor c);
		void setThreadColors(const vector<ofColor> & tc); //supply your own thread color list

		//[0..1], 0.5 means inactive times show 50% darker than active ones
		void setIdleTimeColorFadePercent(float p){ idleTimeColorFadePercent = p;}

		//[0..1], 0.97 means that timeSample decreases 3% its life per frame
		void setIdleTimeDecay(float decay){ idleTimeColorDecay = decay;}
		void setDeadThreadTimeDecay(float decay); //when a thread is dead and to be removed from screen, extend a but its stay by using a slower decay

		//set keyboard command activation keys
		void setUIActivationKey(unsigned int k){activateKey = k;} //to show/hide the widget
		void setGlobalEnableDisableKey(unsigned int k){enableKey = k;} //to enable/disable widget interaction
		void setEnableDisableSectionKey(unsigned int k){toggleSampleKey = k;} //to enable/disable the selected time measurement

	
		float getWidth() const;
		float getHeight() const{ return (drawLines.size() + 2 ) * charH - 9;}

		float getLastDurationFor(const string & ID); //ms
		float getAvgDurationFor(const string & ID); //ms

		void setUiScale(float scale){uiScale = scale;};

		void setRemoveExpiredThreads(bool b){removeExpiredThreads = b;}
		#if defined(USE_OFX_HISTORYPLOT)
		void setPlotHeight(int h){plotHeight = h;}
		void setPlotBaseY(float y){plotBaseY = y;}
		#endif

		#ifdef USE_OFX_FONTSTASH
		void drawUiWithFontStash(string fontPath, float fontSize = 13.0f /*good with VeraMono*/);
		void drawUiWithBitmapFont();
		#endif

		void setAutoDraw(bool b){drawAuto = b;}
		void draw(int x, int y) ;

		void addSetupHooks();

	private:

		ofxTimeMeasurements(); // use ofxTimeMeasurements::instance() instead!

		struct TimeMeasurementSettings{
			bool visible;
			bool enabled;
			#if defined(USE_OFX_HISTORYPLOT)
			bool plotting;
			TimeMeasurementSettings(){plotting = false;}
			#endif
		};

		struct TimeMeasurement{
			uint64_t microsecondsStart;
			uint64_t microsecondsStop;
			uint64_t microsecondsAccum;
			uint64_t duration;
			double avgDuration;
			bool measuring;
			bool error;
			bool updatedLastFrame;
			bool accumulating; //start stop doesnt reset the timing, unless you call stop with accum=true
			int numAccumulations;
			int frame; //used to compare start-stop calls frame, and see if its an across-frames measurement
			bool acrossFrames;
			string key;
			Poco::Thread* thread;
			float life;
			TimeMeasurementSettings settings;

			TimeMeasurement(){
				thread = NULL;
				microsecondsAccum = 0;
				numAccumulations = 0;
				settings.visible = true;
				settings.enabled = true;
				life = 1.0f;
				accumulating = false;
				duration = 0;
				avgDuration = 0.0;
			}
		};

		struct PrintedLine{
			string key;
			string formattedKey;
			string time;
			string fullLine;
			ofColor color;
			ofColor lineBgColor;
			ofColor plotColor;
			TimeMeasurement * tm;
			PrintedLine(){ tm = NULL; plotColor.a = 0; }
		};

		struct ThreadInfo{
			core::tree<string>::iterator	tit; //tree iterator, to keep track of which node are we measuring now
			core::tree<string>				tree;
			ofColor							color;
			int								order;
		};

		void _beforeSetup(ofEventArgs &d){startMeasuring(TIME_MEASUREMENTS_SETUP_KEY, false);};
		void _afterSetup(ofEventArgs &d){stopMeasuring(TIME_MEASUREMENTS_SETUP_KEY, false);};
		void _beforeUpdate(ofEventArgs &d){startMeasuring(TIME_MEASUREMENTS_UPDATE_KEY, false);};
		void _afterUpdate(ofEventArgs &d){stopMeasuring(TIME_MEASUREMENTS_UPDATE_KEY, false);};
		void _beforeDraw(ofEventArgs &d){startMeasuring(TIME_MEASUREMENTS_DRAW_KEY, false);};
		void _afterDraw(ofEventArgs &d);

		void _appExited(ofEventArgs &e);
		void _keyPressed(ofKeyEventArgs &e);

		void _windowResized(ofResizeEventArgs &e);


		void autoDraw();
		void collapseExpand(string sel, bool colapse);
		void updateLongestLabel();
		void loadSettings();
		void saveSettings();

		string formatTime(uint64_t microSeconds, int precision);
		string getTimeStringForTM(TimeMeasurement* tm);
		void drawString(const string & text, const float & x, const float & y);


		static bool compareThreadPairs(const pair<Poco::Thread*, ThreadInfo>& l, const pair<Poco::Thread*, ThreadInfo>& r){
			return l.second.order < r.second.order;
		}

		static ofxTimeMeasurements*				singleton;

		float									desiredFrameRate;
		bool									enabled;

		unordered_map<string, TimeMeasurement*>			times;
		unordered_map<string, TimeMeasurementSettings>	settings; //visible/not at startup

		unordered_map<Poco::Thread*, ThreadInfo>		threadInfo;

		vector<PrintedLine>						drawLines; //what's drawn line by line

		double									timeAveragePercent;
		bool									averaging;
		int										msPrecision; //number of decimals to show

		ofxTMDrawLocation						drawLocation;
		ofVec2f									customDrawLocation;
		int										maxW; //for a text line
		int										longestLabel; //

		ofColor									bgColor;
		ofColor									hilightColor;
		ofColor									textColor;
		ofColor									disabledTextColor;
		ofColor									measuringColor;


		float									idleTimeColorFadePercent;
		float									idleTimeColorDecay;
		float									deadThreadExtendedLifeDecSpeed;

		vector<ofColor>							threadColorTable;
		int										numThreads;

		string									selection;
		int										numVisible;

		unsigned int							enableKey; //the whole addon
		unsigned int							activateKey;
		unsigned int							toggleSampleKey;  //selected time sample

		bool									menuActive;
		#ifdef USE_MSA_TIMER
		ofxMSATimer								timer;
		#endif

		Poco::Thread*							mainThreadID; //usually NULL
		ofMutex									mutex;

		string									configsDir;

		bool									removeExpiredThreads;
		bool 									settingsLoaded;
		float 									uiScale;
		#if defined(USE_OFX_HISTORYPLOT)
		map<string, ofxHistoryPlot*>			plots;
		int										plotHeight;
		int										plotBaseY;
		int										numAllocatdPlots;

		ofxHistoryPlot*							makeNewPlot(string name);
		#endif

		#ifdef USE_OFX_FONTSTASH
		bool									useFontStash;
		ofxFontStash							font;
		float									fontSize;
		#endif
		float									charW;
		float									charH;

		void walkTree(core::tree<string>::iterator Arg, int levelArg, vector<string> &result);

		bool									drawAuto;
		unsigned char							dimColorA;

};

