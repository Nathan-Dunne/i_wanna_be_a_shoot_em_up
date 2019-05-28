//
//  PauseState.h
//  SDL Game Programming Book
//
//  Created by shaun mitchell on 16/02/2013.
//  Copyright (c) 2013 shaun mitchell. All rights reserved.
//  Modified by Eoghain McGrath

#ifndef __SDL_Game_Programming_Book__PauseState__
#define __SDL_Game_Programming_Book__PauseState__

#include <iostream>
#include <vector>
#include "MenuState.h"

class GameObject;

class PauseState : public MenuState
{
public:
    
    virtual ~PauseState() {}
    
    virtual void update();
    virtual void render();
    
    virtual bool onEnter();
    virtual bool onExit();
    
    virtual std::string getStateID() const { return s_pauseID; }
    
    virtual void setCallbacks(const std::vector<Callback>& callbacks);

	static void s_leftHandedControls();
	static void s_rightHandedControls();

    
private:
    
    static void s_pauseToMain();
    static void s_resumePlay();

	bool s_whichControls = false; // Boolean to check which controls are selected - Right handed (false) by default.

    static const std::string s_pauseID;
    
    std::vector<GameObject*> m_gameObjects;
};


#endif /* defined(__SDL_Game_Programming_Book__PauseState__) */
