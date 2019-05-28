//
//  GameOverState.cpp
//  SDL Game Programming Book
//
//  Created by shaun mitchell on 17/02/2013.
//  Copyright (c) 2013 shaun mitchell. All rights reserved.
//
// Edited by Luke O Brien

#include "GameOverState.h"
#include "MainMenuState.h"
#include "PlayState.h"
#include "TextureManager.h"
#include "AnimatedGraphic.h"
#include "Game.h"
#include "MenuButton.h"
#include "InputHandler.h"
#include "StateParser.h"


const std::string GameOverState::s_gameOverID = "GAMEOVER"; //used to read the correct objects from the attack.xml file

//handles buttons
void GameOverState::s_gameOverToMain()
{
    TheGame::Instance()->getStateMachine()->changeState(new MainMenuState());
}

void GameOverState::s_restartPlay()
{
    TheGame::Instance()->getStateMachine()->changeState(new PlayState());
}

void GameOverState::update()
{
	//activates buttons when game controller buttons are pressed
	if (TheInputHandler::Instance()->isButtonDown(SDL_CONTROLLER_BUTTON_START))
	{
		s_restartPlay();
	}
	else if(TheInputHandler::Instance()->isButtonDown(SDL_CONTROLLER_BUTTON_BACK))
	{
		s_gameOverToMain();
	}

    if(m_loadingComplete && !m_gameObjects.empty())
    {
        for(int i = 0; i < m_gameObjects.size(); i++)
        {
            m_gameObjects[i]->update();
        }
    }
}

void GameOverState::render()
{
    if(m_loadingComplete && !m_gameObjects.empty())
    {
        for(int i = 0; i < m_gameObjects.size(); i++)
        {
            m_gameObjects[i]->draw();
        }
    }
	//using sdl_text to print the score to the player
	TheTextureManager::Instance()->printText("Your Score was: ", 760, 100, 300, 100, TheGame::Instance()->getRenderer(), { 255,255,255 });
	TheTextureManager::Instance()->printText(std::to_string(score), 930, 100, 200, 100, TheGame::Instance()->getRenderer(), { 255,255,255 });

	int scoresToPrint = 0;

	if (scores.size() > 10)
	{
		scoresToPrint = 10;
	}
	else
	{
		scoresToPrint = scores.size();
	}

	TheTextureManager::Instance()->printText("Top 10 scores: ", 760, 190, 200, 100, TheGame::Instance()->getRenderer(), { 255,255,255 });

	TheTextureManager::Instance()->printText("Press Select to return to Main Menu. ", 285, 500, 200, 100, TheGame::Instance()->getRenderer(), { 255,255,255 });
	TheTextureManager::Instance()->printText("Press Start to Restart. ", 285, 530, 200, 100, TheGame::Instance()->getRenderer(), { 255,255,255 });

	for (int i = 0; i < scoresToPrint; i++)
	{
		TheTextureManager::Instance()->printText(std::to_string(i+1) + ". " + std::to_string(scores[i]), 910, 190 + (i*40), 200, 100, TheGame::Instance()->getRenderer(), { 255,255,255 });
	}

}

bool GameOverState::onEnter()
{
	TheDatabaseManager::Instance()->addScore(TheGame::Instance()->getScore());

	scores = TheDatabaseManager::Instance()->getScores();

	std::sort(scores.begin(), scores.end(), std::greater<int>());

    // parse the state
    StateParser stateParser;
    stateParser.parseState("assets/attack.xml", s_gameOverID, &m_gameObjects, &m_textureIDList);
    
	score = (int)TheGame::Instance()->getScore();
    m_callbacks.push_back(0);
    m_callbacks.push_back(s_gameOverToMain);
    m_callbacks.push_back(s_restartPlay);
    
    // set the callbacks for menu items
    setCallbacks(m_callbacks);
    
    m_loadingComplete = true;
    
    std::cout << "entering GameOverState\n";
    return true;
}

bool GameOverState::onExit()
{
	m_exiting = true;

	// reset the game
	TheGame::Instance()->resetGame();
	TheDatabaseManager::Instance()->runInitialScript();

	if (m_loadingComplete && !m_gameObjects.empty())
	{
		for (int i = 0; i < m_gameObjects.size(); i++)
		{
			m_gameObjects[i]->clean();
			delete m_gameObjects[i];
		}

		m_gameObjects.clear();
	}


    std::cout << "exiting GameOverState\n";
    return true;
}

void GameOverState::setCallbacks(const std::vector<Callback>& callbacks)
{
    // go through the game objects
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

