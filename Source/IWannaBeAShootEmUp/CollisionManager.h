//
//  CollisionManager.h
//  SDL Game Programming Book
//
//  Created by shaun mitchell on 28/03/2013.
//  Copyright (c) 2013 shaun mitchell. All rights reserved.
//
// Edited by Nathan Dunne and Luke O Brien

#ifndef __SDL_Game_Programming_Book__CollisionManager__
#define __SDL_Game_Programming_Book__CollisionManager__

#include <iostream>
#include <vector>
#include "Vector2D.h"

class Player;
class GameObject;
class TileLayer;
class Vector2D;

class CollisionManager
{
public:
    
    void checkPlayerEnemyBulletCollision(Player* pPlayer);
    void checkPlayerEnemyCollision(Player* pPlayer, const std::vector<GameObject*> &objects);
    void checkEnemyPlayerBulletCollision(const std::vector<GameObject*>& objects);
    void checkPlayerTileCollision(Player* pPlayer, const std::vector<TileLayer*>& collisionLayers);
	void checkBulletTileCollision(const std::vector<TileLayer*>& collisionLayers);
	Vector2D getPlayerPosition(Player* pPlayer); // Will return players position
	void checkPlayerPickupCollision(Player* pPlayer, const std::vector<GameObject*> &objects);
	//bool checkCollideTile(Vector2D newPos, Player* pPlayer, const std::vector<TileLayer*>& collisionLayers);
};

#endif /* defined(__SDL_Game_Programming_Book__CollisionManager__) */
