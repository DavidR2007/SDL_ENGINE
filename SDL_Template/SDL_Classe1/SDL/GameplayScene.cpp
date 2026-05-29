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
constexpr float kLargeRockHalf = 41.f;

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

Vector2 RandomRockVelocity() {
	Vector2 d = RandomUnitVector();
	const float s = kRockSpeedMin + (float)rand() / (float)RAND_MAX * (kRockSpeedMax - kRockSpeedMin);
	return d * s;
}

int PointsForDestroyedRock(AsteroidTier t) {
	switch (t) {
	case AsteroidTier::Large:
		return 20;
	case AsteroidTier::Medium:
		return 50;
	case AsteroidTier::Small:
		return 100;
	default:
		return 0;
	}
}

} // namespace

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
		objects.push_back(new Asteroid(cachedRenderer, pos, AsteroidTier::Large, RandomRockVelocity(), RRange(-26.f, 26.f)));
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
	for (GameObject* o : objects) {
		Asteroid* a = dynamic_cast<Asteroid*>(o);
		if (a != nullptr && !a->IsDead()) {
			++n;
		}
	}
	return n;
}

void GamePlayScene::KillAllBullets() {
	for (size_t i = 0; i < objects.size();) {
		if (dynamic_cast<Bullet*>(objects[i]) != nullptr) {
			delete objects[i];
			objects.erase(objects.begin() + static_cast<std::ptrdiff_t>(i));
		} else {
			++i;
		}
	}
}

void GamePlayScene::ResolveBulletVsAsteroid() {
	const float kShrink = kHitboxShrink;
	for (size_t i = 0; i < objects.size(); ++i) {
		Bullet* b = dynamic_cast<Bullet*>(objects[i]);
		if (b == nullptr || b->IsDead()) {
			continue;
		}
		const RectF br = ShrinkRectCentered(b->WorldBounds(), kShrink);
		for (size_t j = 0; j < objects.size(); ++j) {
			if (i == j) {
				continue;
			}
			Asteroid* a = dynamic_cast<Asteroid*>(objects[j]);
			if (a == nullptr || a->IsDead()) {
				continue;
			}
			const RectF ar = ShrinkRectCentered(a->WorldBounds(), kShrink);
			if (!RectOverlap(br, ar)) {
				continue;
			}
			score += PointsForDestroyedRock(a->GetTier());
			b->Hit();
			std::vector<Asteroid*> spawns;
			a->ExplodeFromShot(cachedRenderer, spawns);
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
	for (GameObject* o : objects) {
		Asteroid* a = dynamic_cast<Asteroid*>(o);
		if (a == nullptr || a->IsDead()) {
			continue;
		}
		const RectF ar = ShrinkRectCentered(a->WorldBounds(), kHitboxShrink);
		if (RectOverlap(sr, ar)) {
			ship->CenterOnPlayfield(static_cast<float>(playfieldW), static_cast<float>(playfieldH));
			ship->SetInvulnerable(kShipRespawnInvuln);
			break;
		}
	}
}

void GamePlayScene::AppendSpawnedAsteroids(std::vector<Asteroid*>& spawns) {
	for (Asteroid* a : spawns) {
		if (a != nullptr) {
			objects.push_back(a);
		}
	}
	spawns.clear();
}

void GamePlayScene::RemoveDeadBulletsAndAsteroids() {
	for (size_t i = 0; i < objects.size();) {
		Bullet* b = dynamic_cast<Bullet*>(objects[i]);
		if (b != nullptr && b->IsDead()) {
			delete b;
			objects.erase(objects.begin() + static_cast<std::ptrdiff_t>(i));
			continue;
		}
		Asteroid* a = dynamic_cast<Asteroid*>(objects[i]);
		if (a != nullptr && a->IsDead()) {
			delete a;
			objects.erase(objects.begin() + static_cast<std::ptrdiff_t>(i));
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
	scoreTextureForValue = -1;
	waveLevel = 0;
	TryLoadScoreFont();
	SpawnWave();

	ship = new SpaceShip(rend, Vector2(0.f, 0.f), 0.f, Vector2(1.f, 1.f));
	if (playfieldW > 0 && playfieldH > 0) {
		ship->CenterOnPlayfield(static_cast<float>(playfieldW), static_cast<float>(playfieldH));
	}
	objects.push_back(ship);
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
			objects.push_back(new Bullet(cachedRenderer, spawnPos, spawnVel));
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
	for (GameObject* obj : objects) {
		if (obj != nullptr) {
			obj->WrapToroidal(w, h);
		}
	}
}

void GamePlayScene::Render(SDL_Renderer* rend) {
	Scene::Render(rend);
	DrawScoreHud(rend);
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
