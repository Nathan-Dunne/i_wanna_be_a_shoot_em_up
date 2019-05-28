//
//  Assassin.h
//  SDL Game Programming Book
//
//  Created by shaun mitchell on 29/03/2013.
//  Copyright (c) 2013 shaun mitchell. All rights reserved.
//

#ifndef __SDL_Game_Programming_Book__Assassin__
#define __SDL_Game_Programming_Book__Assassin__

#include <iostream>
#include "GameObjectFactory.h"
#include "BulletHandler.h"
#include "SoundManager.h"
#include <math.h>
#include "Enemy.h"
#include "Vector2D.h"

class Assassin : public Enemy
{
public:

	Assassin()
	{
		GAMEDEBUG_MSG("Assassin at : " << this << " spawned");
		m_dyingTime = 66;
		m_bulletFiringSpeed = 100;
	}

	virtual std::string type() { return "Assassin"; }

	virtual ~Assassin() {}

	Vector2D moveToPlayer(Vector2D getPlayerPosition(Player* pPlayer));

	virtual void load(std::unique_ptr<LoaderParams> const &pParams)
	{
		// inherited load function
		ShooterObject::load(std::move(pParams));

		// can set up the players inherited values here
		std::cout << type() << std::endl;
		// set up bullets
		m_Attributes.m_firingRate = TheDatabaseManager::Instance()->getAttribute(type(), "firingRate");
		m_Attributes.m_movementSpeed = TheDatabaseManager::Instance()->getAttribute(type(), "movementSpeed");
		m_Attributes.m_damage = TheDatabaseManager::Instance()->getAttribute(type(), "damage");
		m_Attributes.m_projectileSpeed = TheDatabaseManager::Instance()->getAttribute(type(), "projectileSpeed");
		m_health = TheDatabaseManager::Instance()->getAttribute(type(), "health");

		// we want to be able to fire instantly
		m_bulletCounter = m_Attributes.m_firingRate;

		// time it takes for death explosion
		m_dyingTime = 66;

		if (rand() % 2 == 1)
		{
			m_velocity.m_y = -1;
		}
		else
		{
			m_velocity.m_x = -1;
		}
	}

	virtual void collision()
	{
		m_health -= 1;

		if (m_health == 0)
		{
			if (!m_bPlayedDeathSound)
			{
				TheSoundManager::Instance()->playSound("death", 0);

				//m_textureID = "largeexplosion";

				m_currentFrame = 0;
				m_numFrames = 2;
				m_width = 60;
				m_height = 60;
				m_bDying = true;
				TheGame::Instance()->enemyDied();
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
				m_bDying = true;
				TheGame::Instance()->enemyDied();
			}
		}
	}

	virtual void update()
	{
		if (!m_bDying)
		{

			m_currentFrame = int(((SDL_GetTicks() / (800 / 4)) % m_numFrames));
			// we want to scroll this object with the rest
			scroll(TheGame::Instance()->getScrollSpeed());

			if (m_bulletCounter == m_Attributes.m_firingRate)
			{
				if (TheGame::Instance()->getDifficultyIndex() > 8)
				{
					TheBulletHandler::Instance()->addEnemyBullet(m_position.getX() + 30, m_position.getY() + 70, 16, 16, "bullet2", 1, (TheGame::Instance()->targetPosition - m_position /= 100));
					TheBulletHandler::Instance()->addEnemyBullet(m_position.getX() + 50, m_position.getY() + 70, 16, 16, "bullet2", 1, (TheGame::Instance()->targetPosition - m_position /= 100));
					TheBulletHandler::Instance()->addEnemyBullet(m_position.getX() + 70, m_position.getY() + 70, 16, 16, "bullet2", 1, (TheGame::Instance()->targetPosition - m_position /= 100));
				}
				else if (TheGame::Instance()->getDifficultyIndex() > 4)
				{
					TheBulletHandler::Instance()->addEnemyBullet(m_position.getX() + 30, m_position.getY() + 70, 16, 16, "bullet2", 1, (TheGame::Instance()->targetPosition - m_position /= 100));
					TheBulletHandler::Instance()->addEnemyBullet(m_position.getX() + 50, m_position.getY() + 70, 16, 16, "bullet2", 1, (TheGame::Instance()->targetPosition - m_position /= 100));
				}
				else
				{
					TheBulletHandler::Instance()->addEnemyBullet(m_position.getX() + 50, m_position.getY() + 70, 16, 16, "bullet2", 1, (TheGame::Instance()->targetPosition - m_position /= 100));
				}

				m_bulletCounter = 0;
			}

			m_bulletCounter++;

			handleMovement(m_velocity);
		}
		else
		{

			doDyingAnimation();

			if (m_alpha == 255)
			{
				m_alpha = 0;
			}
			else
			{
				m_alpha = 255;
			}

			m_dyingCounter++;
		}
	}
};

class AssassinCreator : public BaseCreator
{
	GameObject* createGameObject() const
	{
		return new Assassin();
	}
};

Vector2D moveToPlayer(Vector2D getPlayerPosition(Player* pPlayer))
{
	return Vector2D();
}


#endif /* defined(__SDL_Game_Programming_Book__Assassin__) */
