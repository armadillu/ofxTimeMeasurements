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

static int threadCounter = 0;
ofxTimeMeasurements* ofxTimeMeasurements::singleton = NULL; 

ofxTimeMeasurements::ofxTimeMeasurements(){

	desiredFrameRate = 60.0f;
	enabled = true;
	timeAveragePercent = 1;
	msPrecision = 1;
	maxW = 27;

	bgColor = ofColor(15);
	selectionColor = ofColor(77,100,255);
	hilightColor = selectionColor * 0.7;
	textColor = ofColor(128);
	disabledTextColor = ofColor(255,0,255);

	longestLabel = 0;
	selection = TIME_MEASUREMENTS_UPDATE_KEY;
	drawLocation = TIME_MEASUREMENTS_BOTTOM_RIGHT;
	numVisible = 0;

	activateKey = TIME_MEASUREMENTS_INTERACT_KEY;
	toggleSampleKey = TIME_MEASUREMENTS_TOGGLE_SAMPLE_KEY;
	enableKey = TIME_MEASUREMENTS_GLOBAL_TOGGLE_KEY;

	menuActive = false;

	mainThreadID = Poco::Thread::current();

	loadSettings();

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


bool ofxTimeMeasurements::startMeasuring(string ID){

	if (!enabled) return true;

	Poco::Thread * thread = Poco::Thread::current();
	bool isMainThread = (mainThreadID == thread);

	mutex.lock();

	map<Poco::Thread*, tree<string>	>::iterator threadIt;
	threadIt = threadTrees.find(thread);

	if (threadIt == threadTrees.end()){ //new thread!

		if (!isMainThread){
			threadCounter++; //count different threads, to ID them in some human readble way
		}
		//string tName = isMainThread ? "mainThread" : string("Thread " + ofToString(threadCounter));
		string tName = isMainThread ? "main Thread" : string(Poco::Thread::current()->getName());

		//init the iterator
		threadTreesIterators[thread] = threadTrees[thread].insert(threadTrees[thread].begin(), tName);
	}

	tree<string> &tr = threadTrees[thread]; //easier to read, tr is our tree from now on

	//see if the new measurement already was in tree
	tree<string>::iterator current = threadTreesIterators[thread];
	//cout << "currently at " << *current << " measuring '" << ID << "'" << endl;
	tree<string>::sibling_iterator searchIt = find(tr.begin(), tr.end(), ID);

	if(searchIt == tr.end()){ //if it wasst in the tree, append it
		threadTreesIterators[thread] = tr.append_child(current, ID);
	}else{
		threadTreesIterators[thread] = searchIt;
	}

	//if(thread){
		//int d = tr.depth(threadTreesIterators[thread]);
		//cout << "### thread: '" << thread << "' START >> '" << ID << "' : " << d << endl;
		//cout << "  it is at: " << *threadTreesIterators[thread] << endl;
		//see if we already had it, if we didnt, set its add order #
	//}

	map<string, TimeMeasurement*>::iterator tit = times.find(ID);

	if (tit == times.end()){ //not found, let alloc a new TimeMeasurement
		times[ID] = new TimeMeasurement();
		keyOrder[ keyOrder.size() ] = ID;
//		map<string, TimeMeasurementSettings>::iterator it2 = settings.find(ID);
//		if (it2 != settings.end()){
//			settings[ID].visible = it2->second.visible;
//			settings[ID].enabled = it2->second.enabled;
//		}
	}

	TimeMeasurement* t = times[ID];
	mutex.unlock();

	t->key = ID;
	t->life = 1.0f; //
	t->measuring = true;
	t->microsecondsStart = TM_GET_MICROS();
	t->microsecondsStop = 0;
	t->error = false;
	t->frame = ofGetFrameNum();
	t->measuring = true;
	t->updatedLastFrame = true;
	return t->settings.enabled;
}


float ofxTimeMeasurements::stopMeasuring(string ID){

	float ret = 0.0f;
	if (!enabled) return ret;

	uint64_t timeNow = TM_GET_MICROS(); //get the time before the lock() to avoid affecting
	//the measurement as much as possible

	Poco::Thread * thread = Poco::Thread::current();

	mutex.lock();
	tree<string> &tr = threadTrees[thread]; //easier to read, tr is our tree from now on
	tree<string>::iterator & tit = threadTreesIterators[thread];
	tit = tr.parent(tit);
	if(tit == NULL) tit = tr.begin();

	//if(thread){
		map<Poco::Thread*, tree<string>	>::iterator ii;
//		for( ii = threadTrees.begin(); ii != threadTrees.end(); ++ii ){
//			cout << "    ";
//			kptree::print_tree_bracketed(ii->second);
//			cout << endl;
//		}
//		cout << "### thread: '" << thread << "' STOP >> '" << ID << "'" << endl;
//		if(threadTreesIterators[thread] != NULL){
//			cout << "    iter is at: " << *threadTreesIterators[thread] << endl;
//		}
	//}
	//int d = tr.depth(threadTreesIterators[thread]);
	//cout << "  depth: " << d << endl ;

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
	loc = p;
}

void ofxTimeMeasurements::_afterDraw(ofEventArgs &d){
	stopMeasuring(TIME_MEASUREMENTS_DRAW_KEY);
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
			draw(loc.x, loc.y);
			break;
	}
}


