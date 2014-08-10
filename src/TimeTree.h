/*
 *  TimeTreeNode.h
 *  emptyExample
 *
 *  Created by Oriol Ferrer Mesià on 3/08/14.
 *  Copyright 2014 uri.cat. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"

struct TimeMeasurement;

class TimeTreeNode{
public:

	TimeTreeNode(){parent = NULL;}
	TimeTreeNode(string ID_){parent = NULL; tm = NULL;}
	bool isLeaf(){return children.size() == 0;}
	bool isRoot(){return parent == NULL;}
	vector<TimeTreeNode*> getChildren(){return children;}
private:
	TimeMeasurement * tm;
	vector<TimeTreeNode*> children;
	TimeTreeNode* parent;
};


class SimpleTimeTree{
public:

	SimpleTimeTree(){
		root = new TimeTreeNode();
	}

	TimeTreeNode* getRoot();
	void addChildren(TimeMeasurement *t){
	}

private:

	TimeTreeNode* root;
	
};
