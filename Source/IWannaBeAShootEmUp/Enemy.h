//
//  Enemy.h
//  SDL Game Programming Book
//
//  Created by shaun mitchell on 19/01/2013.
//  Copyright (c) 2013 shaun mitchell. All rights reserved.
//

#ifndef __SDL_Game_Programming_Book__Enemy__
#define __SDL_Game_Programming_Book__Enemy__

#include <iostream>
#include "ShooterObject.h"
#include "CollisionManager.h"
#include "Vector2D.h"
#include "TileLayer.h"

// Enemy base class
class Enemy : public ShooterObject
{
public:
    
    virtual std::string type() { return "Enemy"; }

	void Enemy::handleMovement(Vector2D velocity)
	{
		// get the current position
		Vector2D newPos = m_position;

		// add velocity to the x position
		newPos.m_x = m_position.m_x + velocity.m_x;

		// check if the new x position would collide with a tile
		if (!checkCollideTile(newPos))
		{
			// no collision, add to the actual x position
			m_position.m_x = newPos.m_x;
		}
		else
		{
			// collision, invert x movement
			m_velocity.m_x *= -1;

			if (m_velocity.m_x < 0)
			{
				m_flip = SDL_FLIP_NONE;
			}
			else
			{
				m_flip = SDL_FLIP_HORIZONTAL;
			}

			//SDL_RendererFlip 
		}

		// get the current position after x movement
		newPos = m_position;

		// add velocity to y position
		newPos.m_y += velocity.m_y;

		// check if new y position would collide with a tile
		if (!checkCollideTile(newPos))
		{
			// no collision, add to the actual x position
			m_position.m_y = newPos.m_y;
		}
		else
		{
			// collision, invert y movement
			m_velocity.m_y *= -1;
		}
	}

	bool Enemy::checkCollideTile(Vector2D newPos)
	{
		// for each collision layer on map
		for (std::vector<TileLayer*>::iterator it = m_pCollisionLayers->begin(); it != m_pCollisionLayers->end(); ++it)
		{
			// get the tilelayer
			TileLayer* pTileLayer = (*it);

			// get each tileID in the n*n grid, tildIDs != 0 are collidable tiles
			std::vector<std::vector<int>> tiles = pTileLayer->getTileIDs();

			// get up the grid of tiles we want to look at and something to store their state
			int tileColumn, tileRow, tileid = 0;

			// the top left position of a rectangle checking this collision
			Vector2D startPos = newPos;

			// the bottom right position of a rectangle checking this collision
			Vector2D endPos(newPos.m_x + (m_width), (newPos.m_y) + m_height);

			// for each pixel, check if that pixel overlaps a tile with a tileID != 0
			for (int i = startPos.m_x; i < endPos.m_x; i++)
			{
				for (int j = startPos.m_y; j < endPos.m_y; j++)
				{
					tileColumn = i / pTileLayer->getTileSize();
					tileRow = j / pTileLayer->getTileSize();

					tileid = tiles[tileRow][tileColumn];

					if (tileid != 0)
					{
						// we have collided
						return true;
					}
				}
			}
		}

		// there has been no collision
		return false;
	}

protected:
    
    int m_health;
    
    Enemy() : ShooterObject() {}
	virtual ~Enemy() {}
};

#endif /* defined(__SDL_Game_Programming_Book__Enemy__) */
