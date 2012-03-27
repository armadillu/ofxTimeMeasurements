Simple of addon to easily mesure execution times on any parts of your code

	ofxTimeMeasurements * chrono = ofxTimeMeasurements::instance();
	chrono->setDesiredFrameRate( 60.0f ); 
	
	void testApp::update(){
		chrono->startMeasuring("update");
		//YOUR UPDATE CODE HERE
		chrono->stopMeasuring("update"); 
	}
	
	void testApp::draw(){
		chrono->startMeasuring("draw");
		//YOUR DRAW CODE HERE
		chrono->stopMeasuring("draw");
	
		chrono->draw(10, 10);
	}


![ofxTimeMeasurements screenshot](http://farm8.staticflickr.com/7102/6874577102_de187340b7_o.png)

