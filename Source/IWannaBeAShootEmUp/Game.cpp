//
//  Game.cpp
//  SDL Game Programming Book
//
// Modified by All Team Members

// Final build test

// Singletons
#include "Game.h"
#include "TextureManager.h"
#include "InputHandler.h"
#include "DatabaseManager.h"

#include "MainMenuState.h"
#include "GameObjectFactory.h"
#include "MenuButton.h"
#include "AnimatedGraphic.h"
#include "Player.h"
#include "ScrollingBackground.h"
#include "SoundManager.h"
#include "Assassin.h"
#include "Uno.h"
#include "Newborn.h"
#include "Boss.h"
#include "GameOverState.h"
#include <iostream>
#include <random>
#include "MoreBullets.h"
#include "Pickup.h"
#include "DamageUp.h"
#include "HealthUp.h"
#include "FiringRateUp.h"
#include "MoveSpeedUp.h"
#include "OverclockItem.h"
#include "ShotgunItem.h"
#include <algorithm>

using namespace std;

Game* Game::s_pInstance = 0;


Game::Game():
			m_pWindow(0),
			m_pRenderer(0),
			m_bRunning(false),
			m_pGameStateMachine(0),
			m_playerLives(5),
			m_scrollSpeed(0.0),
			m_bLevelComplete(false),
			m_bChangingState(false),
			m_score(1000),
			m_bossHealth(0),
			m_difficultyIndex(3)
{
    // start at this level
    m_currentLevel = 1;
}

Game::~Game()
{
    // we must clean up after ourselves to prevent memory leaks
    m_pRenderer= 0;
    m_pWindow = 0;
}


bool Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen)
{
    int flags = 0;
    
    // store the game width and height
    m_gameWidth = width;
    m_gameHeight = height;
    
    if(fullscreen)
    {
        flags = SDL_WINDOW_FULLSCREEN;
    }
    
	TTF_Init();
    // attempt to initialise SDL
    if(SDL_Init(SDL_INIT_EVERYTHING) == 0)
    {
        cout << "SDL init success\n";
        // init the window
        m_pWindow = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
        
        if(m_pWindow != 0) // window init success
        {
            cout << "window creation success\n";
            m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED);
            
            if(m_pRenderer != 0) // renderer init success
            {
                cout << "renderer creation success\n";
                SDL_SetRenderDrawColor(m_pRenderer, 0,0,0,255);
            }
            else
            {
                cout << "renderer init fail\n";
                return false; // renderer init fail
            }
        }
        else
        {
            cout << "window init fail\n";
            return false; // window init fail
        }
    }
    else
    {
        cout << "SDL init fail\n";
        return false; // SDL init fail
    }
    
    // add some sound effects - TODO move to better place
    TheSoundManager::Instance()->load("assets/Power_Bots_Loop.ogg", "music1", SOUND_MUSIC);
    TheSoundManager::Instance()->load("assets/boom.wav", "explode", SOUND_SFX);
    TheSoundManager::Instance()->load("assets/shoot.wav", "shoot", SOUND_SFX);
	TheSoundManager::Instance()->load("assets/death.wav", "death", SOUND_SFX);
	TheSoundManager::Instance()->load("assets/pickup.wav", "pickup", SOUND_SFX);
	TheSoundManager::Instance()->load("assets/hitSound.wav", "gotHitSound", SOUND_SFX);
    
    TheSoundManager::Instance()->playMusic("music1", -1);
    
    TheInputHandler::Instance()->initialiseJoysticks();
 
    // register the types for the game
    TheGameObjectFactory::Instance()->registerType("MenuButton", new MenuButtonCreator());
    TheGameObjectFactory::Instance()->registerType("Player", new PlayerCreator());
    TheGameObjectFactory::Instance()->registerType("AnimatedGraphic", new AnimatedGraphicCreator());
    TheGameObjectFactory::Instance()->registerType("ScrollingBackground", new ScrollingBackgroundCreator());
    TheGameObjectFactory::Instance()->registerType("Assassin", new AssassinCreator());
    TheGameObjectFactory::Instance()->registerType("Newborn", new NewbornCreator());
    TheGameObjectFactory::Instance()->registerType("Uno", new UnoCreator());
    TheGameObjectFactory::Instance()->registerType("Boss", new BossCreator());
	TheGameObjectFactory::Instance()->registerType("Pickup", new PickupCreator());
	TheGameObjectFactory::Instance()->registerType("DamageUp", new DamageUpCreator());
	TheGameObjectFactory::Instance()->registerType("HealthUp", new HealthUpCreator());
	TheGameObjectFactory::Instance()->registerType("FiringRateUp", new FiringRateUpCreator());
	TheGameObjectFactory::Instance()->registerType("MoveSpeedUp", new MoveSpeedUpCreator());
	TheGameObjectFactory::Instance()->registerType("OverclockItem", new OverclockItemCreator());
	TheGameObjectFactory::Instance()->registerType("ShotgunItem", new ShotgunItemCreator());

    // start the menu state
    m_pGameStateMachine = new GameStateMachine();
    m_pGameStateMachine->changeState(new MainMenuState());
    
    m_bRunning = true; // everything inited successfully, start the main loop
    return true;
}

