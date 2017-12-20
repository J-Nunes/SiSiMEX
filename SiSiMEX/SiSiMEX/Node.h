#pragma once
#include "Item.h"
#include <memory>

class Node
{
public:

	// Constructor and destructor
	Node();
	~Node();

	// Initialization of its item list
	void initialize();

	// Getters
	ItemList &itemList() { return _itemList; }
	const ItemList &itemList() const { return _itemList; }

private:

	ItemList _itemList; /**< All items owned by this node. */
};

using NodePtr = std::shared_ptr<Node>;
