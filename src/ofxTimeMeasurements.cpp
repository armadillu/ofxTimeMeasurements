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

	uiScale = 1.0;
	desiredFrameRate = 60.0f;
	enabled = true;
	timeAveragePercent = 1;
	msPrecision = 1;
	maxW = 27;

	#if defined(USE_OFX_HISTORYPLOT)
	plotHeight = 60;
	numAllocatdPlots = 0;
	#endif

	bgColor = ofColor(15);
	hilightColor = ofColor(44,77,255) * 1.5;
	disabledTextColor = ofColor(255,0,255);
	measuringColor = ofColor(0,130,0);

	idleTimeColorFadePercent = 0.5;
	idleTimeColorDecay = 0.96;
	deadThreadExtendedLifeDecSpeed = 0.975;

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
	charW = 8;
	charH = TIME_MEASUREMENTS_LINE_HEIGHT;

#if (OF_VERSION_MINOR >= 8)
		//-100 and +100 are to make sure we are always the first AND last at update and draw events, so we can sum everyone's times
		ofAddListener(ofEvents().setup, this, &ofxTimeMeasurements::_beforeSetup, OF_EVENT_ORDER_BEFORE_APP - 100);
		ofAddListener(ofEvents().setup, this, &ofxTimeMeasurements::_afterSetup, OF_EVENT_ORDER_AFTER_APP + 100);
		ofAddListener(ofEvents().update, this, &ofxTimeMeasurements::_beforeUpdate, OF_EVENT_ORDER_BEFORE_APP - 100);
		ofAddListener(ofEvents().update, this, &ofxTimeMeasurements::_afterUpdate, OF_EVENT_ORDER_AFTER_APP + 100);
		ofAddListener(ofEvents().draw, this, &ofxTimeMeasurements::_beforeDraw, OF_EVENT_ORDER_BEFORE_APP - 100);
		ofAddListener(ofEvents().draw, this, &ofxTimeMeasurements::_afterDraw, OF_EVENT_ORDER_AFTER_APP + 100);
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

void ofxTimeMeasurements::setConfigsDir(string d){
	configsDir = d;
}


void ofxTimeMeasurements::setDeadThreadTimeDecay(float decay){
	deadThreadExtendedLifeDecSpeed = ofClamp(decay, idleTimeColorDecay, 1.0);
}

float ofxTimeMeasurements::getLastDurationFor(string ID){

	float r = 0.0f;
	unordered_map<string,TimeMeasurement*>::iterator it;
	it = times.find(ID);
	if ( it != times.end() ){	//not found!
		r = times[ID]->duration / 1000.0f; //to ms
	}
	return r;
}


