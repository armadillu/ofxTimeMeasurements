#ofxTimeMeasurements


Simple OpenFrameworks addon to easily measure execution times across any sections of your code.

![ofxTimeMeasurements anim](https://www.dropbox.com/s/y6nsin52sugpmms/ofxTimeMeasurementsAnim.gif?dl=1)

## FEATURES

*	Measure execution time of any parts of your app
*	Enable / disable execution of code sections on the fly through the widget
*	Collapsable GUI, see only what matters at the time (saves on quit)
*	Execution times organized by Thread
*	Measure exact times, or averaged time across frames
*	Measure cumulative times (TS_START_ACC) for shared code sections
*	Highlight frequently used calls, slowly fade unused calls
*	update(), and draw() are automatically time sampled (measuring setup() needs some work in main.cpp).
*	Optionally use [ofxMSATimer](https://github.com/obviousjim/ofxMSATimer) for higher precision (recommended on windows)
*	Optionally use [ofxHistoryPlot](https://github.com/armadillu/ofxHistoryPlot) to show timings over time
*	Optionally use [ofxFontStash](https://github.com/armadillu/ofxFontStash) for rendering, which looks nicer and is much faster than the default ofDrawBitmapFont. (see drawUiWithFontStash())

## QUICK START

That's all you need to get basic measurements of update() and draw();

```
#include "ofxTimeMeasurements.h"

void setup(){	
	TIME_SAMPLE_SET_FRAMERATE(60.0f); //specify a target framerate
}
```

## HOW TO MEASURE TIMES

### Standard Mode

You can measure times across any code section with TS_START() - TS_STOP()

```
	TS_START("measurement1");
	//my code here
	TS_STOP("measurement1")	
```
The ofxTimeMeasurements widget will list a line named "measurement1" showing how long it took for the section to execute.

### Accumulation Mode

You can accumulate the time spent across one frame over sections of your code. This can be handy when a methods is called several times over one frame.

```
	for(int i = 0; i < 10; i++){
		TS_START_ACC("acc measurement");
		//do something here
		TS_STOP_ACC("acc measurement")
	}
```
If we were to use TS_START/STOP on the example above, ofxTimeMeasurements would only report the time spent on the last iteration of the for loop. By using TS_START_ACC / TS_STOP_ACC, it accumulates the time spent on each iteration of the loop, reporting the total time spent.


### "No If" Mode (_NIF)

ofxTimeMeasurements wraps your code around an if(){} clause. It does so to be able to disable code on the fly from its GUI. This can be problematic if you declare variables inside a measurement, as they will not be in scope outside the measurement. If that's the case, you use the _NIF extension in your macros, this way your code will not be wrapped around an if(){} clause. The only drawback is that you will not be able to enable/disable that code section from the GUI.

```
	TS_START_NIF("nif");
		int a = 0;
	TS_STOP_NIF("nif");
	
	a = 1; //we can now access the variable declared in the TM scope.
```

### TS() Mode

TS() is a very convenient ultra-short macro to measure the time a single method takes to execute. This will show up as "myMethod()" on your measurements widget.

```
	TS(myMethod());
```

##KEYBOARD COMMANDS
*	OF_KEY_PAGE_DOWN to toggle all time measuring, and the drawing of the widget
* 	'T' when widget is visible (enabled) to get into interactive mode.
* 	When in interactive mode, a "KEYBOARD COMMANDS" list is shown
 	![](https://farm1.staticflickr.com/473/20007668446_d428e77cdf_o_d.png)

 Up / Down keys to select, Left / Right keys to expand/collapse the sample tree.


## NOTES

####SAMPLE AVERAGING

If times vary too much from frame to frame to be readable on the widget, you can enable sample averaging; if you do so, each new time sample will be blended with the previous one; obtaining a smoother reading. Keep in mind that several samples will be required to get accurate readings when using averaging. You can also completely disable averaging.

	TIME_SAMPLE_SET_AVERAGE_RATE(0.01); //every new sample effects 1% of the value shown 
	TIME_SAMPLE_DISABLE_AVERAGE();  //disable time sample averaging
	

#### COLORS

The ofxTimeMeasurements widget can show measurements in different colored sections. The first section will always represent times measured in the main thread. Following sections show times measured other threads. 

![](https://farm1.staticflickr.com/298/20034358155_b4a49742b7_o_d.png)

Active Time Measurements will appear in a bright color if the corresponding code section has just been executed, and they will slowly fade to a darker color if that section of code is not accessed / measured. The final fading percentage and the fade speed are customizable.


####CUSTOMIZATIONS
Most key commands and ui colors are customizable, you only need to get a hold of the instance

	TIME_SAMPLE_GET_INSTANCE()->setUIActivationKey('T');
	TIME_SAMPLE_GET_INSTANCE()->setHighlightColor(ofColor::red);
	

####MEASURING setup()
If you want the setup() call to be automatically measured, you need to at least call once ofxTimeMeasurements in your main.cpp, just before the ofRunApp() line. This is so that the ofxTimeMeasurements instance is allocated before setup() is called. Make sure you call it after ofSetupOpenGL() otherwise it might not be able to find its settings file, because ofToDataPath might not be set yet. 

	int main(){
		ofSetupOpenGL(1680,1050, OF_WINDOW);
		TIME_SAMPLE_SET_FRAMERATE(60);
		ofRunApp(new testApp());
	}

in OF v09, you need to call ```TIME_SAMPLE_ADD_SETUP_HOOKS()``` in your main()

####PERCENTAGES
The percentages shown besides the ms counts represent how much of a full frame that code section takes. For example; on a 60fps app, if a code section takes 16.6ms, it will fully saturate the desired between-frames time (16.6ms), and it will show 100% 

####TIMING RESOLUTION
You can increase the resolution/accuracy of the time measurements by using [ofxMSATimer](https://github.com/obviousjim/ofxMSATimer). This is recommended on Windows, as the default OF timer resolution is not very high on that platform. To use ofxMSATimer you need to add the ofxMSATimer to the project, and define USE_MSA_TIMER in your project Pre-Processor macros.

####USING ofxHistoryPlot

If you add [ofxHistoryPlot](https://github.com/armadillu/ofxHistoryPlot) to your project, you will also be able to track your timings over time. To do so, add ofxHistoryPlot to your project, and define USE_OFX_HISTORYPLOT in your project's Pre-Processor Macros.

If you do so, when in interactive mode, you can press 'P' to toggle the plotting of the timings of the selected section over time.

![img](https://farm1.staticflickr.com/510/19847290279_4b9761ff4d_o_d.png)

Measurements that are being plotted will show a colored label on the left side, matching the color of the plot.

####USING ofxFontStash

If you include [ofxFontStash](https://github.com/armadillu/ofxFontStash) in your project, you can use it to draw the widget with any font of your liking. Define USE_OFX_FONTSTASH in your project's PreProcessor Macros, and call
```TIME_SAMPLE_GET_INSTANCE()->drawUiWithFontStash()``` 
supplying a monospaced ttf font and a font size. 

![img](https://farm1.staticflickr.com/533/20034108485_fdaa20bd72_o_d.png)

You can also set the font scale by calling setUiScale(), but make sure you set the UI Scale before you supply the font. This can be useful on retina/4k screens to make the widget more legible. (See screenshot above)

##LICENSE
ofxTimeMeasurements is made available under the [MIT](http://opensource.org/licenses/MIT) license.

ofxTimeMeasurements makes small use of [tree.h](http://archive.gamedev.net/archive/reference/programming/features/coretree2/tree.h) by Justin Gottschlich which is licensed under the terms of the GNU Lesser General Public License 2.1 ([LGPL v2.1](http://choosealicense.com/licenses/lgpl-2.1/)).