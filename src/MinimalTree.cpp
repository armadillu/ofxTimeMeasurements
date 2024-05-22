//
//  MinimalTree.cpp
//  ofxTimeMeasurements_example
//
//  Created by Oriol Ferrer Mesià on 07/11/2020.
//

#include <algorithm> // for std::sort

#include "MinimalTree.h"
#include "ofLog.h"

MinimalTree::Node::Node(const TREE_DATA_TYPE & d){
	data = d;
	parent = nullptr;
}

MinimalTree::Node::Node(const TREE_DATA_TYPE & d, MinimalTree::Node* parent){
	data = d;
	this->parent = parent;
}

const TREE_DATA_TYPE & MinimalTree::Node::getData(){
	return data;
}

MinimalTree::Node* MinimalTree::Node::addChildren(const TREE_DATA_TYPE & d){
	MinimalTree::Node * n = new MinimalTree::Node(d, this);
	children.push_back(n);
	return n;
}


MinimalTree::Node* MinimalTree::Node::addSibling(const TREE_DATA_TYPE & d){
	if(this->parent){
		return this->parent->addChildren(d);
	}else{
		ofLogError("MinimalTree::Node") << "can't add sibling to root node!";
	}
	return nullptr;
}


MinimalTree::Node * MinimalTree::Node::getParent() const{
	return parent;
}


MinimalTree::Node* MinimalTree::Node::getFirstChild() const{
	if(children.size()) return children[0];
	return nullptr;
}

MinimalTree::Node* MinimalTree::Node::getLastChild() const{
	if(children.size()) return children[children.size()-1];
	return nullptr;
}

MinimalTree::Node* MinimalTree::Node::getNextChildren() const{
	if(parent){
		if(parent->children.size()){
			auto it = std::find(parent->children.begin(), parent->children.end(), this);
			if(it != parent->children.end()){
				int index = it - parent->children.begin();
				if(index + 1 < parent->children.size()){
					return parent->children[index + 1];
				}
			}
		}
	}
	return nullptr;
}

const std::vector<MinimalTree::Node*> & MinimalTree::Node::getChildren() const{
	return children;
}


int MinimalTree::Node::getNumChildren() const{
	return children.size();
}


MinimalTree::Node* MinimalTree::Node::findInChildren(const TREE_DATA_TYPE & d){
	for(auto & n : children){
		if(n->data == d) return n;
	}
	for(auto & n : children){
		MinimalTree::Node* res = n->findInChildren(d);
		if(res){
			return res;
		}
	}
	return nullptr;
}

int MinimalTree::Node::level() const{

	int l = 0;
	const MinimalTree::Node * it = this;
	while(it->getParent()){
		it = it->getParent();
		l++;
	}
	return l;
}


void MinimalTree::Node::getAllData(std::vector<std::pair<Node*, int>> & data) const{
	walkAndStore(data, true);
}

void MinimalTree::Node::walkAndStore(std::vector<std::pair<Node*, int>> & data, bool isRoot) const{
	if(!isRoot) data.push_back(std::make_pair((Node*)this, this->level()));
	for(auto & n : children){
		n->walkAndStore(data, false);
	}
}


///////////////////////////////////////////////////////////////////

MinimalTree::Node* MinimalTree::setup(const TREE_DATA_TYPE & d){
	root = new Node(d);
	return root;
}


MinimalTree::Node* MinimalTree::getRoot(){
	return root;
}

MinimalTree::Node* MinimalTree::find(const TREE_DATA_TYPE & d){
	if(root) return root->findInChildren(d);
	return nullptr;
}
