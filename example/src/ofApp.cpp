#include "ofApp.h"
#include "Tree.h"

void ofApp::setup(){


	Tree t;
	Tree::Node * root = t.setup("root");

	Tree::Node * a = root->addChildren("a");
	Tree::Node * b = root->addChildren("b");
	a->addChildren("aa");
	a->addChildren("ab");

	auto ba = b->addChildren("ba");
	auto bb = b->addChildren("bb");
	auto bc = b->addChildren("bc");

	auto bac = ba->addChildren("bac");

	std::vector<std::pair<Tree::Node*, int>> allData1;
	root->getAllData(allData1);

	string list1;
	for(auto & l : allData1){
		list1 += l.first->getData() + ", ";
	}
	ofLogNotice() << "allData1:" << list1;

	auto findRes = t.find("bac");

	string fullPath;
	while(findRes->getParent()){
		fullPath = findRes->getData() + "/" + fullPath;
		findRes = findRes->getParent();
	}
	ofLogNotice() << "fullPath: " << fullPath;

	ofBackground(22);

	//specify where the widget is to be drawn
	TIME_SAMPLE_SET_DRAW_LOCATION( TIME_MEASUREMENTS_TOP_RIGHT ); //specify a drawing location (OPTIONAL)


	TIME_SAMPLE_SET_AVERAGE_RATE(0.1);	//averaging samples, (0..1],
										//1.0 gets you no averaging at all
										//use lower values to get steadier readings
	TIME_SAMPLE_DISABLE_AVERAGE();	//disable averaging

	TIME_SAMPLE_SET_REMOVE_EXPIRED_THREADS(true); //inactive threads will be dropped from the table
	//customize color
	//TIME_SAMPLE_GET_INSTANCE()->setHighlightColor(ofColor::yellow);

	//TIME_SAMPLE_GET_INSTANCE()->drawUiWithFontStash("fonts/UbuntuMono-R.ttf");
	//TIME_SAMPLE_GET_INSTANCE()->drawUiWithFontStash2("fonts/UbuntuMono-R.ttf");
	startThread();


}


void ofApp::threadedFunction(){

	while(isThreadRunning()){
		TS_START("task");
			ofSleepMillis(30);
			TS_START("subtask1");
			ofSleepMillis(300);
			TS_STOP("subtask1");
		TS_STOP("task");
		ofSleepMillis(100);
	}
}


void ofApp::update(){

	TS_START("simple measurement");
		ofSleepMillis(1);
	TS_STOP("simple measurement");

	TS_START("nested measurement1");
		TS_START("nested measurement11");
			TS_START("nested measurement111");
			ofSleepMillis(1);
			TS_STOP("nested measurement111");
		TS_STOP("nested measurement11");
		TS_START("nested measurement12");
			ofSleepMillis(1);
		TS_STOP("nested measurement12");
	TS_STOP("nested measurement1");


	if (ofGetFrameNum()%60 == 1){
		TS_START_NIF("sample across frames");
	}

	if (ofGetFrameNum()%60 == 3){
		TS_STOP_NIF("sample across frames");
	}

	if (ofGetFrameNum()%600 == 30 || ofGetFrameNum() == 1){
		TS_START("some uncommon method")
			ofSleepMillis(ofRandom(3));
		TS_STOP("some uncommon method");
	}

	//test accumulation time sampling
	for(int i = 0; i < 3; i++){
		TS_START_ACC("accum test");
		ofSleepMillis(1);
		TS_STOP_ACC("accum test");
		{
			TS_SCOPE_ACC("scope measurement acc");
			ofSleepMillis(1);
		}
	}

	for(int i = myThreads.size() - 1; i >= 0 ; i--){
		if (!myThreads[i]->isThreadRunning()){
			delete myThreads[i];
			myThreads.erase(myThreads.begin() + i);
		}
	}

	{
		TS_SCOPE("scope measurement");
		ofSleepMillis(1);
	}
}


void ofApp::draw(){

	TSGL_START("drawDotsGL");
	TS_START("draw dots") ///////////////////////////////  START MEASURING ///
		for(int i = 0; i < ofGetMouseX() * 5; i++){
			ofSetColor( ofRandom(96) );
			ofRect( ofRandom( ofGetWidth()), ofRandom( ofGetHeight()), 4, 4);
		}
	TS_STOP("draw dots"); ///////////////////////////////  STOP MEASURING  ///
	TSGL_STOP("drawDotsGL");


	//testing late samples
	if (ofGetFrameNum() > 10){
		TS_START("Nested Test L2");
		TS_STOP("Nested Test L2");
	}

	ofSetColor(255);
	ofDrawBitmapString("Move mouse to the right to incrase drawing complexity\n"
					   "Notice how drawing more cubes takes longer\n"
					   "Press 'SPACE' to spawn a new thread\n"
					   "Press 'T' to toggle interactive mode\n\n"
					   "*instructions are show in the widget when active",
					   10,
					   20);
}


void ofApp::keyPressed( ofKeyEventArgs & key ){

	TS_START("keyDown");
	if(key.key == ' '){
		if(myThreads.size() < 10){
			MyThread *t = new MyThread();
			t->startThread();
			myThreads.push_back(t);
		}
	}
	TS_STOP("keyDown");
}

void ofApp::exit(){
	waitForThread(true);
	for(auto t : myThreads){
		t->waitForThread(true);
	}
}
