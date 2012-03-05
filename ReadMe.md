Simple of addon to easily mesure execution times on any parts of your code

	void testApp::update(){
		chrono.startMeasuring("update");
		//YOUR UPDATE CODE HERE
		chrono.stopMeasuring("update");
	}
	
	void testApp::draw(){
		chrono.startMeasuring("draw");
		//YOUR DRAW CODE HERE
		chrono.stopMeasuring("draw");
		chrono.draw(10, 10);
	}


![ofxTimeMeasurements screenshot](http://farm8.staticflickr.com/7196/6809412212_4443e7b14d_o_d.png)

