//
//  InputHandler.cpp
//  SDL Game Programming Book
//
//  Created by shaun mitchell on 24/01/2013.
//  Copyright (c) 2013 shaun mitchell. All rights reserved.
//
// Edited by Nathan Dunne and Luke O Brien
#include "InputHandler.h"
#include "Game.h"
#include <iostream>
#include <Math.h>

InputHandler* InputHandler::s_pInstance = 0;

InputHandler::InputHandler() :  m_keystates(0),
m_bJoysticksInitialised(false),
m_mousePosition(new Vector2D(0,0))
{
    // create button states for the mouse
    for(int i = 0; i < 3; i++)
    {
        m_mouseButtonStates.push_back(false);
    }
}

InputHandler::~InputHandler()
{
    // delete anything we created dynamically
    delete m_keystates;
    delete m_mousePosition;
    
    // clear our arrays
    m_joystickValues.clear();
    m_buttonStates.clear();
    m_mouseButtonStates.clear();
}

void InputHandler::clean()
{
    // we need to clean up after ourselves and close the joysticks we opened
    if(m_bJoysticksInitialised)
    {
        for(unsigned int i = 0; i < SDL_NumJoysticks(); i++)
        {
			SDL_GameControllerClose(m_controller);
        }
    }
}

void InputHandler::initialiseJoysticks()
{
    // if we haven't already initialised the joystick subystem, we will do it here
    if(SDL_WasInit(SDL_INIT_JOYSTICK) == 0)
    {
        SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    }
    
    // if there is a controller present
    if(SDL_NumJoysticks() > 0)
    {   
        // if the controller opened correctly we need to set it as our controller
        if(SDL_GameControllerOpen(0))
        {
            // Load the open controller 
			m_controller = SDL_GameControllerOpen(0);
        }
        else
        {
            // if there was an error initialising a controller we want to know about it
            std::cout << SDL_GetError();

        }
		// get controller haptic device
		gControllerHaptic = SDL_HapticOpen(0);

		if (gControllerHaptic == NULL)
		{
			printf("Warning: Controller does not support haptics! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			// get initialize rumble
			if (SDL_HapticRumbleInit(gControllerHaptic) < 0)
			{
				printf("Warning: Unable to initialize rumble! SDL Error: %s\n", SDL_GetError());
			}
		}
        // enable controller events
        SDL_GameControllerEventState(SDL_ENABLE);
        m_bJoysticksInitialised = true;
        
        std::cout << "Initialised " << SDL_GameControllerName(m_controller) << "\n";
    }
    else
    {
        m_bJoysticksInitialised = false;
    }
}

void InputHandler::reset()
{
    m_mouseButtonStates[LEFT] = false;
    m_mouseButtonStates[RIGHT] = false;
    m_mouseButtonStates[MIDDLE] = false;
}

bool InputHandler::isKeyDown(SDL_Scancode key) const
{
    if(m_keystates != 0)
    {
        if(m_keystates[key] == 1)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    
    return false;
}

int InputHandler::getAxisX(int joy, int stick) const
{
	// get the X axis of the left strick
	short xAxis = SDL_GameControllerGetAxis(m_controller, SDL_CONTROLLER_AXIS_LEFTX);

	// if the stick facing right
	if (xAxis > m_joystickDeadZone)
	{
		return 1;
	}
	else if (xAxis < -m_joystickDeadZone) // if the stick is facing left
	{
		return -1;
	}

	return 0;
}

int InputHandler::getAxisY(int joy, int stick) const
{
	// get the Y axis of the left strick
	short YAxis = SDL_GameControllerGetAxis(m_controller, SDL_CONTROLLER_AXIS_LEFTY);

	// if the stick is facing down
	if (YAxis > m_joystickDeadZone)
	{
		return 1;
	}
	else if (YAxis < -m_joystickDeadZone) // if the stick is facing up
	{
		return -1;
	}

	return 0;
}

std::pair<float, float> InputHandler::getDirection(int moveSpeed)
{
	joyStickDirection quarter;

	// x and y axis of left thumbstick
	float xAxis = SDL_GameControllerGetAxis(m_controller, SDL_CONTROLLER_AXIS_LEFTX);
	float yAxis = SDL_GameControllerGetAxis(m_controller, SDL_CONTROLLER_AXIS_LEFTY);
	quarter = DOWNRIGHT;

	//getting rid of the negitive values
	if (xAxis >= 0 && yAxis <= 0)
	{
		yAxis *= -1;
		quarter = UPRIGHT;
	}
	else if(xAxis <= 0 && yAxis <= 0)
	{
		xAxis *= -1;
		yAxis *= -1;
		quarter = UPLEFT;
	}
	else if (xAxis <= 0 && yAxis >= 0)
	{
		xAxis *= -1;
		quarter = DOWNLEFT;
	}


	//getting the full axis value
	float fullAxis = xAxis + yAxis;

	// getting the percentage of the axis thats X or Y
	float percentageX = xAxis / fullAxis;
	float percentageY = yAxis / fullAxis;

	// applying the percentages to the players move speed
	float xChange = (moveSpeed*1.5) * percentageX;
	float yChange = (moveSpeed*1.5) * percentageY;

	//putting the negitive values back in
	if (quarter == UPRIGHT)
	{
		yChange *= -1;
	}
	else if (quarter == UPLEFT)
	{
		xChange *= -1;
		yChange *= -1;
	}
	else if (quarter == DOWNLEFT)
	{
		xChange *= -1;
	}

	//checking against the deadzon and returning the correct value
	if (fullAxis > m_joystickDeadZone)
	{
		return std::make_pair(xChange, yChange);
	}
	else
	{
		return std::make_pair(0, 0);
	}
	
}

// Rumble the controller
void InputHandler::callHaptic()
{
	SDL_HapticRumblePlay(gControllerHaptic, 0.75, 350);
}

// Get the state of a passed in button
bool InputHandler::isButtonDown(SDL_GameControllerButton  button) const
{
	bool isButtonDown = SDL_GameControllerGetButton(m_controller, button);

	return isButtonDown;
}

bool InputHandler::getButtonState(int joy, int buttonNumber) const
{
    return m_buttonStates[joy][buttonNumber];
}

bool InputHandler::getMouseButtonState(int buttonNumber) const
{
    return m_mouseButtonStates[buttonNumber];
}

Vector2D* InputHandler::getMousePosition() const
{
    return m_mousePosition;
}

void InputHandler::update()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                TheGame::Instance()->quit();
                break;
                
            //case SDL_JOYAXISMOTION:
            //    onJoystickAxisMove(event);
            //    break;
            //    
            //case SDL_JOYBUTTONDOWN:
            //    onJoystickButtonDown(event);
            //    break;
            //    
            //case SDL_JOYBUTTONUP:
            //    onJoystickButtonUp(event);
            //    break;
                
            case SDL_MOUSEMOTION:
                onMouseMove(event);
                break;
                
            case SDL_MOUSEBUTTONDOWN:
                onMouseButtonDown(event);
                break;
                
            case SDL_MOUSEBUTTONUP:
                onMouseButtonUp(event);
                break;
                
            case SDL_KEYDOWN:
                onKeyDown();
                break;
                
            case SDL_KEYUP:
                onKeyUp();
                break;
                
            default:
                break;
        }
    }
}

