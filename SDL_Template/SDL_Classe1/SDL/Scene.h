#pragma once
#include "GameObject.h"
#include <vector>

class Scene {
protected:
	std::vector<GameObject*> objects;
	bool finished = false;
	std::string targetScene;

public:
	virtual ~Scene() {
		for (GameObject* o : objects) {
			delete o;
		}
		objects.clear();
	}

	virtual void Start(SDL_Renderer *rend) {
		for (GameObject* o : objects) {
			delete o;
		}
		objects.clear();
		finished = false;
	}

	virtual void Update(float dt) {
		for (GameObject* var : objects) {
			if (var != nullptr) {
				var->Update(dt);
			}
		}
	}

	virtual void Render(SDL_Renderer* rend) {
		for (GameObject* var : objects) {
			if (var != nullptr && rend != nullptr) {
				var->Render(rend);
			}
		}
	}

	virtual void Exit() = 0;

	bool IsFinished() { return finished; }
	std::string GetTargetScene() { return targetScene; }

};