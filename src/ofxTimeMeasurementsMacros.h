/*
 *  ofxTimeMeasurementsMacros.h
 *  emptyExample
 *
 *  Created by Oriol Ferrer Mesià on 28/01/11.
 *  Copyright 2011 uri.cat. All rights reserved.
 *
 */

#pragma once


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

//shortcuts!

//time sample a whole method - ie TS(drawMonkeys());
#define TS(x)			if(ofxTimeMeasurements::instance()->startMeasuring(#x, false)){ x; }ofxTimeMeasurements::instance()->stopMeasuring(#x, false)
#define TS_ACC(x)		if(ofxTimeMeasurements::instance()->startMeasuring(#x, true)){ x; }ofxTimeMeasurements::instance()->stopMeasuring(#x, true)

//includes if(){ } caluse for you
#define TS_START(x, ...)				TIME_SAMPLE_START(x, ##__VA_ARGS__)
#define TS_STOP(x)						TIME_SAMPLE_STOP(x)
#define TS_START_ACC(x, ...	)			TIME_SAMPLE_START_ACC(x, ##__VA_ARGS__)
#define TS_STOP_ACC(x)					TIME_SAMPLE_STOP_ACC(x)


//no if clause, to be used in timings that are measured across several methods / frames
#define TS_START_NIF(x, ...)				TIME_SAMPLE_START_NOIF(x, ##__VA_ARGS__)
#define TS_STOP_NIF(x)						TIME_SAMPLE_STOP_NOIF(x)
#define TS_START_ACC_NIF(x, ...)			TIME_SAMPLE_START_ACC_NOIF(x, ##__VA_ARGS__)
#define TS_STOP_ACC_NIF(x)					TIME_SAMPLE_STOP_ACC_NOIF(x)


//locations shortcuts
#define TIME_SAMPLE_DRAW_LOC_TOP_LEFT 		TIME_MEASUREMENTS_TOP_LEFT
#define TIME_SAMPLE_DRAW_LOC_BOTTOM_LEFT 	TIME_MEASUREMENTS_BOTTOM_LEFT
#define TIME_SAMPLE_DRAW_LOC_BOTTOM_RIGHT 	TIME_MEASUREMENTS_BOTTOM_RIGHT
#define TIME_SAMPLE_DRAW_LOC_TOP_RIGHT 		TIME_MEASUREMENTS_TOP_RIGHT
