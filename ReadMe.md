#ofxTimeMeasurements

![ofxTimeMeasurements anim](https://www.dropbox.com/s/y6nsin52sugpmms/ofxTimeMeasurementsAnim.gif?dl=1)

Simple of addon to easily measure execution times across any sections of your code.
## HOW TO

	void setup(){	
		//setup a target framerate (mandatory)
		TIME_SAMPLE_SET_FRAMERATE(60.0f);  
	}	

	void draw(){		
	
		if( TS_START("myTimeSample") ){
			//your code here! will get time sampled!
		}TS_STOP("myTimeSample");
	}

Note the if() clause wrapping TIME_SAMPLE_START(); it is not required, but it will let you dynamically disable that part of your code through the widget UI if so required.


## FEATURES

*	Measure execution time of any parts of your app
*	Enable/disable execution of code sections on the fly
*	Collapsable GUI, see only what matters at the time (saved on quit)
*	Execution times clustered by Thread
*	Measure exact times, or averaged time across frames
*	Measure cumulative times (TS_START_ACC) for shared code sections
*	Highlight frequently used calls, slowly fade unused calls
*	setup(), update(), and draw() are autmatically time sampled.
*	Optionally use [ofxMSATimer](https://github.com/obviousjim/ofxMSATimer) for higher precision (recommended windows)

##KEY COMMANDS
*	OF_KEY_PAGE_DOWN to toggle all time measurings, and the drawing of the widget
* 	'T' when widget is visible (enabled) to get into interactive mode. 
* 	When in interactive mode:
	* Up / Down keys to select, Left / Right keys to expand/collapse the sample tree
	* Return key to toggle the execution of the selected item. This allows to disable parts of your code on the fly, as long as its wrapped around an if() clause

## NOTES

####Sample Averaging
If times vary too much from frame to frame to be readable, you can enable smoothing; if you do so, each new time sample will be blended with the previous one; obtaining a smoother reading. Keep in mind that several samples will be required to get accurate readings when using averaging. You can also completelly disable averaging.

	TIME_SAMPLE_SET_AVERAGE_RATE(0.01); //every new sample effects 1% of the value shown 
	TIME_SAMPLE_DISABLE_AVERAGE();  //disable time sample averaging
	

#### Colors
Time samples on the widget list appear in bright color (depending on the thread) if the corresponding code section has just been executed, and they slowly fade to a darker color of the same hue if that section of code is not accessed.


####Customizations
All key commands and ui colors are cusomizable, you only need to get a hold of the instance

	TIME_SAMPLE_GET_INSTANCE()->setUIActivationKey('T');
	TIME_SAMPLE_GET_INSTANCE()->setHighlightColor(ofColor::red);
	

####Measuring setup()
If you want the setup() call to be automatically measured, you need to at least call once ofxTimeMeasurements in your main.cpp, just before the ofRunApp() line. This is so that ofxTimeMeasurements is allocated before setup() is called. Make sure you call it after ofSetupOpenGL() otherwise it might not be able to find its settings file bc ofToDataPath might not be set yet. 

	int main(){
		ofAppGLFWWindow win;
		ofSetupOpenGL(&win, 1680,1050, OF_WINDOW);
		TIME_SAMPLE_SET_FRAMERATE(60);
		TIME_SAMPLE_DISABLE_AVERAGE();
		TIME_SAMPLE_SET_DRAW_LOCATION(TIME_MEASUREMENTS_TOP_RIGHT);
	
		ofRunApp(new testApp());
	}

####Percentages
The percentages shown besides the ms counts represent how much of a full frame that code section takes. For example; on a 60fps app, if a code section takes 16.6ms, it will fully saturate the desired between-frames time (16.6ms), and it will show 100% 

