//
//  Player.cpp
//  SDL Game Programming Book
//
//  Created by shaun mitchell on 12/01/2013.
//  Copyright (c) 2013 shaun mitchell. All rights reserved.
//
//  Edited by All Team Members

#include "Player.h"
#include "Game.h"
#include "InputHandler.h"
#include "TileLayer.h"
#include "BulletHandler.h"
#include "SoundManager.h"
#include <algorithm>
#include "DatabaseManager.h"

using namespace std;

Player::Player() :  ShooterObject(),
m_invulnerable(false),
m_invulnerableTime(100),
m_invulnerableCounter(0)
{

}

void Player::collision()
{
    // if the player is not invulnerable then set to dying and change values for death animation tile sheet
    if(!m_invulnerable && !TheGame::Instance()->getLevelComplete())
    {
		//m_currentFrame = 0;
		//m_numFrames = 2;
		TheGame::Instance()->influenceDifficultyIndex(-1);
		TheSoundManager::Instance()->playSound("gotHitSound", 0);
        m_bDying = true;
		TheInputHandler::Instance()->callHaptic();
    }
}

void Player::load(std::unique_ptr<LoaderParams> const &pParams)
{
    // inherited load function
    ShooterObject::load(std::move(pParams));
    
    // can set up the players inherited values here
    
    // set up bullets
    m_Attributes.m_firingRate = TheDatabaseManager::Instance()->getAttribute(type(), "firingRate");
	m_Attributes.m_movementSpeed = TheDatabaseManager::Instance()->getAttribute(type(), "movementSpeed");
	m_Attributes.m_damage = TheDatabaseManager::Instance()->getAttribute(type(), "damage");
	m_Attributes.m_projectileSpeed = TheDatabaseManager::Instance()->getAttribute(type(), "projectileSpeed");

	TheGame::Instance()->setPlayerAttributes(m_Attributes);

    // we want to be able to fire instantly
    m_bulletCounter = m_Attributes.m_firingRate;
    
    // time it takes for death explosion
    m_dyingTime = 1;
}

void Player::draw()
{
    // player has no special drawing requirements
    ShooterObject::draw();
}

void Player::handleAnimation()
{
    // if the player is invulnerable we can flash its alpha to let people know
    if(m_invulnerable)
    {
        // invulnerability is finished, set values back
        if(m_invulnerableCounter == m_invulnerableTime)
        {
            m_invulnerable = false;
            m_invulnerableCounter = 0;
            m_alpha = 255;
        }
        else // otherwise, flash the alpha on and off
        {
            if(m_alpha == 255)
            {
                m_alpha = 0;
            }
            else
            {
                m_alpha = 255;
            }
        }
        
        // increment our counter
        m_invulnerableCounter++;
    }
    
    // our standard animation code 
	if (m_velocity.getY() < 0 || TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_W))
	{
		m_currentFrame = int(((SDL_GetTicks() / (200)) % 3)+ 3);
	}
	else if (m_velocity.getX() > 0 || TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_D))
	{
		m_currentFrame = int(((SDL_GetTicks() / (200)) % 3)+9);
	}
	else if (m_velocity.getX() < 0 || TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_A))
	{
		m_currentFrame = int(((SDL_GetTicks() / (200)) % 3)+6);
	}
	else 
	{
		m_currentFrame = int(((SDL_GetTicks() / (200)) % 3));
	}
    
}

