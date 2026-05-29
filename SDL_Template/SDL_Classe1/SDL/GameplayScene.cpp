#include "GameplayScene.h"
#include "Asteroid.h"
#include "Bullet.h"
#include "InputManager.h"
#include <SDL_ttf.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>

namespace {

constexpr float kRockSpeedMin = 24.f;
constexpr float kRockSpeedMax = 56.f;
constexpr float kSpawnAvoidRadius = 130.f;
constexpr float kLargeRockHalf = 49.5f;

float LenSq(const Vector2& v) {
	return v.x * v.x + v.y * v.y;
}

float RRange(float lo, float hi) {
	return lo + (hi - lo) * (float)rand() / (float)RAND_MAX;
}

Vector2 RandomUnitVector() {
	const float a = (float)rand() / (float)RAND_MAX * 6.2831853f;
	return Vector2(std::cos(a), std::sin(a));
}

// Genera velocidad aleatoria multiplicada por un factor según la oleada (van más rápido cada nivel)
Vector2 RandomRockVelocity(int waveLevel) {
	Vector2 d = RandomUnitVector();
	const float factor = 1.0f + waveLevel * 0.15f; // +15% de velocidad por cada nivel
	const float s = (kRockSpeedMin + (float)rand() / (float)RAND_MAX * (kRockSpeedMax - kRockSpeedMin)) * factor;
	return d * s;
}

// Puntuaciones oficiales requeridas: Grande=50, Mediano=30, Pequeño=20
int PointsForDestroyedRock(CategoriaAsteroide t) {
	switch (t) {
	case CategoriaAsteroide::Grande:
		return 50;
	case CategoriaAsteroide::Mediano:
		return 30;
	case CategoriaAsteroide::Pequeno:
		return 20;
	default:
		return 0;
	}
}

} 

void GamePlayScene::SpawnWave() {
	if (cachedRenderer == nullptr || playfieldW <= 0 || playfieldH <= 0) {
		return;
	}

	const float pw = static_cast<float>(playfieldW);
	const float ph = static_cast<float>(playfieldH);
	Vector2 avoid(0.5f * pw, 0.5f * ph);
	if (ship != nullptr) {
		const RectF sr = ship->WorldBounds();
		avoid = Vector2(sr.x + 0.5f * sr.w, sr.y + 0.5f * sr.h);
	}

	const int count = (std::min)(4 + waveLevel * 2, 11);
	for (int i = 0; i < count; ++i) {
		Vector2 pos = RandomEdgePosition(pw, ph, avoid, kSpawnAvoidRadius);
		objetos.push_back(new Asteroide(cachedRenderer, pos, CategoriaAsteroide::Grande, RandomRockVelocity(waveLevel), RRange(-26.f, 26.f)));
	}
}

Vector2 GamePlayScene::RandomEdgePosition(float pw, float ph, const Vector2& avoidCenter, float avoidRadius) {
	for (int tries = 0; tries < 40; ++tries) {
		const int edge = rand() % 4;
		const float m = 20.f;
		Vector2 p;
		switch (edge) {
		case 0:
			p = Vector2(m + ((float)rand() / (float)RAND_MAX) * (pw - 2.f * m - 2.f * kLargeRockHalf), m);
			break;
		case 1:
			p = Vector2(pw - m - 2.f * kLargeRockHalf, m + ((float)rand() / (float)RAND_MAX) * (ph - 2.f * m - 2.f * kLargeRockHalf));
			break;
		case 2:
			p = Vector2(m + ((float)rand() / (float)RAND_MAX) * (pw - 2.f * m - 2.f * kLargeRockHalf), ph - m - 2.f * kLargeRockHalf);
			break;
		default:
			p = Vector2(m, m + ((float)rand() / (float)RAND_MAX) * (ph - 2.f * m - 2.f * kLargeRockHalf));
			break;
		}
		const Vector2 rockCenter(p.x + kLargeRockHalf, p.y + kLargeRockHalf);
		if (LenSq(rockCenter - avoidCenter) > avoidRadius * avoidRadius) {
			return p;
		}
	}
	return Vector2(40.f, 40.f);
}

size_t GamePlayScene::CountLiveAsteroids() const {
	size_t n = 0;
	for (GameObject* o : objetos) {
		Asteroide* a = dynamic_cast<Asteroide*>(o);
		if (a != nullptr && !a->EstaMuerto()) {
			++n;
		}
	}
	return n;
}

void GamePlayScene::KillAllBullets() {
	for (size_t i = 0; i < objetos.size();) {
		if (dynamic_cast<Bala*>(objetos[i]) != nullptr) {
			delete objetos[i];
			objetos.erase(objetos.begin() + static_cast<std::ptrdiff_t>(i));
		} else {
			++i;
		}
	}
}

