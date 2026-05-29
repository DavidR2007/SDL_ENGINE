#pragma once
#include "GameObject.h"
#include <vector>

enum class CategoriaAsteroide { Grande, Mediano, Pequeno };

class Asteroide : public GameObject {
	CategoriaAsteroide categoria = CategoriaAsteroide::Grande;
	bool muerto = false;

public:
	Asteroide(SDL_Renderer* renderer, Vector2 pos, CategoriaAsteroide t, Vector2 vel, float angularVelDeg);

	bool EstaMuerto() const { return muerto; }
	CategoriaAsteroide ObtenerCategoria() const { return categoria; }

	/** Marca el meteorito como destruido y encola 0–2 hijos más pequeños (como el Asteroids clásico). */
	void ExplotarPorDisparo(SDL_Renderer* renderer, std::vector<Asteroide*>& outSpawn);
};
