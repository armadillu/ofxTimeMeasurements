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
	maxW = 27;

	bgColor = ofColor(15);
	hilightColor = ofColor(44,77,255) * 1.5;
	disabledTextColor = ofColor(255,0,255);
	measuringColor = ofColor(0,130,0);

	idleTimeColorFadePercent = 0.5;
	idleTimeColorDecay = 0.96;

	longestLabel = 0;
	selection = TIME_MEASUREMENTS_UPDATE_KEY;
	drawLocation = TIME_MEASUREMENTS_BOTTOM_RIGHT;
	numVisible = 0;

	enableKey = TIME_MEASUREMENTS_GLOBAL_TOGGLE_KEY;
	activateKey = TIME_MEASUREMENTS_INTERACT_KEY;
	toggleSampleKey = TIME_MEASUREMENTS_TOGGLE_SAMPLE_KEY;

	menuActive = false;

	mainThreadID = Poco::Thread::current();

	int v = 180;
	threadColorTable.push_back(ofColor(v,0,0));
	threadColorTable.push_back(ofColor(0,v,0));
	threadColorTable.push_back(ofColor(v,v,0));
	threadColorTable.push_back(ofColor(0,v,v));
	threadColorTable.push_back(ofColor(v,0,v));
	threadColorTable.push_back(ofColor(v,v/2,0));
	numThreads = 0;

	configsDir = ".";
	removeExpiredThreads = true;

	settingsLoaded = false;

#if (OF_VERSION_MINOR >= 8)
		ofAddListener(ofEvents().setup, this, &ofxTimeMeasurements::_beforeSetup, OF_EVENT_ORDER_BEFORE_APP);
		ofAddListener(ofEvents().setup, this, &ofxTimeMeasurements::_afterSetup, OF_EVENT_ORDER_AFTER_APP);
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


void ofxTimeMeasurements::setThreadColors(vector<ofColor> tc){
	threadColorTable.clear();
	threadColorTable = tc;
}


ofxTimeMeasurements* ofxTimeMeasurements::instance(){	
	if (!singleton){   // Only allow one instance of class to be generated.
		singleton = new ofxTimeMeasurements();
	}
	return singleton;
}


float ofxTimeMeasurements::getLastDurationFor(string ID){

	float r = 0.0f;
	map<string,TimeMeasurement*>::iterator it;
	it = times.find(ID);
	if ( it != times.end() ){	//not found!
		r = times[ID]->duration / 1000.0f; //to ms
	}
	return r;
}


float ofxTimeMeasurements::getAvgDurationFor(string ID){

	float r = 0.0f;
	map<string,TimeMeasurement*>::iterator it;
	it = times.find(ID);
	if ( it != times.end() ){	//not found!
		r = times[ID]->avgDuration / 1000.0f; //to ms
	}
	return r;
}

void ofxTimeMeasurements::setHighlightColor(ofColor c){
	hilightColor = c;
	threadInfo[mainThreadID].color = c;
}


