//
//  Player.h
//  SDL Game Programming Book
//
//  Created by shaun mitchell on 12/01/2013.
//  Copyright (c) 2013 shaun mitchell. All rights reserved.
//
// Edited by All Team Members

#ifndef __SDL_Game_Programming_Book__Player__
#define __SDL_Game_Programming_Book__Player__

#include <iostream>
#include <vector>
#include "ShooterObject.h"
#include "GameObjectFactory.h"
#include "PassiveItemTest.h"
class Player : public ShooterObject
{
public:
    
    Player();
    virtual ~Player() {}
    
    virtual void load(std::unique_ptr<LoaderParams> const &pParams);
    
    virtual void draw();
    virtual void update();
	void handleMovement(Vector2D velocity);
	bool checkCollideTile(Vector2D newPos);
    virtual void clean();

	void moveInput();  
    virtual void collision();
    
    virtual std::string type() { return "Player"; }

	Vector2D m_lastSafePos;
	int getMoveSpeed();

	void checkLevelComplete();
private:
    // bring the player back if there are lives left
    void ressurect();
    
    // handle any input from the keyboard, mouse, or joystick
    void handleInput();
    
    // handle any animation for the player
    void handleAnimation();

    // player can be invulnerable for a time
    int m_invulnerable;
    int m_invulnerableTime;
    int m_invulnerableCounter;

};

// for the factory
class PlayerCreator : public BaseCreator
{
    GameObject* createGameObject() const
    {
        return new Player();
    }
};


#endif /* defined(__SDL_Game_Programming_Book__Player__) */
