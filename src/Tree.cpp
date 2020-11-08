//
//  Tree.cpp
//  ofxTimeMeasurements_example
//
//  Created by Oriol Ferrer Mesià on 07/11/2020.
//

#include "Tree.h"
#include "ofLog.h"

Tree::Node::Node(const TREE_DATA & d){
	data = d;
	parent = nullptr;
}

Tree::Node::Node(const TREE_DATA & d, Tree::Node* parent){
	data = d;
	this->parent = parent;
}

const TREE_DATA & Tree::Node::getData(){
	return data;
}

Tree::Node* Tree::Node::addChildren(const TREE_DATA & d){
	Tree::Node * n = new Tree::Node(d, this);
	children.push_back(n);
	return n;
}


Tree::Node* Tree::Node::addSibling(const TREE_DATA & d){
	if(this->parent){
		return this->parent->addChildren(d);
	}else{
		ofLogError("Tree::Node") << "can't add sibling to root node!";
	}
	return nullptr;
}


Tree::Node * Tree::Node::getParent() const{
	return parent;
}


Tree::Node* Tree::Node::getFirstChild() const{
	if(children.size()) return children[0];
	return nullptr;
}

Tree::Node* Tree::Node::getLastChild() const{
	if(children.size()) return children[children.size()-1];
	return nullptr;
}


const std::vector<Tree::Node*> & Tree::Node::getChildren() const{
	return children;
}


Tree::Node* Tree::Node::findInChildren(const TREE_DATA & d){
	for(auto & n : children){
		if(n->data == d) return n;
	}
	for(auto & n : children){
		Tree::Node* res = n->findInChildren(d);
		if(res){
			return res;
		}
	}
	return nullptr;
}


void Tree::Node::getAllData1(std::vector<TREE_DATA> & data) const{
	data.push_back(this->data);
	for(auto & n : children){
		n->getAllData1(data);
	}
}

void Tree::Node::getAllData2(std::vector<TREE_DATA> & data) const{
	for(auto & n : children){
		n->getAllData2(data);
	}
	data.push_back(this->data);
}


///////////////////////////////////////////////////////////////////

Tree::Node* Tree::setup(const TREE_DATA & d){
	root = new Node(d);
	return root;
}


Tree::Node* Tree::getRoot(){
	return root;
}

Tree::Node* Tree::find(const TREE_DATA & d){
	if(root) return root->findInChildren(d);
	return nullptr;
}
