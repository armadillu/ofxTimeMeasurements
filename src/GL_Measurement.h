//
//  GL_Measurement.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 20/01/2017.
//
//

#pragma once
#include "ofMain.h"

#ifndef TARGET_OPENGLES

class GL_Measurement{

public:
	
	void init(){
		if (glGenQueries && glBeginQuery && glEndQuery && glGetQueryiv && glGetQueryObjectiv && glDeleteQueries){
			if ((GLEW_VERSION_3_3 || GLEW_ARB_timer_query) && glGetQueryObjectui64v && glQueryCounter){
				backend = CORE;
			}
		}
		if (backend == CORE){
			GLint counterBits = 0;
			glGetQueryiv(GL_TIMESTAMP, GL_QUERY_COUNTER_BITS, &counterBits);
			if (counterBits == 0){
				backend = UNSUPPORTED;
			}
		}
		if (backend == UNSUPPORTED && glGenQueries && glBeginQuery && glEndQuery && glGetQueryiv && glGetQueryObjectiv && glDeleteQueries){
			GLint counterBits = 0;
			if (((GLEW_VERSION_3_3 || GLEW_ARB_timer_query) && glGetQueryObjectui64v) || glGetQueryObjectui64vEXT){
				glGetQueryiv(GL_TIME_ELAPSED, GL_QUERY_COUNTER_BITS, &counterBits);
				if (counterBits > 0){
					backend = glGetQueryObjectui64vEXT ? EXTENSION : CORE_ELAPSED;
				}
			}
		}
		if (backend == UNSUPPORTED){
			static bool warned = false;
			if (!warned){
				ofLogWarning("ofxTimeMeasurements") << "GPU timer queries are not supported by the current OpenGL context";
				warned = true;
			}
			return;
		}
		glGenQueries(backend == CORE ? 2 : 1, queryID);
		state = INITED;
	}

	void start(){
		if (backend == CORE){
			glQueryCounter(queryID[0], GL_TIMESTAMP);
		}else{
			glBeginQuery(GL_TIME_ELAPSED, queryID[0]);
		}
		state = MEASURING;
	}

	void stop(){
		if (backend == CORE){
			glQueryCounter(queryID[1], GL_TIMESTAMP);
		}else{
			glEndQuery(GL_TIME_ELAPSED);
		}
		state = MEASURED;
		measuredFrame = ofGetFrameNum();
	}

	void update(){
		if (state == MEASURED && ofGetFrameNum() - measuredFrame > 2 ){
			GLint available = false;
			glGetQueryObjectiv(queryID[backend == CORE ? 1 : 0], GL_QUERY_RESULT_AVAILABLE, &available);
			if (available){
				GLuint64 measurementResult;
				state = RESULT_READY;
				if (backend == CORE){
					GLuint64 startTime = 0;
					GLuint64 endTime = 0;
					glGetQueryObjectui64v(queryID[0], GL_QUERY_RESULT, &startTime);
					glGetQueryObjectui64v(queryID[1], GL_QUERY_RESULT, &endTime);
					measurementResult = endTime >= startTime ? endTime - startTime : 0;
				}else if (backend == CORE_ELAPSED){
					glGetQueryObjectui64v(queryID[0], GL_QUERY_RESULT, &measurementResult);
				}else{
					glGetQueryObjectui64vEXT(queryID[0], GL_QUERY_RESULT, &measurementResult);
				}
				this->measurementResult = measurementResult * 0.000001; //convert to msec
			}else{
				ofLogError("ofxTimeMeasurements") << "GL measurement not ready yet!";
			}
		}
	}

	bool isMeasurementReady(){return state == RESULT_READY;}
	double getMeasurement(){return measurementResult;}
	void acknowledgeMeasurement(){ state = INITED;} // after you successfully read the measurement, ack it
												//so that the timer can be started again.


	bool canStartMeasuring(){return state == INITED || state == RESULT_READY;}
	bool canStopMeasuring(){return state == MEASURING;}
	bool supportsNesting(){return backend == CORE || backend == UNSUPPORTED;}

	~GL_Measurement(){
		if (state > UNINITED){
			glDeleteQueries(backend == CORE ? 2 : 1, queryID);
		}
	}

protected:

	enum Backend{
		UNSUPPORTED,
		CORE,
		CORE_ELAPSED,
		EXTENSION
	};

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
	Backend backend = UNSUPPORTED;
	GLuint queryID[2];
};


#endif
