#ofxTimeMeasurements


Simple OpenFrameworks addon to easily measure execution times across any sections of your code.

![ofxTimeMeasurements anim](https://www.dropbox.com/s/y6nsin52sugpmms/ofxTimeMeasurementsAnim.gif?dl=1)

## 1. FEATURES

*	Measure execution time of any parts of your app
*	Enable / disable execution of code sections on the fly through the widget
*	Collapsable GUI, see only what matters at the time (saves on quit)
*	Execution times organized by Thread
*	Measure exact times, or averaged time across frames
*	Measure cumulative times (TS_START_ACC) for shared code sections
*	Highlight frequently used calls, slowly fade unused calls
*	update(), and draw() are automatically time sampled
*	Measure GL driver execution times; actually querying openGL with glBeginQuery(GL_TIME_ELAPSED_EXT) & co.  
*	Optionally use [ofxMSATimer](https://github.com/obviousjim/ofxMSATimer) for higher precision (recommended on windows) for < OF_09
*	Optionally use [ofxHistoryPlot](https://github.com/armadillu/ofxHistoryPlot) to show timings over time
*	Optionally use [ofxFontStash](https://github.com/armadillu/ofxFontStash) for rendering, which looks nicer and is much faster to draw than the default ofDrawBitmapFont. (see drawUiWithFontStash())

## 2. QUICK START

That's all you need to get basic measurements of update() and draw();

```
#include "ofxTimeMeasurements.h"

void setup(){	
	TIME_SAMPLE_SET_FRAMERATE(60.0f); //specify a target framerate
}
```

## 3. HOW TO MEASURE TIMES / PROFILE YOUR CODE

### 3.1 Standard Mode

You can measure times across any code section with TS_START() - TS_STOP()

```
	TS_START("measurement1");
	//my code here
	TS_STOP("measurement1")	
```
The ofxTimeMeasurements widget will list a line named "measurement1" showing how long it took for the section to execute.

### 3.2 Accumulation Mode

You can accumulate the time spent across one frame over sections of your code. This can be handy when a methods is called several times over one frame.

```
	for(int i = 0; i < 10; i++){
		TS_START_ACC("acc measurement");
		//do something here
		TS_STOP_ACC("acc measurement")
	}
```
If we were to use TS_START/STOP on the example above, ofxTimeMeasurements would only report the time spent on the last iteration of the for loop. By using TS_START_ACC / TS_STOP_ACC, it accumulates the time spent on each iteration of the loop, reporting the total time spent.


### 3.3 "No If" Mode (_NIF)

ofxTimeMeasurements wraps your code around an if(){} clause. It does so to be able to disable code on the fly from its GUI. This can be problematic if you declare variables inside a measurement, as they will not be in scope outside the measurement. If that's the case, you use the _NIF extension in your macros, this way your code will not be wrapped around an if(){} clause. The only drawback is that you will not be able to enable/disable that code section from the GUI.

```
	TS_START_NIF("nif");
		int a = 0;
	TS_STOP_NIF("nif");
	
	a = 1; //we can now access the variable declared in the TM scope.
```

You can also use both NIF and ACC, see TS_START_ACC_NIF() and family.

### 3.4 Scope Mode

This mode measures execution times in a particular scope. It creates a temporary minimal object that will start measuring when its constructor is called, and it will stop measuring when its destructor is called.

```
	{
		TS_SCOPE("myTime");
		doSomething();
		doSomethingElse();
	}
```
The example above will report the time it takes for doSomething() and doSomethingElse() to execute. The advantage here is that only one ofxTimeMeasurements call is needed to measure time.

### 3.5 TS() Mode

TS() is a very convenient ultra-short macro to measure the time a single method takes to execute. This will show up as "myMethod()" on your measurements widget.

```
	TS(myMethod());
```

### 3.6 OpenGL Execution Times

This will NOT measure the time it takes the CPU to execute the code between the start() and stop() methods, but the time it takes the GPU to execute all the OpenGL instructions issued between start() and stop().  

It does so by sending glBeginQuery(GL_TIME_ELAPSED_EXT) and glEndQuery(GL_TIME_ELAPSED_EXT) OpenGL commands, and retrieving the time it actually took to render by querying OpenGL after the rendering has happened. This method was chosen vs the GL_TIMESTAMP based, bc it seems to work with OpenGL 2.0, vs the GL_TIMESTAMP one requiring OpenGL 3.3.

![OpenGL Timings](https://farm1.staticflickr.com/576/32429675235_3844e560bf_o_d.png)

OpenGL timings will show in their own separate section named "OpenGL", similar to timings in different threads. They can be disabled the same way as CPU timings, and also averaged and plotted.

```
TSGL_START("FancyShader");
myFancyShader.run();
TSGL_STOP("FancyShader");
```
	

*Some Caveats:
OpenGL timing measurements can't be nested, you can't start a measurement unless you stopped the previous one. It's still ok to measure several things each frame, but not nested.
*It's ok to nest/mix these within standard "CPU" measurements.
*There's also "no if" version of this measurement, see 3.3 to learn more. TSGL_START_NIF() and TSGL_STOP_NIF().
*Because the timings can only be known after the frame has been fully drawn, the timings appear 2-3 frames after measured.


## 4. KEYBOARD COMMANDS

ofxTimeMeasurements responds to a few pre-defined keyboard commands:

*	OF_KEY_PAGE_DOWN to toggle all time measuring, and the drawing of the widget
* 	'T' when widget is visible (enabled) to get into interactive mode.
* 	When in interactive mode, a "KEYBOARD COMMANDS" list is shown : basically on-screen instructions.
 	![](https://farm1.staticflickr.com/473/20007668446_d428e77cdf_o_d.png)

 Up / Down keys to select, Left / Right keys to expand/collapse the time measurements tree.


## 5. RANDOM NOTES

####5.1 OVERHEAD

ofxTimeMeasurements adds some overhead when measuring times; and it adds even more when drawing its widget. You can see how much it adds by pressing the "B" key, it will display two extra timings, the time it takes for the measurements to be taken, and the time it takes for the widget to be drawn. When ofxTimeMeasurements is disabled (by pressing PgDown or by calling TIME_SAMPLE_DISABLE() at setup), it barely adds any overhead (it doesn't measure anything when the widget is not drawn) but it does add a tiny bit as the calls to its methods are still going through. To entirely remove all ofxTimeMeasurements calls without actually removing them from your code, you can define "TIME_MEASUREMENTS_DISABLED" in your project pro-processor macros for it to be entirely bypassed at compile time. This will only work if you use the recommended MACRO based API; if you have code that gets the ofxTimeMeasurements instance ```ofxTimeMeasurements::instance()``` and operates with it, your project will not compile.

#### 5.2 SAMPLE AVERAGING

If times vary too much from frame to frame to be readable on the widget, you can enable sample averaging; if you do so, each new time sample will be blended with the previous one; obtaining a smoother reading. Keep in mind that several samples will be required to get accurate readings when using averaging. You can also completely disable averaging.

	TIME_SAMPLE_SET_AVERAGE_RATE(0.01); //every new sample effects 1% of the value shown 
	TIME_SAMPLE_DISABLE_AVERAGE();  //disable time sample averaging
	

#### 5.3 COLORS / THREADS

The ofxTimeMeasurements widget can show measurements in different colored sections. The first section will always represent times measured in the main thread. Following sections show times measured other threads. ofxTimeMeasurements keeps a collapsable tree per each thread.

![](https://farm1.staticflickr.com/298/20034358155_b4a49742b7_o_d.png)

Active Time Measurements will appear in a bright color if the corresponding code section has just been executed, and they will slowly fade to a darker color if that section of code is not accessed / measured. The final fading percentage and the fade speed are customizable.


#### 5.4 CUSTOMIZATIONS
Most key commands and ui colors are customizable, you only need to get a hold of the instance. Keep in mind that by doing that the use of the "TIME_MEASUREMENTS_DISABLED" macro might lead to compile errors.

	TIME_SAMPLE_GET_INSTANCE()->setUIActivationKey('T');
	TIME_SAMPLE_GET_INSTANCE()->setHighlightColor(ofColor::red);
	

#### 5.5 MEASURING setup()
If you want the setup() call to be automatically measured, you need to at least call once ofxTimeMeasurements in your main.cpp, just before the ofRunApp() line. This is so that the ofxTimeMeasurements instance is allocated before setup() is called. Make sure you call it after ofSetupOpenGL() otherwise it might not be able to find its settings file, because ofToDataPath might not be set yet. 

	int main(){
		ofSetupOpenGL(1680,1050, OF_WINDOW);
		TIME_SAMPLE_SET_FRAMERATE(60);
		ofRunApp(new testApp());
	}

in OF v09, you need to call ```TIME_SAMPLE_ADD_SETUP_HOOKS()``` in your main()

#### 5.6 PERCENTAGES
The percentages shown besides the ms counts represent how much of a full frame that code section takes. For example; on a 60fps app, if a code section takes 16.6ms, it will fully saturate the desired between-frames time (16.6ms), and it will show 100% 

#### 5.7 TIMING RESOLUTION
You can increase the resolution/accuracy of the time measurements by using [ofxMSATimer](https://github.com/obviousjim/ofxMSATimer). This is recommended on Windows, as the default OF timer resolution is not very high on that platform. To use ofxMSATimer you need to add the ofxMSATimer to the project, and define USE_MSA_TIMER in your project Pre-Processor macros.

#### 5.8 USING ofxHistoryPlot

If you add [ofxHistoryPlot](https://github.com/armadillu/ofxHistoryPlot) to your project, you will also be able to track your timings over time. To do so, add ofxHistoryPlot to your project, and ofxTimeMeasurements will automatically detect it and enable the use of time plots. * On Windows, you will still need to manually add USE_OFX_HISTORYPLOT to your project pre-processor macros for HistoryPlot to be available..

If you do so, when in interactive mode, you can press 'P' to toggle the plotting of the timings of the selected section over time.

![img](https://farm1.staticflickr.com/510/19847290279_4b9761ff4d_o_d.png)

Measurements that are being plotted will show a colored label on the left side, matching the color of the plot.

####5.9 USING ofxFontStash

If you include [ofxFontStash](https://github.com/armadillu/ofxFontStash) in your project, you can use it to draw the widget with any font of your liking. FontStash is faster at drawing text than ofDrawBitmapString(), which is what ofxTimeMeasurements uses by default. Define USE_OFX_FONTSTASH in your project's PreProcessor Macros, and call drawUiWithFontStash() supplying a monospaced ttf font. 

```
TIME_SAMPLE_GET_INSTANCE()->setUiScale( 2.0 ); //x2 the size for 4k screens
TIME_SAMPLE_GET_INSTANCE()->drawUiWithFontStash("VeraMono.ttf");
```

Another perk of using ofxFontStash is that you can set a custom font/UI scale. Look for setUiScale(); but make sure you set the UI Scale before you supply the TTF font.

This can be useful on retina/4k screens to make the widget more legible, as shown on the screenshot below.
![img](https://farm1.staticflickr.com/533/20034108485_fdaa20bd72_o_d.png)


##LICENSE
ofxTimeMeasurements is made available under the [MIT](http://opensource.org/licenses/MIT) license.

ofxTimeMeasurements makes small use of [tree.h](http://archive.gamedev.net/archive/reference/programming/features/coretree2/tree.h) by Justin Gottschlich which is licensed under the terms of the GNU Lesser General Public License 2.1 ([LGPL v2.1](http://choosealicense.com/licenses/lgpl-2.1/)).