// This function will push a level file into a working level vector dependent on the difficulty Index

bool Game::loadNextLevel()
{
	int hardFloor = 8; // difficulty index range for hard levels
	int mediumFloor = 4; // difficulty index range for medium levels
	int easyFloor = 0; // difficulty index range for easy levels

	int maxLevels = 21; 
	int itemRoomInterval = 4;

	if (m_currentLevel < maxLevels)
	{
		// if the next room should be an item room, every 4 levels
		if (m_currentLevel % itemRoomInterval == 0 && m_itemLevelFiles.size() != 0)
		{
			m_levelFiles.push_back(m_itemLevelFiles.back());
		}
		else if (m_difficultyIndex >= hardFloor  && m_hardLevelFiles.size() != 0) // if the player has achieved the highest range of difficulty
		{
			m_levelFiles.push_back(m_hardLevelFiles.back());
			m_hardLevelFiles.pop_back(); // we don't want any levels to be played through twice in one run
		}
		else if (m_difficultyIndex >= mediumFloor && m_mediumLevelFiles.size() != 0) // if the player is within the medium range of difficulty
		{
			m_levelFiles.push_back(m_mediumLevelFiles.back());
			m_mediumLevelFiles.pop_back(); // we don't want any levels to be played through twice in one run
		}
		else if (m_difficultyIndex >= easyFloor && m_easyLevelFiles.size() != 0) // if the player is within the easy range of difficulty
		{
			m_levelFiles.push_back(m_easyLevelFiles.back());
			m_easyLevelFiles.pop_back(); // we don't want any levels to be played through twice in one run
		}
		else if (m_easyLevelFiles.size() != 0) // if we somehow can't load based on difficulty, begin loading the easy levels
		{
			m_levelFiles.push_back(m_easyLevelFiles.back());
			m_easyLevelFiles.pop_back(); // we don't want any levels to be played through twice in one run
		}
		else if (m_mediumLevelFiles.size() != 0) // if we somehow can't load based on difficulty, begin loading the medium levels next
		{
			m_levelFiles.push_back(m_mediumLevelFiles.back());
			m_mediumLevelFiles.pop_back(); // we don't want any levels to be played through twice in one run
		}
		else if (m_hardLevelFiles.size() != 0) // if we somehow can't load based on difficulty, begin loading the hard levels next
		{
			m_levelFiles.push_back(m_hardLevelFiles.back());
			m_hardLevelFiles.pop_back(); // we don't want any levels to be played through twice in one run
		}
		else
		{
			std::cout << "Game Over " << std::endl; // if something has gone completely wrong, go to the gameover states
			return false;
		}
	}
	else if (m_currentLevel < maxLevels +1) // if we have gone through the amount of levels intended for the player, push the boss room onto the working vector
	{
		m_levelFiles.push_back(m_bossLevelFiles.back());
	}
	else
	{
		return false;
	}

	return true; // we can safely load the next level or the game hasn't ended yet
}

