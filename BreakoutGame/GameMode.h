#pragma once
#include "GameModeBase.h"
#include <cmath>
#include <iostream>
#include <vector>
#include "SDL_ttf.h"
#include <string>
//
union Vector2D
{
    struct { float x, y; };
    float Values[2];
};

struct Box2D
{
    Vector2D min;
    Vector2D max;
};

struct BrickType {
    int HitPoints = 0;
    int BreakScore = 0;
    std::string Id = "";
    std::string Texture = "";
    std::string HitSound = "";
    std::string BreakSound = "";

    struct Box2D brickBox;
};


class GameMode : public GameModeBase
{

private:
    /* Window Width and Height */
    int WindowWidth;
    int WindowHeight;

    /* Forward declarations of Window and Renderer */
    struct SDL_Window* GameWindow;
    struct SDL_Renderer* GameRenderer;

    TTF_Font* FontArial_16;
    TTF_Font* FontArial_24;

    /* Attributes for placing Level elements */
    std::vector<const char*> Levels;
    int RowCount;
    int ColumnCount;
    int RowSpacing;
    int ColumnSpacing;
    std::string BackgroundPath;
    std::vector<BrickType> LevelBricks;
    std::vector<std::vector<char>> BricksLayout;
    std::vector<BrickType> BricksInGame;

    /* Updates the mouse position */
    float MouseX;
    float MouseY;

    /* Updates the time */
    float Time;
    int startTime;
    float Seconds;
    unsigned int BeforeTime;
    unsigned int BeforeTimeForTime;
    unsigned int TimeForTime;

    /* Monitors the state of the main loop */
    bool bQuit;
   
    /* Center of the Game objects */
    Vector2D Paddle;
    Vector2D Cube;
    Vector2D Brick;
    Vector2D CubeDirection;
    
    /* Event tracking flags */
    bool bLostLife;
    bool bStartGame;
    bool bGameOver;
    bool bShouldPause;

    int LifeCount;
    int LevelCounter;
    int Score;
    int CurrentScore;
    int MaxScore;
    int MaxLevelScore;
   
protected:
    /* Sets all Bricks to the values specified in the Level and adds the Bricks to a BricksInGame vector */
    void SetBricks();

    /* Draws a frame for each Brick */
    void RenderRectFrame(float x, float y, float w, float h, struct SDL_Color Color);

    /* Draws texture for all Game objects */
    void RenderTexture(float x, float y, float w, float h, std::string Texture);

    /* Additional method for drawing objects from the initial position to the size of the object */
    void RenderMinAndSizeTexture(Vector2D worldMin, Vector2D worldSize, std::string Texture, bool Frame);

    /* Additional method for drawing objects from the initial position to the specified maximum position */
    void RenderMinAndMaxTexture(Vector2D worldMin, Vector2D worldMax, std::string Texture, bool Frame);

    /* Draws left, right and top border */
    void RenderBorder();

    /* Draws information about the Game such as the level the player is currently at, the current score etc. */
    void RenderGameInfo(float x, float y, std::string Text, int value, bool GameOver);

    /* Draws information about whether the player lost or won */
    void RenderGameOver();

    /* Uploads levels from XML documents */
    void UploadLevel(const char* Level);

public:
    GameMode(int WindowWidth, int WindowHeight);

    /* Initializes SDL, Window, Renderer etc. */
    void Init();

    void ResetGame() override;

    void NextLevel() override;

    void ResetLevel() override;

    /* Updates the Game state */
    void Update(float MouseX, float MouseY, float Time) override;

    /* The main method for drawing all objects in the game */
    void Render() override;

    void Run() override;
};



