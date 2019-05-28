#pragma once
#ifndef PASSIVE_ITEM_TEST
#define PASSIVE_ITEM_TEST
#include "PassiveItem.h"

class PassiveItemTest : public PassiveItem
{
public:
	PassiveItemTest();
	~PassiveItemTest();
	int getMoveSpeedChange();
protected:
};


#endif