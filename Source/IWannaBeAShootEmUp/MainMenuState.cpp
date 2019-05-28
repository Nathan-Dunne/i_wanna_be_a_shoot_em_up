//
//  MenuState.cpp
//  SDL Game Programming Book
//
//  Created by shaun mitchell on 09/02/2013.
//  Copyright (c) 2013 shaun mitchell. All rights reserved.
//
// Edited by Luke O Brien and Nathan Dunne
#include <iostream>
#include "MainMenuState.h"
#include "TextureManager.h"
#include "Game.h"
#include "MenuButton.h"
#include "PlayState.h"
#include "ControlsState.h"
#include "InputHandler.h"
#include "StateParser.h"
#include <assert.h>
#include "DatabaseManager.h"

const std::string MainMenuState::s_menuID = "MENU";

// Callbacks
void MainMenuState::s_menuToPlay()
{
    TheGame::Instance()->getStateMachine()->changeState(new ControlsState());	//go to controls state on button press
}

void MainMenuState::s_exitFromMenu()
{
    TheGame::Instance()->quit();
}

// end callbacks

void MainMenuState::update()
{
	if(TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_SPACE) || TheInputHandler::Instance()->isButtonDown(SDL_CONTROLLER_BUTTON_START))
	{
		s_menuToPlay();
	}
    if(!m_gameObjects.empty())
    {
			for(int i = 0; i < m_gameObjects.size(); i++)
			{
				if(m_gameObjects[i] != 0)
				{
					m_gameObjects[i]->update();
				}
			}
    }
}

//renders everything on the screen
void MainMenuState::render()
{
    if(m_loadingComplete && !m_gameObjects.empty())
    {
		
		TheTextureManager::Instance()->drawFrame("title", 250, 150, 768, 192, 0, 0, TheGame::Instance()->getRenderer(), 0.0, 255);
		TheTextureManager::Instance()->drawFrame("pressStart", 460, 520, 354, 29, 0, 0, TheGame::Instance()->getRenderer(), 0.0, 255);

        for(int i = 0; i < m_gameObjects.size(); i++)
        {
            m_gameObjects[i]->draw();
        }
    }
}

bool MainMenuState::onEnter()
{
	// Runs a function that sets up the Database
	TheDatabaseManager::Instance()->runInitialScript();

	//loads textures used for Title
	TheTextureManager::Instance()->load("assets/TitleText.png", "title", TheGame::Instance()->getRenderer());
	TheTextureManager::Instance()->load("assets/pressStart.png", "pressStart", TheGame::Instance()->getRenderer());

	// set the amount of lives the player has here so that restarts work as intended
	TheGame::Instance()->setPlayerLives(5);

    // parse the state
    StateParser stateParser;
    stateParser.parseState("assets/attack.xml", s_menuID, &m_gameObjects, &m_textureIDList);
    
    m_callbacks.push_back(0);
    m_callbacks.push_back(s_menuToPlay);
    m_callbacks.push_back(s_exitFromMenu);
    
    // set the callbacks for menu items
    setCallbacks(m_callbacks);
    
    m_loadingComplete = true;
    std::cout << "entering MainMenuState\n";
    return true;
}

bool MainMenuState::onExit()
{
    m_exiting = true;

	// reset the game
	TheGame::Instance()->resetGame();
    
    // clean the game objects
    if(m_loadingComplete && !m_gameObjects.empty())
    {
		m_gameObjects.back()->clean();
		m_gameObjects.pop_back();
    }

	// free memory
	m_gameObjects.clear();
	TheTextureManager::Instance()->clearTextureMap();

    std::cout << "exiting MainMenuState\n";
    return true;
}

void MainMenuState::setCallbacks(const std::vector<Callback>& callbacks)
{
    // go through the game objects
    if(!m_gameObjects.empty())
    {
        for(int i = 0; i < m_gameObjects.size(); i++)
        {
            // if they are of type MenuButton then assign a callback based on the id passed in from the file
            if(dynamic_cast<MenuButton*>(m_gameObjects[i]))
            {
                MenuButton* pButton = dynamic_cast<MenuButton*>(m_gameObjects[i]);
                pButton->setCallback(callbacks[pButton->getCallbackID()]);
            }
        }
    }
}

