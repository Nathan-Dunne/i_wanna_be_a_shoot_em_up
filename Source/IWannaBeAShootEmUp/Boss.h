//
//  Boss.h
//  SDL Game Programming Book
//
//  Created by shaun mitchell on 31/03/2013.
//  Copyright (c) 2013 shaun mitchell. All rights reserved.
//
//  Modified by Eoghain McGrath

#ifndef SDL_Game_Programming_Book_Boss_h
#define SDL_Game_Programming_Book_Boss_h

#include "Enemy.h"
#include "Game.h"

// TODO: Split this up into .h/.cpp
class Boss : public Enemy
{
public:
    
    virtual ~Boss() {}
    
    Boss() : Enemy()
    {
        m_health = 50; // TODO: From database.
        m_dyingTime = 100;
        m_bulletFiringSpeed = 50;
        
        m_moveSpeed = 2;
        
        m_entered = false;

		//m_angle = 90.0;
    }
    
    virtual void load(std::unique_ptr<LoaderParams> const &pParams)
    {
        ShooterObject::load(std::move(pParams));
		TheGame::Instance()->setBossAlive(true);

      //  m_velocity.setY(-m_moveSpeed);
    }
    
    virtual void collision()
    {
        if(m_entered)
        {
            m_health -= 1;
            
            if(m_health == 0)
            {
                if(!m_bPlayedDeathSound)
                {
                    m_position.setX(m_position.getX() + 30);
                    m_position.setY(m_position.getY() + 70);
                    TheSoundManager::Instance()->playSound("death", 0);
                    
                    m_textureID = "bossexplosion";
                    m_currentFrame = 0;
                    m_numFrames = 7;
                    m_width = 180;
                    m_height = 180;
                    m_bDying = true;
                } 
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
			}

		}
	}
    
    virtual void update()
    {
		TheGame::Instance()->setBossHealth(m_health);
        if(!m_entered)
        {
            scroll(TheGame::Instance()->getScrollSpeed());
            
            if(m_position.getX() < (TheGame::Instance()->getGameWidth() - (m_width + 20)))
            {
                m_entered = true;
            }
        }
        else
        {
            if(!m_bDying)
            {
                if(m_bulletCounter == m_bulletFiringSpeed)
                {
                    TheBulletHandler::Instance()->addEnemyBullet(m_position.getX(), m_position.getY() + 15, 33, 33, "bullet3", 1, Vector2D(-10, 0));
                    TheBulletHandler::Instance()->addEnemyBullet(m_position.getX(), m_position.getY() + 25, 33, 33, "bullet3", 1, Vector2D(-10, 0));
                   
					TheBulletHandler::Instance()->addEnemyBullet(m_position.getX() + 164, m_position.getY() + 15, 33, 33, "bullet3", 1, Vector2D(10, 0));
					TheBulletHandler::Instance()->addEnemyBullet(m_position.getX() + 164, m_position.getY() + 25, 33, 33, "bullet3", 1, Vector2D(10, 0));

					TheBulletHandler::Instance()->addEnemyBullet(m_position.getX(), m_position.getY() + 15, 33, 33, "bullet3", 1, Vector2D(-10, 10));
					TheBulletHandler::Instance()->addEnemyBullet(m_position.getX(), m_position.getY() + 25, 33, 33, "bullet3", 1, Vector2D(-10, 10));

					TheBulletHandler::Instance()->addEnemyBullet(m_position.getX() + 164, m_position.getY() + 15, 33, 33, "bullet3", 1, Vector2D(10, 10));
					TheBulletHandler::Instance()->addEnemyBullet(m_position.getX() + 164, m_position.getY() + 25, 33, 33, "bullet3", 1, Vector2D(10, 10));

					TheBulletHandler::Instance()->addEnemyBullet(m_position.getX(), m_position.getY() + 15, 33, 33, "bullet3", 1, Vector2D(-10, -10));
					TheBulletHandler::Instance()->addEnemyBullet(m_position.getX(), m_position.getY() + 25, 33, 33, "bullet3", 1, Vector2D(-10, -10));

					TheBulletHandler::Instance()->addEnemyBullet(m_position.getX() + 164, m_position.getY() + 15, 33, 33, "bullet3", 1, Vector2D(10, 10));
					TheBulletHandler::Instance()->addEnemyBullet(m_position.getX() + 164, m_position.getY() + 25, 33, 33, "bullet3", 1, Vector2D(10, -10));
                    m_bulletCounter = 0;
                }
                
                m_bulletCounter++;

				// If statements to regulate the speed at which the boss moves
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
            else
            {
                scroll(TheGame::Instance()->getScrollSpeed());
				m_currentFrame = int(((SDL_GetTicks() / (800 / 4)) % m_numFrames));
                
                if(m_dyingCounter == m_dyingTime)
                {
                    m_bDead = true;
					TheGame::Instance()->enemyDied();
					TheGame::Instance()->setBossAlive(false);
                }
                m_dyingCounter++;

            }

        }
		m_velocity = TheGame::Instance()->targetPosition - m_position /= 100;

		
    }
    
private:
    
    bool m_entered;
};

class BossCreator : public BaseCreator
{
    GameObject* createGameObject() const
    {
        return new Boss();
    }
};

#endif
