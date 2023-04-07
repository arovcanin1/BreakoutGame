#include "GameMode.h"
#include "SDL.h"
#include <codecvt>
#include "SDL_mixer.h"
#include "DirectXTex.h"
#include "SDL_ttf.h"
#include <sstream>
#include <string>
#include <stdlib.h>

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#pragma warning(push)
#pragma warning(disable: 26812) // Prefer enum class over enum  
#include "tinyxml2.h"
#pragma warning(pop)

using namespace tinyxml2;

/* Operators for easy vector addition, vector substraction etc. */
Vector2D operator+(Vector2D a, Vector2D b)
{
	return { a.x + b.x, a.y + b.y };
}

Vector2D operator-(Vector2D a, Vector2D b)
{
	return { a.x - b.x, a.y - b.y };
}

Vector2D operator*(Vector2D vector, float scalar)
{
	return { vector.x * scalar, vector.y * scalar };
}

float dot(Vector2D a, Vector2D b)
{
	return a.x * b.x + a.y * b.y;
}

float lengthSquared(Vector2D vector)
{
	return dot(vector, vector);
}

float length(Vector2D vector)
{
	return (float)sqrt(lengthSquared(vector));
}

Vector2D normalize(Vector2D vector)
{
	float inverseLength = 1.0f / length(vector);
	return { vector.x * inverseLength, vector.y * inverseLength };
}



const Vector2D PaddleSize = { 0.1f, 0.025f };
const Vector2D CubeSize = { 0.015f, 0.02f };
const Vector2D WorldSize = { 1.0f, 5.0f / 4.0f };
const float PaddleY = 0.9f * WorldSize.y;
const float Border = 0.05f;
const float PaddleCornerWidth = PaddleSize.x * 0.1f;
const float AspectRatio = WorldSize.x / WorldSize.y;


GameMode::GameMode(int WindowWidth, int WindowHeight) :
	WindowWidth(WindowWidth),
	WindowHeight(WindowHeight),
	Time(0),
	bQuit(false),
	ColumnCount(0),
	ColumnSpacing(0),
	RowCount(0),
	RowSpacing(0),
	Seconds(0),
	MaxScore(0)
{
	Init();
	Run();
}

void GameMode::Init()
{
	/* Initialize SDL. If Initialization fails log error.  */
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		std::cerr << "SDL Initialization failed: " << SDL_GetError() << std::endl;
	}

	/* Open audio. If opening audio fails log error. */
	if ((Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048)) < 0)
	{
		std::cerr << "Open audio failed: " << SDL_GetError() << std::endl;
	}

	/* Initialize TTF. If Initialization fails log error */
	if (TTF_Init() == -1)
	{
		std::cerr << "TTF Initialization failed: " << SDL_GetError() << std::endl;
	}
	
	/* Open fonts */
	FontArial_16 = TTF_OpenFont("Assets/Fonts/arial.ttf", 16);
	FontArial_24 = TTF_OpenFont("Assets/Fonts/arial.ttf", 24);

	/* Create Window. If Window creation fails log error. */
	GameWindow = SDL_CreateWindow("Breakout", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WindowWidth, WindowHeight, SDL_WINDOW_SHOWN);
	if (GameWindow == nullptr)
	{
		std::cerr << "Creating Window failed: " << SDL_GetError() << std::endl;
	}

	/* Create Renderer. If Renderer creation fails log error. */
	GameRenderer = SDL_CreateRenderer(GameWindow, -1, SDL_RENDERER_PRESENTVSYNC);
	if (GameRenderer == nullptr)
	{
		std::cerr << "Creating Renderer failed: " << SDL_GetError() << std::endl;
	}

	Levels.push_back("Assets/Levels/Level1.xml");
	Levels.push_back("Assets/Levels/Level2.xml");
	Levels.push_back("Assets/Levels/Level3.xml");
}

