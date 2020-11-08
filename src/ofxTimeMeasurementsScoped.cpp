/*
 *  ofxTimeMeasurements.h
 *  emptyExample
 *
 *  Created by Oriol Ferrer Mesià on 28/01/11.
 *  Copyright 2011 uri.cat. All rights reserved.
 *
 */

#include "ofxTimeMeasurements.h"
#include "ofxTimeMeasurementsMacros.h"

ofxTimeMeasurementsScoped::ofxTimeMeasurementsScoped(std::string key, bool acc) {

	this->key = key;
	this->acc = acc;

	if(acc) {
		TS_START_ACC_NIF(key);
	} else {
		TS_START_NIF(key);
	}
}

ofxTimeMeasurementsScoped::~ofxTimeMeasurementsScoped(){
	if(acc) {
		TS_STOP_ACC_NIF(key);
	} else {
		TS_STOP_NIF(key);
	}
}