void Game::fillLevelFiles()
{
	//m_easyLevelFiles.push_back("assets/TEST_MAP.tmx");
	 //easy
	m_easyLevelFiles.push_back("assets/mapEasy1.tmx");
	m_easyLevelFiles.push_back("assets/mapEasy2.tmx");
	m_easyLevelFiles.push_back("assets/mapEasy3.tmx");
	m_easyLevelFiles.push_back("assets/mapEasy5.tmx");
	m_easyLevelFiles.push_back("assets/mapEasy6.tmx");
	m_easyLevelFiles.push_back("assets/mapEasy7.tmx");
	m_easyLevelFiles.push_back("assets/mapEasy8.tmx");
	m_easyLevelFiles.push_back("assets/mapEasy9.tmx");
	m_easyLevelFiles.push_back("assets/mapEasy10.tmx");
	m_easyLevelFiles.push_back("assets/mapEasy11.tmx");
	m_easyLevelFiles.push_back("assets/mapEasy12.tmx");
	m_easyLevelFiles.push_back("assets/mapEasy13.tmx");
	m_easyLevelFiles.push_back("assets/mapEasy14.tmx");
	m_easyLevelFiles.push_back("assets/mapEasy15.tmx");
	m_easyLevelFiles.push_back("assets/mapEasy4.tmx");

	// medium
	m_mediumLevelFiles.push_back("assets/mapMedium1.tmx");
	m_mediumLevelFiles.push_back("assets/mapMedium2.tmx");
	m_mediumLevelFiles.push_back("assets/mapMedium3.tmx");
	m_mediumLevelFiles.push_back("assets/mapMedium4.tmx");
	m_mediumLevelFiles.push_back("assets/mapMedium5.tmx");
	m_mediumLevelFiles.push_back("assets/mapMedium6.tmx");
	m_mediumLevelFiles.push_back("assets/mapMedium7.tmx");
	m_mediumLevelFiles.push_back("assets/mapMedium8.tmx");
	m_mediumLevelFiles.push_back("assets/mapMedium9.tmx");
	m_mediumLevelFiles.push_back("assets/mapMedium10.tmx");
	m_mediumLevelFiles.push_back("assets/mapMedium11.tmx");
	m_mediumLevelFiles.push_back("assets/mapMedium12.tmx");
	m_mediumLevelFiles.push_back("assets/mapMedium13.tmx");
	m_mediumLevelFiles.push_back("assets/mapMedium14.tmx");
	m_mediumLevelFiles.push_back("assets/mapMedium15.tmx");
	
	//// hard
	m_hardLevelFiles.push_back("assets/mapHard1.tmx");
	m_hardLevelFiles.push_back("assets/mapHard2.tmx");
	m_hardLevelFiles.push_back("assets/mapHard3.tmx");
	m_hardLevelFiles.push_back("assets/mapHard4.tmx");
	m_hardLevelFiles.push_back("assets/mapHard5.tmx");
	m_hardLevelFiles.push_back("assets/mapHard6.tmx");
	m_hardLevelFiles.push_back("assets/mapHard7.tmx");
	m_hardLevelFiles.push_back("assets/mapHard8.tmx");
	m_hardLevelFiles.push_back("assets/mapHard9.tmx");
	m_hardLevelFiles.push_back("assets/mapHard10.tmx");
	m_hardLevelFiles.push_back("assets/mapHard11.tmx");
	m_hardLevelFiles.push_back("assets/mapHard12.tmx");
	m_hardLevelFiles.push_back("assets/mapHard13.tmx");
	m_hardLevelFiles.push_back("assets/mapHard14.tmx");
	m_hardLevelFiles.push_back("assets/mapHard15.tmx");

	// boss
	m_bossLevelFiles.push_back("assets/mapBossBattle.tmx");

	// items
	m_itemLevelFiles.push_back("assets/itemRoomFiringRateUp.tmx");
	m_itemLevelFiles.push_back("assets/itemRoomMoveSpeedUp.tmx");
	m_itemLevelFiles.push_back("assets/itemRoomDamageUp.tmx");
	m_itemLevelFiles.push_back("assets/itemRoomShotgun.tmx");

	// force the intended tutorial level to be played first
	m_levelFiles.push_back(m_easyLevelFiles.back());
	m_easyLevelFiles.pop_back();

	m_currentLevel = 1;

	randomiseLevelOrder(); // randomise the order of the levels 
}

