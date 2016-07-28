/*
 *  ofxTimeMeasurementsMacros.h
 *  emptyExample
 *
 *  Created by Oriol Ferrer Mesià on 28/01/11.
 *  Copyright 2011 uri.cat. All rights reserved.
 *
 */

#pragma once

#if( defined(TARGET_OSX) || defined(TARGET_LINUX))
#include <cxxabi.h>
#endif

#ifdef TARGET_WIN32
	#define AUTOMAGICAL_FUNC_NAME __FUNCTION__
#else
	#define AUTOMAGICAL_FUNC_NAME (demangled_type_info_name(typeid(this)) + "::" + __FUNCTION__)
#endif


//you can define "TIME_MEASUREMENTS_DISABLED" in your project pre-processor macros to ENTIRELY disable time measurements
#ifndef TIME_MEASUREMENTS_DISABLED

	#define TIME_SAMPLE_SET_CONFIG_DIR(x) 	(ofxTimeMeasurements::instance()->setConfigsDir(x))
	#define TIME_SAMPLE_SET_FRAMERATE(x)	(ofxTimeMeasurements::instance()->setDesiredFrameRate(x))

	#define TIME_SAMPLE_START(x, ...)		if(ofxTimeMeasurements::instance()->startMeasuring(x, false, true)){
	#define TIME_SAMPLE_STOP(x)				}ofxTimeMeasurements::instance()->stopMeasuring(x, false)

	#define TIME_SAMPLE_START_ACC(x, ...)	if(ofxTimeMeasurements::instance()->startMeasuring(x, true, true)){
	#define TIME_SAMPLE_STOP_ACC(x)			}ofxTimeMeasurements::instance()->stopMeasuring(x, true)

	#define TIME_SAMPLE_START_NOIF(x, ...)	ofxTimeMeasurements::instance()->startMeasuring(x, false, false)
	#define TIME_SAMPLE_STOP_NOIF(x)		ofxTimeMeasurements::instance()->stopMeasuring(x, false)

	#define TIME_SAMPLE_START_ACC_NOIF(x, ...)	ofxTimeMeasurements::instance()->startMeasuring(x, true, false)
	#define TIME_SAMPLE_STOP_ACC_NOIF(x)	ofxTimeMeasurements::instance()->stopMeasuring(x, true)

	#define TIME_SAMPLE_SET_DRAW_LOCATION(x,...)(ofxTimeMeasurements::instance()->setDrawLocation(x, ##__VA_ARGS__))
	#define TIME_SAMPLE_GET_ENABLED()		(ofxTimeMeasurements::instance()->getEnabled())
	#define TIME_SAMPLE_SET_ENABLED(e)		(ofxTimeMeasurements::instance()->setEnabled(e))
	#define TIME_SAMPLE_ENABLE()			(ofxTimeMeasurements::instance()->setEnabled(true))
	#define TIME_SAMPLE_DISABLE()			(ofxTimeMeasurements::instance()->setEnabled(false))
	#define TIME_SAMPLE_SET_AVERAGE_RATE(x)	(ofxTimeMeasurements::instance()->setTimeAveragePercent(x)) /* 1.0 means no averaging, 0.01 means each new sample only affects 1% on previous sample */
	#define TIME_SAMPLE_DISABLE_AVERAGE()	(ofxTimeMeasurements::instance()->setTimeAveragePercent(1))
	#define TIME_SAMPLE_SET_PRECISION(x)	(ofxTimeMeasurements::instance()->setMsPrecision(x)) /* how many precion digits to show on time measurements */
	#define TIME_SAMPLE_GET_LAST_DURATION(x)(ofxTimeMeasurements::instance()->getLastDurationFor(x)) /* ms it took for last frame*/
	#define TIME_SAMPLE_GET_AVG_DURATION(x)	(ofxTimeMeasurements::instance()->getAvgDurationFor(x)) /* ms it took for last frame avgd*/
	#define TIME_SAMPLE_SET_REMOVE_EXPIRED_THREADS(x) (ofxTimeMeasurements::instance()->setRemoveExpiredThreads(x))
	#define TIME_SAMPLE_GET_INSTANCE()		(ofxTimeMeasurements::instance())

	//this is to add auto measurements on setup()
	//only to be used in OF >0.9, and in this case, only to be called from main();
	#define TIME_SAMPLE_ADD_SETUP_HOOKS()	(ofxTimeMeasurements::instance()->addSetupHooks())

	//shorter macros for most of the above!

	//time sample a whole method - ie TS(drawMonkeys());
	#define TS(x)			if(ofxTimeMeasurements::instance()->startMeasuring(#x, false)){ x; }ofxTimeMeasurements::instance()->stopMeasuring(#x, false)
	#define TS_ACC(x)		if(ofxTimeMeasurements::instance()->startMeasuring(#x, true)){ x; }ofxTimeMeasurements::instance()->stopMeasuring(#x, true)

	//includes if(){ } caluse for you
	#define TS_START(x, ...)				TIME_SAMPLE_START(x, ##__VA_ARGS__)
	#define TS_STOP(x)						TIME_SAMPLE_STOP(x)
	#define TS_START_ACC(x, ...	)			TIME_SAMPLE_START_ACC(x, ##__VA_ARGS__)
	#define TS_STOP_ACC(x)					TIME_SAMPLE_STOP_ACC(x)


	//"no if clause" (NIF) timings, to be used in timings that are measured across several methods / frames
	#define TS_START_NIF(x, ...)				TIME_SAMPLE_START_NOIF(x, ##__VA_ARGS__)
	#define TS_STOP_NIF(x)						TIME_SAMPLE_STOP_NOIF(x)
	#define TS_START_ACC_NIF(x, ...)			TIME_SAMPLE_START_ACC_NOIF(x, ##__VA_ARGS__)
	#define TS_STOP_ACC_NIF(x)					TIME_SAMPLE_STOP_ACC_NOIF(x)

	//scope timings
	#define TS_SCOPE(x) 		auto scopeTiming = ofxTimeMeasurementsScoped(x, false)
	#define TS_SCOPE_ACC(x) 	auto scopeTiming = ofxTimeMeasurementsScoped(x, true)
	//automatic naming for this scoped time - will look like "Class::method()"
	#define TS_ASCOPE() 		auto scopeTiming = ofxTimeMeasurementsScoped(AUTOMAGICAL_FUNC_NAME, false)

	//auto-named timings
	#define TS_ASTART()				if(ofxTimeMeasurements::instance()->startMeasuring(AUTOMAGICAL_FUNC_NAME, false, true)){
	#define TS_ASTOP()				}ofxTimeMeasurements::instance()->stopMeasuring(AUTOMAGICAL_FUNC_NAME, false)
	#define TS_ASTART_NIF()			ofxTimeMeasurements::instance()->startMeasuring(AUTOMAGICAL_FUNC_NAME, false, false)
	#define TS_ASTOP_NIF()			ofxTimeMeasurements::instance()->stopMeasuring(AUTOMAGICAL_FUNC_NAME, false)
	#define TS_ASTART_ACC()			if(ofxTimeMeasurements::instance()->startMeasuring(AUTOMAGICAL_FUNC_NAME, true, true)){
	#define TS_ASTOP_ACC()			}ofxTimeMeasurements::instance()->stopMeasuring(AUTOMAGICAL_FUNC_NAME, true)
	#define TS_ASTART_ACC_NIF()		ofxTimeMeasurements::instance()->startMeasuring(AUTOMAGICAL_FUNC_NAME, true, false)
	#define TS_ASTOP_ACC_NIF()		ofxTimeMeasurements::instance()->stopMeasuring(AUTOMAGICAL_FUNC_NAME, true)

