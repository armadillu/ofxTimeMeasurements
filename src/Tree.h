//
//  Tree.hpp
//  ofxTimeMeasurements_example
//
//  Created by Oriol Ferrer Mesià on 07/11/2020.
//

#ifndef Tree_hpp
#define Tree_hpp

#include <stdio.h>
#include <string>
#include <vector>

#define TREE_DATA	std::string


class Tree{
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////

	class Node{

		friend class Tree;

	public:

		Node(const TREE_DATA & d);
		const TREE_DATA & getData();

		Node* addChildren(const TREE_DATA & d);
		Node* addSibling(const TREE_DATA & d);

		Node * getParent() const;

		Node* getFirstChild() const;
		Node* getLastChild() const;
		const std::vector<Node*> & getChildren() const;

		void getAllData1(std::vector<TREE_DATA> & data) const;
		void getAllData2(std::vector<TREE_DATA> & data) const;

		Node* findInChildren(const TREE_DATA & d);

	protected:

		Node(const TREE_DATA & d, Node* parent);

		Node * parent = nullptr;
		std::vector<Node*> children;
		TREE_DATA data;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////

	Node * getRoot();
	Node* setup(const TREE_DATA & d);
	Node * find(const TREE_DATA & d);


protected:

	Node * root = nullptr;


};

#endif /* Tree_hpp */