void Player::update()
{	
	if (TheGame::Instance()->getIsPlayerUpdating())
	{
		//getting the players attributes from the database
		m_Attributes.m_firingRate = TheDatabaseManager::Instance()->getAttribute(type(), "firingRate");
		m_Attributes.m_movementSpeed = TheDatabaseManager::Instance()->getAttribute(type(), "movementSpeed");
		m_Attributes.m_damage = TheDatabaseManager::Instance()->getAttribute(type(), "damage");
		m_Attributes.m_projectileSpeed = TheDatabaseManager::Instance()->getAttribute(type(), "projectileSpeed");

		TheGame::Instance()->setPlayerUpdating(false);
		TheGame::Instance()->setPlayerAttributes(m_Attributes);
	} 

	if(!m_bDying)
    {
        // reset velocity
        m_velocity.setX(0);
        m_velocity.setY(0);
            
        // get input
        handleInput();
            
        // do normal position += velocity update
        //ShooterObject::update();
		handleMovement(m_velocity);

        // update the animation
        handleAnimation();
    }
    else // if the player is doing the death animation
    {
        m_currentFrame = int(((SDL_GetTicks() / (100)) % m_numFrames));
            
        // if the death animation has completed
        if(m_dyingCounter == m_dyingTime)
        {
            // ressurect the player
            ressurect();
        }
        m_dyingCounter++;
    }
 
	TheGame::Instance()->storeTargetPosition(m_position);
	checkLevelComplete();
}

void Player::handleMovement(Vector2D velocity)
{
	// get the current position
	Vector2D newPos = m_position;

	// add velocity to the x position
	newPos.m_x = m_position.m_x + velocity.m_x;

	// check if the new x position would collide with a tile
	if (!checkCollideTile(newPos))
	{
		// no collision, add to the actual x position
		m_position.m_x = newPos.m_x;
	}
	else
	{
		// collision, stop x movement
		m_velocity.m_x = 0;
	}

	// get the current position after x movement
	newPos = m_position;

	// add velocity to y position
	newPos.m_y += velocity.m_y;

	// check if new y position would collide with a tile
	if (!checkCollideTile(newPos))
	{
		// no collision, add to the actual x position
		m_position.m_y = newPos.m_y;
	}
	else
	{
		// collision, stop y movement
		m_velocity.m_y = 0;
	}
}

bool Player::checkCollideTile(Vector2D newPos)
{
	// for each collision layer on map
	for (std::vector<TileLayer*>::iterator it = m_pCollisionLayers->begin(); it != m_pCollisionLayers->end(); ++it)
	{
		// get the tilelayer
		TileLayer* pTileLayer = (*it);

		// get each tileID in the n*n grid, tildIDs != 0 are collidable tiles
		std::vector<std::vector<int>> tiles = pTileLayer->getTileIDs();

		// get up the grid of tiles we want to look at and something to store their state
		int tileColumn, tileRow, tileid = 0;

		// the top left position of a rectangle checking this collision
		Vector2D startPos = newPos;

		// the bottom right position of a rectangle checking this collision
		Vector2D endPos(newPos.m_x + (m_width), (newPos.m_y) + m_height);

		bool hasCollided = false;

		// for each pixel, check if that pixel overlaps a tile with a tileID != 0
		for (int i = startPos.m_x; i < endPos.m_x; i++)
		{
			for (int j = startPos.m_y; j < endPos.m_y; j++)
			{
				tileColumn = i / pTileLayer->getTileSize();
				tileRow = j / pTileLayer->getTileSize();

				tileid = tiles[tileRow][tileColumn];

				if (tileid != 0 && tileid != 3)
				{
					// we have collided
					return true;
				}

				if (tileid == 3 && !TheGame::Instance()->getLevelComplete() && !hasCollided)
				{

					//hasCollided = true;

					TheGame::Instance()->setCurrentLevel(TheGame::Instance()->getCurrentLevel() + 1);

					return true;
				}
			}
		}
	}

	// there has been no collision
	return false;
}

int Player::getMoveSpeed()
{
	return m_Attributes.m_movementSpeed;
}

// unused in favor of checking level completion by colliding with open doors
void Player::checkLevelComplete()
{
	if ((m_position.m_y <= 0 && m_position.m_y >= -10) && (TheGame::Instance()->getLevelEnemyCount() == 0))
	{
		TheGame::Instance()->setLevelComplete(true);
	}
}


