//
//
//created by Luke O Brien
//A simple state created to provide a transition between the levels, using the gamestate framework provided to us

#pragma once
#ifndef ROOM_TRANSITION_STATE
#define ROOM_TRANSITION_STATE

#include <iostream>
#include <vector>
#include "MenuState.h"
#include "TextureManager.h"
#include "Game.h"
#include "PlayState.h"
class GameObject;

class RoomTransitionState : public MenuState
{
public:

	virtual ~RoomTransitionState() {}

	virtual void update();
	virtual void render();

	virtual bool onEnter();
	virtual bool onExit();

	virtual std::string getStateID() const { return s_transitionID; }

	virtual void setCallbacks(const std::vector<Callback>& callbacks);

private:

	static const std::string s_transitionID;

	std::vector<GameObject*> m_gameObjects;
};


#endif /* ROOM_TRANSITION_STATE */
