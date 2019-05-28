//
//  PlayState.cpp
//  SDL Game Programming Book
//
//  Created by shaun mitchell on 09/02/2013.
//  Copyright (c) 2013 shaun mitchell. All rights reserved.
//
// Edited by all team members

#include <iostream>
#include "PlayState.h"
#include "GameOverState.h"
#include "PauseState.h"
#include "Game.h"
#include "InputHandler.h"
#include "LevelParser.h"
#include "Level.h"
#include "BulletHandler.h"
#include <math.h> 

const std::string PlayState::s_playID = "PLAY";

void PlayState::update()
{
    if(m_loadingComplete && !m_exiting)
    {
		//pauses the game when the player presses either the escape key or the 'start' button
        if(TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_ESCAPE) || TheInputHandler::Instance()->isButtonDown(SDL_CONTROLLER_BUTTON_START))
        {
            TheGame::Instance()->getStateMachine()->pushState(new PauseState());
        }
        
        TheBulletHandler::Instance()->updateBullets();	//updates the bullets
        
		//if the player runs out of lives, go to the gameOver state
        if(TheGame::Instance()->getPlayerLives() == 0)
        {
            TheGame::Instance()->getStateMachine()->changeState(new GameOverState());
        }
        if(pLevel != 0)
        {
            pLevel->update();
        }

    }

	if (TheGame::Instance()->getScore() > 1)
	{
		TheGame::Instance()->m_score -= (0.20 / (TheGame::Instance()->m_difficultyIndex / 0.90));
	}
}

void PlayState::render()
{
    if(m_loadingComplete)
    {
		TheTextureManager::Instance()->drawFrame("doorSprite", 512, 0, 256, 32, 0, 0, TheGame::Instance()->getRenderer(), 0.0, 255);

		
        if(pLevel != 0)
        {
            pLevel->render();
        }

		int healthBarYPos = 60;
		int healthBarWidth = 60;

		// renders a background for the players healthbar
		TheTextureManager::Instance()->drawFrame("healthBarBackground",5, healthBarYPos, 295, 30, 0, 0, TheGame::Instance()->getRenderer(), 0.0, 255);

        for(int i = 1; i <= TheGame::Instance()->getPlayerLives(); i++)
        {
			// makes the health bar scalable 
			if (i == 1)
			{
				TheTextureManager::Instance()->drawFrame("healthBarStart", 5, healthBarYPos, healthBarWidth, 30, 0, 0, TheGame::Instance()->getRenderer(), 0.0, 255);
			}
			else if((i != 1) && (i != TheGame::Instance()->getPlayerLives())) 
			{
				TheTextureManager::Instance()->drawFrame("healthBarMiddle", (i * healthBarWidth) - healthBarWidth, healthBarYPos, healthBarWidth, 30, 0, 0, TheGame::Instance()->getRenderer(), 0.0, 255);
			}
			else
			{
				TheTextureManager::Instance()->drawFrame("healthBarEnd", (i * healthBarWidth) - healthBarWidth, healthBarYPos, healthBarWidth, 30, 0, 0, TheGame::Instance()->getRenderer(), 0.0, 255);
			}
        }
        
        TheBulletHandler::Instance()->drawBullets();	//renders the bullets

		SDL_Color difficultyColor = { 0, 142, 142 };

		//changes the color of the difficulty index text to corrilate with how hard the game currently is.
		if (TheGame::Instance()->getDifficultyIndex() >= 8)
		{
			difficultyColor = { 143, 0, 0 };
		}
		else if (TheGame::Instance()->getDifficultyIndex() >= 4)
		{
			difficultyColor = { 196, 171, 27 };
		}

		//this block of code is used to print out the players stats on the screen
		int difficultyDisplay = floor(int(TheGame::Instance()->getDifficultyIndex()));
		SDL_Color defaultColor = { 255, 255, 255 };

		TheTextureManager::Instance()->printText("Difficulty: ", 2, 15, 0, 0, TheGame::Instance()->getRenderer(), defaultColor);
		TheTextureManager::Instance()->printText(std::to_string(difficultyDisplay), 97, 15, 0, 0, TheGame::Instance()->getRenderer(), difficultyColor);

		TheTextureManager::Instance()->printText("Score: " + std::to_string(int(TheGame::Instance()->getScore())), 1075, 25, 0, 0, TheGame::Instance()->getRenderer(), defaultColor);
		TheTextureManager::Instance()->printText("Level Time: " + std::to_string((SDL_GetTicks() - levelStartTime)/1000) + "." + std::to_string((SDL_GetTicks() - levelStartTime) % 1000), 1075, 0, 0, 0, TheGame::Instance()->getRenderer(), defaultColor);

		TheTextureManager::Instance()->printText("Damage: " + std::to_string(TheGame::Instance()->getPlayerAttributes().m_damage),125, 0, 0, 0, TheGame::Instance()->getRenderer(), defaultColor);
		TheTextureManager::Instance()->printText("Fire Rate: " + std::to_string(TheGame::Instance()->getPlayerAttributes().m_firingRate), 258, 0, 0, 0, TheGame::Instance()->getRenderer(), defaultColor);

		TheTextureManager::Instance()->printText("Move Spd: " + std::to_string(TheGame::Instance()->getPlayerAttributes().m_movementSpeed), 125, 25, 0, 0, TheGame::Instance()->getRenderer(), defaultColor);
		TheTextureManager::Instance()->printText("Shot Spd: " + std::to_string(TheGame::Instance()->getPlayerAttributes().m_projectileSpeed), 257, 25, 0, 0, TheGame::Instance()->getRenderer(), defaultColor);

		//this if statement checks to see if a boss is alive and renders its healthbar if it is
		if (TheGame::Instance()->isBossAlive())
		{
			TheTextureManager::Instance()->drawFrame("bossHealthbarBackground", 450, 40, 506, 60, 0, 0, TheGame::Instance()->getRenderer(), 0.0, 255);
			int j = 0;
			//this for loop renders a segment of the health bar for each point of health that the boss has
			for (int i = 0; i < TheGame::Instance()->getBossHealth(); i++)
			{
				TheTextureManager::Instance()->drawFrame("bossHealthbar", 453 + j, 66, 10, 30, 0, 0, TheGame::Instance()->getRenderer(), 0.0, 255);
				j += 10;
			}
		}

		SDL_Color pickUpTextColor = { 0, 204, 0 };
		// this if statement displays text whenever the player picks up an item
		if (TheGame::Instance()->getIsItemPickedUp() && TheGame::Instance()->getItemPickedUp() != "Pickup")
		{
			TheTextureManager::Instance()->printText(TheGame::Instance()->getItemPickedUp() + " picked up.", 550, 500, 0, 0, TheGame::Instance()->getRenderer(), pickUpTextColor);
		}

		if (freeze == 1)
		{
			while (freeze2 < 2000)
			{
				freeze2++;
				//std::cout << " 3 freeze = " << freeze2 << std::endl;
			}
		}
		freeze++;
    }
	
}

