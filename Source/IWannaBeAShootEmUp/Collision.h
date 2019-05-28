//
//  Header.h
//  SDL Game Programming Book
//
//  Created by shaun mitchell on 28/03/2013.
//  Copyright (c) 2013 shaun mitchell. All rights reserved.
//

#ifndef SDL_Game_Programming_Book_Header_h
#define SDL_Game_Programming_Book_Header_h

#include "SDL.h"
#include "Vector2D.h"
#include <algorithm>
#include <climits>

static bool RectRect(SDL_Rect* A, SDL_Rect* B)
{
	int s_buffer = 8; // Set to eight so you cannot go inside the boss

	int aHBuf = A->h / s_buffer;
	int aWBuf = A->w / s_buffer;
    
	int bHBuf = B->h / s_buffer;
	int bWBuf = B->w / s_buffer;
    
    // if the bottom of A is less than the top of B - no collision
    if((A->y + A->h) - aHBuf <= B->y + bHBuf)  { return false; }
    
    // if the top of A is more than the bottom of B = no collision
    if(A->y + aHBuf >= (B->y + B->h) - bHBuf)  { return false; }
    
    // if the right of A is less than the left of B - no collision
    if((A->x + A->w) - aWBuf <= B->x +  bWBuf) { return false; }
    
    // if the left of A is more than the right of B - no collision
    if(A->x + aWBuf >= (B->x + B->w) - bWBuf)  { return false; }
    
    // otherwise there has been a collision
    return true;
}

// TODO refactor this function into two seperate functions.
static std::pair <bool, int> minimumDisplacementEdgeResolution(SDL_Rect* A, SDL_Rect* B)
{
	int tileSize = 64;

	int s_buffer = tileSize;

	int aHBuf = A->h / s_buffer;
	int aWBuf = A->w / s_buffer;

	int bHBuf = B->h / s_buffer;
	int bWBuf = B->w / s_buffer;

	bool collided = true;

	// if the bottom of A is less than the top of B - no collision
	if ((A->y + A->h) - aHBuf <= B->y + bHBuf) 
	{ 
		collided = false;
	}

	// if the top of A is more than the bottom of B = no collision
	if (A->y + aHBuf >= (B->y + B->h) - bHBuf)
	{
		collided = false;
	}

	// if the right of A is less than the left of B - no collision
	if ((A->x + A->w) - aWBuf <= B->x + bWBuf)
	{
		collided = false;
	}

	// if the left of A is more than the right of B - no collision
	if (A->x + aWBuf >= (B->x + B->w) - bWBuf)
	{
		collided = false;
	}

	int displacementAmtLeft = INT_MAX;
	int displacementAmtDown = INT_MAX;
	int displacementAmtRight = INT_MAX;
	int displacementAmtUp = INT_MAX;

	std::pair <bool, int> isCollisionAndSide;

	if (collided)
	{
		// if the bottom edge of A has minimum displacement of B
		if ((A->y < B->y) && (B->y < A->y + A->h) && (A->y + A->h < B->y + B->h))
		{
			// how far down was it displaced
			displacementAmtDown = A->y + A->h - B->y;
		}

		if (B->y < A->y && A->y < B->y + B->h && B->y + B->h < A->y + A->h)
		{
			// how far up was it displaced
			displacementAmtUp = B->y - A->y + A->h;
		}

		if (A->x < B->x && B->x < A->x + A->w && A->x + A->w < B->x + B->w)
		{
			// how far right was it displaced
			displacementAmtRight = A->x + A->w - B->x;
		}

		if (B->x < A->x && A->x < B->x + B->w && B->x + B->w < A->x + A->w)
		{
			// how far left was it displaced
			displacementAmtLeft = B->x - A->x + A->w;
		}

		// get the minimum displacement
		int min = std::min(displacementAmtDown, std::min(displacementAmtUp, std::min(displacementAmtRight, displacementAmtLeft)));

		if (min == displacementAmtDown)
		{
			// Collision direction top edge
			isCollisionAndSide.second = 0;
		}
		else if (min == displacementAmtUp)
		{
			// Collision direction bottom edge
			isCollisionAndSide.second = 1;
		}
		else if (min == displacementAmtRight)
		{
			// Collision direction left edge
			isCollisionAndSide.second = 2;
		}
		else if (min == displacementAmtLeft)
		{
			// Collision direction right edge
			isCollisionAndSide.second = 3;
		}
	}

	isCollisionAndSide.first = collided;

	// return the parity of collision and the side it occured on
	return isCollisionAndSide;

}

#endif
