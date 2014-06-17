/*
 *  ofxTimeMeasurements.cpp
 *  emptyExample
 *
 *  Created by Oriol Ferrer Mesià on 28/01/11.
 *  Copyright 2011 uri.cat. All rights reserved.
 *
 */

#include "ofxTimeMeasurements.h"
#include <float.h>

ofxTimeMeasurements* ofxTimeMeasurements::singleton = NULL; 

ofxTimeMeasurements::ofxTimeMeasurements(){
	desiredFrameRate = 60.0f;
	enabled = true;
	timeAveragePercent = 1;
	msPrecision = 1;
	stackLevel = 0;
	maxW = 27;

	bgColor = ofColor(15);
	selectionColor = ofColor::dodgerBlue;
	hilightColor = selectionColor * 0.7;
	textColor = ofColor(128);
	disabledTextColor = ofColor::purple;

	longestLabel = 0;
	selection = 0;
	drawLocation = TIME_MEASUREMENTS_BOTTOM_RIGHT;
	lastKey = "";
	numVisible = 0;

	activateKey = TIME_MEASUREMENTS_INTERACT_KEY;
	toggleSampleKey = TIME_MEASUREMENTS_TOGGLE_SAMPLE_KEY;
	enableKey = TIME_MEASUREMENTS_GLOBAL_TOGGLE_KEY;

	menuActive = false;
	loadSettings();

#if (OF_VERSION_MINOR >= 8)
		ofAddListener(ofEvents().update, this, &ofxTimeMeasurements::_beforeUpdate, OF_EVENT_ORDER_BEFORE_APP);
		ofAddListener(ofEvents().update, this, &ofxTimeMeasurements::_afterUpdate, OF_EVENT_ORDER_AFTER_APP);
		ofAddListener(ofEvents().draw, this, &ofxTimeMeasurements::_beforeDraw, OF_EVENT_ORDER_BEFORE_APP);
		ofAddListener(ofEvents().draw, this, &ofxTimeMeasurements::_afterDraw, OF_EVENT_ORDER_AFTER_APP);
		ofAddListener(ofEvents().keyPressed, this, &ofxTimeMeasurements::_keyPressed);
		ofAddListener(ofEvents().exit, this, &ofxTimeMeasurements::_appExited); //to save to xml
#else
	#if (OF_VERSION == 7 && OF_VERSION_MINOR >= 2 )
		ofAddListener(ofEvents().update, this, &ofxTimeMeasurements::_beforeUpdate);
		ofAddListener(ofEvents().update, this, &ofxTimeMeasurements::_afterUpdate);
		ofAddListener(ofEvents().draw, this, &ofxTimeMeasurements::_afterDraw);
		ofAddListener(ofEvents().draw, this, &ofxTimeMeasurements::_beforeDraw);
	#else
		ofAddListener(ofEvents.update, this, &ofxTimeMeasurements::_afterUpdate);
		ofAddListener(ofEvents.update, this, &ofxTimeMeasurements::_beforeUpdate);
		ofAddListener(ofEvents.draw, this, &ofxTimeMeasurements::_afterDraw);
		ofAddListener(ofEvents.draw, this, &ofxTimeMeasurements::_beforeDraw);
	#endif
#endif
}


ofxTimeMeasurements* ofxTimeMeasurements::instance(){	
	if (!singleton){   // Only allow one instance of class to be generated.
		singleton = new ofxTimeMeasurements();
	}
	return singleton;
}


float ofxTimeMeasurements::getLastDurationFor(string ID){

	float r = 0.0f;
	map<string,TimeMeasurement>::iterator it;
	it = times.find(ID);
	if ( it != times.end() ){	//not found!
		r = times[ID].duration / 1000.0f;
	}
	return r;
}


float ofxTimeMeasurements::getAvgDurationFor(string ID){

	float r = 0.0f;
	map<string,TimeMeasurement>::iterator it;
	it = times.find(ID);
	if ( it != times.end() ){	//not found!
		r = times[ID].avgDuration / 1000.0f;
	}
	return r;
}


bool ofxTimeMeasurements::startMeasuring(string ID){

	if (!enabled) return true;

	//see if we already had it, if we didnt, set its add order #
	map<string,TimeMeasurement>::iterator it;
	it = times.find(ID);
	if ( it == times.end() ){	//not found!
		keyOrder[ keyOrder.size() ] = ID;
		map<string, TimeMeasurementSettings>::iterator it2 = settings.find(ID);
		if (it2 != settings.end()){
			times[ID].visible = it2->second.visible;
			times[ID].enabled = it2->second.enabled;
		}
		updateNumVisible();
		updateLongestLabel();
	}
	
	TimeMeasurement t = times[ID];
	t.intensity = 1.0;
	t.measuring = true;
	t.microsecondsStart = ofGetElapsedTimeMicros();
	t.microsecondsStop = 0;
	t.error = true;
	t.updatedLastFrame = true;
	t.level = stackLevel;
	times[ID] = t;
	stackLevel ++;
	if(lastKey.length() && ID != TIME_MEASUREMENTS_DRAW_KEY && ID != TIME_MEASUREMENTS_UPDATE_KEY){
		times[lastKey].nextKey = ID;
	}
	lastKey = ID;
	return t.enabled;
}