void GameMode::SetBricks()
{
	/* Removes all the bricks from BricksInGame vector. Reduces capacity of the vector to fit its size. */
	BricksInGame.clear();
	BricksInGame.shrink_to_fit();

	MaxLevelScore = 0;
	Vector2D BrickSize = { (1 - 2 * Border - (ColumnCount + 1) * 0.0011875f) / ColumnCount, WorldSize.y * 0.025f };
	float TopOffset = BrickSize.y * 4;

	for (int i = 0; i < BricksLayout.size(); i++)
	{
		int ColumnCounter = 1;

		for (int j = 0; j < BricksLayout.at(i).size(); j++)
		{
			/* Sets Bricks position */
			BrickType Brick;
			Brick.brickBox.min = Vector2D{ Border + j * BrickSize.x + ColumnCounter * 0.0011875f, Border + TopOffset + i * BrickSize.y + i * 0.0022875f };
			Brick.brickBox.max = Brick.brickBox.min + BrickSize;

			ColumnCounter++;

			if (BricksLayout.at(i).at(j) == LevelBricks.at(0).Id.at(0))
			{
				Brick.Id = LevelBricks.at(0).Id;
				Brick.Texture = LevelBricks.at(0).Texture;
				Brick.HitPoints = LevelBricks.at(0).HitPoints;
				Brick.HitSound = LevelBricks.at(0).HitSound;
				Brick.BreakSound = LevelBricks.at(0).BreakSound;
				Brick.BreakScore = LevelBricks.at(0).BreakScore;
			}

			if (BricksLayout.at(i).at(j) == LevelBricks.at(1).Id.at(0))
			{
				Brick.Id = LevelBricks.at(1).Id;
				Brick.Texture = LevelBricks.at(1).Texture;
				Brick.HitPoints = LevelBricks.at(1).HitPoints;
				Brick.HitSound = LevelBricks.at(1).HitSound;
				Brick.BreakSound = LevelBricks.at(1).BreakSound;
				Brick.BreakScore = LevelBricks.at(1).BreakScore;

			}

			if (BricksLayout.at(i).at(j) == LevelBricks.at(2).Id.at(0))
			{
				Brick.Id = LevelBricks.at(2).Id;
				Brick.Texture = LevelBricks.at(2).Texture;
				Brick.HitPoints = LevelBricks.at(2).HitPoints;
				Brick.HitSound = LevelBricks.at(2).HitSound;
				Brick.BreakSound = LevelBricks.at(2).BreakSound;
				Brick.BreakScore = LevelBricks.at(2).BreakScore;
			}

			if (BricksLayout.at(i).at(j) == LevelBricks.at(3).Id.at(0))
			{
				Brick.Id = LevelBricks.at(3).Id;
				Brick.Texture = LevelBricks.at(3).Texture;
				Brick.HitPoints = INT_MAX;
				Brick.HitSound = LevelBricks.at(3).HitSound;
				Brick.BreakSound = "";
				Brick.BreakScore = 0;

			}

			if (BricksLayout.at(i).at(j) == '_') continue;

			MaxScore += Brick.BreakScore;
			MaxLevelScore += Brick.BreakScore;
			BricksInGame.push_back(Brick);
		}
	}
}

void GameMode::NextLevel()
{
	bShouldPause = true;
	CurrentScore = 0;
	UploadLevel(Levels.at(LevelCounter));
	ResetLevel();
}

void GameMode::ResetLevel()
{
	/* Resets positions of all Game objects */
	Paddle = { 0.5f, PaddleY };
	Cube = { 0.5f, PaddleY - PaddleSize.y };
	SetBricks();
	if (!bShouldPause) CubeDirection = normalize({ 0, -1 });
}

void GameMode::ResetGame()
{
	TimeForTime = SDL_GetTicks();
	BeforeTimeForTime = SDL_GetTicks();
	LifeCount = 4;
	LevelCounter = 0;
	Score = 0;
	CurrentScore = 0;
	MaxScore = 0;
	NextLevel();
}

