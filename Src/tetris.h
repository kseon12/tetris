#pragma once
#include <string>
#include "SFML/Graphics.hpp"

struct TetrisTextureDataSctruct
{
	std::string backgroundTexture = "./Res/img/background.png";
	std::string tileTexture = "./Res/img/tiles.png";
	std::string borderTexture = "./Res/img/frame.png";
	//std::string fieldBackgroundTexture = "";

	int tileSize = 18;
};

struct Point
{
	int x, y;
};

enum class GameplayState
{
	START,
	PLAY,
	PAUSE,
	DEFEAT,
	PLAY_TO_DEFEAT_TRANSITION
};

class Tetris
{
private:
	TetrisTextureDataSctruct _tetrisTextureData;
	
	int _windowHeight = 480;
	int _windowWidth = 320;
	int _fieldWidth = 10;
	int _fieldHeight = 20;
	sf::RenderWindow window;

	sf::Texture backgroundTexture, tileTexture, borderTexture, fieldBackgroundTexture;

	GameplayState _gameplayState = GameplayState::PLAY;
	int _gameScore = 0;

	int** field = nullptr;

	int figures[7][4] 
	{
		{1,3,5,7}, // I
		{2,4,5,7}, // S
		{3,5,4,6}, // Z
		{3,5,4,7}, // T
		{2,3,5,7}, // L
		{3,5,7,6}, // J
		{2,3,4,5}, // O
	};

	Point moveableTetranimo[4], previousTetranimo[4];

	void InitWindowInternal(int inputHeight, int inputWidth);

public:

	Tetris()=default;
	~Tetris();

	void Game();
	bool CollisionCheck();

	void InitFieldDefault();
	void ResetFieldToDefaut();
	void DestroyField();

	void InitWindow();
	void InitWindow(int inputHeight, int inputWidth);

	bool LoadTextures(TetrisTextureDataSctruct inputData);

};

