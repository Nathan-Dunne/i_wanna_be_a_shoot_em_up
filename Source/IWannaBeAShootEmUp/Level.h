//
//  Map.h
//  SDL Game Programming Book
//
//  Created by shaun mitchell on 09/03/2013.
//  Copyright (c) 2013 shaun mitchell. All rights reserved.
//

#ifndef __SDL_Game_Programming_Book__Map__
#define __SDL_Game_Programming_Book__Map__

#include <iostream>
#include <vector>
#include "Layer.h"
#include "LevelParser.h"
#include "Player.h"
#include "CollisionManager.h"

class TileLayer; // TODO: Why is this here?

struct Tileset
{
    int firstGridID;
    int tileWidth;
    int tileHeight;
    int spacing;
    int margin;
    int width;
    int height;
    int numColumns;
    std::string name;
};

class Level
{
public:
    
    ~Level();
    
    void update();
    void render();
    
    std::vector<Tileset>* getTilesets() { return &m_tilesets; }
    std::vector<Layer*>* getLayers() { return &m_layers; }    
    std::vector<TileLayer*>* getCollisionLayers() { return &m_collisionLayers; }
    
    const std::vector<TileLayer*>& getCollidableLayers() { return m_collisionLayers; }
    
    Player* getPlayer() { return m_pPlayer; }
    void setPlayer(Player* pPlayer) { m_pPlayer = pPlayer; }
    
private:
    
    friend class LevelParser;
    
    Level();
    
    Player* m_pPlayer;
    
    std::vector<Layer*> m_layers;
    std::vector<Tileset> m_tilesets;
    std::vector<TileLayer*> m_collisionLayers; // Vector will have a size larger than 1 if we add more collision types.
};

#endif /* defined(__SDL_Game_Programming_Book__Map__) */