float ofxTimeMeasurements::stopMeasuring(string ID){

	float ret = 0.0;
	if (!enabled) return ret;
	
	map<string,TimeMeasurement>::iterator it;
	it = times.find(ID);
	
	if ( it == times.end() ){	//not found!
		
		ofLog( OF_LOG_WARNING, "ID (%s)not found at stopMeasuring(). Make sure you called startMeasuring with that ID first.", ID.c_str());
		
	}else{
		
		if ( times[ID].measuring ){

			TimeMeasurement t = times[ID];
			t.measuring = false;
			t.error = false;
			t.microsecondsStop = ofGetElapsedTimeMicros();
			t.microsecondsStart = times[ID].microsecondsStart;
			ret = t.duration = t.microsecondsStop - t.microsecondsStart;
			t.avgDuration = (1.0f - timeAveragePercent) * times[ID].avgDuration + t.duration * timeAveragePercent;
			times[ID] = t;

		}else{	//wrong use, start first, then stop
			
			ofLog( OF_LOG_WARNING, "Can't stopMeasuring(%s). Make sure you called startMeasuring with that ID first.", ID.c_str());				
		}
		stackLevel--;
	}
	return ret;
}


void ofxTimeMeasurements::setDrawLocation(ofxTMDrawLocation l, ofVec2f p){
	drawLocation = l;
	loc = p;
}


void ofxTimeMeasurements::autoDraw(){

	switch(drawLocation){

		case TIME_MEASUREMENTS_TOP_LEFT:
			draw(TIME_MEASUREMENTS_EDGE_GAP_H,TIME_MEASUREMENTS_EDGE_GAP_V);
			break;
		case TIME_MEASUREMENTS_TOP_RIGHT:
			draw( ofGetWidth() - getWidth() - TIME_MEASUREMENTS_EDGE_GAP_H,TIME_MEASUREMENTS_EDGE_GAP_V);
			break;
		case TIME_MEASUREMENTS_BOTTOM_LEFT:
			draw(TIME_MEASUREMENTS_EDGE_GAP_H,ofGetHeight() - getHeight() - TIME_MEASUREMENTS_EDGE_GAP_V);
			break;
		case TIME_MEASUREMENTS_BOTTOM_RIGHT:
			draw( ofGetWidth() - getWidth() - TIME_MEASUREMENTS_EDGE_GAP_H,ofGetHeight() - getHeight() - TIME_MEASUREMENTS_EDGE_GAP_V);
			break;
		case TIME_MEASUREMENTS_CUSTOM_LOCATION:
			draw(loc.x, loc.y);
			break;
	}
}


void ofxTimeMeasurements::updateLongestLabel(){

	longestLabel = 0;
	for( map<int,string>::iterator ii = keyOrder.begin(); ii != keyOrder.end(); ++ii ){

		string key = (*ii).second;
		TimeMeasurement t = times[key];

		if (t.visible){

			string nesting = "";
			for(int i = 0; i < t.level; i++){
				nesting += " ";
			}

			if ( t.error == false ){
				bool isLast = (ii->first == keyOrder.size() -1);
				string label = " " + nesting + " " + key + "  "; //TODO this has to be same lenght as line #285
				int len = label.length();
				if (len > longestLabel){
					longestLabel = len;
				}
			}
		}
	}
}