void ofxTimeMeasurements::updateLongestLabel(){

//	longestLabel = 0;
//	for( map<int,string>::iterator ii = keyOrder.begin(); ii != keyOrder.end(); ++ii ){
//
//		string key = (*ii).second;
//		TimeMeasurement *t = times[key];
//
//		if (t.visible){
//
//			string nesting = "";
//			for(int i = 0; i < t.level; i++){
//				nesting += " ";
//			}
//
//			if ( t.error == false ){
//				bool isLast = (ii->first == keyOrder.size() -1);
//				string label = " " + nesting + " " + key + "  "; //TODO this has to be same lenght as line #285
//				int len = label.length();
//				if (len > longestLabel){
//					longestLabel = len;
//				}
//			}
//		}
//	}
}


void ofxTimeMeasurements::draw(float x, float y){

	vector<string> activeKeys;

	mutex.lock();

	string allTrees = "###############################\n";

	map<Poco::Thread*, tree<string>	>::iterator ii;
	for( ii = threadTrees.begin(); ii != threadTrees.end(); ++ii ){
		//kptree::print_tree_bracketed(ii->second); cout << endl;
		//ii->second.erase_children(ii->second.begin());
		//int size = ii->second.size();
		//threadTreesIterators[ii->first] = ii->second.begin();

		//kptree::print_tree_bracketed(ii->second); cout << endl;
		//activeKeys.push_back(ii->first);
		//cout << "currently at " << *threadTreesIterators[ii->first] << endl;

		tree<string> &tr = ii->second;
		tree<string>::iterator walker = tr.begin();
		string totalTree = "+" + *tr.begin() + "\n";

		if( walker != tr.end()) {
			tree<string>::iterator sib = tr.begin(walker);
			tree<string>::iterator end = tr.end(walker);
			while(sib != end) {
				for(int i = 0; i < tr.depth(sib); ++i)
					totalTree += " ";
				if (sib.number_of_children() == 0){
					totalTree += "-";
				}else{
					totalTree += "+";
				}
				totalTree += *sib + " " + ofToString(times[*sib]->acrossFrames) +
					/* + " " + ofToString(times[*sib]->life,1) */ + "\n";

				++sib;
			}
		}
		//cout << totalTree << endl;
		allTrees += totalTree + "###############################\n";
	}

	mutex.unlock();

	//internalTimeSample = ofGetElapsedTimef();

	if (!enabled) return;
	//if (ofGetFrameNum()%60 == 2){ //todo ghetto!
		updateLongestLabel();
	//}

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

	int tempMaxW = 0;

	ofDrawBitmapString(allTrees, x -100, y + 40);

	return; ///////

	mutex.lock(); /////////////////////////////////////////
	
	for( map<string,TimeMeasurement*>::iterator ii = times.begin(); ii != times.end(); ++ii ){

		string key = (*ii).first;
		TimeMeasurement *t = ii->second;

		if (t->settings.visible){
			c++;

			float ms = t->avgDuration / 1000.0f;
			float percent = 100.0f * ms / timePerFrame;
			//average here, only if enabled
			if (!t->updatedLastFrame && timeAveragePercent < 1.0f){ // if we didnt update that time, make it tend to zero slowly
				t->avgDuration = (1.0f - timeAveragePercent) * t->avgDuration;
			}

			t->updatedLastFrame = false;
			times[key] = t;
			bool isRoot = (key == TIME_MEASUREMENTS_UPDATE_KEY || key == TIME_MEASUREMENTS_DRAW_KEY || key == TIME_MEASUREMENTS_SETUP_KEY);
			string nesting = "";
//			for(int i = 0; i < t->level; i++){
//				nesting += " ";
//			}

			if ( !t->error ){

				string fullLine;
				ofColor lineColor = textColor;
				if ( t->measuring ){
					string anim = "";
					switch ((ofGetFrameNum()/10)%6) {
						case 0: anim = "   "; break;
						case 1: anim = ".  "; break;
						case 2: anim = ".. "; break;
						case 3: anim = "..."; break;
						case 4: anim = " .."; break;
						case 5: anim = "  ."; break;
					}
					string label = " *" + key;
					string padding = "";
					for(int i = label.length(); i < longestLabel; i++){
						padding += " ";
					}
					fullLine = label + " " + padding + anim;

				}else{

//					bool isLast = (ii->first == keyOrder.size() -1);
//					bool isEnabled = times[ii->second].enabled;
//					bool hasChild = false;
//
//					if (t.nextKey.length()){
//						if (times[t.nextKey].level != t.level){
//							hasChild = true;
//						}
//					}

//					string label =	" " +
//					nesting +
//					string(hasChild && !isLast ? "+" : "-") +
//					key +
//					string(isEnabled ? " " : "!");

					string label = key;
					string padding = "";
					for(int i = label.length(); i < longestLabel; i++){
						padding += " ";
					}

					sprintf(msChar, "%*.*f", 4, msPrecision, ms );
					sprintf(percentChar, "% 6.1f",  percent );

					//fullLine = label + padding + " " + msChar + "ms " + percentChar + "%";
					fullLine = label + padding + " " + msChar + "ms " + percentChar + "%";


					if(fullLine.length() > tempMaxW){
						tempMaxW = fullLine.length();
					}

					ofColor lineColor = textColor;
//					ofColor lineColor = textColor * (0.5 + 0.5 * t.life);
//					if (!isEnabled) lineColor = disabledTextColor;
//					if(key == selection && menuActive){
//						if(ofGetFrameNum()%5 < 3){
//							lineColor = selectionColor;
//						}
//					}

				}
				ofSetColor(lineColor);
				ofDrawBitmapString( fullLine, x, y + c * TIME_MEASUREMENTS_LINE_HEIGHT );
			}else{
				ofDrawBitmapString( " " + key + " Usage Error!", x, y + c * TIME_MEASUREMENTS_LINE_HEIGHT );
			}
			if(key == TIME_MEASUREMENTS_DRAW_KEY || key == TIME_MEASUREMENTS_UPDATE_KEY ){
				percentTotal += percent;
			}
		}
		t->life *= 0.8; //TODO magic #
		times[key] = t;
	}

	mutex.unlock(); ////////////////////////////////////////////

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
	ofDrawBitmapString( " '" + ofToString(char(activateKey)) + "'" + string(timeAveragePercent < 1.0 ? " avgd!" : ""), x, y + c * TIME_MEASUREMENTS_LINE_HEIGHT );
	ofPopStyle();
	c += TIME_MEASUREMENTS_LINE_H_MULT * 2;
}


