//
//  Glider.h
//  SDL Game Programming Book
//
//  Created by shaun mitchell on 30/03/2013.
//  Copyright (c) 2013 shaun mitchell. All rights reserved.
//

#ifndef __SDL_Game_Programming_Book__Newborn__
#define __SDL_Game_Programming_Book__Newborn__

#include "Enemy.h"
//#include "Game.h"

class Newborn : public Enemy
{
public:
    
	virtual ~Newborn() {}
    Newborn() : Enemy()
    {
		GAMEDEBUG_MSG("Newborn at : " << this << " spawned");
        m_dyingTime = 10;
        m_health = 10;
        m_moveSpeed = 20;
    }

	virtual std::string type() { return "Newborn"; }
    
    virtual void load(std::unique_ptr<LoaderParams> const &pParams)
    {
		// inherited load function
		ShooterObject::load(std::move(pParams));

		// can set up the players inherited values here

		// set up bullets
		m_Attributes.m_movementSpeed = TheDatabaseManager::Instance()->getAttribute(type(), "movementSpeed");
		m_Attributes.m_damage = TheDatabaseManager::Instance()->getAttribute(type(), "damage");

		// time it takes for death explosion
		m_dyingTime = 1;
    }
    
	virtual void collision()
	{
		m_health -= 1;

		if (m_health == 0)
		{
			if (!m_bPlayedDeathSound)
			{
				TheSoundManager::Instance()->playSound("explode", 0);

				//m_textureID = "largeexplosion";
				m_currentFrame = 0;
				m_numFrames = 9;
				m_width = 60;
				m_height = 60;
				m_bDying = true;
			}
		}
	}

	// Collision to occur when the entity is hit by a bullet
	virtual void bulletCollision(int damageAmt)
	{
		m_health -= damageAmt;

		if (m_health <= 0)
		{
			if (!m_bPlayedDeathSound)
			{
				TheSoundManager::Instance()->playSound("death", 0);

				m_textureID = "largeexplosion";
				m_currentFrame = 0;
				m_numFrames = 9;
				m_width = 60;
				m_height = 60;
				m_bDying = true;
				TheGame::Instance()->enemyDied();
			}
		}
	}
    
	virtual void update()
	{
		m_currentFrame = int(((SDL_GetTicks() / (1000 / 3)) % m_numFrames));

		if (!m_bDying)
		{
			if (m_bulletCounter == m_bulletFiringSpeed)
			{
				m_bulletCounter = 0;
			}

			m_bulletCounter++;
		}
		else
		{
			m_currentFrame = int(((SDL_GetTicks() / (1000 / 3)) % m_numFrames));

			if (m_dyingCounter == m_dyingTime)
			{
				m_bDead = true;
			}
			m_dyingCounter++;

		}

		m_velocity = TheGame::Instance()->targetPosition - m_position /= 100;

		if (m_velocity.getX() < 2 && m_velocity.getX() > 0)
		{
			m_velocity.setX(2);
		}
		else if (m_velocity.getX() > -2 && m_velocity.getX() < 0)
		{
			m_velocity.setX(-2);
		}

		if (m_velocity.getY() < 2 && m_velocity.getY() > 0)
		{
			m_velocity.setY(2);
		}
		else if (m_velocity.getY() > -2 && m_velocity.getY() < 0)
		{
			m_velocity.setY(-2);
		}

		handleMovement(m_velocity);
	}

private:

	bool m_entered;
};

class NewbornCreator : public BaseCreator
{
    GameObject* createGameObject() const
    {
        return new Newborn();
    }
};



#endif /* defined(__SDL_Game_Programming_Book__Newborn__) */
