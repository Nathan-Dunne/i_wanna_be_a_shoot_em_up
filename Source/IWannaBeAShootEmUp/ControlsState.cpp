//
//created by Luke O Brien
// A state that displays the controls to the player, using the gamestate framework provided to us

#include <iostream>
#include "ControlsState.h"
#include "TextureManager.h"
#include "Game.h"
#include "MenuButton.h"
#include "TutorialState.h"
#include "InputHandler.h"
#include "StateParser.h"
#include <assert.h>
#include "DatabaseManager.h"

const std::string ControlsState::s_controlsID = "CONTROLS";	//used to read the correct objects from the attack.xml file

// Callbacks
void ControlsState::s_menuToPlay()	
{
	//when the button is pressed, enter a new TutorialState
	TheGame::Instance()->getStateMachine()->changeState(new TutorialState());
}
// end callbacks

void ControlsState::update()
{
	//if the player presses space or the 'a' button on a controller, they can activate the button
	if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_SPACE) || TheInputHandler::Instance()->isButtonDown(SDL_CONTROLLER_BUTTON_A))
	{
		s_menuToPlay();
	}
	//update loop for the buttons
	if (!m_gameObjects.empty())
	{
		for (int i = 0; i < m_gameObjects.size(); i++)
		{
			if (m_gameObjects[i] != 0)
			{
				m_gameObjects[i]->update();
			}
		}
	}
}

void ControlsState::render()
{
	if (m_loadingComplete && !m_gameObjects.empty())
	{
		//renders the controls image
		TheTextureManager::Instance()->drawFrame("controls", 170, 10, 960, 700, 0, 0, TheGame::Instance()->getRenderer(), 0.0, 255);

		//render loop for menu buttons
		for (int i = 0; i < m_gameObjects.size(); i++)
		{
			m_gameObjects[i]->draw();
		}
	}
}

bool ControlsState::onEnter()
{
	//loading the texture to be used
	TheTextureManager::Instance()->load("assets/controlScreen.png", "controls", TheGame::Instance()->getRenderer());

	// parse the state
	StateParser stateParser;
	stateParser.parseState("assets/attack.xml", s_controlsID, &m_gameObjects, &m_textureIDList);

	m_callbacks.push_back(0);
	m_callbacks.push_back(s_menuToPlay);

	// set the callbacks for menu items
	setCallbacks(m_callbacks);

	m_loadingComplete = true;
	std::cout << "entering ControlsState\n";
	return true;
}

bool ControlsState::onExit()
{
	m_exiting = true;

	// reset the game
	TheGame::Instance()->resetGame();

	// clean the game objects
	if (m_loadingComplete && !m_gameObjects.empty())
	{
		m_gameObjects.back()->clean();
		m_gameObjects.pop_back();
	}

	//freeing memory
	m_gameObjects.clear();
	TheTextureManager::Instance()->clearTextureMap();

	std::cout << "exiting ControlsState\n";
	return true;
}

void ControlsState::setCallbacks(const std::vector<Callback>& callbacks)
{
	// go through the game objects
	if (!m_gameObjects.empty())
	{
		for (int i = 0; i < m_gameObjects.size(); i++)
		{
			// if they are of type MenuButton then assign a callback based on the id passed in from the file
			if (dynamic_cast<MenuButton*>(m_gameObjects[i]))
			{
				MenuButton* pButton = dynamic_cast<MenuButton*>(m_gameObjects[i]);
				pButton->setCallback(callbacks[pButton->getCallbackID()]);
			}
		}
	}
}