void GameMode::Update(float MouseX, float MouseY, float Time)
{
	this->MouseX = MouseX;
	this->MouseY = MouseY;

	Paddle.x = MouseX / WindowWidth;
	Paddle.y = PaddleY;

	/* Paddle and Wall collision */
	if (Paddle.x - PaddleSize.x * 0.5f < Border)
	{
		Paddle.x = PaddleSize.x * 0.5f + Border;
	}

	else if (Paddle.x + PaddleSize.x * 0.5f > 1 - Border)
	{
		Paddle.x = 1 - Border - PaddleSize.x * 0.5f;
	}

	float TimeAllowed = Time;
	int HitIndex = -1;
	bool bCollisionDetected = false;
	Vector2D ChangeDirection = CubeDirection;
	std::string WallSoundPath = "Assets/Sounds/HitWall.wav";
	std::string PaddleSoundPath = "Assets/Sounds/HitPaddle.wav";

	Box2D CubeBox = { Cube - CubeSize * 0.5f, Cube + CubeSize * 0.5f };
	Box2D PaddleBox = { Paddle - PaddleSize * 0.5f, Paddle + PaddleSize * 0.5f };

	{
		/* Cube and Wall collision */
		if (CubeDirection.x > 0)
		{
			float TimeOfHit = (1 - Border - CubeBox.max.x) / CubeDirection.x;
			if ((TimeOfHit >= 0) && (TimeOfHit < TimeAllowed))
			{
				TimeAllowed = TimeOfHit;
				ChangeDirection = { -CubeDirection.x, CubeDirection.y };
				bCollisionDetected = true;
				HitIndex = -1;
				Mix_PlayChannel(-1, Mix_LoadWAV(WallSoundPath.c_str()), 0);
			}
		}

		else if (CubeDirection.x < 0)
		{
			float TimeOfHit = (Border - CubeBox.min.x) / CubeDirection.x;
			if ((TimeOfHit >= 0) && (TimeOfHit < TimeAllowed))
			{
				TimeAllowed = TimeOfHit;
				ChangeDirection = { -CubeDirection.x, CubeDirection.y };
				bCollisionDetected = true;
				HitIndex = -1;
				Mix_PlayChannel(-1, Mix_LoadWAV(WallSoundPath.c_str()), 0);
			}
		}

		if (CubeDirection.y < 0)
		{
			float TimeOfHit = (Border - CubeBox.min.y) / CubeDirection.y;
			if ((TimeOfHit >= 0) && (TimeOfHit < TimeAllowed))
			{
				TimeAllowed = TimeOfHit;
				ChangeDirection = { CubeDirection.x, -CubeDirection.y };
				bCollisionDetected = true;
				HitIndex = -1;
				Mix_PlayChannel(-1, Mix_LoadWAV(WallSoundPath.c_str()), 0);
			}
		}

		if (CubeDirection.y > 0)
		{
			float TimeOfHit = (PaddleBox.min.y - CubeBox.max.y) / CubeDirection.y;

			if ((TimeOfHit >= 0) && (TimeOfHit < TimeAllowed))
			{
				float NewCube = Cube.x + CubeDirection.x * TimeOfHit;
				float NewCubeMin = CubeBox.min.x + CubeDirection.x * TimeOfHit;
				float NewCubeMax = CubeBox.max.x + CubeDirection.x * TimeOfHit;

				/* Cube and Paddle collision */
				if ((NewCubeMax >= PaddleBox.min.x) && (PaddleBox.max.x >= NewCubeMin))
				{
					Mix_PlayChannel(-1, Mix_LoadWAV(PaddleSoundPath.c_str()), 0);
					if (NewCubeMin < PaddleBox.min.x + PaddleCornerWidth)
					{
						ChangeDirection = normalize({ -1,-1 });
					}

					else if (NewCubeMax > PaddleBox.max.x - PaddleCornerWidth)
					{
						ChangeDirection = normalize({ 1,-1 });
					}

					else if (NewCube <= Paddle.x)
					{
						ChangeDirection = normalize({ 0,-1 });
					}

					else
					{
						ChangeDirection = normalize({ 1,-1 });
					}

					TimeAllowed = TimeOfHit;
					bCollisionDetected = true;
					HitIndex = -1;

				}
			}
		}

		
		/* Cube and Bricks collision */
		for (int i = 0; i < BricksInGame.size(); i++)
		{
			auto Brick = &BricksInGame[i];

			if (CubeDirection.x > 0)
			{
				float TimeOfHit = (Brick->brickBox.min.x - CubeBox.max.x) / CubeDirection.x;
				if ((TimeOfHit >= 0) && (TimeOfHit < TimeAllowed))
				{
					float NewCubeMin = CubeBox.min.y + CubeDirection.y * TimeOfHit;
					float NewCubeMax = CubeBox.max.y + CubeDirection.y * TimeOfHit;

					if ((NewCubeMax >= Brick->brickBox.min.y) && (Brick->brickBox.max.y >= NewCubeMin))
					{
						TimeAllowed = TimeOfHit;
						ChangeDirection = { -CubeDirection.x, CubeDirection.y };
						bCollisionDetected = true;
						HitIndex = i;
					}
				}
			}

			else if (CubeDirection.x < 0)
			{
				float TimeOfHit = (Brick->brickBox.max.x - CubeBox.min.x) / CubeDirection.x;
				if ((TimeOfHit >= 0) && (TimeOfHit < TimeAllowed))
				{
					float NewCubeMin = CubeBox.min.y + CubeDirection.y * TimeOfHit;
					float NewCubeMax = CubeBox.max.y + CubeDirection.y * TimeOfHit;

					if ((NewCubeMax >= Brick->brickBox.min.y) && (Brick->brickBox.max.y >= NewCubeMin))
					{
						TimeAllowed = TimeOfHit;
						ChangeDirection = { -CubeDirection.x, CubeDirection.y };
						bCollisionDetected = true;
						HitIndex = i;
					}
				}
			}

			if (CubeDirection.y > 0)
			{
				float TimeOfHit = (Brick->brickBox.min.y - CubeBox.max.y) / CubeDirection.y;

				if ((TimeOfHit >= 0) && (TimeOfHit < TimeAllowed))
				{
					float NewCubeMin = CubeBox.min.x + CubeDirection.x * TimeOfHit;
					float NewCubeMax = CubeBox.max.x + CubeDirection.x * TimeOfHit;

					if ((NewCubeMax >= Brick->brickBox.min.x) && (Brick->brickBox.max.x >= NewCubeMin))
					{
						TimeAllowed = TimeOfHit;
						ChangeDirection = { CubeDirection.x, -CubeDirection.y };
						bCollisionDetected = true;
						HitIndex = i;
					}
				}

			}

			else if (CubeDirection.y < 0)
			{
				float TimeOfHit = (Brick->brickBox.max.y - CubeBox.min.y) / CubeDirection.y;
				if ((TimeOfHit >= 0) && (TimeOfHit < TimeAllowed))
				{
					float NewCubeMin = CubeBox.min.x + CubeDirection.x * TimeOfHit;
					float NewCubeMax = CubeBox.max.x + CubeDirection.x * TimeOfHit;

					if ((NewCubeMax >= Brick->brickBox.min.x) && (Brick->brickBox.max.x >= NewCubeMin))
					{
						TimeAllowed = TimeOfHit;
						ChangeDirection = { CubeDirection.x, -CubeDirection.y };
						bCollisionDetected = true;
						HitIndex = i;
					}
				}
			}
		}

		// Slow down cube speed *0.6f
		Cube = Cube + CubeDirection * TimeAllowed * 0.6f;

		if (bCollisionDetected)
		{
			CubeDirection = ChangeDirection;

			if (HitIndex != -1)
			{
				auto Brick = &BricksInGame[HitIndex];

				if (Brick->HitPoints > 0)
				{
					Brick->HitPoints--;
					Mix_PlayChannel(-1, Mix_LoadWAV((Brick->HitSound).c_str()), 0);
				}

				if (Brick->HitPoints == 0)
				{
					Mix_PlayChannel(-1, Mix_LoadWAV((Brick->BreakSound).c_str()), 0);
					CurrentScore += Brick->BreakScore;
					Score += Brick->BreakScore;
					BricksInGame[HitIndex] = BricksInGame.back();
					BricksInGame.pop_back();

					
					std::cout << "HIT!" << std::endl;
					std::cout << "CurrentScore: " << CurrentScore << std::endl;
					std::cout << "MaxLevelScore: " << MaxLevelScore << std::endl;
					std::cout << "LevelCounter: " << LevelCounter << std::endl;
					std::cout << "Score: " << Score << std::endl;
					std::cout << "MaxScore: " << MaxScore << std::endl;

					if (CurrentScore == MaxLevelScore && LevelCounter < Levels.size() - 1)
					{
						bShouldPause = true;
						if (LevelCounter <= 1) LevelCounter++;
						NextLevel();
					}

					else if (CurrentScore == MaxLevelScore && LevelCounter == Levels.size() - 1)
					{
						bGameOver = true;
					}

				}
			}
		}

		if (CubeBox.min.y >= WorldSize.y)
		{
			if (LifeCount == 0)
			{
				LevelCounter = 0;
				bGameOver = true;
				bShouldPause = true;
				ResetGame();
			}

			else if (LifeCount > 0)
			{
				Score = Score - CurrentScore;
				MaxScore = Score;
				CurrentScore = 0;
				LifeCount--;
				bShouldPause = true;
				ResetLevel();
			}
		}

	}

}

