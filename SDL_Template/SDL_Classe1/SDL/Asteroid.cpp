#include "Asteroid.h"
#include <cmath>
#include <cstdlib>

namespace {

struct InfoCategoria {
	Vector2 margen;
	Vector2 tamano;
	Vector2 escala;
};

InfoCategoria DescriptorCategoria(CategoriaAsteroide t) {
	switch (t) {
	case CategoriaAsteroide::Grande:
		// [ASTEROIDE GRANDE]: 
		return InfoCategoria{ Vector2(155.f, 0.f), Vector2(99.f, 92.f), Vector2(1.f, 1.f) };
	case CategoriaAsteroide::Mediano:
		// [ASTEROIDE MEDIANO]:
		return InfoCategoria{ Vector2(82.f, 2.f), Vector2(42.f, 39.f), Vector2(1.f, 1.f) };
	case CategoriaAsteroide::Pequeno:
	default:
		// [ASTEROIDE PEQUEÑO]: 
		return InfoCategoria{ Vector2(128.f, 0.f), Vector2(19.f, 20.f), Vector2(1.f, 1.f) };
	}
}

Vector2 MargenPara(CategoriaAsteroide t) {
	return DescriptorCategoria(t).margen;
}

Vector2 TamanoPara(CategoriaAsteroide t) {
	return DescriptorCategoria(t).tamano;
}

float RRange(float lo, float hi) {
	return lo + (hi - lo) * (float)rand() / (float)RAND_MAX;
}

} 

Asteroide::Asteroide(SDL_Renderer* renderer, Vector2 pos, CategoriaAsteroide t, Vector2 vel, float angularVelDeg)
	: GameObject(renderer, TamanoPara(t), MargenPara(t)) {
	categoria = t;
	const InfoCategoria inf = DescriptorCategoria(t);
	scale = inf.escala;
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
	muerto = false;
}

void Asteroide::ExplotarPorDisparo(SDL_Renderer* renderer, std::vector<Asteroide*>& outSpawn) {
	if (muerto) {
		return;
	}
	muerto = true;

	const float halfW = 0.5f * size.x * scale.x;
	const float halfH = 0.5f * size.y * scale.y;
	const Vector2 center(position.x + halfW, position.y + halfH);

	auto generarHijo = [&](CategoriaAsteroide childTier, float ox, float oy) {
		const InfoCategoria nd = DescriptorCategoria(childTier);
		const float nw = nd.tamano.x * nd.escala.x;
		const float nh = nd.tamano.y * nd.escala.y;
		Vector2 p(center.x - 0.5f * nw + ox, center.y - 0.5f * nh + oy);
		Vector2 v(
			linearVelocity.x + RRange(-48.f, 48.f),
			linearVelocity.y + RRange(-48.f, 48.f));
		const float spin = RRange(-28.f, 28.f);
		outSpawn.push_back(new Asteroide(renderer, p, childTier, v, spin));
	};

	if (categoria == CategoriaAsteroide::Grande) {
		generarHijo(CategoriaAsteroide::Mediano, -14.f, -10.f);
		generarHijo(CategoriaAsteroide::Mediano, 14.f, 10.f);
	} else if (categoria == CategoriaAsteroide::Mediano) {
		generarHijo(CategoriaAsteroide::Pequeno, -10.f, -8.f);
		generarHijo(CategoriaAsteroide::Pequeno, 10.f, 8.f);
	}

}
