#pragma once
#include <Net.h>

/**
 * A single item and its properties.
 */
class Item
{
public:

	// Constructors and destructor
	Item();
	Item(int id);
	~Item();

	// Item identifier
	int id() const { return _id; }

	//// Serialization methods
	//void Write(OutputMemoryStream &stream);
	//void Read(InputMemoryStream &stream);

	// Comparison operator
	bool operator<(const Item &item) { return _id < item._id; }

private:
	int _id; /**< Item identifier. */
};

/**
 * A list of items.
 */
class ItemList
{
public:

	// Constructor and destructor
	ItemList();
	~ItemList();

	// It initializes the item list randomly
	void randomInitialization();

	// Methods to add and remove items to/from the list
	void addItem(const Item &item);
	void removeItem(int itemId);

	// It returns the inner std::list of items
	const std::list<Item> &items() const { return _items; }

	// It returns a new list with all the repeated items
	ItemList getSpareItems() const;

	//// Serialization methods
	//void Write(OutputMemoryStream &stream);
	//void Read(InputMemoryStream &stream);

private:

	std::list<Item> _items; /**< A list of items. */
};