void GameMode::Run()
{
	BeforeTime = SDL_GetTicks();
	BeforeTimeForTime = SDL_GetTicks();

	bShouldPause = true;
	bStartGame = true;
	ResetGame();

	while (!bQuit)
	{
		SDL_Event Event;

		if (bGameOver) {
			std::cout << "Game END!" << std::endl;
			RenderGameOver();
		}
			
		/* Handle events */
		while (SDL_PollEvent(&Event) || bGameOver)
		{
			if (Event.type == SDL_QUIT)
			{
				bQuit = true;
				break;
			}

			if (Event.type == SDL_MOUSEMOTION)
			{
				SDL_ShowCursor(SDL_DISABLE);
				MouseX = (float)Event.motion.x;
				MouseY = (float)Event.motion.y;
			}

			if (bShouldPause)
			{
				if (Event.type == SDL_KEYDOWN)
				{
					if (Event.key.keysym.sym == SDLK_SPACE)
					{
						std::cout << "SPACE pressed - Release cube!" << std::endl;
						bShouldPause = false;
						if (bStartGame) CubeDirection = normalize({ 0, -1 });
						break;
					}
				}
			}

			if (Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_RETURN)
			{
				if (bGameOver) {
					std::cout << "ENTER pressed - End game!" << std::endl;
					Seconds = 0;
					bGameOver = false;
					ResetGame();
					break;
				}
			}
		}

		unsigned int time = SDL_GetTicks();
		TimeForTime = SDL_GetTicks();
		float timeStep = (float)(time - BeforeTime) / 1000.0f;
		Seconds = (TimeForTime - BeforeTimeForTime) / 1000.0f;
		BeforeTime = time;

		if (!bGameOver) Render();
		if (!bShouldPause && !bGameOver) Update(MouseX, MouseY, timeStep);

		SDL_RenderPresent(GameRenderer);
		SDL_RenderClear(GameRenderer);

	}

	SDL_DestroyRenderer(GameRenderer);
	SDL_DestroyWindow(GameWindow);
	TTF_Quit();
	SDL_Quit();
}

