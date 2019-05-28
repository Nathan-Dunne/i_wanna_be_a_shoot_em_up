//
//  CollisionManager.cpp
//  SDL Game Programming Book
//
//  Created by shaun mitchell on 28/03/2013.
//  Copyright (c) 2013 shaun mitchell. All rights reserved.
//
// Edited by Nathan Dunne and Luke O Brien

#include "CollisionManager.h"
#include "Collision.h"
#include "Player.h"
#include "Enemy.h"
#include "BulletHandler.h"
#include "TileLayer.h"

// AABB collision detection between the enemies's bullets and the player
void CollisionManager::checkPlayerEnemyBulletCollision(Player* pPlayer)
{
    SDL_Rect* pRect1 = new SDL_Rect();
    pRect1->x = pPlayer->getPosition().getX();
    pRect1->y = pPlayer->getPosition().getY();
    pRect1->w = pPlayer->getWidth();
    pRect1->h = pPlayer->getHeight();
    
    for(int i = 0; i < TheBulletHandler::Instance()->getEnemyBullets().size(); i++)
    {
        EnemyBullet* pEnemyBullet = TheBulletHandler::Instance()->getEnemyBullets()[i];
        
        SDL_Rect* pRect2 = new SDL_Rect();
        pRect2->x = pEnemyBullet->getPosition().getX();
        pRect2->y = pEnemyBullet->getPosition().getY();
        
        pRect2->w = pEnemyBullet->getWidth();
        pRect2->h = pEnemyBullet->getHeight();
        
        if(RectRect(pRect1, pRect2))
        {
            if(!pPlayer->dying() && !pEnemyBullet->dying())
            {
                pEnemyBullet->collision(); // destroy the bullet
                pPlayer->collision();
            }
        }
        
        delete pRect2;
    }
    
    delete pRect1;
}

void CollisionManager::checkEnemyPlayerBulletCollision(const std::vector<GameObject *> &objects)
{
    for(int i = 0; i < objects.size(); i++)
    {
        GameObject* pObject = objects[i];
        
        for(int j = 0; j < TheBulletHandler::Instance()->getPlayerBullets().size(); j++)
        {

            if(pObject->type() != "Enemy" && pObject->type() != "Assassin"  && pObject->type() != "Uno" && pObject->type() != "Newborn" || !pObject->updating())
            {
                continue;
            }
            
            SDL_Rect* pRect1 = new SDL_Rect();
            pRect1->x = pObject->getPosition().getX();
            pRect1->y = pObject->getPosition().getY();
            pRect1->w = pObject->getWidth();
            pRect1->h = pObject->getHeight();
            
            PlayerBullet* pPlayerBullet = TheBulletHandler::Instance()->getPlayerBullets()[j];
            
            SDL_Rect* pRect2 = new SDL_Rect();
            pRect2->x = pPlayerBullet->getPosition().getX();
            pRect2->y = pPlayerBullet->getPosition().getY();
            pRect2->w = pPlayerBullet->getWidth();
            pRect2->h = pPlayerBullet->getHeight();
            
            if(RectRect(pRect1, pRect2))
            {
                if(!pObject->dying() && !pPlayerBullet->dying())
                {
				
                    pPlayerBullet->collision();
                    //pObject->collision();
					pObject->bulletCollision(TheBulletHandler::Instance()->getPlayerBulletDamage());
                }
                
            }
            
            delete pRect1;
            delete pRect2;
        }
    }
}

