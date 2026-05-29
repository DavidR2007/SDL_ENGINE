#pragma once
#include "GameObject.h"
#include <vector>

enum class AsteroidTier { Large, Medium, Small };

class Asteroid : public GameObject {
	AsteroidTier tier = AsteroidTier::Large;
	bool dead = false;

public:
	Asteroid(SDL_Renderer* renderer, Vector2 pos, AsteroidTier t, Vector2 vel, float angularVelDeg);

	bool IsDead() const { return dead; }
	AsteroidTier GetTier() const { return tier; }

	/** Marca el meteorito como destruido y encola 0–2 hijos más pequeños (como el Asteroids clásico). */
	void ExplodeFromShot(SDL_Renderer* renderer, std::vector<Asteroid*>& outSpawn);
};
