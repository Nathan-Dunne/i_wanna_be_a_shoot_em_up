#pragma once
//
//  ShotgunItem.h
//  SDL Game Programming Book
//
//  Created by shaun mitchell on 30/03/2013.
//  Copyright (c) 2013 shaun mitchell. All rights reserved.
//
//  Created by Eoghain McGrath

#ifndef __SDL_Game_Programming_Book__ShotgunItem__
#define __SDL_Game_Programming_Book__ShotgunItem__

#include <iostream>
#include "GameObjectFactory.h"
#include "BulletHandler.h"
#include "SoundManager.h"
#include <math.h>
#include "Pickup.h"

class ShotgunItem : public Pickup
{
public:

	ShotgunItem() : Pickup()
	{
		m_dyingTime = 25;
		m_health = 1;
		m_moveSpeed = 0;
		m_gap = 0;
	}

	virtual std::string type() { return "Shotgun"; }

	virtual ~ShotgunItem() {}

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

				// Adds the shotgun item to the players inventory
				TheGame::Instance()->addStringToInventory("Shotgun");
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
	

private:

	int m_maxHeight;
	int m_minHeight;
	int m_gap;
};

class ShotgunItemCreator : public BaseCreator
{
	GameObject* createGameObject() const
	{
		return new ShotgunItem();
	}
};

#endif /* __SDL_Game_Programming_Book__ShotgunItem__ */

