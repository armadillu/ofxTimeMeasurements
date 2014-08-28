#ofxTimeMeasurements

![ofxTimeMeasurements anim](https://www.dropbox.com/s/y6nsin52sugpmms/ofxTimeMeasurementsAnim.gif?dl=1)

Simple OpenFrameworks addon to easily measure execution times across any sections of your code.

## FEATURES

*	Measure execution time of any parts of your app
*	Enable / disable execution of code sections on the fly through the widget
*	Collapsable GUI, see only what matters at the time (saves on quit)
*	Execution times organized by Thread
*	Measure exact times, or averaged time across frames
*	Measure cumulative times (TS_START_ACC) for shared code sections
*	Highlight frequently used calls, slowly fade unused calls
*	setup(), update(), and draw() are automatically time sampled.
*	Optionally use [ofxMSATimer](https://github.com/obviousjim/ofxMSATimer) for higher precision (recommended windows)

## QUICK START

	void setup(){
		//setup a target framerate (mandatory)
		TIME_SAMPLE_SET_FRAMERATE(60.0f);
	}

	void draw(){
		TS_START("myTimeSample");
			//your code here! will get time sampled!
		TS_STOP("myTimeSample");
	}


##KEYBOARD COMMANDS
*	OF_KEY_PAGE_DOWN to toggle all time measuring, and the drawing of the widget
* 	'T' when widget is visible (enabled) to get into interactive mode. 
* 	When in interactive mode:
	* Up / Down keys to select, Left / Right keys to expand/collapse the sample tree
	* Return key to toggle the execution of the selected item. This allows to disable parts of your code on the fly

## NOTES

#####How does it work? Why am I getting compile errors?

TS_START and TS_STOP create an if clause around the code between them; to allow the enabling/disabling of the code section on the fly. This means that if you define variables inside the timing section, it will not be visible outside of it, as it is only declared withing the scope of the if() clause. To fix this, just declare the variabled outside the TS_START() and TS_STOP() section.

If you want to time sections from different functions, or across several frames, then there's no way to make 
TS_START TS_STOP work; as the if clause will be unbalanced. For those cases, you should use TS_START_NIF and TS_STOP_NIF, which don't include the if clause. Disabling that section from the gui will not work though.

####Sample Averaging
If times vary too much from frame to frame to be readable, you can enable smoothing; if you do so, each new time sample will be blended with the previous one; obtaining a smoother reading. Keep in mind that several samples will be required to get accurate readings when using averaging. You can also completely disable averaging.

	TIME_SAMPLE_SET_AVERAGE_RATE(0.01); //every new sample effects 1% of the value shown 
	TIME_SAMPLE_DISABLE_AVERAGE();  //disable time sample averaging
	

#### Colors
Time samples on the widget list appear in different colors. The first section will always represent times measured in the main thread. Following sections show times measured in different threads. Time Measurements will appear in a bright color if the corresponding code section has just been executed, and they will slowly fade to a darker color of the same hue if that section of code is not accessed. The final fading percentage and the fade speed are customizable.


####Customizations
All key commands and ui colors are customizable, you only need to get a hold of the instance

	TIME_SAMPLE_GET_INSTANCE()->setUIActivationKey('T');
	TIME_SAMPLE_GET_INSTANCE()->setHighlightColor(ofColor::red);
	
	
	

####Measuring setup()
If you want the setup() call to be automatically measured, you need to at least call once ofxTimeMeasurements in your main.cpp, just before the ofRunApp() line. This is so that the ofxTimeMeasurements instance is allocated before setup() is called. Make sure you call it after ofSetupOpenGL() otherwise it might not be able to find its settings file, because ofToDataPath might not be set yet. 

	int main(){
		ofSetupOpenGL(1680,1050, OF_WINDOW);
		TIME_SAMPLE_SET_FRAMERATE(60);
		ofRunApp(new testApp());
	}

####Percentages
The percentages shown besides the ms counts represent how much of a full frame that code section takes. For example; on a 60fps app, if a code section takes 16.6ms, it will fully saturate the desired between-frames time (16.6ms), and it will show 100% 

####LICENSE
ofxTimeMeasurements is made available under the [MIT](http://opensource.org/licenses/MIT) license.

ofxTimeMeasurements makes small use of [tree.hh](http://tree.phi-sci.com) which is licensed under the terms of the GNU General Public License [2](http://www.gnu.org/licenses/old-licenses/gpl-2.0.html) or [3](http://www.gnu.org/licenses/old-licenses/gpl-3.0.html).