bool ofxTimeMeasurements::startMeasuring(string ID, bool accumulate){

	if (!enabled) return true;
	if (!settingsLoaded){
		loadSettings();
		settingsLoaded = true;
	}

	Poco::Thread * thread = Poco::Thread::current();
	bool isMainThread = (mainThreadID == thread);

	uint64_t timeNow = TM_GET_MICROS(); //get the time before the lock() to avoid affecting
	//the measurement as much as possible

	mutex.lock();

	map<Poco::Thread*, ThreadInfo>::iterator threadIt = threadInfo.find(thread);

	if (threadIt == threadInfo.end()){ //new thread!

		threadOrder[threadOrder.size()] = thread;
		//string tName = isMainThread ? "mainThread" : string("Thread " + ofToString(threadCounter));
		string tName = isMainThread ? "Main Thread" : string(Poco::Thread::current()->getName() +
															 " Thread(" + ofToString(numThreads) + ")");
		//init the iterator
		threadInfo[thread].tit = threadInfo[thread].tree.insert(threadInfo[thread].tree.begin(), tName);
		if (thread){
			threadInfo[thread].color = threadColorTable[numThreads%(threadColorTable.size())];
			numThreads++;
		}else{
			threadInfo[thread].color = hilightColor;
		}
	}

	tree<string> &tr = threadInfo[thread].tree; //easier to read, tr is our tree from now on

//	if(thread){ //add thread name prefix to ID to minimize name conflicts
//		ID = thread->getName() + " " + ID;
//	}

	//see if the new measurement already was in tree
	tree<string>::iterator current = threadInfo[thread].tit;
	tree<string>::sibling_iterator searchIt = find(tr.begin(), tr.end(), ID);

	if(searchIt == tr.end()){ //if it wasnt in the tree, append it

		if (ID == TIME_MEASUREMENTS_SETUP_KEY ||
			ID == TIME_MEASUREMENTS_UPDATE_KEY ||
			ID == TIME_MEASUREMENTS_DRAW_KEY
			){ //setup update and draw are always at root level!
			threadInfo[thread].tit = tr.append_child(tr.begin(), ID);
		}else{
			threadInfo[thread].tit = tr.append_child(current, ID);
		}
	}else{
		threadInfo[thread].tit = searchIt;
	}

	//see if we had an actual measurement, or its a new one
	map<string, TimeMeasurement*>::iterator tit = times.find(ID);

	if (tit == times.end()){ //not found, let alloc a new TimeMeasurement
		times[ID] = new TimeMeasurement();
		//keyOrder[ keyOrder.size() ] = ID;
		map<string, TimeMeasurementSettings>::iterator it2 = settings.find(ID);
		if (it2 != settings.end()){
			times[ID]->settings = settings[ID];
		}
	}

	TimeMeasurement* t = times[ID];
	t->key = ID;
	t->life = 1.0f; //
	t->measuring = true;
	t->microsecondsStart = timeNow;
	t->microsecondsStop = 0;
	t->accumulating = accumulate;
	t->error = false;
	t->frame = ofGetFrameNum();
	t->measuring = true;
	t->updatedLastFrame = true;

	mutex.unlock();

	return t->settings.enabled;
}


float ofxTimeMeasurements::stopMeasuring(string ID, bool accumulate){

	float ret = 0.0f;
	if (!enabled) return ret;

	uint64_t timeNow = TM_GET_MICROS(); //get the time before the lock() to avoid affecting
	//the measurement as much as possible

	Poco::Thread * thread = Poco::Thread::current();
//	if(thread){ //add thread name prefix to ID to minimize name conflicts
//		ID = thread->getName() + " " + ID;
//	}

	mutex.lock();

	tree<string> &tr = threadInfo[thread].tree; //easier to read, tr is our tree from now on
	tree<string>::iterator & tit = threadInfo[thread].tit;
	tit = tr.parent(tit);
	if(tit == NULL) tit = tr.begin();

	map<string,TimeMeasurement*>::iterator it;
	it = times.find(ID);
	
	if ( it == times.end() ){	//not found!
		ofLog( OF_LOG_WARNING, "ID (%s)not found at stopMeasuring(). Make sure you called"
			  " startMeasuring with that ID first.", ID.c_str());
	}else{
		
		TimeMeasurement* t = times[ID];

		if ( times[ID]->measuring ){

			t->measuring = false;
			t->error = false;
			t->acrossFrames = (t->frame != ofGetFrameNum() && thread == NULL); //we only care about across-frames in main thread
			t->microsecondsStop = timeNow;
			ret = t->duration = t->microsecondsStop - t->microsecondsStart;
			t->avgDuration = (1.0f - timeAveragePercent) * t->avgDuration + t->duration * timeAveragePercent;
			if (accumulate){
				t->microsecondsAccum += t->duration;
			}

		}else{	//wrong use, start first, then stop

			t->error = true;
			ofLog( OF_LOG_WARNING, "Can't stopMeasuring(%s). Make sure you called startMeasuring"
				  " with that ID first.", ID.c_str());
		}
	}
	ret = ret / 1000.0f;
	mutex.unlock();
	return ret; //convert to ms
}


void ofxTimeMeasurements::setDrawLocation(ofxTMDrawLocation l, ofVec2f p){
	drawLocation = l;
	customDrawLocation = p;
}