float ofxTimeMeasurements::getAvgDurationFor(string ID){

	float r = 0.0f;
	unordered_map<string,TimeMeasurement*>::iterator it;
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


bool ofxTimeMeasurements::startMeasuring(string ID, bool accumulate, ofColor color){

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

	unordered_map<Poco::Thread*, ThreadInfo>::iterator threadIt = threadInfo.find(thread);
	ThreadInfo & tinfo = threadInfo[thread];
	core::tree<string> &tr = tinfo.tree; //easier to read, tr is our tree from now on

	if (threadIt == threadInfo.end()){ //new thread!

		//string tName = isMainThread ? "mainThread" : string("Thread " + ofToString(threadCounter));
		string tName = isMainThread ? "Main Thread" : string(Poco::Thread::current()->getName() +
															 " (" + ofToString(numThreads) + ")");
		//init the iterator
		*tr = tName; //thread name is root
		tinfo.tit = (core::tree<string>::iterator)tr;
		tinfo.order = numThreads;

		if (thread){
			if(color.a == 0 && color.r == 0 && color.g == 0 && color.b == 0){
				tinfo.color = threadColorTable[numThreads%(threadColorTable.size())];
			}else{
				tinfo.color = color;
			}
			numThreads++;
		}else{
			tinfo.color = hilightColor;
		}
	}

	//see if the new measurement already was in tree
	core::tree<string>::iterator searchIt = tr.tree_find_depth(ID);

	if(searchIt == tr.end()){ //if it wasnt in the tree, append it

		if (ID == TIME_MEASUREMENTS_SETUP_KEY ||
			ID == TIME_MEASUREMENTS_UPDATE_KEY ||
			ID == TIME_MEASUREMENTS_DRAW_KEY
			){ //setup update and draw are always at root level!
			if (tr.size()){
				tinfo.tit = tr.push_back(ID);
			}else{
				tinfo.tit = tr.insert(ID);
			}

		}else{
			if(tinfo.tit == tr.end()){
				ofLogError() << "ofxTimeMeasurements error - unordered tree cant climb back up!"; //TODO handle this better!
				tinfo.tit = tr.push_back(ID);
			}else{
				tinfo.tit = tinfo.tit.push_back(ID);
			}
		}
	}else{
		tinfo.tit = searchIt;
	}

	//see if we had an actual measurement, or its a new one
	unordered_map<string, TimeMeasurement*>::iterator tit = times.find(ID);

	if (tit == times.end()){ //not found, let alloc a new TimeMeasurement
		times[ID] = new TimeMeasurement();

		//keyOrder[ keyOrder.size() ] = ID;
		unordered_map<string, TimeMeasurementSettings>::iterator it2 = settings.find(ID);
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

	mutex.lock();

	core::tree<string> &tr = threadInfo[thread].tree; //easier to read, tr is our tree from now on
	core::tree<string>::iterator & tit = threadInfo[thread].tit;
	if (tit != tr.begin()){
		if( tit != tr.end()){
			tit = tit.out();
		}
	}

	unordered_map<string,TimeMeasurement*>::iterator it;
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
			draw( ofGetWidth() / uiScale - getWidth() - TIME_MEASUREMENTS_EDGE_GAP_H,TIME_MEASUREMENTS_EDGE_GAP_V);
			break;
		case TIME_MEASUREMENTS_BOTTOM_LEFT:
			draw(TIME_MEASUREMENTS_EDGE_GAP_H, ofGetHeight() / uiScale - getHeight() - TIME_MEASUREMENTS_EDGE_GAP_V);
			break;
		case TIME_MEASUREMENTS_BOTTOM_RIGHT:
			draw( ofGetWidth() / uiScale - getWidth() - TIME_MEASUREMENTS_EDGE_GAP_H,
				 ofGetHeight() / uiScale - getHeight() - TIME_MEASUREMENTS_EDGE_GAP_V);
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


void ofxTimeMeasurements::draw(float x, float y) {

	if (!enabled) return;
	
	drawLines.clear();
	float percentTotal = 0.0f;
	float timePerFrame = 1000.0f / desiredFrameRate;

	mutex.lock();

	vector<TimeMeasurement*> toResetUpdatedLastFrameFlag;

	//update time stuff, build draw lists
	for( unordered_map<string,TimeMeasurement*>::iterator ii = times.begin(); ii != times.end(); ++ii ){
		TimeMeasurement* t = ii->second;
		string key = ii->first;
		if(!t->measuring){
			if (t->life > 0.01){
				t->life *= idleTimeColorDecay; //decrease life
			}else{ //life decays very slow when very low
				t->life *= deadThreadExtendedLifeDecSpeed; //decrease life very slowly
			}
		}
		if (!t->updatedLastFrame && timeAveragePercent < 1.0f){ // if we didnt update that time, make it tend to zero slowly
			t->avgDuration = (1.0f - timeAveragePercent) * t->avgDuration;
		}
		toResetUpdatedLastFrameFlag.push_back(t);
	}

	unordered_map<Poco::Thread*, ThreadInfo>::iterator ii;
	vector<Poco::Thread*> expiredThreads;

	//lets make sure the Main Thread is always on top
	vector< pair<Poco::Thread*, ThreadInfo> > sortedThreadList;

	for( ii = threadInfo.begin(); ii != threadInfo.end(); ++ii ){ //walk all thread trees
		if (ii->first == NULL){ //main thread is NULL!
			sortedThreadList.insert(sortedThreadList.begin(), *ii);
		}else{
			sortedThreadList.push_back(*ii);
		}
	}
	std::sort(sortedThreadList.begin(), sortedThreadList.end(), compareThreadPairs);

	#if defined(USE_OFX_HISTORYPLOT)
	vector<ofxHistoryPlot*> plotsToDraw;
	#endif

	for( int k = 0; k < sortedThreadList.size(); k++ ){ //walk all thread trees

		Poco::Thread* thread = sortedThreadList[k].first;
		core::tree<string> &tr = sortedThreadList[k].second.tree;

		PrintedLine header;
		header.formattedKey = "+" + *tr;
		header.color = threadInfo[thread].color;
		header.key = *tr; //key for selection, is thread name
		drawLines.push_back(header); //add header to drawLines

		int numAlive = 0;
		int numAdded = 0;

		core::tree<string>::iterator wholeTreeWalker = tr.in();
		bool finishedWalking = false;

		while( !finishedWalking ){

			string key = *wholeTreeWalker;
			TimeMeasurement * t = times[*wholeTreeWalker];

			#if defined(USE_OFX_HISTORYPLOT)
			bool plotActive = false;
			if(plots[key]){
				if(t->settings.plotting){
					if(t->updatedLastFrame){
						if (t->accumulating){
							plots[key]->update(t->microsecondsAccum / 1000.0f);
						}else{
							plots[key]->update(t->avgDuration / 1000.0f);
						}
					}
					plotsToDraw.push_back(plots[key]);
					plotActive = true;
				}
			}
			#endif

			bool visible = t->settings.visible;
			bool alive = t->life > 0.0001;
			if(alive){
				numAlive++;
			}

			if (visible){
				PrintedLine l;
				l.key = key;
				l.tm = t;

				int depth = wholeTreeWalker.level();
				for(int i = 0; i < depth; ++i) l.formattedKey += " ";

				if (wholeTreeWalker.size() == 0){
					l.formattedKey += "-";
				}else{
					l.formattedKey += "+";
				}
				l.formattedKey += key;
				#if defined(USE_OFX_HISTORYPLOT)
				if(plotActive) l.formattedKey += " [p]";
				#endif

				l.time = getTimeStringForTM(t);

				l.color = threadInfo[thread].color * ((1.0 - idleTimeColorFadePercent) + idleTimeColorFadePercent * t->life);
				if (!t->settings.enabled){
					l.color = disabledTextColor;
				}
				if (t->key == selection && menuActive){
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

			//control the iterator to walk the tree "recursivelly" without doing so.
			if(wholeTreeWalker.size()){
				wholeTreeWalker = wholeTreeWalker.in();
			}else{
				if ( wholeTreeWalker.next() == wholeTreeWalker.end() ){
					wholeTreeWalker = wholeTreeWalker.out();
					while( wholeTreeWalker.next() == wholeTreeWalker.end() && wholeTreeWalker != tr){
						wholeTreeWalker = wholeTreeWalker.out();
					}
					if(wholeTreeWalker == tr){
						finishedWalking = true;
					}else{
						wholeTreeWalker++;
					}
				}else{
					++wholeTreeWalker;
				}
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
		unordered_map<Poco::Thread*, ThreadInfo>::iterator treeIt = threadInfo.find(expiredThreads[i]);
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
	ofPushMatrix();
	ofScale(uiScale,uiScale);
	ofSetDrawBitmapMode(OF_BITMAPMODE_SIMPLE);

	ofFill();
	ofEnableAlphaBlending();

	#if defined(USE_OFX_HISTORYPLOT)
	//int numCols = plotsToDraw.size()
	for(int i = 0; i < plotsToDraw.size(); i++){
		int y = ofGetHeight() / uiScale - plotHeight * (i + 1);
		plotsToDraw[i]->draw(0, y, ofGetWidth() / uiScale, plotHeight);
		ofSetColor(99);
		if(i != plotsToDraw.size() -1){
			ofLine(0, y, ofGetWidth() / uiScale, y );
		}
	}
	#endif

	float totalW = getWidth();
	float totalH = getHeight();

	ofSetColor(bgColor, 245);
	int barH = 1;
	ofRect(x, y + 1, totalW, totalH);

	//thread header bg highlight
	for(int i = 0; i < headerLocations.size(); i++){
		int loc = headerLocations[i];
		//whole section
		ofSetColor(drawLines[loc].color, 40);
		int h = charH * ((i < headerLocations.size() - 1) ? headerLocations[i+1] - headerLocations[i] : drawLines.size() - loc );
		ofRect(x, y + 2 + loc * charH, totalW, h);
		//thread header
		ofSetColor(drawLines[loc].color, 40);
		ofRect(x, y + 2 + loc * charH, totalW, charH + 1);
	}

	ofSetColor(hilightColor);
	ofRect(x, y + 1, totalW, barH);
	ofRect(x, y + totalH - charH - 4 , totalW, barH);
	ofRect(x, y + totalH, totalW - barH, barH);

	for(int i = 0; i < drawLines.size(); i++){
		ofSetColor(drawLines[i].color);
		drawString(drawLines[i].fullLine, x , y + (i + 1) * charH);
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
	int lastLine = ( drawLines.size() + 1 ) * charH + 2;
	drawString( pad + msg, x, y + lastLine );
	ofSetColor(hilightColor);
	drawString( " '" + ofToString(char(activateKey)) + "'" + string(timeAveragePercent < 1.0 ? " avgd!" : ""),
					   x, y + lastLine );

	for(int i = 0; i < toResetUpdatedLastFrameFlag.size(); i++){
		toResetUpdatedLastFrameFlag[i]->updatedLastFrame = false;
	}
	ofPopMatrix();
	ofPopStyle();
}

#if defined(USE_OFX_HISTORYPLOT)
ofxHistoryPlot* ofxTimeMeasurements::makeNewPlot(string name){

	ofxHistoryPlot * plot = new ofxHistoryPlot( NULL, name, 1024, false);
	int colorID = numAllocatdPlots%(threadColorTable.size());
	plot->setColor( threadColorTable[colorID] );
	plot->setBackgroundColor(ofColor(0,220));
	plot->setShowNumericalInfo(true);
	plot->setRespectBorders(true);
	plot->setLineWidth(1);
	plot->setLowerRange(0);
	plot->addHorizontalGuide(1000.0f/desiredFrameRate, ofColor(0,255,0));
	plot->setDrawGrid(true);
	plot->setGridUnit(16);
	plot->setGridColor(ofColor(20,255));
	plot->setAutoRangeShrinksBack(true);
	plot->setShowSmoothedCurve(true);
	plot->setSmoothFilter(0.1);
	numAllocatdPlots++;
	return plot;
}
#endif

void ofxTimeMeasurements::_keyPressed(ofKeyEventArgs &e){

	if (e.key == enableKey){
		TIME_SAMPLE_SET_ENABLED(!TIME_SAMPLE_GET_ENABLED());
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

					#if defined(USE_OFX_HISTORYPLOT)
					case 'P':{
						if (!plots[selection]){
							plots[selection] = makeNewPlot(selection);
							times[selection]->settings.plotting = true;
						}else{
							times[selection]->settings.plotting ^= true;
						}
					}break;
					#endif

					case OF_KEY_RETURN:{
							//cant disable update() & draw()
							if (selection != TIME_MEASUREMENTS_SETUP_KEY &&
								selection != TIME_MEASUREMENTS_UPDATE_KEY &&
								selection != TIME_MEASUREMENTS_DRAW_KEY &&
								drawLines[selIndex].tm
								){
									times[selection]->settings.enabled ^= true;
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

	unordered_map<Poco::Thread*, ThreadInfo>::iterator ii;

	for( ii = threadInfo.begin(); ii != threadInfo.end(); ++ii ){

		core::tree<string> &tr = ii->second.tree;
		core::tree<string>::iterator loc = tr.tree_find_depth(sel);

		if( loc != tr.end()) {
			vector<string> subTree;
			walkTree(loc, 0, subTree);
			for(int p = 0; p < subTree.size(); p++ ){
				times[subTree[p]]->settings.visible = !collapse;
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
			if(time > 3600){ //if more than a minute
				time /= 3600.0f;
				timeUnit = "hrs";
			}else{
				time /= 60.0f;
				timeUnit = "min";
			}
		}
	}
	return ofToString(time,  precision) + timeUnit;
}


string ofxTimeMeasurements::getTimeStringForTM(TimeMeasurement* tm) {

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
				formatTime( TM_GET_MICROS() - tm->microsecondsStart, 1) +
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
			int originalLen = timeString.length();

			int expectedLen = 8;
			for(int i = 0; i < expectedLen - originalLen; i++){
				timeString = " " + timeString;
			}

			if (over){
				sprintf(percentChar, int(ofGetFrameNum() * 0.8)%5 < 3  ? " >100": "  100");
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
	string name, visible, enabled_, plotting;
	bool fileHasPlotData = false;
	if (myfile.is_open()){

		int c = 0;
		while( !myfile.eof() ){

			if (c == 0){ //see if file has PlotData, 2 '|' per line if it does, only 1 if it doesnt
				string wholeLine;
				getline( myfile, wholeLine, '\n' );//see what version we are on
				int numBars = 0;
				for(int i = 0; i < wholeLine.size(); i++){
					if (wholeLine[i] == '|') numBars++;
				}
				if(numBars == 2) fileHasPlotData = true;
				myfile.clear();
				myfile.seekg(0, ios::beg);
				c++;
			}

			getline( myfile, name, '=' );//name
			getline( myfile, visible, '|' ); //visible
			if(fileHasPlotData){
				getline( myfile, enabled_, '|' ); //enabled
				getline( myfile, plotting, '\n' ); //enabled
			}else{
				getline( myfile, enabled_, '\n' ); //enabled
			}

			if (name == TIME_MEASUREMENTS_SETUP_KEY ||
				name == TIME_MEASUREMENTS_UPDATE_KEY ||
				name == TIME_MEASUREMENTS_DRAW_KEY ){
				visible = enabled_ = "1";
			}
			if(name.length()){
				settings[name].visible = bool(visible == "1" ? true : false);
				settings[name].enabled = bool(enabled_ == "1" ? true : false);
				#if defined(USE_OFX_HISTORYPLOT)
				settings[name].plotting = bool(plotting == "1" ? true : false);
				if(settings[name].plotting){
					ofxHistoryPlot * plot = makeNewPlot(name);
					plots[name] = plot;
				}
				#endif
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
	for( unordered_map<string, TimeMeasurement*>::iterator ii = times.begin(); ii != times.end(); ++ii ){
		bool visible = times[ii->first]->settings.visible;
		bool enabled = times[ii->first]->settings.enabled;
		#if defined(USE_OFX_HISTORYPLOT)
		bool plotting = times[ii->first]->settings.plotting;
		#endif

		if (ii->first == TIME_MEASUREMENTS_SETUP_KEY ||
			ii->first == TIME_MEASUREMENTS_UPDATE_KEY ||
			ii->first == TIME_MEASUREMENTS_DRAW_KEY){
			visible = enabled = true;
		}

		myfile << ii->first << "=" << string(visible ? "1" : "0") << "|" << string(enabled ? "1" : "0")
		#if defined(USE_OFX_HISTORYPLOT)
		<< "|" << string(plotting ? "1" : "0")
		#endif
		<< endl;
	}
	myfile.close();
}


void ofxTimeMeasurements::walkTree(core::tree<string>::iterator Arg, int levelArg, vector<string> &result){
	levelArg++;
	for(core::tree<string>::iterator x = Arg.begin(); x != Arg.end(); ++x){
		result.push_back(x.data());
		walkTree(x, levelArg, result);
	}
}


void ofxTimeMeasurements::_appExited(ofEventArgs &e){
	saveSettings();
}

#ifdef USE_OFX_FONTSTASH
void ofxTimeMeasurements::drawUiWithFontStash(string fontPath, float fontSize_){
	useFontStash = true;
	fontSize = fontSize_;
	font.setup(ofToDataPath(fontPath, true), 1.0, 512, false, 0, uiScale);
	ofRectangle r = font.getBBox("M", fontSize, 0, 0);
	charW = r.width;
	charH = ceil(r.height * 1.55);
}

void ofxTimeMeasurements::drawUiWithBitmapFont(){
	useFontStash = false;
	charW = 8;
	charH = TIME_MEASUREMENTS_LINE_HEIGHT;
}
#endif

void ofxTimeMeasurements::drawString(const string & text, const float & x, const float & y){
	#ifdef USE_OFX_FONTSTASH
	if(useFontStash){
		font.draw(text, fontSize, x + 2, y);
	}else{
		ofDrawBitmapString(text, x, y);
	}
	#else
	ofDrawBitmapString(text, x, y);
	#endif
}


float ofxTimeMeasurements::durationForID( string ID){

	unordered_map<string,TimeMeasurement*>::iterator it;
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


float ofxTimeMeasurements::getWidth() const{
	#ifdef USE_OFX_FONTSTASH
	return (maxW + 1) * charW + float(useFontStash ? 4.0f: 0.0f);
	#else
	return (maxW + 1) * charW;
	#endif
}