void Player::ressurect()
{
   TheGame::Instance()->addToScore(-100);
   TheGame::Instance()->setPlayerLives(TheGame::Instance()->getPlayerLives() - 1);
   
   m_bDying = false; 
   m_dyingCounter = 0;
   m_invulnerable = true;
}

void Player::clean()
{
    ShooterObject::clean();
}

void Player::moveInput()
{
	SDL_Scancode UP = SDL_SCANCODE_W;
	SDL_Scancode DOWN = SDL_SCANCODE_S;
	SDL_Scancode LEFT = SDL_SCANCODE_A;
	SDL_Scancode RIGHT = SDL_SCANCODE_D;

	if (!m_bDead)
	{
		// handle joysticks 
		if (TheInputHandler::Instance()->joysticksInitialised())
		{
			m_velocity.setX(TheInputHandler::Instance()->getDirection(m_Attributes.m_movementSpeed).first);
			m_velocity.setY(TheInputHandler::Instance()->getDirection(m_Attributes.m_movementSpeed).second);
		}

		// Right handed controls
		if (TheGame::Instance()->getControls())
		{
			UP = SDL_SCANCODE_UP;
			DOWN = SDL_SCANCODE_DOWN;
			LEFT = SDL_SCANCODE_LEFT;
			RIGHT = SDL_SCANCODE_RIGHT;
		}

		// handle keys
		if (TheInputHandler::Instance()->isKeyDown(UP) && m_position.getY() > 0)
		{
			m_velocity.setY(-m_Attributes.m_movementSpeed);
		}
		else if (TheInputHandler::Instance()->isKeyDown(DOWN) && (m_position.getY() + m_height) < TheGame::Instance()->getGameHeight() - 10)
		{
			m_velocity.setY(m_Attributes.m_movementSpeed);
		}

		if (TheInputHandler::Instance()->isKeyDown(LEFT) && m_position.getX() > 0)
		{
			m_velocity.setX(-m_Attributes.m_movementSpeed);
		}
		else if (TheInputHandler::Instance()->isKeyDown(RIGHT) && (m_position.getX() + m_width) < TheGame::Instance()->getGameWidth())
		{
			m_velocity.setX(m_Attributes.m_movementSpeed);
		}
	}
}

