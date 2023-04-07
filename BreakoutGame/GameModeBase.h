#pragma once
class GameModeBase
{

public:
	GameModeBase() {}
	virtual void Init() = 0;
	virtual void ResetGame() = 0;
	virtual void NextLevel() = 0;
	virtual void ResetLevel() = 0;
	virtual void Update(float MouseX, float MouseY, float Time) = 0;
	virtual void Render() = 0;
	virtual void Run() = 0;
};