void ofxTimeMeasurements::_afterDraw(ofEventArgs &d){
	stopMeasuring(TIME_MEASUREMENTS_DRAW_KEY, false);
	autoDraw();
};


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
			draw( ofGetWidth() - getWidth() - TIME_MEASUREMENTS_EDGE_GAP_H,
				 ofGetHeight() - getHeight() - TIME_MEASUREMENTS_EDGE_GAP_V);
			break;
		case TIME_MEASUREMENTS_CUSTOM_LOCATION:
			draw(customDrawLocation.x, customDrawLocation.y);
			break;
	}
}


void ofxTimeMeasurements::updateLongestLabel(){

	longestLabel = 0;
	for( int i = 0; i < drawLines.size(); i++ ){

		TimeMeasurement *t = drawLines[i].tm;
		if (t){
			if (t->settings.visible){ //kinda redundant ...
				int len = drawLines[i].formattedKey.length();
				if (len > longestLabel){
					longestLabel = len;
				}
			}
		}
	}
}


void ofxTimeMeasurements::draw(float x, float y){

	vector<string> hiddenKeys;

	if (!enabled) return;

	drawLines.clear();
	float percentTotal = 0.0f;
	float timePerFrame = 1000.0f / desiredFrameRate;

	mutex.lock();

	//update time stuff, build draw lists
	for( map<string,TimeMeasurement*>::iterator ii = times.begin(); ii != times.end(); ++ii ){
		TimeMeasurement* t = ii->second;
		string key = ii->first;
		if(!t->measuring){
			t->life *= idleTimeColorDecay; //decrease life
		}
		if (!t->updatedLastFrame && timeAveragePercent < 1.0f){ // if we didnt update that time, make it tend to zero slowly
			t->avgDuration = (1.0f - timeAveragePercent) * t->avgDuration;
		}
		t->updatedLastFrame = false;
	}

	map<Poco::Thread*, ThreadInfo>::iterator ii;
	vector<Poco::Thread*> expiredThreads;

	int c = 0;
	for( ii = threadInfo.begin(); ii != threadInfo.end(); ++ii ){ //walk all thread trees

		Poco::Thread* thread = ii->first;
		tree<string> &tr = ii->second.tree;

		c++;
		tree<string>::iterator walker = tr.begin();

		PrintedLine header;
		header.formattedKey = "+" + *walker;
		header.color = threadInfo[thread].color;
		header.key = *walker; //key for selection, is thread name
		drawLines.push_back(header); //add header to drawLines

		int numAlive = 0;
		int numAdded = 0;
		if( walker != tr.end()){

			tree<string>::iterator sib = tr.begin(walker);
			tree<string>::iterator end = tr.end(walker);

			while(sib != end) {

				string key = *sib;
				TimeMeasurement * t = times[key];

				bool visible = t->settings.visible;
				bool alive = t->life > 0.001;
				if(alive){
					numAlive++;
				}

				if (visible){
					PrintedLine l;
					l.key = key;
					l.tm = t;
					for(int i = 0; i < tr.depth(sib); ++i)
						l.formattedKey += " ";
					if (sib.number_of_children() == 0){
						l.formattedKey += "-";
					}else{
						l.formattedKey += "+";
					}
					l.formattedKey += key;
					l.time = getTimeStringForTM(t);

					//l.color = textColor * (0.35f + 0.65f * t->life);
					l.color = threadInfo[thread].color * ((1.0 - idleTimeColorFadePercent) + idleTimeColorFadePercent * t->life);
					if (!t->settings.enabled){
						l.color = disabledTextColor;
					}
//					if (t->measuring){
//						l.color = measuringColor;
//					}
					if (*sib == selection && menuActive){
						if(ofGetFrameNum()%5 < 4){
							l.color.invert();
						}
					}
					drawLines.push_back(l);
					numAdded++;
				}

				//only update() and draw() count to the final %
				if(key == TIME_MEASUREMENTS_DRAW_KEY || key == TIME_MEASUREMENTS_UPDATE_KEY){
					percentTotal += (t->avgDuration * 0.1f) / timePerFrame;
				}
				t->accumulating = false;
				t->microsecondsAccum = 0;
				++sib;
			}
		}

		if (numAlive == 0 && removeExpiredThreads){
			//drop that whole section if all entries in it are not alive
			for(int i = 0; i < numAdded + 1; i++){
				if(drawLines.size() > 0){
					int delID = drawLines.size() - 1;
					//clear selection if needed
					if (selection == drawLines[delID].key){
						selection = TIME_MEASUREMENTS_UPDATE_KEY;
					}
					drawLines.erase(drawLines.begin() + delID);
				}
			}
			expiredThreads.push_back(thread);
		}
	}

	//delete expired threads
	for(int i = 0; i < expiredThreads.size(); i++){
		map<Poco::Thread*, ThreadInfo>::iterator treeIt = threadInfo.find(expiredThreads[i]);
		if (treeIt != threadInfo.end()) threadInfo.erase(treeIt);
	}

	mutex.unlock();

	updateLongestLabel();

	//update max width, find headers
	int tempMaxW = -1;
	vector<int> headerLocations;
	for( int i = 0; i < drawLines.size(); i++ ){
		if (drawLines[i].tm){ //its a measurement
			//add padding to draw in columns
			for(int j = drawLines[i].formattedKey.length(); j < longestLabel; j++){
				drawLines[i].formattedKey += " ";
			}
			if (!drawLines[i].tm->error){
				drawLines[i].fullLine = drawLines[i].formattedKey + " " + drawLines[i].time;
			}else{
				drawLines[i].fullLine = drawLines[i].formattedKey + "    Error!" ;
			}
			int len = drawLines[i].fullLine.length();
			if(len > tempMaxW) tempMaxW = len;
		}else{ //its a header
			drawLines[i].fullLine = drawLines[i].formattedKey;
			headerLocations.push_back(i);
		}
	}
	maxW = tempMaxW;

	ofSetupScreen(); //mmmm----

	ofPushStyle();
	ofSetColor(bgColor, 245);
	int barH = 1;
	ofRect(x, y + 1, getWidth(), getHeight());

	//thread header bg highlight
	for(int i = 0; i < headerLocations.size(); i++){
		int loc = headerLocations[i];
		//whole section
		ofSetColor(drawLines[loc].color, 40);
		int h = TIME_MEASUREMENTS_LINE_HEIGHT * ((i < headerLocations.size() - 1) ? headerLocations[i+1] - headerLocations[i] : drawLines.size() - loc );
		ofRect(x, y + 2 + loc * TIME_MEASUREMENTS_LINE_HEIGHT, getWidth(), h);
		//thread header
		ofSetColor(drawLines[loc].color, 40);
		ofRect(x, y + 2 + loc * TIME_MEASUREMENTS_LINE_HEIGHT, getWidth(), TIME_MEASUREMENTS_LINE_HEIGHT + 1);
	}

	ofSetColor(hilightColor);
	ofRect(x, y + 1, getWidth(), barH);
	ofRect(x, y + getHeight() - TIME_MEASUREMENTS_LINE_HEIGHT - 4 , getWidth(), barH);
	ofRect(x, y + getHeight(), getWidth() - barH, barH);

	for(int i = 0; i < drawLines.size(); i++){
		ofSetColor(drawLines[i].color);
		ofDrawBitmapString(drawLines[i].fullLine, x , y + (i + 1) * TIME_MEASUREMENTS_LINE_HEIGHT);
	}

	//print bottom line, fps and stuff
	bool missingFrames = ( ofGetFrameRate() < desiredFrameRate - 1.0 ); // tolerance of 1 fps TODO!
	static char msg[128];

	sprintf(msg, "%2.1f fps % 5.1f%%", ofGetFrameRate(), percentTotal );
	if(missingFrames){
		ofSetColor(170,33,33);
	}else{
		ofSetColor(hilightColor);
	}
	int len = strlen(msg);
	string pad = " ";
	int diff = (maxW - len) - 1;
	for(int i = 0; i < diff; i++) pad += " ";
	int lastLine = ( drawLines.size() + 1 ) * TIME_MEASUREMENTS_LINE_HEIGHT + 2;
	ofDrawBitmapString( pad + msg, x, y + lastLine );
	ofSetColor(hilightColor);
	ofDrawBitmapString( " '" + ofToString(char(activateKey)) + "'" + string(timeAveragePercent < 1.0 ? " avgd!" : ""),
					   x, y + lastLine );
	ofPopStyle();
}