void InputHandler::onKeyDown()
{
    m_keystates = SDL_GetKeyboardState(0);
}

void InputHandler::onKeyUp()
{
    m_keystates = SDL_GetKeyboardState(0);
}

void InputHandler::onMouseMove(SDL_Event &event)
{
    m_mousePosition->setX(event.motion.x);
    m_mousePosition->setY(event.motion.y);
}

void InputHandler::onMouseButtonDown(SDL_Event &event)
{
    if(event.button.button == SDL_BUTTON_LEFT)
    {
        m_mouseButtonStates[LEFT] = true;
    }
    
    if(event.button.button == SDL_BUTTON_MIDDLE)
    {
        m_mouseButtonStates[MIDDLE] = true;
    }
    
    if(event.button.button == SDL_BUTTON_RIGHT)
    {
        m_mouseButtonStates[RIGHT] = true;
    }
}

void InputHandler::onMouseButtonUp(SDL_Event &event)
{
    if(event.button.button == SDL_BUTTON_LEFT)
    {
        m_mouseButtonStates[LEFT] = false;
    }
    
    if(event.button.button == SDL_BUTTON_MIDDLE)
    {
        m_mouseButtonStates[MIDDLE] = false;
    }
    
    if(event.button.button == SDL_BUTTON_RIGHT)
    {
        m_mouseButtonStates[RIGHT] = false;
    }
}

void InputHandler::onJoystickAxisMove(SDL_Event &event)
{
    int whichOne = event.jaxis.which;
    
    // left stick move left or right
    if(event.jaxis.axis == 0)
    {
        if (event.jaxis.value > m_joystickDeadZone)
        {
            m_joystickValues[whichOne].first->setX(1);
        }
        else if(event.jaxis.value < -m_joystickDeadZone)
        {
            m_joystickValues[whichOne].first->setX(-1);
        }
        else
        {
            m_joystickValues[whichOne].first->setX(0);
        }
    }
    
    // left stick move up or down
    if(event.jaxis.axis == 1)
    {
        if (event.jaxis.value > m_joystickDeadZone)
        {
            m_joystickValues[whichOne].first->setY(1);
        }
        else if(event.jaxis.value < -m_joystickDeadZone)
        {
            m_joystickValues[whichOne].first->setY(-1);
        }
        else
        {
            m_joystickValues[whichOne].first->setY(0);
        }
    }
    
    // right stick move left or right
    if(event.jaxis.axis == 3)
    {
        if (event.jaxis.value > m_joystickDeadZone)
        {
            m_joystickValues[whichOne].second->setX(1);
        }
        else if(event.jaxis.value < -m_joystickDeadZone)
        {
            m_joystickValues[whichOne].second->setX(-1);
        }
        else
        {
            m_joystickValues[whichOne].second->setX(0);
        }
    }
    
    // right stick move up or down
    if(event.jaxis.axis == 4)
    {
        if (event.jaxis.value > m_joystickDeadZone)
        {
            m_joystickValues[whichOne].second->setY(1);
        }
        else if(event.jaxis.value < -m_joystickDeadZone)
        {
            m_joystickValues[whichOne].second->setY(-1);
        }
        else
        {
            m_joystickValues[whichOne].second->setY(0);
        }
    }
}

void InputHandler::onJoystickButtonDown(SDL_Event &event)
{
    int whichOne = event.jaxis.which;
    
    m_buttonStates[whichOne][event.jbutton.button] = true;
}

void InputHandler::onJoystickButtonUp(SDL_Event &event)
{
    int whichOne = event.jaxis.which;
    
    m_buttonStates[whichOne][event.jbutton.button] = false;
}