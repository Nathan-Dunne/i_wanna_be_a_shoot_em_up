//
//  Uno.h
//  SDL Game Programming Book
//
//  Created by shaun mitchell on 30/03/2013.
//  Copyright (c) 2013 shaun mitchell. All rights reserved.
//

#ifndef Uno_h
#define Uno_h

#include "Enemy.h"

class Uno : public Enemy
{
public:
    
    virtual ~Uno() {}
	int m_movementTimer;
    Uno() : Enemy()
    {
		GAMEDEBUG_MSG("Uno at: " << this << " spawned");
        m_dyingTime = 50;
        m_health = 3;
        m_moveSpeed = 6;
        m_bulletFiringSpeed = 200;
    }
    
	virtual std::string type() { return "Uno"; }


	virtual void load(std::unique_ptr<LoaderParams> const &pParams)
	{
		// inherited load function
		ShooterObject::load(std::move(pParams));

		// can set up the players inherited values here

		// set up bullets
		m_Attributes.m_firingRate = TheDatabaseManager::Instance()->getAttribute(type(), "firingRate");
		m_Attributes.m_movementSpeed = TheDatabaseManager::Instance()->getAttribute(type(), "movementSpeed");
		m_Attributes.m_damage = TheDatabaseManager::Instance()->getAttribute(type(), "damage");
		m_Attributes.m_projectileSpeed = TheDatabaseManager::Instance()->getAttribute(type(), "projectileSpeed");

		// we want to be able to fire instantly
		m_bulletCounter = m_Attributes.m_firingRate;

		// time it takes for death explosion
		m_dyingTime = 1;

		m_movementTimer = rand() % 120;

	}

    virtual void collision()
    {
        m_health -= 1;
        
        if(m_health == 0)
        {
            if(!m_bPlayedDeathSound)
            {
                TheSoundManager::Instance()->playSound("explode", 0);
                
				// handles the death animation
               // m_textureID = "largeexplosion";
                m_currentFrame = 0;
                m_numFrames = 9;
                m_width = 60;
                m_height = 60;
                m_bDying = true;
            }
            
        }
    }

	// handles what happens when hit by a player bullet
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
		if (!m_bDying)
		{
			m_currentFrame = int(((SDL_GetTicks() / (200)) % 2));	// sets the speed that it loops through its spritesheet

			// we want to scroll this object with the rest
			scroll(TheGame::Instance()->getScrollSpeed());

			m_movementTimer++;	// incrementing a  timer for its movement

			if (m_bulletCounter == m_Attributes.m_firingRate)
			{
				m_bulletCounter = 0;
			}

			m_bulletCounter++;
			//when the movement timer is equal to 50, the enemy will fire a shot
			if (m_movementTimer == 50)
			{
				TheBulletHandler::Instance()->addEnemyBullet(m_position.getX()+10, m_position.getY()+5, 32, 32, "spiderBullet", 1, (TheGame::Instance()->targetPosition - m_position /= 100));
			}
			// when the movement timer is equal to 100, the enemy will call its movement function
			if (m_movementTimer == 100)
			{
				EnemyMovementAI();
			}
			// when the movement timer is equal to 125, the enemy will stop moving. this allows short, erratic bursts of movement
			if (m_movementTimer == 125)
			{
				m_velocity.setX(0);
				m_velocity.setY(0);
				m_movementTimer = 0;
			}

			handleMovement(m_velocity);
		}
		else
		{
			scroll(TheGame::Instance()->getScrollSpeed());
			doDyingAnimation();
		}
        
        //ShooterObject::update();
    }

	// This functions job is to generate a random direction that the enemy will move in
	void EnemyMovementAI()
	{
		float randomX = rand() % 21 + (-10);	//gets a random X axis heading
		float randomY = rand() % 21 + (-10);	//gets a random Y axis heading

		float percentageX = 0;	
		float percentageY = 0;
		float sum = 0;

		if (randomX >= 0 && randomY >= 0)  // if both X and Y values are positive
		{ 
			sum = randomX + randomY + 0.0000001;	//get the sum of the X and Y
			percentageX = randomX / sum;	//get the percentage of the sum that is X
			percentageY = randomY / sum;	//get the percentage of the sum that is Y
			m_velocity.setX((m_moveSpeed * 2)*percentageX);		// map the percentages to the movespeed and set the X velocity. this gives us a constant movement speed
			m_velocity.setY((m_moveSpeed * 2)*percentageY);		// map the percentages to the movespeed and set the Y velocity.
			
		}
		else if (randomX < 0 && randomY >= 0)	//if X is negitive
		{
			randomX *= -1;	//multiply X by -1 to make it positive so that my algorithm works
			sum = randomX + randomY;	//get the sum of the X and Y
			percentageX = randomX / sum;	//get the percentage of the sum that is X
			percentageY = randomY / sum;	//get the percentage of the sum that is Y
			m_velocity.setX(((m_moveSpeed * 2)*percentageX) * -1.0);	// map the percentages to the movespeed and set the X velocity. multiply by -1 again to reset the X axis
			m_velocity.setY((m_moveSpeed * 2)*percentageY);		// map the percentages to the movespeed and set the Y velocity.
		}
		else if (randomY < 0 && randomX >= 0) 
		{ 
			randomY *= -1;	//multiply Y by -1 to make it positive
			sum = randomX + randomY;	//get the sum of the X and Y
			percentageX = randomX / sum;	//get the percentage of the sum that is X
			percentageY = randomY / sum;	//get the percentage of the sum that is Y
			m_velocity.setX((m_moveSpeed * 2)*percentageX);		// map the percentages to the movespeed and set the X velocity.
			m_velocity.setY(((m_moveSpeed * 2)*percentageY) * -1.0);	// map the percentages to the movespeed and set the Y velocity. multiply by -1 again to reset the Y axis
		}
		else
		{
			randomX *= -1;	//multiply X by -1 to make it positive
			randomY *= -1;	//multiply Y by -1 to make it positive
			sum = randomX + randomY;	//get the sum of the X and Y
			percentageX = randomX / sum;	//get the percentage of the sum that is X
			percentageY = randomY / sum;	//get the percentage of the sum that is Y
			m_velocity.setX(((m_moveSpeed * 2)*percentageX) * -1.0);	// map the percentages to the movespeed and set the X velocity. multiply by -1 again to reset the X axis
			m_velocity.setY(((m_moveSpeed * 2)*percentageY) * -1.0);	// map the percentages to the movespeed and set the Y velocity. multiply by -1 again to reset the Y axis
		}
	}
};

class UnoCreator : public BaseCreator
{
    GameObject* createGameObject() const
    {
        return new Uno();
    }
};


#endif