void Player::handleInput()
{
	moveInput();

	//// Sets the players firing rate to 40 if they have the shotgun item - Simulates shotgun firerate
	//if (TheGame::Instance()->checkInventory("Shotgun"))
	//{
	//	m_Attributes.m_firingRate = 10;
	//}

    if(!m_bDead)
    {
		// Right handed controls
		if (TheGame::Instance()->getControls() == false)
		{
			// shooting right
			if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT) || TheInputHandler::Instance()->isButtonDown(SDL_CONTROLLER_BUTTON_B))
			{
				if (m_bulletCounter == m_Attributes.m_firingRate)
				{
					// Checks if the players inventory includes a shotgun
					if (TheGame::Instance()->checkInventory("Shotgun"))
					{
						TheSoundManager::Instance()->playSound("shoot", 0); // Change this to be a different sound for Gamesfleadh
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() + 64, m_position.getY() + 10, 30, 30, "bullet1", 1, Vector2D(m_Attributes.m_projectileSpeed, -10), m_Attributes.m_damage);
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() + 64, m_position.getY() + 10, 30, 30, "bullet1", 1, Vector2D(m_Attributes.m_projectileSpeed, 10), m_Attributes.m_damage);
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() + 64, m_position.getY() + 10, 30, 30, "bullet1", 1, Vector2D(m_Attributes.m_projectileSpeed, 0), m_Attributes.m_damage);
					}
					// Else fire shots normally
					else
					{
						TheSoundManager::Instance()->playSound("shoot", 0);
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() , m_position.getY() + 10, 30, 30, "bullet1", 1, Vector2D(m_Attributes.m_projectileSpeed, 0), m_Attributes.m_damage);
					}
					m_bulletCounter = 0;// Bullet counter being set to zero is here to allow for continous firing
				}
				m_bulletCounter++;
			}
			// shooting up
			else if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP) || TheInputHandler::Instance()->isButtonDown(SDL_CONTROLLER_BUTTON_Y))
			{
				if (m_bulletCounter == m_Attributes.m_firingRate)
				{
					// Checks if the players inventory includes a shotgun
					if (TheGame::Instance()->checkInventory("Shotgun"))
					{
						TheSoundManager::Instance()->playSound("shoot", 0); // Change this to be a different sound
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() + 10, m_position.getY() - 20, 30, 30, "bullet1", 1, Vector2D(-10, -m_Attributes.m_projectileSpeed), m_Attributes.m_damage);
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() + 10, m_position.getY() - 20, 30, 30, "bullet1", 1, Vector2D(10, -m_Attributes.m_projectileSpeed), m_Attributes.m_damage);
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() + 10, m_position.getY() - 20, 30, 30, "bullet1", 1, Vector2D(0, -m_Attributes.m_projectileSpeed), m_Attributes.m_damage);
					}
					// Else fire shots normally
					else
					{
						TheSoundManager::Instance()->playSound("shoot", 0);
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX(), m_position.getY() - 20, 30, 30, "bullet1", 1, Vector2D(0, -m_Attributes.m_projectileSpeed), m_Attributes.m_damage);
					}
					m_bulletCounter = 0;
				}
				m_bulletCounter++;
			}
			// shooting left
			else if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT) || TheInputHandler::Instance()->isButtonDown(SDL_CONTROLLER_BUTTON_X))
			{
				if (m_bulletCounter == m_Attributes.m_firingRate)
				{
					// Checks if the players inventory includes a shotgun
					if (TheGame::Instance()->checkInventory("Shotgun"))
					{
						TheSoundManager::Instance()->playSound("shoot", 0); // Change this to be a different sound
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() - 10, m_position.getY() + 10, 30, 30, "bullet1", 1, Vector2D(-m_Attributes.m_projectileSpeed, -10), m_Attributes.m_damage);
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() - 10, m_position.getY() + 10, 30, 30, "bullet1", 1, Vector2D(-m_Attributes.m_projectileSpeed, 10), m_Attributes.m_damage);
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() - 10, m_position.getY() + 10, 30, 30, "bullet1", 1, Vector2D(-m_Attributes.m_projectileSpeed, 0), m_Attributes.m_damage);
					}
					// Else fire shots normally
					else
					{
						TheSoundManager::Instance()->playSound("shoot", 0);
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() - 10, m_position.getY() + 10, 30, 30, "bullet1", 1, Vector2D(-m_Attributes.m_projectileSpeed, 0), m_Attributes.m_damage);
					}
					m_bulletCounter = 0;
				}
				m_bulletCounter++;
			}
			// shooting down
			else if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN) || TheInputHandler::Instance()->isButtonDown(SDL_CONTROLLER_BUTTON_A))
			{
				if (m_bulletCounter == m_Attributes.m_firingRate)
				{
					// Checks if the players inventory includes a shotgun
					if (TheGame::Instance()->checkInventory("Shotgun"))
					{
						TheSoundManager::Instance()->playSound("shoot", 0); // Change this to be a different sound
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() + 10, m_position.getY() + 20, 30, 30, "bullet1", 1, Vector2D(-10, m_Attributes.m_projectileSpeed), m_Attributes.m_damage);
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() + 10, m_position.getY() + 20, 30, 30, "bullet1", 1, Vector2D(10, m_Attributes.m_projectileSpeed), m_Attributes.m_damage);
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() + 10, m_position.getY() + 20, 30, 30, "bullet1", 1, Vector2D(0, m_Attributes.m_projectileSpeed), m_Attributes.m_damage);
					}
					// Else fire shots normally
					else
					{
						TheSoundManager::Instance()->playSound("shoot", 0);
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() + 10, m_position.getY() + 20, 30, 30, "bullet1", 1, Vector2D(0, m_Attributes.m_projectileSpeed), m_Attributes.m_damage);
					}
					m_bulletCounter = 0;
				}
				m_bulletCounter++;
			}
			else
			{
				// Checks the firing rate to handle a bug we had - Sets the firing rate to four so it never reaches zero and breaks shooting
				if (m_Attributes.m_firingRate <= 4)
				{
					m_Attributes.m_firingRate = 4;
				}

				m_bulletCounter = m_Attributes.m_firingRate;
			}


		}

		if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_F10))
		{
			TheGame::Instance()->setLevelEnemyCount(0);
		}

		// Left handed controls
		if (TheGame::Instance()->getControls() == true)
		{
			// handle keys
			if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP) && m_position.getY() > 0)
			{
				m_velocity.setY(-m_Attributes.m_movementSpeed);
			}
			else if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN) && (m_position.getY() + m_height) < TheGame::Instance()->getGameHeight() - 10)
			{
				m_velocity.setY(m_Attributes.m_movementSpeed);
			}

			if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT) && m_position.getX() > 0)
			{
				m_velocity.setX(-m_Attributes.m_movementSpeed);
			}
			else if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT) && (m_position.getX() + m_width) < TheGame::Instance()->getGameWidth())
			{
				m_velocity.setX(m_Attributes.m_movementSpeed);
			}

			// shooting right
			if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_D))
			{
				if (m_bulletCounter == m_Attributes.m_firingRate)
				{
					// Checks if the players inventory includes a shotgun
					if (TheGame::Instance()->checkInventory("Shotgun"))
					{
						TheSoundManager::Instance()->playSound("shoot", 0); // Change this to be a different sound
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() + 64, m_position.getY() + 10, 30, 30, "bullet1", 1, Vector2D(m_Attributes.m_projectileSpeed, -10), m_Attributes.m_damage);
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() + 64, m_position.getY() + 10, 30, 30, "bullet1", 1, Vector2D(m_Attributes.m_projectileSpeed, 10), m_Attributes.m_damage);
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() + 64, m_position.getY() + 10, 30, 30, "bullet1", 1, Vector2D(m_Attributes.m_projectileSpeed, 0), m_Attributes.m_damage);
					}
					// Else fire shots normally
					else
					{
						TheSoundManager::Instance()->playSound("shoot", 0);
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() + 64, m_position.getY() + 10, 30, 30, "bullet1", 1, Vector2D(m_Attributes.m_projectileSpeed, 0), m_Attributes.m_damage);
					}
					m_bulletCounter = 0;
				}
				m_bulletCounter++;
			}
			// shooting up
			else if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_W))
			{
				if (m_bulletCounter == m_Attributes.m_firingRate)
				{
					// Checks if the players inventory includes a shotgun
					if (TheGame::Instance()->checkInventory("Shotgun"))
					{
						TheSoundManager::Instance()->playSound("shoot", 0); // Change this to be a different sound
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() + 10, m_position.getY() - 20, 30, 30, "bullet1", 1, Vector2D(-10, -m_Attributes.m_projectileSpeed), m_Attributes.m_damage);
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() + 10, m_position.getY() - 20, 30, 30, "bullet1", 1, Vector2D(10, -m_Attributes.m_projectileSpeed), m_Attributes.m_damage);
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() + 10, m_position.getY() - 20, 30, 30, "bullet1", 1, Vector2D(0, -m_Attributes.m_projectileSpeed), m_Attributes.m_damage);
					}
					// Else fire shots normally
					else
					{
						TheSoundManager::Instance()->playSound("shoot", 0);
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() , m_position.getY() - 20, 30, 30, "bullet1", 1, Vector2D(0, -m_Attributes.m_projectileSpeed), m_Attributes.m_damage);
					}
					m_bulletCounter = 0;
				}
				m_bulletCounter++;
			}
			// shooting left
			else if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_A))
			{
				if (m_bulletCounter == m_Attributes.m_firingRate)
				{
					// Checks if the players inventory includes a shotgun
					if (TheGame::Instance()->checkInventory("Shotgun"))
					{
						TheSoundManager::Instance()->playSound("shoot", 0); // Change this to be a different sound
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() - 10, m_position.getY() + 10, 30, 30, "bullet1", 1, Vector2D(-m_Attributes.m_projectileSpeed, -10), m_Attributes.m_damage);
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() - 10, m_position.getY() + 10, 30, 30, "bullet1", 1, Vector2D(-m_Attributes.m_projectileSpeed, 10), m_Attributes.m_damage);
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() - 10, m_position.getY() + 10, 30, 30, "bullet1", 1, Vector2D(-m_Attributes.m_projectileSpeed, 0), m_Attributes.m_damage);
					}
					// Else fire shots normally
					else
					{
						TheSoundManager::Instance()->playSound("shoot", 0);
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() - 10, m_position.getY() + 10, 30, 30, "bullet1", 1, Vector2D(-m_Attributes.m_projectileSpeed, 0), m_Attributes.m_damage);
					}
					m_bulletCounter = 0;
				}
				m_bulletCounter++;
			}
			// shooting down
			else if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_S))
			{
				if (m_bulletCounter == m_Attributes.m_firingRate)
				{
					// Checks if the players inventory includes a shotgun
					if (TheGame::Instance()->checkInventory("Shotgun"))
					{
						TheSoundManager::Instance()->playSound("shoot", 0); // Change this to be a different sound
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() + 10, m_position.getY() + 20, 30, 30, "bullet1", 1, Vector2D(-10, m_Attributes.m_projectileSpeed), m_Attributes.m_damage);
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() + 10, m_position.getY() + 20, 30, 30, "bullet1", 1, Vector2D(10, m_Attributes.m_projectileSpeed), m_Attributes.m_damage);
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() + 10, m_position.getY() + 20, 30, 30, "bullet1", 1, Vector2D(0, m_Attributes.m_projectileSpeed), m_Attributes.m_damage);
					}
					// Else fire shots normally
					else
					{
						TheSoundManager::Instance()->playSound("shoot", 0);
						TheBulletHandler::Instance()->addPlayerBullet(m_position.getX() + 10, m_position.getY() + 20, 30, 30, "bullet1", 1, Vector2D(0, m_Attributes.m_projectileSpeed), m_Attributes.m_damage);
					}
					m_bulletCounter = 0;
				}
				m_bulletCounter++;
			}

			else
			{
				if (m_Attributes.m_firingRate <= 4)
				{
					m_Attributes.m_firingRate = 4;
				}

				m_bulletCounter = m_Attributes.m_firingRate;
			}

			// handle joysticks 
			if (TheInputHandler::Instance()->joysticksInitialised())
			{
				if ((TheInputHandler::Instance()->getAxisX(0, 1) > 0 && (m_position.getX() + m_width) < TheGame::Instance()->getGameWidth()) || (TheInputHandler::Instance()->getAxisX(0, 1) < 0 && m_position.getX() > 0))
				{
					m_velocity.setX(m_Attributes.m_movementSpeed * TheInputHandler::Instance()->getAxisX(0, 1));
				}

				if ((TheInputHandler::Instance()->getAxisY(0, 1) > 0 && (m_position.getY() + m_height) < TheGame::Instance()->getGameHeight() - 10) || (TheInputHandler::Instance()->getAxisY(0, 1) < 0 && m_position.getY() > 0))
				{
					m_velocity.setY(m_Attributes.m_movementSpeed * TheInputHandler::Instance()->getAxisY(0, 1));
				}
			}
		}

    }
}