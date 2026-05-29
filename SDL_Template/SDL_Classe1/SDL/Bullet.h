#pragma once
#include "GameObject.h"

class Bala : public GameObject {
	bool muerto = false;

public:
	Bala(SDL_Renderer* renderer, Vector2 pos, Vector2 velocidad);

	void Update(float dt) override;

	bool EstaMuerto() const { return muerto; }
	void Impactar();
};
