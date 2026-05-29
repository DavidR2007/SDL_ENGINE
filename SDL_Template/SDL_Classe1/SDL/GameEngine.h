#pragma once
#include <SDL.h>
#include <iostream>
#include <map>

#include "GameObject.h"
#include "InputManager.h"
#include "Scene.h"
#include "MenuScene.h"
#include "HighscoreScene.h"
#include "GameplayScene.h"

class GameEngine
{
private:
	SDL_Window* window;
	SDL_Renderer* renderer;

	void InitSDL();
	void InitWindowAndRenderer(int windowWith, int windowHeight);

public:
	GameEngine(int windowWith, int windowHeight);

	void Update();
	void Finish();
};