void CollisionManager::checkPlayerEnemyCollision(Player* pPlayer, const std::vector<GameObject*> &objects)
{
    SDL_Rect* pRect1 = new SDL_Rect();
    pRect1->x = pPlayer->getPosition().getX();
    pRect1->y = pPlayer->getPosition().getY();
    pRect1->w = pPlayer->getWidth();
    pRect1->h = pPlayer->getHeight();
    
    for(int i = 0; i < objects.size(); i++)
    {
		// if the type is not an enemy in the game
        if(objects[i]->type() != "Enemy" && objects[i]->type() != "Assassin" && objects[i]->type() != "Uno" && objects[i]->type() != "Newborn" || !objects[i]->updating())
        {
            continue;
        }
        
        SDL_Rect* pRect2 = new SDL_Rect();
        pRect2->x = objects[i]->getPosition().getX();
        pRect2->y = objects[i]->getPosition().getY();
        pRect2->w = objects[i]->getWidth();
        pRect2->h = objects[i]->getHeight();
        
        if(RectRect(pRect1, pRect2))
        {
            if(!objects[i]->dead() && !objects[i]->dying())
            {
                pPlayer->collision();
            }
        }
        
        delete pRect2;
    }
    
    delete pRect1;
}


// unused in favor of checkCollideTile in Player
void CollisionManager::checkPlayerTileCollision(Player* pPlayer, const std::vector<TileLayer*>& collisionLayers)
{
	for(std::vector<TileLayer*>::const_iterator it = collisionLayers.begin(); it != collisionLayers.end(); ++it)
	{
		TileLayer* pTileLayer = (*it);
		std::vector<std::vector<int>> tiles = pTileLayer->getTileIDs();

		Vector2D layerPos = pTileLayer->getPosition();

		int x, y, tileColumn, tileRow, tileid = 0;

		x = layerPos.getX() / pTileLayer->getTileSize();
		y = layerPos.getY() / pTileLayer->getTileSize();

		if (pPlayer->getVelocity().getX() >= 0 || pPlayer->getVelocity().getY() >= 0)
		{
			tileColumn = ((pPlayer->getPosition().getX() + pPlayer->getWidth()) / pTileLayer->getTileSize());
			tileRow = ((pPlayer->getPosition().getY() + pPlayer->getHeight()) / pTileLayer->getTileSize());
			tileid = tiles[tileRow + y][tileColumn + x];
		}
		else if (pPlayer->getVelocity().getX() < 0 || pPlayer->getVelocity().getY() < 0)
		{
			tileColumn = pPlayer->getPosition().getX() / pTileLayer->getTileSize();
			tileRow = pPlayer->getPosition().getY() / pTileLayer->getTileSize();
			tileid = tiles[tileRow + y][tileColumn + x];
		}

		if (tileid != 0)
		{
			//pPlayer->collision();
		}
		
	}
}

