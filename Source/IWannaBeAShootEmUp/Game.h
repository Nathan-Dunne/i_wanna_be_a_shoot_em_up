//
//  Game.h
//  SDL Game Programming Book
//
//  Modified by All Team Members

#ifndef __SDL_Game_Programming_Book__Game__
#define __SDL_Game_Programming_Book__Game__

//#define GAME_DEBUG
#ifdef GAME_DEBUG
#define GAMEDEBUG_MSG(str) do { std::cout << str << std::endl; } while( false )
#else
#define GAMEDEBUG_MSG(str) do { } while ( false )
#endif

#include "SDL.h"
#include "GameStateMachine.h"
//#include "SDL_mixer.h"
#include <vector>
#include "RoomTransitionState.h"
#include "PassiveItem.h"
#include "Vector2D.h"
#include "Attributes.h"

class Game
{
public:
    
    static Game* Instance()
    {
        if(s_pInstance == 0)
        {
            s_pInstance = new Game();
            return s_pInstance;
        }
        
        return s_pInstance;
    }

    
    bool init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
    
    void render();
    void update();
    void handleEvents();
    void clean();
    
    SDL_Renderer* getRenderer() const { return m_pRenderer; }
    SDL_Window* getWindow() const { return m_pWindow; }
    GameStateMachine* getStateMachine() { return m_pGameStateMachine; }

	Vector2D targetPosition;
	void storeTargetPosition(Vector2D position);
    
	void setPlayerLives(int lives);
    int getPlayerLives() { return m_playerLives; }
    
    void setCurrentLevel(int currentLevel);
    const int getCurrentLevel() { return m_currentLevel; }
    
    void setNextLevel(int nextLevel) { m_nextLevel = nextLevel; }
	const int getNextLevel() { return m_nextLevel; }
    
    void setLevelComplete(bool levelComplete) { m_bLevelComplete = levelComplete; }
    const bool getLevelComplete() { return m_bLevelComplete; }
    
    bool running() { return m_bRunning; }

    void quit() { m_bRunning = false; }
    
    int getGameWidth() const { return m_gameWidth; }
    int getGameHeight() const { return m_gameHeight; }
    float getScrollSpeed() { return m_scrollSpeed; }

	bool changingState() { return m_bChangingState; }
	void changingState(bool cs) { m_bChangingState = cs; }
    
	std::vector<std::string> getLevelFiles() { return m_levelFiles; }

	//functions to handle enemy deaths for level completion calculations
	void setLevelEnemyCount(int enemyCount);	//setter for levelEnemyCount
	int getLevelEnemyCount();	//getter for levelEnemyCount
	void enemyDied();	//used to decrement the level enemy count whenever a enemy dies

	void randomiseLevelOrder();
	
	void leftHanded(); // Setter to change boolean to true - Left handed controls
	void rightHanded(); // Setter to change boolean to false - Right handed controls
	bool getControls(); // Getter to check controls - Used for logic in player

	void setPlayerUpdating(bool isUpdating);
	bool getIsPlayerUpdating();
	Attributes getPlayerAttributes();
	void setPlayerAttributes(Attributes playerAttributes);

	void resetScore();
	double getScore();
	void addToScore(double amount);

	bool loadNextLevel();
	void fillLevelFiles();

	float getDifficultyIndex();
	void influenceDifficultyIndex(float amount);
	void resetDifficultyIndex();

	void clearLevelFiles();

	void resetGame();

	void addStringToInventory(std::string word); // Mutator - Checks if the players inventory includes an item / string
	bool checkInventory(std::string word);// Accessor - Checks if the players inventory includes an item/string

	bool isBossAlive();	//getter for m_bossAlive
	void setBossAlive(bool alive);	//setter for m_bossAlive
	int getBossHealth();	//getter for boss health
	void setBossHealth(int health);	//setter for boss health

	void setItemPickedUp(std::string itemType);
	void setIsItemPickedUp(bool isPickedUp);
	bool getIsItemPickedUp();
	std::string getItemPickedUp();

	float m_difficultyIndex;
	double m_score;


private:

	bool m_controls = false;
	bool m_bossAlive = false;	// a bool to check if a boss is alive
	int m_bossHealth;		//stores the bosses health so that it can be accessed by PlayState
	// the games Item Pool and The players Inventory

	std::vector<std::string> m_Inventory;

	bool m_isItemPickedUp;

	bool m_bChangingState;
    
    SDL_Window* m_pWindow;
    SDL_Renderer* m_pRenderer;
    
    GameStateMachine* m_pGameStateMachine;
    
    bool m_bRunning;
    
    static Game* s_pInstance;
    
    int m_gameWidth;
    int m_gameHeight;
    float m_scrollSpeed;

	bool m_isPlayerUpdating = false;
    
    int m_playerLives;	
    
    int m_currentLevel;
    int m_nextLevel;
    bool m_bLevelComplete;
	int m_levelEnemyCount;



	float m_difficultyIndexChange;

	long m_timeFinishedLevel;
	long m_timeStartedLevel;

	Attributes m_playerAttributes;

	std::vector<std::string> m_levelFiles;

    std::vector<std::string> m_easyLevelFiles; // easy difficulty levels
	std::vector<std::string> m_mediumLevelFiles; // medium difficulty levels
	std::vector<std::string> m_hardLevelFiles; // hard difficulty levels
	std::vector<std::string> m_bossLevelFiles; // levels that have a boss

	std::vector<std::string> m_itemLevelFiles; // levels that have items

	std::string m_itemPickedUp;
    
    Game();
    ~Game();
    
    Game(const Game&);
};

typedef Game TheGame;

#endif /* defined(__SDL_Game_Programming_Book__Game__) */