void GameMode::RenderGameInfo(float x, float y, std::string Text, int value, bool GameOver)
{
	if (GameOver)
	{
		SDL_Color color = { 0, 0, 0, 255 };
		SDL_Surface* GameOverSurface = TTF_RenderText_Solid(FontArial_24, Text.c_str(), color);
		SDL_Texture* GameOverTexture = SDL_CreateTextureFromSurface(GameRenderer, GameOverSurface);
		SDL_Rect GameOverRect = { (int)x, (int)y, 0,0 };
		SDL_Rect dst = { int((WindowWidth - GameOverSurface->w) / 2), int((WindowHeight - Border*WindowWidth + GameOverSurface->h) / 2), 0, 0 };
		SDL_QueryTexture(GameOverTexture, NULL, NULL, &dst.w, &dst.h);
		SDL_RenderCopy(GameRenderer, GameOverTexture, NULL, &dst);
		SDL_FreeSurface(GameOverSurface);
		SDL_DestroyTexture(GameOverTexture);

		SDL_RenderPresent(GameRenderer);
		SDL_RenderClear(GameRenderer);
	}

	else
	{
		SDL_Color color = { 0, 0, 0, 255 };
		SDL_Surface* InfoSurface = TTF_RenderText_Solid(FontArial_16, (Text + std::to_string(value)).c_str(), color);
		SDL_Texture* InfoTexture = SDL_CreateTextureFromSurface(GameRenderer, InfoSurface);
		SDL_Rect InfoRect = { (int)x, (int)y, 0,0 };
		SDL_QueryTexture(InfoTexture, NULL, NULL, &InfoRect.w, &InfoRect.h);
		SDL_RenderCopy(GameRenderer, InfoTexture, NULL, &InfoRect);

		SDL_FreeSurface(InfoSurface);
		SDL_DestroyTexture(InfoTexture);
	}
}

