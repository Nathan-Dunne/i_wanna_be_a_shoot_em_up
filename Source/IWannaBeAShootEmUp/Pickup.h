//
//  Pickup.h
//  SDL Game Programming Book
//
//  created by Liam Noonan in class
// Edited by Luke O Brien

#ifndef Pickup_h
#define Pickup_h

#include <iostream>
#include "GameObjectFactory.h"
#include "BulletHandler.h"
#include "SoundManager.h"
#include <math.h>
#include "Enemy.h"


// because of the framework we were given, It makes more sense for the item pickups to inherit
// from enemy so that they have collision and the various functionality needed.
class Pickup : public Enemy
{
public:
    
    Pickup() : Enemy()
    {
        m_dyingTime = 25;
        m_health = 1;
        m_moveSpeed = 0;	//we dont want the items to move
        m_gap = 0;
    }
    
    virtual ~Pickup() {}
    
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

        if(m_health == 0)
        {
            if(!m_bPlayedDeathSound)
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
        if(!m_bDying)
        {
            if(m_position.getY() >= m_maxHeight)
            {
                m_velocity.setY(-m_moveSpeed);
            }
            else if(m_position.getY() <= m_minHeight)
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
	//virtual function we added so that we could inherit and easily code more items while only changing this function
	virtual void applyItemEffect() {}
	virtual std::string type() { return "Pickup"; }
    
private:
    
    int m_maxHeight;
    int m_minHeight;
    int m_gap;
};

class PickupCreator : public BaseCreator
{
    GameObject* createGameObject() const
    {
        return new Pickup();
    }
};



#endif
