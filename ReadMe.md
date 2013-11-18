#ofxTimeMeasurements
Simple of addon to easily mesure execution times on any parts of your code

	void setup(){
	
		//setup a target framerate (MANDATORY)
		TIME_SAMPLE_SET_FRAMERATE(60.f); 
		//specify a drawing location (OPTIONAL)
		TIME_SAMPLE_SET_DRAW_LOCATION( TIME_MEASUREMENTS_TOP_RIGHT ); 
	}	

	void testApp::draw(){

		//time anything within your code
		TIME_SAMPLE_START("myTimeSample");
			//YOUR TIME SAMPLED CODE HERE
	
		TIME_SAMPLE_STOP("myTimeSample"); 
	}


![ofxTimeMeasurements screenshot](http://farm6.staticflickr.com/5493/10931923006_c4faf234c5_o.png)

## NOTES

No need to manually time sample update() and draw() methods; this will happen automatically and it will show up as "update()" and "draw()" on the table now.

Also, no need to manually TIME_SAMPLE_DRAW() within draw(); so you can safely delete calls like TIME_SAMPLE_DRAW_BOTTOM_LEFT() from older projects.

The percentages shown besides the ms counts represent how much of a full frame that mehods takes. For example; on a 60fps app, if a method takes 16.6ms, it will fully saturate the desired between-frames time (16.6ms), and it will show 100%

