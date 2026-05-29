#include "SpaceShip.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// [AQUÍ SE RECORTA EL SPRITE DE LA NAVE]: Coordenadas (X=0, Y=0) y tamaño (ancho=32, alto=40) en asteroids_spritesheet.png
SpaceShip::SpaceShip(SDL_Renderer* renderer, Vector2 pos, float rot, Vector2 scl) : GameObject(renderer, Vector2(32,40), Vector2(0,0))
{
	position = pos;
	rotation = rot;
	this->scale = scl;

	linearVelocity = Vector2();
	angularVelocity = 0.0f;

	linearAcceleration = Vector2();
	angularAcceleration = 0.0f;

	linearDrag = 1.2f;
	AngularDrag = 6.0f;

	linearAccFactor = 500.0f; // Px / seg^2
	angularAccFactor = 2400.0f; // Grados / seg^2 (mitad de la sensación de giro anterior)
}


void SpaceShip::Update(float dt) {
	if (invulnerableTime > 0.f) {
		invulnerableTime -= dt;
		if (invulnerableTime < 0.f) {
			invulnerableTime = 0.f;
		}
	}
	GameObject::Update(dt);
}

void SpaceShip::CenterOnPlayfield(float playfieldW, float playfieldH) {
	const float halfW = 0.5f * size.x * scale.x;
	const float halfH = 0.5f * size.y * scale.y;
	position = Vector2(playfieldW * 0.5f - halfW, playfieldH * 0.5f - halfH);
	linearVelocity = Vector2();
	angularVelocity = 0.f;
	rotation = 0.f;
}

void SpaceShip::UpdateMovement(float dt) {
	// ENTRADA

	linearAcceleration = Vector2();

	float thrustSign = 0.f;
	if (IM.GetKey(SDLK_UP, HOLD) || IM.GetKey(SDLK_UP, DOWN) || IM.GetKey(SDLK_w, HOLD) || IM.GetKey(SDLK_w, DOWN)) thrustSign += 1.f;
	if (IM.GetKey(SDLK_DOWN, HOLD) || IM.GetKey(SDLK_DOWN, DOWN) || IM.GetKey(SDLK_s, HOLD) || IM.GetKey(SDLK_s, DOWN)) thrustSign -= 1.f;

	if (thrustSign != 0.f)
	{
		// rotation = rumbo en grados: 0 = nariz a la derecha (+X), crece en sentido horario (pantalla Y abajo).
		// El +90 del Render alinea el recorte del sprite con ese rumbo; el empuje usa solo rotation.
		const float headingRad = rotation * static_cast<float>(M_PI / 180.0);
		Vector2 dir(std::cos(headingRad), std::sin(headingRad));
		linearAcceleration = dir * (thrustSign * linearAccFactor);
	}

	angularAcceleration = 0.f;
	if (IM.GetKey(SDLK_RIGHT, HOLD) || IM.GetKey(SDLK_RIGHT, DOWN) || IM.GetKey(SDLK_d, HOLD) || IM.GetKey(SDLK_d, DOWN))
	{
		angularAcceleration = angularAccFactor;
	}
	else if (IM.GetKey(SDLK_LEFT, HOLD) || IM.GetKey(SDLK_LEFT, DOWN) || IM.GetKey(SDLK_a, HOLD) || IM.GetKey(SDLK_a, DOWN))
	{
		angularAcceleration = -angularAccFactor;
	}


	GameObject::UpdateMovement(dt);
}

void SpaceShip::GetBulletSpawn(Vector2& outPos, Vector2& outVel, float bulletSpeed) const {
	const float halfW = 0.5f * size.x * scale.x;
	const float halfH = 0.5f * size.y * scale.y;
	const float cx = position.x + halfW;
	const float cy = position.y + halfH;
	const float headingRad = rotation * static_cast<float>(M_PI / 180.0);
	const float dx = std::cos(headingRad);
	const float dy = std::sin(headingRad);
	const float forwardOffset = 22.f;
	outPos = Vector2(cx + dx * forwardOffset, cy + dy * forwardOffset);
	outVel = Vector2(linearVelocity.x + dx * bulletSpeed, linearVelocity.y + dy * bulletSpeed);
}