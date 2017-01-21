//
//  GL_Measurement.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 20/01/2017.
//
//

#pragma once
#include "ofMain.h"

class GL_Measurement{

public:
	
	void init(){
		glGenQueries(1, queryID);
		state = INITED;
	}

	void start(){
		glBeginQuery(GL_TIME_ELAPSED_EXT, queryID[0]);
		state = MEASURING;
	}

	void stop(){
		glEndQuery(GL_TIME_ELAPSED_EXT);
		state = MEASURED;
		measuredFrame = ofGetFrameNum();
	}

	void update(){
		if (state == MEASURED && ofGetFrameNum() - measuredFrame > 1 ){
			GLint available = false;
			glGetQueryObjectiv(queryID[0], GL_QUERY_RESULT_AVAILABLE, &available);
			if (available){
				GLuint64 measurementResult;
				state = RESULT_READY;
				glGetQueryObjectui64vEXT(queryID[0], GL_QUERY_RESULT, &measurementResult);
				this->measurementResult = measurementResult * 0.000001; //convert to msec
			}else{
				ofLogError("aa") << "not yet";
			}
		}
	}

	bool isMeasurementReady(){return state == RESULT_READY;}
	double getMeasurement(){return measurementResult;}
	void acknowledgeMeasurement(){ state = INITED;} // after you successfully read the measurement, ack it
												//so that the timer can be started again.


	bool canStartMeasuring(){return state == INITED || state == RESULT_READY;}
	bool canStopMeasuring(){return state == MEASURING;}

	~GL_Measurement(){
		if (state > UNINITED){
			glDeleteQueries(1, queryID);
		}
	}

protected:

	enum State{
		UNINITED,
		INITED,
		MEASURING,
		MEASURED,
		RESULT_READY
	};

	int measuredFrame;
	double measurementResult = 0.0; //msec
	State state = UNINITED;
	GLuint queryID[1];
};

