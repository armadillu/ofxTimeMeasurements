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
#include "MinimalTree.h"
#include <map>
#include "GL_Measurement.h"


#if __cplusplus>=201103L || defined(_MSC_VER)
	#include <unordered_map>
	#include <memory>
#else
	#include <tr1/unordered_map>
	using std::tr1::unordered_map;
#endif


#if defined(__has_include) /*llvm only - query about header files being available or not*/
#if __has_include("ofxHistoryPlot.h")
	 #ifndef USE_OFX_HISTORYPLOT
		 #define USE_OFX_HISTORYPLOT
	 #endif
#endif
#endif

#ifdef USE_OFX_HISTORYPLOT
#include "ofxHistoryPlot.h"
#endif

// ofxFontStash ///////////////////////////////////////////////////////////////////////////////////

#if defined(__has_include) /*llvm only - query about header files being available or not*/
	#if __has_include("ofxFontStash.h") && !defined(DISABLE_AUTO_FIND_FONSTASH_HEADERS)
		#ifndef USE_OFX_FONTSTASH
			#define USE_OFX_FONTSTASH
		#endif
	#endif
#endif

#ifdef USE_OFX_FONTSTASH
	#include "ofxFontStash.h"
#endif

// ofxFontStash2 //////////////////////////////////////////////////////////////////////////////////

#if defined(__has_include) /*llvm only - query about header files being available or not*/
	#if __has_include("ofxFontStash2.h") && !defined(DISABLE_AUTO_FIND_FONSTASH_HEADERS)
		#ifndef USE_OFX_FONTSTASH2
			#define USE_OFX_FONTSTASH2
		#endif
	#endif
#endif

#ifdef USE_OFX_FONTSTASH2
	#include "ofxFontStash2.h"
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////

#include "ofxTimeMeasurementsMacros.h"

#define TM_GET_MICROS() std::chrono::time_point_cast<chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count()


#define TIME_MEASUREMENTS_LINE_HEIGHT		(14)
#define TIME_MEASUREMENTS_EDGE_GAP_H		(5 * uiScale)
#define TIME_MEASUREMENTS_EDGE_GAP_V		(5 * uiScale)

#define TIME_MEASUREMENTS_SETUP_KEY			"setup()"
#define TIME_MEASUREMENTS_UPDATE_KEY		"update()"
#define TIME_MEASUREMENTS_DRAW_KEY			"draw()"
#define TIME_MEASUREMENTS_KEYPRESSED_KEY	"keyPressed()"
#define TIME_MEASUREMENTS_KEYRELEASED_KEY	"keyReleased()"

#define TIME_MEASUREMENTS_GLOBAL_TOGGLE_KEY		(OF_KEY_PAGE_DOWN)
#define TIME_MEASUREMENTS_INTERACT_KEY			'T'
#define TIME_MEASUREMENTS_TOGGLE_SAMPLE_KEY		OF_KEY_RETURN

#define TIME_MEASUREMENTS_SETTINGS_FILENAME	(configsDir + "/" + "ofxTimeMeasurements.settings")

