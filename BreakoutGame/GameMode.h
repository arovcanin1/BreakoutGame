#pragma once
#include "GameModeBase.h"
#include <cmath>
#include <iostream>
#include <vector>
#include "SDL_ttf.h"
#include <string>

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
    std::string Id;
    std::string Texture;
    int HitPoints;
    std::string HitSound;
    std::string BreakSound;
    int BreakScore;

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

    /* Attributes for placing Level elements */
    int RowCount;
    int ColumnCount;
    int RowSpacing;
    int ColumnSpacing;
    std::string BackgroundPath;
    std::vector<BrickType> LevelBricks;
    std::vector<std::vector<char>> BricksLayout;
    std::vector<BrickType> BricksInGame;

    wchar_t* wstr;
    /**/
    float MouseX;
    float MouseY;
    float Time;
    bool bQuit;
    bool pressed;
    bool bEnterPressed;
    int startTime;
    int ElapsedTime;
    float Seconds;
    float timeStep;

    float CubehitAnimationTime;

    /* Paddle */
    Vector2D Paddle;
    Vector2D Cube;
    Vector2D Direction;
    Vector2D Brick;

    unsigned int BeforeTime;
    unsigned int BeforeTimeForTime;
    unsigned int TimeForTime;

    bool bLostLife;
    bool bStartGame;


    bool bClickWaiting;
    int lifeCount;
    int counter;
    int Score;
    bool bGameOver;
    int counterImpenetrable;
    int MaxScore;
    std::vector<const char*> Levels;
    int LevelCounter;
    bool bPaused;

    bool bShouldPause;


    int CurrentScore;
    int MaxLevelScore;
    bool bWinGame;
    TTF_Font* FontArial_16;
    TTF_Font* FontArial_24;
  

protected:
    /* Sets all Bricks to the values specified in the Level and adds the Bricks to a BricksInGame vector */
    void SetBricks();
    /**/
    void DrawRectFrame(float PositionX, float PositionY, float Width, float Height, struct SDL_Color Color);
    /**/
    void DrawTexture(float x, float y, float w, float h, std::string Texture);
    /**/
    void RenderMinAndSizeTexture(Vector2D worldMin, Vector2D worldSize, std::string Texture, bool Frame);
    /**/
    void RenderMinAndMaxTexture(Vector2D worldMin, Vector2D worldMax, std::string Texture, bool Frame);
    /**/
    void DrawBorder();
    /**/
    void DrawGameInfo(float x, float y, std::string Text, int value, bool GameOver);
    /**/
    void DrawGameOver();
    /**/
    void RenderWinGame();

public:
    /**/
    GameMode(int WindowWidth, int WindowHeight);

    /**/
    void Init();

    /**/
    void Render() override;

    /**/
    void ResetGame();

    /**/
    void ResetLevel();

    /**/
    void StartGame();

    /**/
    void UploadLevel(const char* Level);

    /**/
    void NextLevel();

    void GameOver();


    /**/
    void StartMovingCube();


    void Update(float MouseX, float MouseY, float Time) override;

    void Run() override;

    bool Overlaps(Box2D paddle, Box2D cube);

    void nextLevel();



 

    /* Getters and Setters */
    inline int GetWindowWidth() const { return WindowWidth; }
    inline int GetWindowHeight() const { return WindowHeight; }
};



