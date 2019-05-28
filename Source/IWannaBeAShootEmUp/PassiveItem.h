#pragma once
#ifndef PASSIVE_ITEM
#define PASSIVE_ITEM
#include <string>
class PassiveItem
{
public:
	PassiveItem();
	~PassiveItem();
	std::string getItemName();
protected:
	int m_moveSpeedChange;
	std::string m_itemName;

};


#endif