void ofxTimeMeasurements::draw(float x, float y){

	//internalTimeSample = ofGetElapsedTimef();

	if (!enabled) return;
	if (ofGetFrameNum()%60 == 2){ //todo ghetto!
		updateLongestLabel();
	}

	static char msChar[64];
	static char percentChar[64];
	static char msg[128];

	float c = TIME_MEASUREMENTS_LINE_H_MULT;
	float timePerFrame = 1000.0f / desiredFrameRate;

	ofSetupScreen(); //mmmm----

	ofPushStyle();
	ofSetColor(bgColor);
	int barH = 1;
	ofRect(x, y, getWidth(), getHeight());

	ofSetColor(hilightColor);
	ofRect(x, y, getWidth(), barH);
	ofRect(x, y + getHeight() - TIME_MEASUREMENTS_LINE_HEIGHT - TIME_MEASUREMENTS_LINE_H_MULT * TIME_MEASUREMENTS_LINE_HEIGHT * 2.0 , getWidth(), barH);
	ofRect(x, y + getHeight(), getWidth() - barH, barH);

	float percentTotal = 0.0f;

	int lineC = 0;
	int tempMaxW = 0;

	for( map<int,string>::iterator ii = keyOrder.begin(); ii != keyOrder.end(); ++ii ){

		string key = (*ii).second;
		TimeMeasurement t = times[key];


		if (t.visible){
			c++;

			float ms = t.avgDuration / 1000.0f;
			float percent = 100.0f * ms / timePerFrame;
			//average here, only if enabled
			if (!t.updatedLastFrame && timeAveragePercent < 1.0f){ // if we didnt update that time, make it tend to zero slowly
				t.avgDuration = (1.0f - timeAveragePercent) * t.avgDuration;
			}

			t.updatedLastFrame = false;
			times[key] = t;
			bool isRoot = (key == TIME_MEASUREMENTS_UPDATE_KEY || key == TIME_MEASUREMENTS_DRAW_KEY);
			string nesting = "";
			for(int i = 0; i < t.level; i++){
				nesting += " ";
			}

			if ( t.error == false ){

				sprintf(msChar, "%*.*f", 4, msPrecision, ms );
				sprintf(percentChar, "% 6.1f",  percent );
				bool hasChild = false;
				if (t.nextKey.length()){
					if (times[t.nextKey].level != t.level){
						hasChild = true;
					}
				}
				bool isLast = (ii->first == keyOrder.size() -1);
				bool isEnabled = times[ii->second].enabled;
				string label =	" " +
								nesting +
								string(hasChild && !isLast ? "+" : "-") +
								key + " " +
								string(isEnabled ? " " : "!");

				string padding = "";
				for(int i = label.length(); i < longestLabel; i++){
					padding += " ";
				}

				string fullLine = label + padding + " " + msChar + "ms " + percentChar + "%";

				if(fullLine.length() > tempMaxW){
					tempMaxW = fullLine.length();
				}

				ofColor lineColor = textColor * (0.33 + 0.66 * t.intensity);
				if (!isEnabled) lineColor = disabledTextColor;
				if(lineC == selection && menuActive){
					if(ofGetFrameNum()%5 < 3){
						lineColor = selectionColor;
					}
				}
				ofSetColor(lineColor);
				ofDrawBitmapString( fullLine, x, y + c * TIME_MEASUREMENTS_LINE_HEIGHT );
			}else{
				ofDrawBitmapString( " " + key + " = Usage Error! see log...", x, y + c * TIME_MEASUREMENTS_LINE_HEIGHT );
			}
			lineC++;
			if(key == TIME_MEASUREMENTS_DRAW_KEY || key == TIME_MEASUREMENTS_UPDATE_KEY){
				percentTotal += percent;
			}
		}
		t.intensity *= 0.75; //reduce intensity
		times[key] = t;
	}

	maxW = tempMaxW;
	bool missingFrames = ( ofGetFrameRate() < desiredFrameRate - 1.0 ); // tolerance of 1 fps TODO!
	
	c += TIME_MEASUREMENTS_LINE_H_MULT * 2;

	//internalTimeSample = ofGetElapsedTimef() - internalTimeSample;

	sprintf(msg, "%2.1f fps % 5.1f%%", ofGetFrameRate(), percentTotal );
	c++;
	if(missingFrames){
		ofSetColor(170,33,33);
	}else{
		ofSetColor(hilightColor);
	}
	int len = strlen(msg);
	string pad = " ";
	int diff = (maxW - len) - 1;
	for(int i = 0; i < diff; i++) pad += " ";
	ofDrawBitmapString( pad + msg, x, y + c * TIME_MEASUREMENTS_LINE_HEIGHT );
	ofSetColor(hilightColor);
	ofDrawBitmapString( " '" + ofToString(char(activateKey)) + "'", x, y + c * TIME_MEASUREMENTS_LINE_HEIGHT );
	ofPopStyle();
	c += TIME_MEASUREMENTS_LINE_H_MULT * 2;
}


