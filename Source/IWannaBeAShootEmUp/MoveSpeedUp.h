#pragma once
//
//  Pickup.h
//  SDL Game Programming Book
//
//  Created by shaun mitchell on 30/03/2013.
//  Copyright (c) 2013 shaun mitchell. All rights reserved.
//

#ifndef __SDL_Game_Programming_Book__MoveSpeedUp__
#define __SDL_Game_Programming_Book__MoveSpeedUp__

#include <iostream>
#include "GameObjectFactory.h"
#include "BulletHandler.h"
#include "SoundManager.h"
#include <math.h>
#include "Pickup.h"

class MoveSpeedUp : public Pickup
{
public:

	MoveSpeedUp() : Pickup()
	{
		m_dyingTime = 25;
		m_health = 1;
		m_moveSpeed = 0;
		m_gap = 0;
	}

	virtual std::string type() { return "Move and Projectile Speed"; }

	virtual ~MoveSpeedUp() {}

	virtual void load(std::unique_ptr<LoaderParams> const &pParams)
	{
		ShooterObject::load(std::move(pParams));

		m_velocity.setX(-m_moveSpeed);
		m_velocity.setY(m_moveSpeed / 2);

		m_maxHeight = m_position.getY() + m_gap;
		m_minHeight = m_position.getY() - m_gap;
	}

	virtual void collision()
	{
		m_health -= 1;
		Game::Instance()->addToScore(50.00);
		if (m_health == 0)
		{
			if (!m_bPlayedDeathSound)
			{
				TheSoundManager::Instance()->playSound("pickup", 0);

				m_textureID = "reward";
				m_currentFrame = 0;
				m_numFrames = 1;
				m_width = 64;
				m_height = 64;
				m_bDying = true;
				applyItemEffect();
			}
		}
	}

	virtual void update()
	{
		if (!m_bDying)
		{
			if (m_position.getY() >= m_maxHeight)
			{
				m_velocity.setY(-m_moveSpeed);
			}
			else if (m_position.getY() <= m_minHeight)
			{
				m_velocity.setY(m_moveSpeed);
			}
		}
		else
		{
			m_velocity.setX(0);
			m_velocity.setY(0);
			doDyingAnimation();
		}

		ShooterObject::update();

	}
	virtual void applyItemEffect()
	{
		int moveSpeedToChange = 1;
		TheDatabaseManager::Instance()->changeAttribute("PLAYER", "movementSpeed", moveSpeedToChange);

		int projectileSpeedToChange = 5;
		TheDatabaseManager::Instance()->changeAttribute("PLAYER", "projectileSpeed", projectileSpeedToChange);

		std::cout << "Movement and Projectile Speed Up recieved" << std::endl;

		TheGame::Instance()->setPlayerUpdating(true);
	}

private:

	int m_maxHeight;
	int m_minHeight;
	int m_gap;
};

class MoveSpeedUpCreator : public BaseCreator
{
	GameObject* createGameObject() const
	{
		return new MoveSpeedUp();
	}
};



#endif /* defined(__SDL_Game_Programming_Book__Glider__) */