void CollisionManager::checkBulletTileCollision(const std::vector<TileLayer*>& collisionLayers)
{
	for (std::vector<TileLayer*>::const_iterator it = collisionLayers.begin(); it != collisionLayers.end(); ++it)
	{
		TileLayer* pTileLayer = (*it);
		std::vector<std::vector<int>> tiles = pTileLayer->getTileIDs();

		Vector2D layerPos = pTileLayer->getPosition();

		int x, y, tileColumn, tileRow, tileid = 0;

		x = layerPos.getX() / pTileLayer->getTileSize();
		y = layerPos.getY() / pTileLayer->getTileSize();		

		// for all the player bullets
		for (int j = 0; j < TheBulletHandler::Instance()->getPlayerBullets().size(); j++)
		{

			PlayerBullet* pPlayerBullet = TheBulletHandler::Instance()->getPlayerBullets()[j];

			if ((pPlayerBullet->getVelocity().getX() >= 0 || pPlayerBullet->getVelocity().getY() >= 0) && (pPlayerBullet->getPosition().getY() + 32 < TheGame::Instance()->getGameHeight()))
			{
				tileColumn = ((pPlayerBullet->getPosition().getX() + pPlayerBullet->getWidth()) / pTileLayer->getTileSize());
				tileRow = ((pPlayerBullet->getPosition().getY() + pPlayerBullet->getHeight()) / pTileLayer->getTileSize());
				tileid = tiles[tileRow + y][tileColumn + x];
			}
			else if ((pPlayerBullet->getVelocity().getX() < 0 || pPlayerBullet->getVelocity().getY() < 0) && (pPlayerBullet->getPosition().getY()  < TheGame::Instance()->getGameHeight()))
			{
				tileColumn = pPlayerBullet->getPosition().getX() / pTileLayer->getTileSize();
				tileRow = pPlayerBullet->getPosition().getY() / pTileLayer->getTileSize();
				tileid = tiles[tileRow + y][tileColumn + x];
			}

			if (tileid == 4)
			{
				pPlayerBullet->collision();
				pTileLayer->setTileType(tileRow, tileColumn, 0);
			}
			else if (tileid != 0)
			{
				pPlayerBullet->collision();
			}
		}

		// for all the enemy bullets
		for (int j = 0; j < TheBulletHandler::Instance()->getEnemyBullets().size(); j++)
		{

			EnemyBullet* pEnemyBullet = TheBulletHandler::Instance()->getEnemyBullets()[j];

			if (pEnemyBullet->getVelocity().getX() >= 0 || pEnemyBullet->getVelocity().getY() >= 0)
			{
				tileColumn = ((pEnemyBullet->getPosition().getX() + pEnemyBullet->getWidth()) / pTileLayer->getTileSize());
				tileRow = ((pEnemyBullet->getPosition().getY() + pEnemyBullet->getHeight()) / pTileLayer->getTileSize());
				tileid = tiles[tileRow + y][tileColumn + x];
			}
			else if (pEnemyBullet->getVelocity().getX() < 0 || pEnemyBullet->getVelocity().getY() < 0)
			{
				tileColumn = pEnemyBullet->getPosition().getX() / pTileLayer->getTileSize();
				tileRow = pEnemyBullet->getPosition().getY() / pTileLayer->getTileSize();
				tileid = tiles[tileRow + y][tileColumn + x];
			}

			if (tileid == 4)
			{
				pEnemyBullet->collision();
				pTileLayer->setTileType(tileRow, tileColumn, 0);
			}
			if (tileid != 0)
			{
				pEnemyBullet->collision();
			}
		}
	}
}

// Will return players position
Vector2D CollisionManager::getPlayerPosition(Player* pPlayer)
{
	SDL_Rect* pRect1 = new SDL_Rect();
	pRect1->x = pPlayer->getPosition().getX();
	pRect1->y = pPlayer->getPosition().getY();
	return Vector2D(pRect1->x, pRect1->y);
}

void CollisionManager::checkPlayerPickupCollision(Player * pPlayer, const std::vector<GameObject*>& objects)
{
	SDL_Rect* pRect1 = new SDL_Rect();
	pRect1->x = pPlayer->getPosition().getX();
	pRect1->y = pPlayer->getPosition().getY();
	pRect1->w = pPlayer->getWidth();
	pRect1->h = pPlayer->getHeight();

	for (int i = 0; i < objects.size(); i++)
	{
		// if the object is not an item, continue
		if (objects[i]->type() != "Pickup" && objects[i]->type() != "Damage" && objects[i]->type() != "Firing Rate" && objects[i]->type() != "Move and Projectile Speed" && objects[i]->type() != "Shotgun" || !objects[i]->updating())
		{
			continue;
		}

		SDL_Rect* pRect2 = new SDL_Rect();
		pRect2->x = objects[i]->getPosition().getX();
		pRect2->y = objects[i]->getPosition().getY();
		pRect2->w = objects[i]->getWidth();
		pRect2->h = objects[i]->getHeight();

		if (RectRect(pRect1, pRect2))
		{
			if (!objects[i]->dead() && !objects[i]->dying())
			{
				TheGame::Instance()->setItemPickedUp(objects[i]->type());
				TheGame::Instance()->setIsItemPickedUp(true);
				objects[i]->collision();
			}
		}

		delete pRect2;
	}

	delete pRect1;
}