void ofxTimeMeasurements::_keyPressed(ofKeyEventArgs &e){

	if (e.key == enableKey){
		TIME_SAMPLE_SET_ENABLED(!TIME_SAMPLE_GET_ENABLED());
		if (!TIME_SAMPLE_GET_ENABLED()){
			for( map<string, TimeMeasurement*>::iterator ii = times.begin(); ii != times.end(); ++ii ){
				string key = ii->first;
				settings[key].visible = times[key]->settings.visible;
				settings[key].enabled = times[key]->settings.enabled;
			}
			//times.clear();
		}
	}

	if (TIME_SAMPLE_GET_ENABLED()){
		if (e.key == activateKey){
			menuActive = !menuActive;
		}

		if(menuActive){

			if (drawLines.size()){
				int selIndex = -1;
				for(int i = 0; i < drawLines.size(); i++){
					if (drawLines[i].key == selection) selIndex = i;
				}
				if(selIndex == -1){
					return;
				}

				switch (e.key) {

					case OF_KEY_DOWN:{
						selIndex ++;
						if(selIndex >= drawLines.size()) selIndex = 0;
						while(drawLines[selIndex].tm == NULL){
							selIndex ++;
							if(selIndex >= drawLines.size()) selIndex = 0;
						}
						selection = drawLines[selIndex].key;
					}break;

					case OF_KEY_UP:{
						selIndex --;
						if(selIndex < 0 ) selIndex = drawLines.size() - 1;
						while(drawLines[selIndex].tm == NULL){
							selIndex --;
							if(selIndex < 0 ) selIndex = drawLines.size() - 1;
						}
						selection = drawLines[selIndex].key;
					}break;

					case OF_KEY_RETURN:{
							//cant disable update() & draw()
							if (selection != TIME_MEASUREMENTS_SETUP_KEY &&
								selection != TIME_MEASUREMENTS_UPDATE_KEY &&
								selection != TIME_MEASUREMENTS_DRAW_KEY &&
								drawLines[selIndex].tm
								){
									times[selection]->settings.enabled ^= 1;
							}
						}break;

					case OF_KEY_RIGHT:
						collapseExpand(selection, false); //expand
					break;

					case OF_KEY_LEFT:
						collapseExpand(selection, true ); //collapse
						break;
				}
			}
		}
	}
}


