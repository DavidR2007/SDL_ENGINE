#include "Bullet.h"

namespace {
// Punto blanco junto al logo en asteroids_spritesheet.png (256x128, gris+alpha)
constexpr float kBulletPadX = 35.f;
constexpr float kBulletPadY = 33.f;
constexpr float kBulletSrcW = 3.f;
constexpr float kBulletSrcH = 4.f;
}

Bullet::Bullet(SDL_Renderer* renderer, Vector2 pos, Vector2 velocity)
	: GameObject(renderer, Vector2(kBulletSrcW, kBulletSrcH), Vector2(kBulletPadX, kBulletPadY)) {
	position = pos;
	rotation = 0.f;
	scale = Vector2(2.f, 2.f);

	linearVelocity = velocity;
	angularVelocity = 0.f;
	linearAcceleration = Vector2();
	angularAcceleration = 0.f;

	linearDrag = 0.f;
	AngularDrag = 0.f;
	linearAccFactor = 0.f;
	angularAccFactor = 0.f;

	lifetime = 1.4f;
	dead = false;
}

void Bullet::Update(float dt) {
	if (dead) {
		return;
	}
	lifetime -= dt;
	if (lifetime <= 0.f) {
		dead = true;
		return;
	}
	GameObject::Update(dt);
}

void Bullet::Hit() {
	dead = true;
}
