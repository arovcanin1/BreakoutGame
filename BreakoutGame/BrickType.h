#pragma once


struct BrickType {
	const char* Id;
	const char* Texture; 
	int HitPoints;
	const char* HitSound;
	const char* BreakSound;
	int BreakScore;
};