#include "tetris.h"
#include"SFML/Graphics.hpp"

//////////////////////////////////////////////////////////////////////////////

Tetris::~Tetris()
{
	DestroyField();
}

//////////////////////////////////////////////////////////////////////////////

void Tetris::Game()
{
	const int tileSize = _tetrisTextureData.tileSize;

	sf::Sprite sprite(tileTexture);
	sprite.setTextureRect(sf::IntRect(0, 0, tileSize, tileSize));

	sf::Sprite backgroundSprite(backgroundTexture);
	sf::Sprite borderSprite(borderTexture);

	sf::Font scoreFont;
	scoreFont.loadFromFile("./Res/BREME.TTF");

	sf::Text scoreText;
	scoreText.setFont(scoreFont);
	scoreText.setString(std::to_string(_gameScore));
	scoreText.setCharacterSize(24); 
	scoreText.setFillColor(sf::Color::White);
	scoreText.setOutlineThickness(2);
	scoreText.setStyle(sf::Text::Regular);

	//Gameplay variables
	int dx = 0;
	bool rotate = false;
	bool createTetramino = true;
	int colorNum = 1;
	float timer = 0.f;
	float delay = 0.3f;

	sf::Clock clock;

	while (window.isOpen())
	{

		timer += clock.getElapsedTime().asSeconds();
		clock.restart();

		///////////////////////////////////////////////////////////////
		//Handle inputs
		///////////////////////////////////////////////////////////////
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
				break;
			}

			switch (_gameplayState)
			{
				case GameplayState::PLAY: 
				{
					if (event.type == sf::Event::KeyPressed)
					{
						if (event.key.code == sf::Keyboard::Up) rotate = true;
						else if (event.key.code == sf::Keyboard::Left) dx = -1;
						else if (event.key.code == sf::Keyboard::Right) dx = 1;

						// Accelerate tetranimo fall by reduce delay between calculation 
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) delay = 0.05;

					}

					break;
				}
				case GameplayState::DEFEAT:
				{
					if (event.type == sf::Event::KeyPressed)
					{
						if (event.key.code == sf::Keyboard::R)
						{
							ResetFieldToDefaut();
							_gameScore = 0;
							_gameplayState = GameplayState::PLAY;
							break;
						}

						if (event.key.code == sf::Keyboard::Q)
						{
							window.close();
							break;
						}

					}

					break;
				}
			}
		}

		///////////////////////////////////////////////////////////////
		//Handle logic update
		///////////////////////////////////////////////////////////////
		switch (_gameplayState)
		{
			case GameplayState::PLAY: 
			{
				int tetraminoType = 3; // Start tetranimo type

				if (createTetramino)
				{
					colorNum = 1 + rand() % 7;
					tetraminoType = rand() % 7;

					for (int i = 0; i < 4; i++)
					{
						moveableTetranimo[i].x = figures[tetraminoType][i] % 2;
						moveableTetranimo[i].y = figures[tetraminoType][i] / 2;
					}
					createTetramino = false;

					//Check if tetranimo spawned on any object. In case true - end game
					if (!CollisionCheck())
					{
						_gameplayState = GameplayState::PLAY_TO_DEFEAT_TRANSITION;
					}
				}

				//// Horizontal move ////
				if (dx)
				{
					for (int i = 0; i < 4; i++)
					{
						previousTetranimo[i] = moveableTetranimo[i]; moveableTetranimo[i].x += dx;
					}

					// In case tetranimo out of field after move - roll back position 
					if (!CollisionCheck()) for (int i = 0; i < 4; i++) moveableTetranimo[i] = previousTetranimo[i];
				}

				//// Tetranimo rotation ////
				if (rotate)
				{
					Point rotationCenter = moveableTetranimo[1];
					for (int i = 0; i < 4; i++)
					{
						int x = moveableTetranimo[i].y - rotationCenter.y; // y - y0
						int y = moveableTetranimo[i].x - rotationCenter.x; // x - x0
						moveableTetranimo[i].x = rotationCenter.x - x;
						moveableTetranimo[i].y = rotationCenter.y + y;
					}

					//In case tetranimo out of field after rotation - roll back position
					if (!CollisionCheck()) for (int i = 0; i < 4; i++) moveableTetranimo[i] = previousTetranimo[i];

				}

				//// Vertical tetranimo move ////
				if (timer > delay)
				{
					for (int i = 0; i < 4; i++)
					{
						previousTetranimo[i] = moveableTetranimo[i]; moveableTetranimo[i].y += 1;
					}

					// In case tetranimo can't fall more - register it on field 
					if (!CollisionCheck())
					{
						//Register object on field with the previous position
						for (int i = 0; i < 4; i++) field[previousTetranimo[i].y][previousTetranimo[i].x] = colorNum;

						_gameScore += 10;
						createTetramino = true;

					}

					timer = 0;
				}

				//// Horizontal line check ////
				int currentLine = _fieldHeight - 1;
				for (int i = _fieldHeight - 1; i > 0; i--)
				{
					int count = 0;
					for (int j = 0; j < _fieldWidth; j++)
					{
						if (field[i][j]) count++;
						field[currentLine][j] = field[i][j];
					}

					if (count < _fieldWidth)
					{
						currentLine--;
					}
					else
					{
						_gameScore += 100;
					}
				}

				break;
			}
			case GameplayState::DEFEAT: break;
		}
		///////////////////////////////////////////////////////////////
		//Handle draw update
		///////////////////////////////////////////////////////////////
		
		scoreText.setString(std::to_string(_gameScore));
		scoreText.setPosition(sf::Vector2f(_windowWidth - scoreText.getLocalBounds().width, 20));

		///////////////////////////////////////////////////////////////
		//Handle draw
		///////////////////////////////////////////////////////////////
		switch (_gameplayState)
		{
			case GameplayState::PLAY:
			{
				window.clear(sf::Color::Cyan);

				window.draw(backgroundSprite);

				//Field draw
				for (int i = 0; i < _fieldHeight; i++)
					for (int j = 0; j < _fieldWidth; j++)
					{
						if (field[i][j] == 0) continue;
						sprite.setTextureRect(sf::IntRect(field[i][j] * tileSize, 0, tileSize, tileSize));
						sprite.setPosition(j * tileSize, i * tileSize);
						sprite.move(28, 31); // offset
						window.draw(sprite);
					}

				//Moveable tetranimo draw
				for (int i = 0; i < 4; i++)
				{
					sprite.setTextureRect(sf::IntRect(colorNum * tileSize, 0, tileSize, tileSize));
					sprite.setPosition(moveableTetranimo[i].x * 18, moveableTetranimo[i].y * 18);
					sprite.move(28, 31); // offset
					window.draw(sprite);
				}

				window.draw(borderSprite);
				window.draw(scoreText);
				window.display();

				break;
			}

			case GameplayState::PLAY_TO_DEFEAT_TRANSITION:
			{
				sf::RectangleShape rectagle(sf::Vector2f(_windowWidth,_windowHeight));
				rectagle.setFillColor(sf::Color(0, 0, 0, 150));

				sf::Text defeatText;
				defeatText.setFont(scoreFont);
				defeatText.setString(L"Defeat");
				defeatText.setCharacterSize(60);
				defeatText.setFillColor(sf::Color::Red);
				defeatText.setOutlineThickness(2);
				defeatText.setLetterSpacing(2);

				sf::Rect textRect = defeatText.getLocalBounds();
				defeatText.setPosition(sf::Vector2f(_windowWidth / 2 - textRect.width/2, _windowHeight / 4));

				sf::Text navigationText;
				navigationText.setFont(scoreFont);
				navigationText.setString(L"R - restart the game\nQ - quit the game");
				navigationText.setCharacterSize(24);
				navigationText.setFillColor(sf::Color::Red);
				navigationText.setOutlineThickness(2);

				textRect = navigationText.getLocalBounds();
				navigationText.setPosition(sf::Vector2f(_windowWidth / 2 - textRect.width / 2, _windowHeight / 2 ));

				
				
				window.draw(rectagle);
				window.draw(defeatText);
				window.draw(navigationText);
				window.display();
				_gameplayState = GameplayState::DEFEAT;
			}
			case GameplayState::DEFEAT:
			{
				break;
			}
		}
		///////////////////////////////////////////////////////////////
		//Handle var defaults
		///////////////////////////////////////////////////////////////

		dx = 0;
		rotate = false;
		delay = 0.3f;
	}
}