void ofxTimeMeasurements::collapseExpand(string sel, bool collapse){

	map<Poco::Thread*, ThreadInfo>::iterator ii;

	for( ii = threadInfo.begin(); ii != threadInfo.end(); ++ii ){

		tree<string> &tr = ii->second.tree;
		tree<string>::iterator loc = find(tr.begin(), tr.end(), sel);

		if( loc != tr.end()) {
			tree<string>::iterator sib2 = tr.begin(loc);
			tree<string>::iterator end2 = tr.end(loc);
			while(sib2 != end2) {
				times[*sib2]->settings.visible = !collapse;
				++sib2;
			}
		}
	}
}

string ofxTimeMeasurements::formatTime(uint64_t microSeconds, int precision){

	float time = microSeconds / 1000.0f; //to ms
	string timeUnit = "ms";
	if (time > 1000){ //if more than 1 sec
		time /= 1000.0f;
		timeUnit = "sec";
		if(time > 60){ //if more than a minute
			time /= 60.0f;
			timeUnit = "min";
		}
	}
	return ofToString(time,  precision) + timeUnit;
}


string ofxTimeMeasurements::getTimeStringForTM(TimeMeasurement* tm) {

	string timeUnit;
	float time;
	if (tm->measuring){
		string anim;
		switch ((int(ofGetFrameNum() * 0.2f))%6) {
			case 0: anim = "   "; break;
			case 1: anim = ".  "; break;
			case 2: anim = ".. "; break;
			case 3: anim = "..."; break;
			case 4: anim = " .."; break;
			case 5: anim = "  ."; break;
		}
		//return "   Running " + anim;
		return string((ofGetFrameNum()% 6 < 3 ) ? " >  " : "    ") +
				formatTime( ofGetElapsedTimeMicros() - tm->microsecondsStart, 1) +
				anim;
	}else{

		string timeString;
		char percentChar[64];

		if (!tm->settings.enabled){
			return "   DISABLED!";
		}else{

			if(tm->accumulating){
				timeString = formatTime(tm->microsecondsAccum, msPrecision);
				time = tm->microsecondsAccum / 1000.0f;
			}else{
				timeString = formatTime(tm->avgDuration, msPrecision);
				time = tm->avgDuration / 1000.0f;
			}

			float percent = 100.0f * time / (1000.0f / desiredFrameRate);
			bool over = false;
			if (percent > 100.0f){
				percent = 100.0f;
				over = true;
			}
			timeString = ofToString(time,  msPrecision) + timeUnit;
			int originalLen = timeString.length();

			int expectedLen = 8;
			for(int i = 0; i < expectedLen - originalLen; i++){
				timeString = " " + timeString;
			}

			if (over){
				sprintf(percentChar, int(ofGetFrameNum() * 0.8)%5 < 3  ? " *100": "  100");
			}else{
				sprintf(percentChar, "% 5.1f", percent);
			}
		}

		return timeString + percentChar + "%" ;
	}
}


