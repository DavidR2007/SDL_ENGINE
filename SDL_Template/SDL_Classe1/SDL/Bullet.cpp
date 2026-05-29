#include "Bullet.h"

namespace {
// [AQUÍ SE RECORTA EL SPRITE DE LA BALA]: Coordenadas (X=35, Y=33) y tamaño (W=3, H=4) en la hoja de sprites
constexpr float kBulletPadX = 35.f;
constexpr float kBulletPadY = 33.f;
constexpr float kBulletSrcW = 3.f;
constexpr float kBulletSrcH = 4.f;
}

Bala::Bala(SDL_Renderer* renderer, Vector2 pos, Vector2 velocidad)
	: GameObject(renderer, Vector2(kBulletSrcW, kBulletSrcH), Vector2(kBulletPadX, kBulletPadY)) {
	position = pos;
	rotation = 0.f;
	scale = Vector2(2.f, 2.f);

	linearVelocity = velocidad;
	angularVelocity = 0.f;
	linearAcceleration = Vector2();
	angularAcceleration = 0.f;

	linearDrag = 0.f;
	AngularDrag = 0.f;
	linearAccFactor = 0.f;
	angularAccFactor = 0.f;

	muerto = false;
}

void Bala::Update(float dt) {
	if (muerto) {
		return;
	}
	GameObject::Update(dt);
}

void Bala::Impactar() {
	muerto = true;
}