float Game::getDifficultyIndex()
{
	return m_difficultyIndex;
}

void Game::influenceDifficultyIndex(float amount)
{

	m_difficultyIndexChange = +amount; 

	if(m_difficultyIndex + amount < 1)
	{ 
		m_difficultyIndex = 1; // set to floor
	}
	else if (m_difficultyIndex + amount > 12)
	{
		m_difficultyIndex = 12; // set to roof
	}
	else
	{
		m_difficultyIndex += amount; // ok to add the change
	}
}

void Game::resetDifficultyIndex()
{
	m_difficultyIndex = 3;
}

void Game::setPlayerAttributes(Attributes playerAttributes)
{
	m_playerAttributes = playerAttributes;
}

void Game::clearLevelFiles()
{
	m_levelFiles.clear();

	m_easyLevelFiles.clear(); // easy difficulty levels
	m_mediumLevelFiles.clear(); // medium difficulty levels
	m_hardLevelFiles.clear(); // hard difficulty levels
	m_bossLevelFiles.clear(); // levels that have a boss

	m_itemLevelFiles.clear(); // levels that have items
}

void Game::resetGame()
{
	// randomise the levels again for consecutive playthroughs
	clearLevelFiles();
	fillLevelFiles();

	// reset the score and the difficulty index
	resetDifficultyIndex();
	resetScore();

	//set the players lives back to 5 and set the enemy count to 0
	setPlayerLives(5);
	setLevelEnemyCount(0);

	TheInputHandler::Instance()->reset();
	//free memory
	TheTextureManager::Instance()->clearTextureMap();

	m_Inventory.clear();

	setBossAlive(false);
}

// Adds a string to the players inventory - Adds an item
void Game::addStringToInventory(std::string word)
{
	m_Inventory.push_back(word);
}