bool PlayState::onEnter()
{
	freeze = 0;
	freeze2 = 0;

	levelStartTime = SDL_GetTicks();

    LevelParser levelParser;
    pLevel = levelParser.parseLevel(TheGame::Instance()->getLevelFiles()[TheGame::Instance()->getCurrentLevel() - 1].c_str());
    
	//loading textures that are used in the playState
    TheTextureManager::Instance()->load("assets/bullet1.png", "bullet1", TheGame::Instance()->getRenderer());
    TheTextureManager::Instance()->load("assets/bullet2.png", "bullet2", TheGame::Instance()->getRenderer());
    TheTextureManager::Instance()->load("assets/bullet3.png", "bullet3", TheGame::Instance()->getRenderer());

	TheTextureManager::Instance()->load("assets/healthBarBackground.png", "healthBarBackground", TheGame::Instance()->getRenderer());

	TheTextureManager::Instance()->load("assets/spiderBullet.png", "spiderBullet", TheGame::Instance()->getRenderer());
    TheTextureManager::Instance()->load("assets/healthBarStart.png", "healthBarStart", TheGame::Instance()->getRenderer());
	TheTextureManager::Instance()->load("assets/healthBarMiddle.png", "healthBarMiddle", TheGame::Instance()->getRenderer());
	TheTextureManager::Instance()->load("assets/healthBarEnd.png", "healthBarEnd", TheGame::Instance()->getRenderer());

	TheTextureManager::Instance()->load("assets/doorSprite.png", "doorSprite", TheGame::Instance()->getRenderer());

	TheTextureManager::Instance()->load("assets/bossHealthbarSegment.png", "bossHealthbar", TheGame::Instance()->getRenderer());
	TheTextureManager::Instance()->load("assets/bossHealthbarBackground.png", "bossHealthbarBackground", TheGame::Instance()->getRenderer());


    if(pLevel != 0)
    {
        m_loadingComplete = true;
    }
 
    std::cout << "entering PlayState\n";
    return true;
}

bool PlayState::onExit()
{
    m_exiting = true;
    
	//resetting the input handler and freeing memory
    TheInputHandler::Instance()->reset();
    TheBulletHandler::Instance()->clearBullets();
    
    std::cout << "exiting PlayState\n";
    return true;
}
