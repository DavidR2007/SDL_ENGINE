#pragma once
#include "Utils.h"
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

class GameObject
{
protected:
	Vector2 position;
	float rotation;
	Vector2 scale;

	SDL_Texture* texture;
	Vector2 size; // Tamaùo de la textura, with and height
	Vector2 padding; // Pos texture cut

	//MOVEMENT	
	Vector2 linearVelocity;
	float angularVelocity;

	Vector2 linearAcceleration;
	float angularAcceleration;

	float linearDrag;
	float AngularDrag;

	float linearAccFactor;
	float angularAccFactor;


	virtual void UpdateMovement(float dt);

public:
	virtual ~GameObject();

	GameObject(SDL_Renderer* renderer, Vector2 textSize, Vector2 textPadding);
	void Render(SDL_Renderer* renderer);
	virtual void Update(float dt);

	RectF WorldBounds() const;

	/** Asteroids: al salir por un borde, reaparece por el opuesto (toroide). */
	void WrapToroidal(float playfieldW, float playfieldH);

};
