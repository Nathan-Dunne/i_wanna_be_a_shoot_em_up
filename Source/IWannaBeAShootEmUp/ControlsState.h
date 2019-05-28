#pragma once

//created by Luke O Brien
// A state that displays the controls to the player, using the gamestate framework provided to us

#ifndef CONTROL_STATE_h
#define CONTROL_STATE_h

#include <vector>
#include "MenuState.h"
#include "GameObject.h"

class ControlsState : public MenuState
{
public:

	virtual ~ControlsState() {}	

	virtual void update();
	virtual void render();

	virtual bool onEnter();
	virtual bool onExit();

	virtual std::string getStateID() const { return s_controlsID; }

private:

	virtual void setCallbacks(const std::vector<Callback>& callbacks);

	// call back functions for menu items
	static void s_menuToPlay();
	//static void s_exitFromMenu();

	static const std::string s_controlsID;

	std::vector<GameObject*> m_gameObjects;
};

#endif