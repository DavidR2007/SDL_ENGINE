#pragma once
#include "GameObject.h"

class Bullet : public GameObject {
	float lifetime = 0.f;
	bool dead = false;

public:
	Bullet(SDL_Renderer* renderer, Vector2 pos, Vector2 velocity);

	void Update(float dt) override;

	bool IsDead() const { return dead; }
	void Hit();
};
