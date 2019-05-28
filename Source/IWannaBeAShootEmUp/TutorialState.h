#pragma once
//
//created by Luke O Brien
// A state that displays a tutorial to the player, using the gamestate framework provided to us
//

#ifndef TUT_STATE_h
#define TUT_STATE_h

#include <vector>
#include "MenuState.h"
#include "GameObject.h"

class TutorialState : public MenuState
{
public:

	virtual ~TutorialState() {}

	virtual void update();
	virtual void render();

	virtual bool onEnter();
	virtual bool onExit();

	virtual std::string getStateID() const { return s_tutorialID; }

private:

	virtual void setCallbacks(const std::vector<Callback>& callbacks);

	// call back functions for menu items
	static void s_menuToPlay();
	//static void s_exitFromMenu();

	static const std::string s_tutorialID;

	std::vector<GameObject*> m_gameObjects;
};

#endif