void GamePlayScene::ResolveBulletVsAsteroid() {
	const float kShrink = kHitboxShrink;
	for (size_t i = 0; i < objetos.size(); ++i) {
		Bala* b = dynamic_cast<Bala*>(objetos[i]);
		if (b == nullptr || b->EstaMuerto()) {
			continue;
		}
		const RectF br = ShrinkRectCentered(b->WorldBounds(), kShrink);
		for (size_t j = 0; j < objetos.size(); ++j) {
			if (i == j) {
				continue;
			}
			Asteroide* a = dynamic_cast<Asteroide*>(objetos[j]);
			if (a == nullptr || a->EstaMuerto()) {
				continue;
			}
			const RectF ar = ShrinkRectCentered(a->WorldBounds(), kShrink);
			if (!RectOverlap(br, ar)) {
				continue;
			}
			score += PointsForDestroyedRock(a->ObtenerCategoria());
			b->Impactar();
			std::vector<Asteroide*> spawns;
			a->ExplotarPorDisparo(cachedRenderer, spawns);
			AppendSpawnedAsteroids(spawns);
			break;
		}
	}
}

void GamePlayScene::ResolveShipVsAsteroid() {
	if (ship == nullptr || ship->IsInvulnerable()) {
		return;
	}
	const RectF sr = ShrinkRectCentered(ship->WorldBounds(), 0.78f);
	for (GameObject* o : objetos) {
		Asteroide* a = dynamic_cast<Asteroide*>(o);
		if (a == nullptr || a->EstaMuerto()) {
			continue;
		}
		const RectF ar = ShrinkRectCentered(a->WorldBounds(), kHitboxShrink);
		if (RectOverlap(sr, ar)) {
			vidas--; // Perder una vida al colisionar
			if (vidas <= 0) {
				// Muerte: volver al menú principal
				finalizada = true;
				escenaDestino = "MainMenu";
			} else {
				// Respawn e invulnerabilidad temporal
				ship->CenterOnPlayfield(static_cast<float>(playfieldW), static_cast<float>(playfieldH));
				ship->SetInvulnerable(kShipRespawnInvuln);
			}
			break;
		}
	}
}

void GamePlayScene::AppendSpawnedAsteroids(std::vector<Asteroide*>& spawns) {
	for (Asteroide* a : spawns) {
		if (a != nullptr) {
			objetos.push_back(a);
		}
	}
	spawns.clear();
}

void GamePlayScene::RemoveDeadBulletsAndAsteroids() {
	for (size_t i = 0; i < objetos.size();) {
		Bala* b = dynamic_cast<Bala*>(objetos[i]);
		if (b != nullptr && b->EstaMuerto()) {
			delete b;
			objetos.erase(objetos.begin() + static_cast<std::ptrdiff_t>(i));
			continue;
		}
		Asteroide* a = dynamic_cast<Asteroide*>(objetos[i]);
		if (a != nullptr && a->EstaMuerto()) {
			delete a;
			objetos.erase(objetos.begin() + static_cast<std::ptrdiff_t>(i));
			continue;
		}
		++i;
	}
}

void GamePlayScene::Start(SDL_Renderer* rend) {
	ReleaseScoreUi();
	ship = nullptr;
	cachedRenderer = rend;
	Scene::Start(rend);

	std::srand(static_cast<unsigned>(std::time(nullptr)));

	if (SDL_GetRendererOutputSize(rend, &playfieldW, &playfieldH) != 0) {
		playfieldW = 0;
		playfieldH = 0;
	}

	score = 0;
	vidas = 3; // Inicializar vidas en 3 al empezar la partida
	scoreTextureForValue = -1;
	waveLevel = 0;
	TryLoadScoreFont();
	SpawnWave();

	ship = new SpaceShip(rend, Vector2(0.f, 0.f), 0.f, Vector2(1.f, 1.f));
	if (playfieldW > 0 && playfieldH > 0) {
		ship->CenterOnPlayfield(static_cast<float>(playfieldW), static_cast<float>(playfieldH));
	}
	objetos.push_back(ship);
	fireCooldownRemaining = 0.f;
}

void GamePlayScene::Update(float dt) {
	if (ship != nullptr && cachedRenderer != nullptr) {
		if (fireCooldownRemaining > 0.f) {
			fireCooldownRemaining -= dt;
			if (fireCooldownRemaining < 0.f) {
				fireCooldownRemaining = 0.f;
			}
		}
		if (IM.GetKey(SDLK_SPACE, DOWN) && fireCooldownRemaining <= 0.f) {
			fireCooldownRemaining = kFireCooldown;
			Vector2 spawnPos;
			Vector2 spawnVel;
			ship->GetBulletSpawn(spawnPos, spawnVel, kBulletSpeed);
			objetos.push_back(new Bala(cachedRenderer, spawnPos, spawnVel));
		}
	}

	Scene::Update(dt);

	ResolveBulletVsAsteroid();
	ResolveShipVsAsteroid();

	RemoveDeadBulletsAndAsteroids();

	if (CountLiveAsteroids() == 0 && cachedRenderer != nullptr && playfieldW > 0) {
		++waveLevel;
		KillAllBullets();
		SpawnWave();
	}

	const float w = static_cast<float>(playfieldW);
	const float h = static_cast<float>(playfieldH);
	for (GameObject* obj : objetos) {
		if (obj != nullptr) {
			Bala* b = dynamic_cast<Bala*>(obj);
			if (b != nullptr) {
				// Si la bala toca o supera cualquier borde, se marca como muerta para ser eliminada
				RectF bounds = b->WorldBounds();
				if (bounds.x < 0.f || bounds.x + bounds.w > w || bounds.y < 0.f || bounds.y + bounds.h > h) {
					b->Impactar();
				}
			} else {
				obj->WrapToroidal(w, h);
			}
		}
	}
}

