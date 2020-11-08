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

Tree::Node* Tree::Node::getNextChildren() const{
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

const std::vector<Tree::Node*> & Tree::Node::getChildren() const{
	return children;
}


int Tree::Node::getNumChildren() const{
	return children.size();
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

int Tree::Node::level() const{

	int l = 0;
	const Tree::Node * it = this;
	while(it->getParent()){
		it = it->getParent();
		l++;
	}
	return l;
}


void Tree::Node::getAllData(std::vector<std::pair<Node*, int>> & data) const{
	walkAndStore(data, true);
}

void Tree::Node::walkAndStore(std::vector<std::pair<Node*, int>> & data, bool isRoot) const{
	if(!isRoot) data.push_back(std::make_pair((Node*)this, this->level()));
	for(auto & n : children){
		n->walkAndStore(data, false);
	}
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
