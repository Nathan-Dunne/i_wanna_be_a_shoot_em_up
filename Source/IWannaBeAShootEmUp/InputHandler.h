//
//  InputHandler.h
//  SDL Game Programming Book
//
//  Created by shaun mitchell on 24/01/2013.
//  Copyright (c) 2013 shaun mitchell. All rights reserved.
//
// Edited by Nathan Dunne and Luke O Brien
#ifndef __SDL_Game_Programming_Book__InputHandler__
#define __SDL_Game_Programming_Book__InputHandler__

#include <iostream>
#include <vector>

#include "SDL.h"
#include "Vector2D.h"
#include "DatabaseManager.h"
enum mouse_buttons
{
    LEFT = 0,
    MIDDLE = 1,
    RIGHT = 2
};

enum joyStickDirection	//enum for the quadrent of the joystick
{
	DOWNRIGHT = 0,
	UPRIGHT = 1,
	DOWNLEFT = 2,
	UPLEFT = 3
};
class InputHandler
{
public:
    
    static InputHandler* Instance()
    {
        if(s_pInstance == 0)
        {
            s_pInstance = new InputHandler();
        }
        
        return s_pInstance;
    }
    
    // init joysticks
    void initialiseJoysticks();
    bool joysticksInitialised() const { return m_bJoysticksInitialised; }
    
    void reset();
    
    // update and clean the input handler
    void update();
    void clean();
    
    // keyboard events
    bool isKeyDown(SDL_Scancode key) const;
    
    // joystick events
	int getAxisX(int joy, int stick) const;
    int getAxisY(int joy, int stick) const;
	std::pair<float, float> getDirection(int movespeed);
    bool getButtonState(int joy, int buttonNumber) const;
    
    // mouse events
    bool getMouseButtonState(int buttonNumber) const;
    Vector2D* getMousePosition() const;
    
	bool isButtonDown(SDL_GameControllerButton  button) const;
	void callHaptic();

private:
    
    InputHandler();
    ~InputHandler();
    
    InputHandler(const InputHandler&);
	InputHandler& operator=(const InputHandler&); // TODO: This is a singleton, this operation should never happen.
    
    // private functions to handle different event types
    
    // handle keyboard events
    void onKeyDown();
    void onKeyUp();
    
    // handle mouse events
    void onMouseMove(SDL_Event& event);
    void onMouseButtonDown(SDL_Event& event);
    void onMouseButtonUp(SDL_Event& event);
    
    // handle joysticks events
    void onJoystickAxisMove(SDL_Event& event);
    void onJoystickButtonDown(SDL_Event& event);
    void onJoystickButtonUp(SDL_Event& event);

    // member variables
    
    // keyboard specific
    const Uint8* m_keystates;
    
    // joystick specific
	SDL_Haptic* gControllerHaptic = NULL;
    std::vector<std::pair<Vector2D*, Vector2D*>> m_joystickValues;
    SDL_GameController* m_controller;
    std::vector<std::vector<bool>> m_buttonStates;
    bool m_bJoysticksInitialised;
    static const int m_joystickDeadZone = 10000;
    
    // mouse specific
    std::vector<bool> m_mouseButtonStates;
    Vector2D* m_mousePosition;
    
    // singleton
    static InputHandler* s_pInstance;
};
typedef InputHandler TheInputHandler;


#endif /* defined(__SDL_Game_Programming_Book__InputHandler__) */