void GamePlayScene::Render(SDL_Renderer* rend) {
	Scene::Render(rend);
	DrawScoreHud(rend);
	DrawLivesHud(rend); // Dibujar HUD de vidas en pantalla
}

void GamePlayScene::Exit() {
	ReleaseScoreUi();
	ship = nullptr;
	cachedRenderer = nullptr;
}

GamePlayScene::~GamePlayScene() {
	ReleaseScoreUi();
}

void GamePlayScene::ReleaseScoreUi() {
	if (scoreTexture != nullptr) {
		SDL_DestroyTexture(scoreTexture);
		scoreTexture = nullptr;
	}
	scoreTextureForValue = -1;
	if (scoreFont != nullptr) {
		TTF_CloseFont(scoreFont);
		scoreFont = nullptr;
	}
}

bool GamePlayScene::TryLoadScoreFont() {
	static const char* kFontPaths[] = {
		"C:/Windows/Fonts/consola.ttf",
		"C:/Windows/Fonts/arial.ttf",
		"C:/Windows/Fonts/cour.ttf",
	};
	for (const char* path : kFontPaths) {
		scoreFont = TTF_OpenFont(path, 22);
		if (scoreFont != nullptr) {
			TTF_SetFontStyle(scoreFont, TTF_STYLE_BOLD);
			return true;
		}
	}
	return false;
}

void GamePlayScene::EnsureScoreTexture(SDL_Renderer* rend) {
	if (rend == nullptr || scoreFont == nullptr) {
		return;
	}
	if (scoreTexture != nullptr && scoreTextureForValue == score) {
		return;
	}
	if (scoreTexture != nullptr) {
		SDL_DestroyTexture(scoreTexture);
		scoreTexture = nullptr;
	}
	char line[96];
	std::snprintf(line, sizeof(line), "SCORE  %d", score);
	SDL_Color fg{ 255, 255, 255, 255 };
	SDL_Surface* surf = TTF_RenderUTF8_Blended(scoreFont, line, fg);
	if (surf == nullptr) {
		return;
	}
	scoreTexture = SDL_CreateTextureFromSurface(rend, surf);
	SDL_FreeSurface(surf);
	if (scoreTexture != nullptr) {
		SDL_SetTextureBlendMode(scoreTexture, SDL_BLENDMODE_BLEND);
		scoreTextureForValue = score;
	}
}

void GamePlayScene::DrawScoreHud(SDL_Renderer* rend) {
	if (rend == nullptr) {
		return;
	}
	EnsureScoreTexture(rend);
	if (scoreTexture == nullptr) {
		return;
	}
	int tw = 0;
	int th = 0;
	SDL_QueryTexture(scoreTexture, nullptr, nullptr, &tw, &th);
	const SDL_Rect dst{ 12, 10, tw, th };
	SDL_RenderCopy(rend, scoreTexture, nullptr, &dst);
}

// Representa las vidas del jugador usando el propio recorte del sprite de la nave
void GamePlayScene::DrawLivesHud(SDL_Renderer* rend) {
	if (rend == nullptr || ship == nullptr) return;
	SDL_Texture* tex = ship->GetTexture();
	if (tex == nullptr) return;

	const Vector2 sz = ship->GetSize();
	const Vector2 pad = ship->GetPadding();

	const int w = static_cast<int>(sz.x * 0.6f);
	const int h = static_cast<int>(sz.y * 0.6f);

	int screenW = 800, screenH = 600;
	SDL_GetRendererOutputSize(rend, &screenW, &screenH);

	// [AQUÍ SE RECORTA EL SPRITE PARA EL HUD]: Recorte del sprite de la nave
	const SDL_Rect src{
		static_cast<int>(pad.x),
		static_cast<int>(pad.y),
		static_cast<int>(sz.x),
		static_cast<int>(sz.y)
	};

	for (int i = 0; i < vidas; ++i) {
		const int posX = screenW - 20 - (i + 1) * (w + 8);
		const int posY = 12;
		const SDL_Rect dst{ posX, posY, w, h };
		// Dibujamos las naves representando vidas apuntando hacia arriba
		SDL_RenderCopyEx(rend, tex, &src, &dst, 0.0, nullptr, SDL_FLIP_NONE);
	}
}