#ifndef TIME_MEASUREMENTS_DISABLED

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

		void setConfigsDir(std::string d);
		void setDesiredFrameRate(float fr);	//forced to do this as I can't access desiredFrameRate once set with ofSetFrameRate
											//affects the % busy indicator

		//ifEnabled  > if true, it means that measurement is wrapped around with an if() caluse
		bool startMeasuring(const std::string & ID, bool accumulate, bool ifEnabled = true);
		float stopMeasuring(const std::string & ID, bool accumulate);

		// GL (GPU render time) measurements
		#ifndef TARGET_OPENGLES
		bool startMeasuringGL(const std::string & name);
		void stopMeasuringGL(const std::string & name);
		#endif
		// end GL

		void setEnabled( bool enable );
		bool getEnabled();
		void setSavesSettingsOnExit(bool save) { savesSettingsOnExit = save; };
		void setDrawLocation(ofxTMDrawLocation loc, ofVec2f p = ofVec2f()); //p only relevant if using TIME_MEASUREMENTS_CUSTOM_LOCATION
		ofxTMDrawLocation getDrawLocation(){return drawLocation;}
		void setMsPrecision(int digits);		//how many decimals for the ms units
		void setTimeAveragePercent(double p);	//[0..1] >> if set to 1.0, 100% of every new sample contributes to the average.
												//if set to 0.1, a new sample contributes 10% to the average
		void setDrawPercentageAsGraph(bool d){drawPercentageAsGraph = d;}
		float durationForID(const std::string & ID);
		void setBgColor(ofColor c){bgColor = c;}
		void setHighlightColor(ofColor c);
		void setThreadColors(const std::vector<ofColor> & tc); //supply your own thread color list

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
		float getHeight() const;

		float getLastDurationFor(const std::string & ID); //ms
		float getAvgDurationFor(const std::string & ID); //ms

		void setUiScale(float scale);
		float getUiScale(){return uiScale;}

		void setRemoveExpiredThreads(bool b){removeExpiredThreads = b;}
		void setRemoveExpiredTimings(bool r){removeExpiredTimings = r;}

		#if defined(USE_OFX_HISTORYPLOT)
		void setPlotHeight(int h){plotHeight = h;}
		void setPlotBaseY(float y){plotBaseY = y;}

		///plot resolution is a scaling factor for the ofxHistoryPlot number of samples.
		///if r = 1.0, plot will have same # of samples as pixels wide the window is.
		/// r = 0.5; plot will have half of that.
		void setPlotResolution(float r){plotResolution = r;}
		void setMaxNumPlotSamples(int nSamples){maxPlotSamples = nSamples;}
		void setDrawAllPlotsOnTopOfEachOther(bool doit){ allPlotsTogether = doit;}
		#endif
		float getPlotsHeight();

		// Font render configs ///////
		void drawUiWithBitmapFont();

		#if defined(USE_OFX_FONTSTASH)
		void drawUiWithFontStash(std::string fontPath, float fontSize = 13.0f /*good with VeraMono*/);
		ofxFontStash & getFont(){return font;}
		#endif

		#if defined(USE_OFX_FONTSTASH2)
		void drawUiWithFontStash2(std::string fontPath, float fontSize = 13.0f /*good with VeraMono*/);
		ofxFontStash2::Fonts & getFont2(){return font2;}
		#endif

		void enableInternalBenchmark(bool bench){internalBenchmark = bench;}

		void setAutoDraw(bool b){drawAuto = b;}
		void draw(int x, int y) ;

		void addEventHooks(ofCoreEvents* eventHooks = nullptr);
		void addSetupHooks(ofCoreEvents* eventHooks = nullptr);
		void removeEventHooks(ofCoreEvents* eventHooks);
		void removeSetupHooks(ofCoreEvents* eventHooks);

		static void drawSmoothFpsClock(float x, float y, float radius);

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

		typedef std::thread::id ThreadId;

		struct TimeMeasurement{
			uint64_t microsecondsStart;
			uint64_t microsecondsStop;
			uint64_t microsecondsAccum;
			uint64_t duration;
			double avgDuration;
			bool measuring;
			bool error;
			bool updatedLastFrame;
			bool ifClause; //is this measurement under an if() clause or not? (TS_START vs TS_START_NIF)
			bool accumulating; //start stop doesnt reset the timing, unless you call stop with accum=true
			int numAccumulations;
			int frame; //used to compare start-stop calls frame, and see if its an across-frames measurement
			bool acrossFrames;
			std::string key;
			ThreadId thread;
			float life;
			bool isGL; //this is a GL measurement, some behaviors don't apply
			TimeMeasurementSettings settings;

			TimeMeasurement();
		};

		struct PrintedLine{
			std::string key;
			std::string formattedKey;
			std::string time;
			std::string fullLine;
			bool shouldDrawPctGraph;
			bool isAccum;
			ofColor color;
			ofColor lineBgColor;
			ofColor plotColor; //if a > 0 , this measurement is being plotted
			float percentGraph;
			TimeMeasurement * tm;
			PrintedLine(){ tm = NULL; plotColor.a = 0; isAccum = false; percentGraph = 0.0f; }
		};

		struct ThreadInfo{
			//core::tree<std::string>::iterator		tit; //tree iterator, to keep track of which node are we measuring now
			//core::tree<std::string>				tree;

			MinimalTree 									tree;
			MinimalTree::Node*	 							tit;

			ofColor							color;
			int								order;
		};

		void _beforeSetup(ofEventArgs &d){startMeasuring(TIME_MEASUREMENTS_SETUP_KEY, false);};
		void _afterSetup(ofEventArgs &d){stopMeasuring(TIME_MEASUREMENTS_SETUP_KEY, false);};
		void _beforeUpdate(ofEventArgs &d){startMeasuring(TIME_MEASUREMENTS_UPDATE_KEY, false);};
		void _afterUpdate(ofEventArgs &d){stopMeasuring(TIME_MEASUREMENTS_UPDATE_KEY, false);};
		void _beforeDraw(ofEventArgs &d){startMeasuring(TIME_MEASUREMENTS_DRAW_KEY, false);};
		void _afterDraw(ofEventArgs &d);
		void _beforeKeyPressed(ofKeyEventArgs &d){startMeasuring(TIME_MEASUREMENTS_KEYPRESSED_KEY, false);};
		void _afterKeyPressed(ofKeyEventArgs &d){stopMeasuring(TIME_MEASUREMENTS_KEYPRESSED_KEY, false);};
	