void GameMode::RenderRectFrame(float x, float y, float w, float h, SDL_Color Color)
{
	SDL_SetRenderDrawColor(GameRenderer, Color.r, Color.g, Color.b, Color.a);
	SDL_Rect ColorRect = { (int)x, (int)y, (int)w, (int)h };
	SDL_RenderDrawRect(GameRenderer, &ColorRect);
}

void GameMode::RenderTexture(float x, float y, float w, float h, std::string Texture)
{
	DirectX::TexMetadata MetaData;
	DirectX::ScratchImage ScratchImage;
	std::string path = Texture;
	std::wstring textures(path.begin(), path.end());

	if (DirectX::LoadFromDDSFile(textures.c_str(), DirectX::DDS_FLAGS_NONE, &MetaData, ScratchImage) != S_OK) {}

	DirectX::ScratchImage ResizedImage;
	if (DirectX::Resize(ScratchImage.GetImages(), ScratchImage.GetImageCount(), ScratchImage.GetMetadata(), size_t(w), size_t(h), DirectX::TEX_FILTER_DEFAULT, ResizedImage) != S_OK) {}

	SDL_Surface* sdlSurface = SDL_CreateRGBSurfaceFrom(ResizedImage.GetPixels(), static_cast<int>(ResizedImage.GetMetadata().width), static_cast<int>(ResizedImage.GetMetadata().height), static_cast<int>(32), static_cast<int>(ResizedImage.GetMetadata().width * 4), 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
	SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(GameRenderer, sdlSurface);

	SDL_Rect dest = { (int)x, (int)y, (int)w, (int)h };
	SDL_RenderCopy(GameRenderer, sdlTexture, NULL, &dest);

	SDL_FreeSurface(sdlSurface);
	SDL_DestroyTexture(sdlTexture);

}

void GameMode::RenderMinAndSizeTexture(Vector2D worldMin, Vector2D worldSize, std::string Texture, bool Frame)
{
	Vector2D Min = { worldMin.x * WindowWidth, worldMin.y * WindowHeight * AspectRatio };
	Vector2D Size = { worldSize.x * WindowWidth, worldSize.y * WindowHeight * AspectRatio };
	if (!Frame) RenderTexture(Min.x, Min.y, Size.x, Size.y, Texture);
	else RenderRectFrame(Min.x, Min.y, Size.x, Size.y, { 220,220,220,255 });

}

void GameMode::RenderMinAndMaxTexture(Vector2D worldMin, Vector2D worldMax, std::string Texture, bool Frame)
{
	RenderMinAndSizeTexture(worldMin, worldMax - worldMin, Texture, Frame);
}

void GameMode::RenderBorder()
{
	std::string Path = "Assets/Textures/Border/Border.dds";
	RenderMinAndMaxTexture({ 0,0 }, { Border, WorldSize.y }, Path, false);
	RenderMinAndMaxTexture({ 1 - Border, 0 }, { 1, WorldSize.y }, Path, false);
	RenderMinAndMaxTexture({ 0,0 }, { 1, Border }, Path, false);
}

void GameMode::RenderGameOver()
{
	RenderBorder();

	if (Score == MaxScore)
	{
		RenderGameInfo(WindowWidth / 2.f, WindowHeight / 2.f, "You WIN! Press Enter to start again!", 0, true);
	}
	else
	{
		RenderGameInfo(WindowWidth / 2.f, WindowHeight / 2.f, "GameOver! Press Enter to start again!", 0, true);
	}
}

void GameMode::Render()
{
	/* Background */
	RenderTexture(Border * WindowWidth, Border * WindowHeight - 10, WindowWidth - 2 * Border * WindowWidth, WindowHeight - Border * WindowHeight + 10, BackgroundPath);

	/* Left Corner */
	RenderMinAndSizeTexture(Paddle - PaddleSize * 0.5f, Vector2D{ PaddleCornerWidth, PaddleSize.y }, "Assets/Textures/Paddle/Paddle.dds", false);

	/* Right Corner */
	RenderMinAndSizeTexture(Paddle + Vector2D{ PaddleSize.x * 0.5f - PaddleCornerWidth, PaddleSize.y * (-0.5f) }, Vector2D{ PaddleCornerWidth, PaddleSize.y }, "Assets/Textures/Paddle/Paddle.dds", false);

	/* Paddle*/
	RenderMinAndSizeTexture(Paddle - PaddleSize * 0.5f + Vector2D{ PaddleCornerWidth, 0 }, PaddleSize - Vector2D{ PaddleCornerWidth * 2, 0 }, "Assets/Textures/Paddle/Paddle.dds", false);

	/* Cube*/
	RenderMinAndSizeTexture(Cube - CubeSize * 0.5f, CubeSize, "Assets/Textures/Cube/Cube.dds", false);

	/* Bricks */
	for (int i = 0; i < BricksInGame.size(); i++)
	{
		auto Brick = &BricksInGame[i];
		RenderMinAndMaxTexture(Brick->brickBox.min, Brick->brickBox.max, Brick->Texture, false);
		RenderMinAndMaxTexture(Brick->brickBox.min, Brick->brickBox.max, Brick->Texture, true);
	}

	// Borders  
	RenderBorder();

	/* GameInfo */
	RenderGameInfo(Border * WindowWidth, 4, "Level: ", LevelCounter + 1, false);
	RenderGameInfo(Border * WindowWidth + WindowWidth * 0.175f, 4, "Lives: ", LifeCount, false);
	RenderGameInfo(Border * WindowWidth + WindowWidth * 0.3f, 4, "Score: ", Score, false);
	RenderGameInfo(Border * WindowWidth + WindowWidth * 0.425f, 4, "Time: ", int(Seconds), false);

}

void GameMode::UploadLevel(const char* Level)
{
	BricksLayout.clear();
	BricksLayout.shrink_to_fit();
	BackgroundPath.clear();

	tinyxml2::XMLDocument Document;
	if (Document.LoadFile(Level) == XML_SUCCESS)
	{
		XMLElement* LevelElement = Document.FirstChildElement("Level");
		LevelElement->QueryIntAttribute("RowCount", &RowCount);
		LevelElement->QueryIntAttribute("ColumnCount", &ColumnCount);
		LevelElement->QueryIntAttribute("RowSpacing", &RowSpacing);
		LevelElement->QueryIntAttribute("ColumnSpacing", &ColumnSpacing);

		BackgroundPath = LevelElement->Attribute("BackgroundTexture");

		XMLElement* BrickTypesElement = LevelElement->FirstChildElement("BrickTypes");
		std::vector<XMLElement*> BrickTypeElements;

		for (XMLElement* BrickTypeElement = BrickTypesElement->FirstChildElement("BrickType"); BrickTypeElement != nullptr; BrickTypeElement = BrickTypeElement->NextSiblingElement("BrickType"))
		{
			BrickTypeElements.push_back(BrickTypeElement);
		}

		for (int i = 0; i < BrickTypeElements.size(); i++)
		{
			BrickType Brick;

			Brick.Id = BrickTypeElements.at(i)->Attribute("Id");
			Brick.Texture = BrickTypeElements.at(i)->Attribute("Texture");
			Brick.HitPoints = atoi(BrickTypeElements.at(i)->Attribute("HitPoints"));
			Brick.HitSound = BrickTypeElements.at(i)->Attribute("HitSound");

			if (i != BrickTypeElements.size() - 1)
			{
				Brick.BreakSound = BrickTypeElements.at(i)->Attribute("BreakSound");
				Brick.BreakScore = atoi(BrickTypeElements.at(i)->Attribute("BreakScore"));
			}

			else
			{
				Brick.BreakSound = "";
				Brick.BreakScore = INT_MAX;
			}

			LevelBricks.push_back(Brick);
		}

		XMLElement* BricksElement = LevelElement->FirstChildElement("Bricks");
		std::string Bricks = BricksElement->GetText();


		std::stringstream StringStream(Bricks);
		std::string SingleLine;

		while (std::getline(StringStream, SingleLine))
		{
			std::vector<char> SingleRow;

			for (char Character : SingleLine)
			{
				if (Character != ' ')
				{
					SingleRow.push_back(Character);
				}
			}

			BricksLayout.push_back(SingleRow);
		}
	}
}
