//
//
//created by Luke O Brien
// A state that displays a tutorial to the player, using the gamestate framework provided to us

#include <iostream>
#include "TutorialState.h"
#include "TextureManager.h"
#include "Game.h"
#include "MenuButton.h"
#include "PlayState.h"
#include "InputHandler.h"
#include "StateParser.h"
#include <assert.h>
#include "DatabaseManager.h"

const std::string TutorialState::s_tutorialID = "TUTORIAL";

// Callbacks
void TutorialState::s_menuToPlay()
{
	TheGame::Instance()->getStateMachine()->changeState(new PlayState()); //go to a new playState on button press
}
// end callbacks

void TutorialState::update()
{
	if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_SPACE) || TheInputHandler::Instance()->isButtonDown(SDL_CONTROLLER_BUTTON_START))
	{
		s_menuToPlay();
	}
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

void TutorialState::render()
{
	if (m_loadingComplete && !m_gameObjects.empty())
	{
		TheTextureManager::Instance()->drawFrame("tutorial", 0, 0, 1280, 960, 0, 0, TheGame::Instance()->getRenderer(), 0.0, 255);

		for (int i = 0; i < m_gameObjects.size(); i++)
		{
			m_gameObjects[i]->draw();
		}
	}
}

bool TutorialState::onEnter()
{

	TheTextureManager::Instance()->load("assets/Tutorial.png", "tutorial", TheGame::Instance()->getRenderer());

	// parse the state
	StateParser stateParser;
	stateParser.parseState("assets/attack.xml", s_tutorialID, &m_gameObjects, &m_textureIDList);

	m_callbacks.push_back(0);
	m_callbacks.push_back(s_menuToPlay);

	// set the callbacks for menu items
	setCallbacks(m_callbacks);

	m_loadingComplete = true;
	std::cout << "entering TutorialState\n";
	return true;
}

bool TutorialState::onExit()
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

	//free memory
	m_gameObjects.clear();
	TheTextureManager::Instance()->clearTextureMap();

	std::cout << "exiting TutorialState\n";
	return true;
}

void TutorialState::setCallbacks(const std::vector<Callback>& callbacks)
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

