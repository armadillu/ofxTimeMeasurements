#ofxTimeMeasurements
Simple of addon to easily measure execution times on any parts of your code

![ofxTimeMeasurements screenshot](https://farm3.staticflickr.com/2907/14501389525_ec53ea6b42_o.png)

	void setup(){	
	
		//setup a target framerate (mandatory)
		TIME_SAMPLE_SET_FRAMERATE(60.0f);  
	}	

	void draw(){		
	
		if( TIME_SAMPLE_START("myTimeSample") ){ 
			//your code here, will get time sampled
		}TIME_SAMPLE_STOP("myTimeSample"); 
		
		//note the if() clause; it is not required, but it will let
		//you disable that part of your code through the widget UI
	}


## FEATURES

*	Measure execution time of any parts of your app
*	Enable/disable execution of code on the fly
*	Collapsable gui, see only what matters at the time
*	Saves state settings across sessions (collapsed/expanded and enabled/disabled)
*	Measure exact times, or average across frames
*	Highlight frquently used calls, slowly fade unused calls

##KEY COMMANDS
*	OF_KEY_PAGE_DOWN to toggle all time measuring and drawing of the widget
* 	'T' when widget is visible (enabled) to enable interactive mode.
	* use up / down keys to select, left / right to expand / collapse
	* use return key to toggle selected item. this allows to disable parts of code

## NOTES

call TIME_SAMPLE_DISABLE_AVERAGE() to get exact times between start() and stop();
otherwise, smoothing is applied to all samples, so several calls are required to get good measurements.


If you want the setup() call to be automatically measured, you need to at least call once ofxTimeMeasurements in your main.cpp, just before the ofRunApp() line. This is so that ofxTimeMeasurements is allocated before setup() is called. Make sure you call it after ofSetupOpenGL() otherwise it might not be able to find its settings file bc ofToDataPath might not be set yet. 

	int main(){
		ofAppGLFWWindow win;	
		ofSetupOpenGL(&win, 1680,1050, OF_WINDOW);		TIME_SAMPLE_SET_FRAMERATE(60);
		TIME_SAMPLE_DISABLE_AVERAGE();
		TIME_SAMPLE_SET_DRAW_LOCATION(TIME_MEASUREMENTS_TOP_RIGHT);
	
		ofRunApp(new testApp());
	}


The percentages shown besides the ms counts represent how much of a full frame that mehods takes. For example; on a 60fps app, if a method takes 16.6ms, it will fully saturate the desired between-frames time (16.6ms), and it will show 100%