void ofxTimeMeasurements::loadSettings(){

	//todo this might get called before OF is setup, os ofToDataPath gives us weird results sometimes?
	string f = ofToDataPath(TIME_MEASUREMENTS_SETTINGS_FILENAME, true);
	ifstream myfile(f.c_str());
	string name, visible, enabled_;

	if (myfile.is_open()){

		while( !myfile.eof() ){
			getline( myfile, name, '=' );//name
			getline( myfile, visible, '|' ); //visible
			getline( myfile, enabled_, '\n' ); //enabled

			if (name == TIME_MEASUREMENTS_SETUP_KEY ||
				name == TIME_MEASUREMENTS_UPDATE_KEY ||
				name == TIME_MEASUREMENTS_DRAW_KEY ){
				visible = enabled_ = "1";
			}
			if(name.length()){
				settings[name].visible = bool(visible == "1" ? true : false);
				settings[name].enabled = bool(enabled_ == "1" ? true : false);
				//cout << name << " " << visible << " " << enabled << endl;
			}
		}
		myfile.close();
	}else{
		ofLogWarning() << "Unable to load Settings file " << TIME_MEASUREMENTS_SETTINGS_FILENAME;
	}
}


void ofxTimeMeasurements::saveSettings(){

	ofDirectory d;
	d.open(configsDir);
	if(!d.exists()){
		d.create(true);
	}
	ofstream myfile;
	myfile.open(ofToDataPath(TIME_MEASUREMENTS_SETTINGS_FILENAME,true).c_str());
	for( map<string, TimeMeasurement*>::iterator ii = times.begin(); ii != times.end(); ++ii ){
		bool visible = times[ii->first]->settings.visible;
		bool enabled = times[ii->first]->settings.enabled;

		if (ii->first == TIME_MEASUREMENTS_SETUP_KEY ||
			ii->first == TIME_MEASUREMENTS_UPDATE_KEY ||
			ii->first == TIME_MEASUREMENTS_DRAW_KEY){
			visible = enabled = true;
		}

		myfile << ii->first << "=" << string(visible ? "1" : "0") << "|" <<
		string(enabled ? "1" : "0") << endl;
	}
	myfile.close();
}


void ofxTimeMeasurements::_appExited(ofEventArgs &e){
	saveSettings();
}



float ofxTimeMeasurements::durationForID( string ID){

	map<string,TimeMeasurement*>::iterator it;
	it = times.find(ID);
	
	if ( it == times.end() ){	//not found!
		if ( times[ID]->error ){
			return times[ID]->duration / 1000.0; //to ms
		}
	}
	return 0;
}


void ofxTimeMeasurements::setTimeAveragePercent(double p){
	if(p > 1.0) p = 1.0;
	if(p < 0.0) p = 0.0;
	timeAveragePercent = p;
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

