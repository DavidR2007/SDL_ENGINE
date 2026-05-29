#pragma once
#include "Scene.h"

class HighscoreScene : public Scene {

public:
	HighscoreScene() : Scene() {}

	void Start(SDL_Renderer* rend) override { Scene::Start(rend); }
	void Update(float dt) override {};
	void Render(SDL_Renderer* rend) override {};
	void Exit() override {};

};