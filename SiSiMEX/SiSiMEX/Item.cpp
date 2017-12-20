#include "Item.h"
#include "Globals.h"
#include <algorithm>


Item::Item() : _id(NULL_ITEM_ID)
{
}

Item::Item(int id) : _id(id)
{
}


Item::~Item()
{
}

//void Item::Write(OutputMemoryStream &stream)
//{
//	stream.Write(_id);
//}
//
//void Item::Read(InputMemoryStream &stream)
//{
//	stream.Read(_id);
//}


ItemList::ItemList()
{
}


ItemList::~ItemList()
{
}

void ItemList::randomInitialization()
{
	// Create random items
	for (int i = 0; i < MAX_ITEMS; ++i) {
		int itemId = rand() % MAX_ITEMS;
		Item item(itemId);
		addItem(item);
	}

	// Sort items
	_items.sort();
}

void ItemList::addItem(const Item &item)
{
	if (item.id() != NULL_ITEM_ID) {
		_items.push_back(item);
	}
}

void ItemList::removeItem(int itemId)
{
	for (auto it = _items.begin(); it != _items.end(); ++it)
	{
		const Item &item(*it);
		if (item.id() == itemId) {
			_items.erase(it);
			break;
		}
	}
}

ItemList ItemList::getSpareItems() const
{
	ItemList spareItems;
	std::set<int> existingItems;
	for (auto item : _items) {
		auto itemId = item.id();
		if (existingItems.find(itemId) != existingItems.end()) {
			spareItems.addItem(item);
		} else {
			existingItems.insert(itemId);
		}
	}
	return spareItems;
}

//void ItemList::Write(OutputMemoryStream &stream)
//{
//	int16_t itemCount = static_cast<int16_t>(_items.size());
//	stream.Write(itemCount);
//	for (auto item : _items) {
//		item.Write(stream);
//	}
//}
//
//void ItemList::Read(InputMemoryStream &stream)
//{
//	int16_t itemCount;
//	stream.Read(itemCount);
//	_items.resize(itemCount);
//	for (auto item : _items) {
//		item.Read(stream);
//	}
//}
