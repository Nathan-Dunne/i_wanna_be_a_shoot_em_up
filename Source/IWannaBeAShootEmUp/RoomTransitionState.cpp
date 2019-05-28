//
//
//created by Luke O Brien
//A simple state created to provide a transition between the levels, using the gamestate framework provided to us


#include "RoomTransitionState.h"

const std::string RoomTransitionState::s_transitionID = "ROOM_TRANSITION";

void RoomTransitionState::update()	
{
	if (m_loadingComplete && !m_exiting)
	{
		TheGame::Instance()->getStateMachine()->changeState(new PlayState());	//after loading is complete, go to the next playstate

	}
}

void RoomTransitionState::render()
{
	if (m_loadingComplete)
	{
		TheTextureManager::Instance()->drawFrame("loading", 500, 400, 256, 128, 0, 0, TheGame::Instance()->getRenderer(), 0.0, 255); //render the loading text
	}
}

bool RoomTransitionState::onEnter()
{
	TheTextureManager::Instance()->load("assets/loading.png", "loading", TheGame::Instance()->getRenderer());	// load the loading image

	m_loadingComplete = true;
	std::cout << "Entering RoomtransitionState\n";
	return true;
}

bool RoomTransitionState::onExit()
{
	m_exiting = true;

	std::cout << "Exiting RoomtransitionState\n";

	TheTextureManager::Instance()->clearTextureMap();
	TheGame::Instance()->setIsItemPickedUp(false);

	return true;
}

void RoomTransitionState::setCallbacks(const std::vector<Callback>& callbacks)
{
}
