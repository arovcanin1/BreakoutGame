#pragma once
class GameModeBase
{

public:
	GameModeBase() {}
	virtual void Update(float MouseX, float MouseY, float Time) = 0;
	virtual void Render() = 0;
	virtual void Run() = 0;
};
