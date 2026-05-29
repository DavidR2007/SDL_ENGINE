#pragma once
#include "GameObject.h"
#include "InputManager.h"
class SpaceShip : public GameObject
{

	
public:

	SpaceShip(SDL_Renderer* renderer, Vector2 pos, float rot, Vector2 scl);

	void Update(float dt) override;
	void UpdateMovement(float dt) override;

	/** Posición y velocidad inicial de la bala (rumbo = rotation, igual que el empuje). */
	void GetBulletSpawn(Vector2& outPos, Vector2& outVel, float bulletSpeed) const;

	bool IsInvulnerable() const { return invulnerableTime > 0.f; }
	void SetInvulnerable(float seconds) { invulnerableTime = seconds; }
	void CenterOnPlayfield(float playfieldW, float playfieldH);

private:
	float invulnerableTime = 0.f;
};