void ofxTimeMeasurements::_keyPressed(ofKeyEventArgs &e){

	if (e.key == enableKey){
		TIME_SAMPLE_SET_ENABLED(!TIME_SAMPLE_GET_ENABLED());
		if (!TIME_SAMPLE_GET_ENABLED()){
			for( map<int,string>::iterator ii = keyOrder.begin(); ii != keyOrder.end(); ++ii ){
				settings[ii->second].visible = times[ii->second].visible;
				settings[ii->second].enabled = times[ii->second].enabled;
			}
			keyOrder.clear();
			times.clear();
		}
	}

	if (TIME_SAMPLE_GET_ENABLED()){
		if (e.key == activateKey){
			menuActive = !menuActive;
		}

		if(menuActive){
			map<int,string>::iterator lastItem = keyOrder.end();
			map<int,string>::iterator firstItem = keyOrder.begin();

			switch (e.key) {

				case OF_KEY_DOWN:{
					map<int,string>::iterator it = keyOrder.find(selection);
					it++;
					if (it == lastItem){
						it = firstItem;
					}else{
						while (!times[it->second].visible) {
							it++;
							if(it == lastItem){
								it = firstItem;
								break;
							}
						}
					}
					selection = it->first;
				}break;

				case OF_KEY_UP:{
					map<int,string>::iterator it = keyOrder.find(selection);
					if (it == firstItem){
						it = lastItem;
						it--;
					}else{
						it--;
					}
					while (!times[it->second].visible) {
						it--;
						if(it == firstItem){
							it = lastItem;
							break;
						}
					}
					selection = it->first;
				}break;

				case OF_KEY_RETURN:{
					map<int,string>::iterator it = keyOrder.find(selection);
					if (it != keyOrder.end() ){
						//cant disable update() & draw()
						if (it->second != TIME_MEASUREMENTS_UPDATE_KEY && it->second != TIME_MEASUREMENTS_DRAW_KEY ){
							times[it->second].enabled = !times[it->second].enabled;
						}
					}
					}break;

				case OF_KEY_RIGHT:
					collapseExpand(selection, false /*expand*/);
					updateNumVisible();
				break;

				case OF_KEY_LEFT:
					collapseExpand(selection, true /*collapse*/);
					updateNumVisible();
					break;
			}
		}
	}
}


void ofxTimeMeasurements::loadSettings(){

	ifstream myfile(ofToDataPath(TIME_MEASUREMENTS_SETTINGS_FILENAME,true).c_str());
	string name, visible, enabled;

	if (myfile.is_open()){
		while( !myfile.eof() ){
			getline( myfile, name, '=' );//name
			getline( myfile, visible, '|' ); //visible
			getline( myfile, enabled, '\n' ); //enabled
			settings[name].visible = bool(visible == "1" ? true : false);
			settings[name].enabled = bool(enabled == "1" ? true : false);
		}
		myfile.close();
	}else{
		ofLogWarning() << "Unable to load Settings file " << TIME_MEASUREMENTS_SETTINGS_FILENAME;
	}
}


void ofxTimeMeasurements::_appExited(ofEventArgs &e){

	ofstream myfile;
	myfile.open(ofToDataPath(TIME_MEASUREMENTS_SETTINGS_FILENAME,true).c_str());
	for( map<int,string>::iterator ii = keyOrder.begin(); ii != keyOrder.end(); ++ii ){
		myfile << ii->second << "=" << string(times[ii->second].visible ? "1" : "0") << "|" <<
		string(times[ii->second].enabled ? "1" : "0") << endl;
	}
	myfile.close();
}


void ofxTimeMeasurements::updateNumVisible(){
	numVisible = 0;
	for( map<int,string>::iterator ii = keyOrder.begin(); ii != keyOrder.end(); ++ii ){
		if(times[ii->second].visible) numVisible++;
	}
}


void ofxTimeMeasurements::collapseExpand(int sel, bool collapse){

	map<int,string>::iterator it = keyOrder.find(sel);
	map<int,string>::iterator lastItem = keyOrder.end();

	int baseLevel = times[it->second].level;
	it++;
	if(it != lastItem){
		while (times[it->second].level > baseLevel ) {
			times[it->second].visible = !collapse;
			it++;
			if(it == lastItem){
				break;
			}
		}
	}
	updateLongestLabel();
}


unsigned long ofxTimeMeasurements::durationForID( string ID){

	map<string,TimeMeasurement>::iterator it;
	it = times.find(ID);
	
	if ( it == times.end() ){	//not found!
		if ( times[ID].error ){
			return times[ID].duration;
		}
	}
	return 0;
}


void ofxTimeMeasurements::setTimeAveragePercent(float p){
	timeAveragePercent = ofClamp(p, 0.0f, 1.0f);
}


void ofxTimeMeasurements::setDesiredFrameRate(float fr){
	desiredFrameRate = fr;
}


void ofxTimeMeasurements::setEnabled(bool ena){
	enabled = ena;
}


bool ofxTimeMeasurements::getEnabled(){
	return enabled;
}


void ofxTimeMeasurements::setMsPrecision(int digits){
	msPrecision = digits;
}

