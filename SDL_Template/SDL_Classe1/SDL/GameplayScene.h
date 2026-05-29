#pragma once
#include "Scene.h"
#include "GameObject.h"
#include "SpaceShip.h"
#include "Asteroid.h"
#include <SDL_ttf.h>
#include <vector>

class GamePlayScene : public Scene {

	int playfieldW = 0;
	int playfieldH = 0;

public:
	GamePlayScene() : Scene() {}
	~GamePlayScene() override;

	void Start(SDL_Renderer* rend) override;
	void Update(float dt) override;
	void Render(SDL_Renderer* rend) override;
	void Exit() override;

private:
	SDL_Renderer* cachedRenderer = nullptr;
	SpaceShip* ship = nullptr;
	float fireCooldownRemaining = 0.f;
	int waveLevel = 0;
	int score = 0;
	TTF_Font* scoreFont = nullptr;
	SDL_Texture* scoreTexture = nullptr;
	int scoreTextureForValue = -1;

	static constexpr float kFireCooldown = 0.18f;
	static constexpr float kBulletSpeed = 480.f;
	static constexpr float kHitboxShrink = 0.72f;
	static constexpr float kShipRespawnInvuln = 2.2f;

	void SpawnWave();
	size_t CountLiveAsteroids() const;
	void KillAllBullets();
	void ResolveBulletVsAsteroid();
	void ResolveShipVsAsteroid();
	void AppendSpawnedAsteroids(std::vector<Asteroid*>& spawns);
	void RemoveDeadBulletsAndAsteroids();
	static Vector2 RandomEdgePosition(float pw, float ph, const Vector2& avoidCenter, float avoidRadius);

	void ReleaseScoreUi();
	bool TryLoadScoreFont();
	void EnsureScoreTexture(SDL_Renderer* rend);
	void DrawScoreHud(SDL_Renderer* rend);
};
