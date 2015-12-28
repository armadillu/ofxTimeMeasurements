/*
 *  ofxTimeMeasurements.h
 *  emptyExample
 *
 *  Created by Oriol Ferrer Mesià on 28/01/11.
 *  Copyright 2011 uri.cat. All rights reserved.
 *
 */

#pragma once

#include "ofxTimeMeasurements.h"
#include "ofxTimeMeasurementsMacros.h"


class ofxTimeMeasurementsScoped {

public:
	ofxTimeMeasurementsScoped(string key, bool acc) {

		this->key = key;
		this->acc = acc;

		if(acc) {
			TS_START_ACC_NIF(key);
		} else {
			TS_START_NIF(key);
		}
	}

	~ofxTimeMeasurementsScoped(){
		if(acc) {
			TS_STOP_ACC_NIF(key);
		} else {
			TS_STOP_NIF(key);
		}
	}

protected:

	ofxTimeMeasurementsScoped(){}

	bool acc;
	string key;
};