void ofxTimeMeasurements::_keyPressed(ofKeyEventArgs &e){

	if (e.key == enableKey){
		TIME_SAMPLE_SET_ENABLED(!TIME_SAMPLE_GET_ENABLED());
//		if (!TIME_SAMPLE_GET_ENABLED()){
//			for( map<int,string>::iterator ii = keyOrder.begin(); ii != keyOrder.end(); ++ii ){
//				settings[ii->second].visible = times[ii->second].visible;
//				settings[ii->second].enabled = times[ii->second].enabled;
//			}
//			keyOrder.clear();
//			times.clear();
//		}
	}

	if (TIME_SAMPLE_GET_ENABLED()){
		if (e.key == activateKey){
			menuActive = !menuActive;
		}

//		if(menuActive){
//			if (keyOrder.size()){
//				map<int,string>::iterator lastItem = keyOrder.end();
//				lastItem--; //get last item back
//				map<int,string>::iterator beyonLast = keyOrder.end();
//				map<int,string>::iterator firstItem = keyOrder.begin();
//
//				switch (e.key) {
//
//					case OF_KEY_DOWN:{
//						map<int,string>::iterator it = getIndexForOrderedKey(selection);
//						it++;
//						if (it == beyonLast){
//							it = firstItem;
//						}else{
//							while (!times[it->second].visible) {
//								it++;
//								if(it == beyonLast){
//									it = firstItem;
//									break;
//								}
//							}
//						}
//						selection = it->second;
//					}break;
//
//					case OF_KEY_UP:{
//						map<int,string>::iterator it = getIndexForOrderedKey(selection);
//						if (it == firstItem){
//							it = lastItem;
//						}else{
//							it--;
//						}
//						while (!times[it->second].visible) {
//							it--;
//							if(it == firstItem){
//								it = lastItem;
//								break;
//							}
//						}
//						selection = it->second;
//					}break;
//
//					case OF_KEY_RETURN:{
//						map<int,string>::iterator it = getIndexForOrderedKey(selection);
//						if (it != keyOrder.end() ){
//							//cant disable update() & draw()
//							if (it->second != TIME_MEASUREMENTS_SETUP_KEY &&
//								it->second != TIME_MEASUREMENTS_UPDATE_KEY &&
//								it->second != TIME_MEASUREMENTS_DRAW_KEY ){
//								times[it->second].enabled = !times[it->second].enabled;
//							}
//						}
//						}break;
//
//					case OF_KEY_RIGHT:
//						collapseExpand(selection, false /*expand*/);
//						updateNumVisible();
//					break;
//
//					case OF_KEY_LEFT:
//						collapseExpand(selection, true /*collapse*/);
//						updateNumVisible();
//						break;
//				}
//			}
//		}
	}
}


