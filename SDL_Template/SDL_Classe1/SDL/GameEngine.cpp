#include "GameEngine.h"
#include <SDL_ttf.h>

GameEngine::GameEngine(int windowWith, int windowHeight) {
	InitSDL();
	InitWindowAndRenderer(windowWith, windowHeight);

}

void GameEngine::InitSDL() {
	int result = SDL_Init(SDL_INIT_VIDEO);

	if (result < 0) {
		std::cout << "SDL_Init error:" << SDL_GetError();
	}
	if (TTF_Init() == -1) {
		std::cout << "TTF_Init error:" << TTF_GetError();
	}
}

void GameEngine::InitWindowAndRenderer(int windowWith, int windowHeight) {
	window = SDL_CreateWindow("MyFirstWindow",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		windowWith, windowHeight,
		SDL_WINDOW_SHOWN);

	if (window == nullptr)
		std::cout << "Error creating window:" << SDL_GetError();

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (renderer == nullptr)
		std::cout << "Error creating renderer:" << SDL_GetError();
}

void GameEngine::Update() {
	//TIME CONTROL
	float dt = 0.0f;
	float lastTime = SDL_GetPerformanceCounter() / (float)SDL_GetPerformanceFrequency();

	const int FPS = 60;
	const float frameTime = 1.0f / (float)FPS;

	//SCENES
	std::map<std::string, Scene*> gameScene;
	gameScene["MainMenu"] = new MenuScene();
	gameScene["Gameplay"] = new GamePlayScene();
	gameScene["HighScore"] = new HighscoreScene();

	Scene* currentScene = gameScene["Gameplay"];
	currentScene->Start(renderer);

	while (!IM.GetQuit())
	{
		//DELTA TIME CONTROL
		float currentTime = SDL_GetPerformanceCounter() / (float)SDL_GetPerformanceFrequency();
		dt += currentTime - lastTime;
		lastTime = currentTime;

		if (dt > frameTime) {
			//INPUT
			IM.Listen();

			//UPDATE
			currentScene->Update(dt);

			//RENDER
			SDL_RenderClear(renderer);
			currentScene->Render(renderer);
			SDL_RenderPresent(renderer);

			//SCENE TRANSITION
			if (currentScene->IsFinished()) {
				currentScene->Exit();
				currentScene = gameScene[currentScene->GetTargetScene()];
				currentScene->Start(renderer);
			}

			dt -= frameTime;
		}
	}

	for (auto& pair : gameScene) {
		delete pair.second;
	}
	gameScene.clear();
}

void GameEngine::Finish() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();
}