//////////////////////////////////////////////////////////////////////////////

bool Tetris::CollisionCheck()
{
	for (int i = 0; i < 4; i++)
		if (moveableTetranimo[i].x < 0 || moveableTetranimo[i].x >= _fieldWidth || moveableTetranimo[i].y >= _fieldHeight) return 0;
		else if (field[moveableTetranimo[i].y][moveableTetranimo[i].x]) return 0;

	return 1;
}

//////////////////////////////////////////////////////////////////////////////

void Tetris::InitFieldDefault()
{
	field = new int* [_fieldHeight];
	for (int i = 0; i < _fieldHeight; ++i)
	{
		field[i] = new int[_fieldWidth];
		for (int j = 0; j < _fieldWidth; j++)
		{
			field[i][j] = 0;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

void Tetris::ResetFieldToDefaut()
{
	for (int i = 0; i < _fieldHeight; ++i)
	{
		for (int j = 0; j < _fieldWidth; j++)
		{
			field[i][j] = 0;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

void Tetris::DestroyField()
{
	for (int i = 0; i < _fieldHeight; ++i)
	{
		delete[] field[i];
	}

	delete[] field;
	field = nullptr;
}

//////////////////////////////////////////////////////////////////////////////

void Tetris::InitWindow()
{
	InitWindowInternal(_windowHeight, _windowWidth);
}

//////////////////////////////////////////////////////////////////////////////

void Tetris::InitWindow(int inputHeight, int inputWidth)
{
	InitWindowInternal(inputHeight, inputWidth);
}

//////////////////////////////////////////////////////////////////////////////

void Tetris::InitWindowInternal(int inputHeight, int inputWidth)
{
	_windowHeight = inputHeight;
	_windowWidth = inputWidth;
	window.create(sf::VideoMode(inputWidth, inputHeight), L"Tetris Game");
}

//////////////////////////////////////////////////////////////////////////////

bool Tetris::LoadTextures(TetrisTextureDataSctruct inputData)
{
	_tetrisTextureData = inputData;

	if (!backgroundTexture.loadFromFile(_tetrisTextureData.backgroundTexture))
	{
		return false;
	}

	if (!tileTexture.loadFromFile(_tetrisTextureData.tileTexture))
	{
		return false;
	}

	if (!borderTexture.loadFromFile(_tetrisTextureData.borderTexture))
	{
		return false;
	}

	/*if (!fieldBackgroundTexture.loadFromFile(_tetrisTextureData.fieldBackgroundTexture))
	{
		return false;
	}*/

	return true;
}

//////////////////////////////////////////////////////////////////////////////