#else

	#define TIME_SAMPLE_SET_CONFIG_DIR(x)
	#define TIME_SAMPLE_SET_FRAMERATE(x)

	#define TIME_SAMPLE_START(x, ...)
	#define TIME_SAMPLE_STOP(x)

	#define TIME_SAMPLE_START_ACC(x, ...)
	#define TIME_SAMPLE_STOP_ACC(x)			

	#define TIME_SAMPLE_START_NOIF(x, ...)	
	#define TIME_SAMPLE_STOP_NOIF(x)

	#define TIME_SAMPLE_START_ACC_NOIF(x, ...)
	#define TIME_SAMPLE_STOP_ACC_NOIF(x)

	#define TIME_SAMPLE_SET_DRAW_LOCATION(x,...)
	#define TIME_SAMPLE_GET_ENABLED()		
	#define TIME_SAMPLE_SET_ENABLED(e)		
	#define TIME_SAMPLE_ENABLE()			
	#define TIME_SAMPLE_DISABLE()			
	#define TIME_SAMPLE_SET_AVERAGE_RATE(x)
	#define TIME_SAMPLE_DISABLE_AVERAGE()	
	#define TIME_SAMPLE_SET_PRECISION(x)	
	#define TIME_SAMPLE_GET_LAST_DURATION(x)
	#define TIME_SAMPLE_GET_AVG_DURATION(x)
	#define TIME_SAMPLE_SET_REMOVE_EXPIRED_THREADS(x)
	#define TIME_SAMPLE_GET_INSTANCE()

	#define TIME_SAMPLE_ADD_SETUP_HOOKS()

	#define TS(x)
	#define TS_ACC(x)

	#define TS_START(x, ...)		
	#define TS_STOP(x)				
	#define TS_START_ACC(x, ...	)	
	#define TS_STOP_ACC(x)

	#define TS_START_NIF(x, ...)		
	#define TS_STOP_NIF(x)				
	#define TS_START_ACC_NIF(x, ...)	
	#define TS_STOP_ACC_NIF(x)

	//scope timings
	#define TS_SCOPE(x)
	#define TS_SCOPE_ACC(x)
	#define TS_ASCOPE()

	//auto named
	#define TS_ASTART()
	#define TS_ASTOP()
	#define TS_ASTART_NIF()
	#define TS_ASTOP_NIF()
	#define TS_ASTART_ACC()
	#define TS_ASTOP_ACC()
	#define TS_ASTART_ACC_NIF()
	#define TS_ASTOP_ACC_NIF()

#endif

//locations shortcuts
#define TIME_SAMPLE_DRAW_LOC_TOP_LEFT 		TIME_MEASUREMENTS_TOP_LEFT
#define TIME_SAMPLE_DRAW_LOC_BOTTOM_LEFT 	TIME_MEASUREMENTS_BOTTOM_LEFT
#define TIME_SAMPLE_DRAW_LOC_BOTTOM_RIGHT 	TIME_MEASUREMENTS_BOTTOM_RIGHT
#define TIME_SAMPLE_DRAW_LOC_TOP_RIGHT 		TIME_MEASUREMENTS_TOP_RIGHT

//auto measurement naming
static char demangleSpace[4096];
static ofMutex autoNamingMutex;

#ifndef TARGET_WIN32
inline std::string demangled_type_info_name(const std::type_info&ti){

	ofScopedLock lock(autoNamingMutex);
	int status = 0;
	size_t len = 4096;
	char * ret = abi::__cxa_demangle(ti.name(),(char*)&demangleSpace, &len, &status);
	string finalS = string(demangleSpace);
	if(finalS.size() > 0){
		finalS = finalS.substr(0, finalS.size() - 1);
	}
	return finalS;
}
#endif
