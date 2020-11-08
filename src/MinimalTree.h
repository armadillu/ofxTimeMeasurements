//
//  MinimalTree.hpp
//  ofxTimeMeasurements_example
//
//  Created by Oriol Ferrer Mesià on 07/11/2020.
//

#ifndef Tree_hpp
#define Tree_hpp

#include <stdio.h>
#include <string>
#include <vector>

#define TREE_DATA_TYPE	std::string

class MinimalTree{
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////

	class Node{

		friend class MinimalTree;

	public:

		Node(const TREE_DATA_TYPE & d);
		const TREE_DATA_TYPE & getData();

		Node* addChildren(const TREE_DATA_TYPE & d);
		Node* addSibling(const TREE_DATA_TYPE & d);

		Node * getParent() const;

		Node* getFirstChild() const;
		Node* getLastChild() const;
		Node* getNextChildren() const;
		const std::vector<Node*> & getChildren() const;
		int getNumChildren() const;

		void getAllData(std::vector<std::pair<Node*, int>> & data) const;

		Node* findInChildren(const TREE_DATA_TYPE & d);

		int level() const;

	protected:

		Node(const TREE_DATA_TYPE & d, Node* parent);

		Node* parent = nullptr;
		std::vector<Node*> children;
		TREE_DATA_TYPE data;

		void walkAndStore(std::vector<std::pair<Node*, int>> & data, bool isRoot = true) const;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////

	Node* getRoot();
	Node* setup(const TREE_DATA_TYPE & d);
	Node* find(const TREE_DATA_TYPE & d);


protected:

	Node* root = nullptr;

};

#endif /* Tree_hpp */
