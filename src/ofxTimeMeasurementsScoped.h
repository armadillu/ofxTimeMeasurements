/*
 *  ofxTimeMeasurements.h
 *  emptyExample
 *
 *  Created by Oriol Ferrer Mesià on 28/01/11.
 *  Copyright 2011 uri.cat. All rights reserved.
 *
 */

#pragma once


class ofxTimeMeasurementsScoped {

public:
	ofxTimeMeasurementsScoped(std::string key, bool acc);
	~ofxTimeMeasurementsScoped();

protected:

	ofxTimeMeasurementsScoped(){}

	bool acc;
	std::string key;
};
