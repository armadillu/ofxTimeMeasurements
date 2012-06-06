Simple of addon to easily mesure execution times on any parts of your code


	void testApp::setup(){
		TIME_SAMPLE_SET_FRAMERATE( 60.0f ); 
	}	

	void testApp::update(){
		TIME_SAMPLE_START("update");
		//YOUR UPDATE CODE HERE
		TIME_SAMPLE_STOP("update"); 
	}
	
	void testApp::draw(){
		TIME_SAMPLE_START("draw");
		//YOUR DRAW CODE HERE
		TIME_SAMPLE_STOP("draw");
	
		TIME_SAMPLE_DRAW(10, 10);
	}


![ofxTimeMeasurements screenshot](http://farm8.staticflickr.com/7102/6874577102_de187340b7_o.png)

