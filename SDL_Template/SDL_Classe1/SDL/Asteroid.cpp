#include "Asteroid.h"
#include <cmath>
#include <cstdlib>

namespace {

struct TierInfo {
	Vector2 pad;
	Vector2 sz;
	Vector2 scale;
};

TierInfo TierDescriptor(AsteroidTier t) {
	switch (t) {
	case AsteroidTier::Large:
		// Roca grande principal (arriba-derecha de la hoja 256x128), bbox ajustado al contorno blanco
		return TierInfo{ Vector2(172.f, 1.f), Vector2(81.f, 90.f), Vector2(1.f, 1.f) };
	case AsteroidTier::Medium:
		// Roca mediana (centro-superior, junto a la zona del logo)
		return TierInfo{ Vector2(157.f, 30.f), Vector2(32.f, 41.f), Vector2(1.f, 1.f) };
	case AsteroidTier::Small:
	default:
		// Roca pequeña (arriba-izquierda, cerca de la "A")
		return TierInfo{ Vector2(129.f, 1.f), Vector2(18.f, 28.f), Vector2(1.f, 1.f) };
	}
}

Vector2 PadFor(AsteroidTier t) {
	return TierDescriptor(t).pad;
}

Vector2 SzFor(AsteroidTier t) {
	return TierDescriptor(t).sz;
}

float RRange(float lo, float hi) {
	return lo + (hi - lo) * (float)rand() / (float)RAND_MAX;
}

} // namespace

Asteroid::Asteroid(SDL_Renderer* renderer, Vector2 pos, AsteroidTier t, Vector2 vel, float angularVelDeg)
	: GameObject(renderer, SzFor(t), PadFor(t)) {
	tier = t;
	const TierInfo inf = TierDescriptor(t);
	scale = inf.scale;
	position = pos;
	rotation = RRange(0.f, 360.f);
	linearVelocity = vel;
	angularVelocity = angularVelDeg;

	linearAcceleration = Vector2();
	angularAcceleration = 0.f;
	linearDrag = 0.f;
	AngularDrag = 0.f;
	linearAccFactor = 0.f;
	angularAccFactor = 0.f;
	dead = false;
}

void Asteroid::ExplodeFromShot(SDL_Renderer* renderer, std::vector<Asteroid*>& outSpawn) {
	if (dead) {
		return;
	}
	dead = true;

	const float halfW = 0.5f * size.x * scale.x;
	const float halfH = 0.5f * size.y * scale.y;
	const Vector2 center(position.x + halfW, position.y + halfH);

	auto spawnChild = [&](AsteroidTier childTier, float ox, float oy) {
		const TierInfo nd = TierDescriptor(childTier);
		const float nw = nd.sz.x * nd.scale.x;
		const float nh = nd.sz.y * nd.scale.y;
		Vector2 p(center.x - 0.5f * nw + ox, center.y - 0.5f * nh + oy);
		Vector2 v(
			linearVelocity.x + RRange(-48.f, 48.f),
			linearVelocity.y + RRange(-48.f, 48.f));
		const float spin = RRange(-28.f, 28.f);
		outSpawn.push_back(new Asteroid(renderer, p, childTier, v, spin));
	};

	if (tier == AsteroidTier::Large) {
		spawnChild(AsteroidTier::Medium, -14.f, -10.f);
		spawnChild(AsteroidTier::Medium, 14.f, 10.f);
	} else if (tier == AsteroidTier::Medium) {
		spawnChild(AsteroidTier::Small, -10.f, -8.f);
		spawnChild(AsteroidTier::Small, 10.f, 8.f);
	}
	// Small: desaparece sin hijos
}