void ofxTimeMeasurements::collapseExpand(string sel, bool collapse){

//	map<int,string>::iterator it = getIndexForOrderedKey(selection);
//	map<int,string>::iterator lastItem = keyOrder.end();
//
//	int baseLevel = times[it->second].level;
//	it++;
//	if(it != lastItem){
//		while (times[it->second].level > baseLevel ) {
//			times[it->second].visible = !collapse;
//			it++;
//			if(it == lastItem){
//				break;
//			}
//		}
//	}
//	updateLongestLabel();
}


map<int, string>::iterator ofxTimeMeasurements::getIndexForOrderedKey(string key){
//	map<int, string>::iterator it = keyOrder.begin();
//	for (map<int, string>::iterator it = keyOrder.begin(); it != keyOrder.end(); ++it){
//		if (it->second == key){
//			return it;
//		}
//	}
//	return it; //not found
}


void ofxTimeMeasurements::loadSettings(){

	//todo this might get called before OF is setup, os ofToDataPath gives us weird results sometimes?
	string f = ofToDataPath(TIME_MEASUREMENTS_SETTINGS_FILENAME, true);
	ifstream myfile(f.c_str());
	string name, visible, enabled;

	if (myfile.is_open()){
		while( !myfile.eof() ){
			getline( myfile, name, '=' );//name
			getline( myfile, visible, '|' ); //visible
			getline( myfile, enabled, '\n' ); //enabled

			if (name == TIME_MEASUREMENTS_SETUP_KEY ||
				name == TIME_MEASUREMENTS_UPDATE_KEY ||
				name == TIME_MEASUREMENTS_DRAW_KEY ){
				visible = enabled = "1";
			}
			if(name.length()){
				settings[name].visible = bool(visible == "1" ? true : false);
				settings[name].enabled = bool(enabled == "1" ? true : false);
				//cout << name << " " << visible << " " << enabled << endl;
			}
		}
		myfile.close();
	}else{
		ofLogWarning() << "Unable to load Settings file " << TIME_MEASUREMENTS_SETTINGS_FILENAME;
	}
}


void ofxTimeMeasurements::saveSettings(){
//	ofstream myfile;
//	myfile.open(ofToDataPath(TIME_MEASUREMENTS_SETTINGS_FILENAME,true).c_str());
//	for( map<int,string>::iterator ii = keyOrder.begin(); ii != keyOrder.end(); ++ii ){
//		bool visible = times[ii->second].visible;
//		bool enabled = times[ii->second].enabled;
//
//		if (ii->second == TIME_MEASUREMENTS_SETUP_KEY ||
//			ii->second == TIME_MEASUREMENTS_UPDATE_KEY ||
//			ii->second == TIME_MEASUREMENTS_DRAW_KEY){
//			visible = enabled = true;
//		}
//
//		myfile << ii->second << "=" << string(visible ? "1" : "0") << "|" <<
//		string(enabled ? "1" : "0") << endl;
//	}
//	myfile.close();
}


void ofxTimeMeasurements::_appExited(ofEventArgs &e){
	saveSettings();
}


void ofxTimeMeasurements::updateNumVisible(){
//	numVisible = 0;
//	for( map<int,string>::iterator ii = keyOrder.begin(); ii != keyOrder.end(); ++ii ){
//		if(times[ii->second].visible) numVisible++;
//	}
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