// Checks if the players inventory includes an item/string
bool Game::checkInventory(std::string word)
{
	if (std::find(m_Inventory.begin(), m_Inventory.end(), word) != m_Inventory.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Game::isBossAlive()	//returns a bool
{
	return m_bossAlive;
}

void Game::setBossAlive(bool alive)	// setter
{
	m_bossAlive = alive;
}

int Game::getBossHealth()	//getter
{
	return m_bossHealth;
}

void Game::setBossHealth(int health) //setter
{
	m_bossHealth = health;
}


void Game::setItemPickedUp(std::string itemType)	//setter
{
	m_itemPickedUp = itemType;
}

void Game::setIsItemPickedUp(bool isPickedUp)	//setter
{
	m_isItemPickedUp = isPickedUp;
}

bool Game::getIsItemPickedUp()	//getter
{
	return m_isItemPickedUp;
}

std::string Game::getItemPickedUp()	//getter
{
	return m_itemPickedUp;
}

// main function to deal with the order in which the levels are generated
void Game::setCurrentLevel(int currentLevel)
{
	m_timeFinishedLevel = SDL_GetTicks();

	long levelTime = m_timeFinishedLevel - m_timeStartedLevel; // how long the player took to complete the level

	// if not in an item room and it's not the tutorial level
	if ( ( (m_currentLevel % 4) != 0 ) && (m_currentLevel !=1) )
	{
		if (levelTime > 25000) // taken more than 25 seconds
		{
			influenceDifficultyIndex(-2); // decrease the index
		}
		else if (levelTime > 10000) // taken more than 10 seconds
		{
			influenceDifficultyIndex(1);  // increase the index
		}
		else if (levelTime <= 10000) // taken less than 10 seconds
		{
			influenceDifficultyIndex(2);  // decrease the index
		}
	}

	m_currentLevel = currentLevel; 

	randomiseLevelOrder(); // randomised the level order

	if (loadNextLevel())  // if we can load a new level, go to the next room
	{
		m_pGameStateMachine->changeState(new RoomTransitionState());
	}
	else // if we cannot it's game over
	{
		m_pGameStateMachine->changeState(new GameOverState());
		m_currentLevel = 1;
	}

    m_bLevelComplete = false;
	m_timeStartedLevel = SDL_GetTicks(); // time started the level
	m_difficultyIndexChange = 0; // reset the difficulty index change
}

void Game::setLevelEnemyCount(int enemyCount)	//setter
{
	m_levelEnemyCount = enemyCount;
}

int Game::getLevelEnemyCount()	//getter
{
	return m_levelEnemyCount;
}

void Game::enemyDied()	//called when an enemy dies. decrements enemy count and gives the player score
{
	m_score += 100;
	m_levelEnemyCount--;
	std::cout << "An enemy has died. The enemy count is now = " << m_levelEnemyCount << std::endl;
}

void Game::randomiseLevelOrder()	// used to shuffle the levels so that each playthrough is different
{
	std::random_device rng; // set up the random device
	std::mt19937 urng(rng()); // seed it with the mersenne twister engine

	// randomise the order in which the levels are in their respective vectors
	std::shuffle(m_easyLevelFiles.begin(), m_easyLevelFiles.end(), urng);
	std::shuffle(m_mediumLevelFiles.begin(), m_mediumLevelFiles.end(), urng);
	std::shuffle(m_hardLevelFiles.begin(), m_hardLevelFiles.end(), urng);
	std::shuffle(m_bossLevelFiles.begin(), m_bossLevelFiles.end(), urng);
	std::shuffle(m_itemLevelFiles.begin(), m_itemLevelFiles.end(), urng);
}


void Game::storeTargetPosition(Vector2D position)	//setter
{
	targetPosition = position;
}

void Game::setPlayerLives(int lives)	//setter
{
	if ( lives >= 5)
	{
		m_playerLives = 5;
	}
	else
	{
		m_playerLives = lives;
	}
}

// Sets the controls to be left handed
void Game::leftHanded()
{
	m_controls = true;
}


// Sets the controls to be right handed
void Game::rightHanded()
{
	m_controls = false;
}


// Gets the controls being used for logic in player
bool Game::getControls()
{
	return m_controls;
}

void Game::setPlayerUpdating(bool isUpdating)	//setter
{
	m_isPlayerUpdating = isUpdating;
}

bool Game::getIsPlayerUpdating()	//getter
{
	return m_isPlayerUpdating;
}

Attributes Game::getPlayerAttributes()	//getter
{
	return m_playerAttributes;
}

void Game::resetScore()	//resetter
{
	m_score = 1000;
}

double Game::getScore()	//getter
{
	return m_score;
}

void Game::addToScore(double amount)	//allows changing of the players score by the value passed in
{
	m_score += amount;
}



void Game::render()	//renders everything in the game
{
    SDL_RenderClear(m_pRenderer);
    

	m_pGameStateMachine->render();


    SDL_RenderPresent(m_pRenderer);
}

void Game::update()	//calls all of the update functions
{
	//std::cout << m_bossAlive << std::endl;
	//if (m_score > 1)
	//{
	//	m_score -= (0.20 / (m_difficultyIndex / 0.90));
	//}

	m_pGameStateMachine->update();
}

void Game::handleEvents()
{
	TheInputHandler::Instance()->update();
}

void Game::clean() //used for freeing memory so that we can avoid leaks
{
    cout << "cleaning game\n";
    
    TheInputHandler::Instance()->clean();
    
    m_pGameStateMachine->clean();
    
    m_pGameStateMachine = 0;
    delete m_pGameStateMachine;
    
    TheTextureManager::Instance()->clearTextureMap();
    
    SDL_DestroyWindow(m_pWindow);
    SDL_DestroyRenderer(m_pRenderer);
    SDL_Quit();
}