//		void _beforeKeyReleased(ofKeyEventArgs &d){startMeasuring(TIME_MEASUREMENTS_KEYRELEASED_KEY, false);};
//		void _afterKeyReleased(ofKeyEventArgs &d){stopMeasuring(TIME_MEASUREMENTS_KEYRELEASED_KEY, false);};

		void _appExited(ofEventArgs &e);
		bool _keyPressed(ofKeyEventArgs &e);

		void _windowResized(ofResizeEventArgs &e);

		void autoDraw();
		void collapseExpand(const string & sel, bool colapse);
		void updateLongestLabel();
		void loadSettings();
		void saveSettings();

		std::string formatTime(uint64_t microSeconds, int precision);
		std::string getTimeStringForTM(TimeMeasurement* tm);
		float getPctForTM(TimeMeasurement* tm);
		void drawString(const std::string & text, const float & x, const float & y);


		struct ThreadContainer{
			ThreadId id;
			ThreadInfo * info;
		};

		static bool compareThreadPairs(const ThreadContainer& l, const ThreadContainer& r){
			return l.info->order < r.info->order;
		}

		static ofxTimeMeasurements*						singleton;

		float											desiredFrameRate;
		bool											enabled;

		std::unordered_map<std::string, TimeMeasurement*>			times;
		std::unordered_map<std::string, TimeMeasurementSettings>	settings; //visible/not at startup

		std::unordered_map<ThreadId, ThreadInfo>		threadInfo;
		ThreadId								mainThreadID; //usually NULL

		bool isMainThread(ThreadId tid){return tid == mainThreadID;}

		ThreadId getThreadID(){
			return std::this_thread::get_id();
		}

		std::vector<PrintedLine>						drawLines; //what's drawn line by line

		double									timeAveragePercent;
		bool									averaging;
		int										msPrecision; //number of decimals to show

		ofxTMDrawLocation						drawLocation;
		ofVec2f									customDrawLocation;
		int										maxW; //for a text line
		int										longestLabel; //

		ofColor									bgColor;
		ofColor									hilightColor;
		ofColor									glColor;
		ofColor									textColor;
		ofColor									disabledTextColor;
		ofColor									measuringColor;
		ofColor									frozenColor;

		float									idleTimeColorFadePercent;
		float									idleTimeColorDecay;
		float									deadThreadExtendedLifeDecSpeed;

		std::vector<ofColor>							threadColorTable;
		int										numThreads;

		std::string									selection;
		int										numVisible;

		unsigned int							enableKey; //the whole addon
		unsigned int							activateKey;
		unsigned int							toggleSampleKey;  //selected time sample

		bool									freeze; //if enabled, ignore current timings and show the last one we had b4 freezing
		bool									menuActive;

		ofMutex									mutex;

		std::string									configsDir;

		bool									removeExpiredThreads;
		bool 									removeExpiredTimings;
		bool									drawPercentageAsGraph;

		bool 									settingsLoaded;
		float 									uiScale;
		#if defined(USE_OFX_HISTORYPLOT)
		std::map<std::string, ofxHistoryPlot*>			plots;
		int										plotHeight;
		int										plotBaseY;
		int										numAllocatdPlots;
		int										numActivePlots;
		bool									allPlotsTogether;

		ofxHistoryPlot*							makeNewPlot(std::string name);
		float									plotResolution;
		int										maxPlotSamples;
		#endif

		enum FontRenderer{
			RENDER_WITH_OF_BITMAP_FONT,
			RENDER_WITH_OFXFONTSTASH,
			RENDER_WITH_OFXFONTSTASH2
		};

		FontRenderer							fontRenderer = RENDER_WITH_OF_BITMAP_FONT;
		#ifdef USE_OFX_FONTSTASH
		std::string 									fontStashFile;
		ofxFontStash							font;
		float									fontSize;
		#endif

		#ifdef USE_OFX_FONTSTASH2
		std::string 								fontStashFile2;
		ofxFontStash2::Fonts					font2;
		float									fontSize2;
		#endif

		float									charW; //to draw text flexibly with bitmap / fontstash
		float									charH;

		bool									drawAuto;
		unsigned char							dimColorA;

		uint64_t								currentFrameNum;

		//internal performance testing
		bool									internalBenchmark;
		uint64_t								wastedTimeThisFrame;
		uint64_t								wastedTimeDrawingThisFrame;
		uint64_t								wastedTimeAvg;
		uint64_t								wastedTimeDrawingAvg;

		/// GL measurements
		#ifndef TARGET_OPENGLES
		std::unordered_map<std::string, GL_Measurement*>	glTimes;
		std::string 									measuringGlLabel;
		const std::string 							glPrefix = "GL_";
		bool glMeasurementMode = false;
		int threadIDGL = -1;
		void									updateGLMeasurements();
		#endif

		bool									savesSettingsOnExit = true;

		void beginTextBatch();
		void endTextBatch();
};

#include "ofxTimeMeasurementsScoped.h"

#endif
