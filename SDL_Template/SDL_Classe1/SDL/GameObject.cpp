#include "GameObject.h"
#include <algorithm>
#include <cmath>

namespace {

float WrapAxis(float v, float dim) {
	if (dim <= 0.f || !std::isfinite(v) || !std::isfinite(dim)) {
		return 0.f;
	}
	v = std::fmod(v, dim);
	if (v < 0.f) {
		v += dim;
	}
	if (v >= dim) {
		v -= dim;
	}
	return v;
}

SDL_Texture* gSharedSpritesheet = nullptr;
int gSharedSpritesheetRefs = 0;

SDL_Texture* LoadSpritesheetTexture(SDL_Renderer* renderer) {
	SDL_Surface* surf = IMG_Load("resources/asteroids_spritesheet.png");

	if (surf == nullptr) {
		std::cout << "Error creating surface:" << SDL_GetError();
		return nullptr;
	}

	SDL_Texture* textureTmp = SDL_CreateTextureFromSurface(renderer, surf);
	if (textureTmp == nullptr) {
		std::cout << "Error creating texture:" << SDL_GetError();
	}

	SDL_FreeSurface(surf);

	return textureTmp;
}

SDL_Texture* AcquireSharedSpritesheet(SDL_Renderer* renderer) {
	if (renderer == nullptr) {
		return nullptr;
	}
	if (gSharedSpritesheet == nullptr) {
		gSharedSpritesheet = LoadSpritesheetTexture(renderer);
	}
	if (gSharedSpritesheet == nullptr) {
		return nullptr;
	}
	++gSharedSpritesheetRefs;
	return gSharedSpritesheet;
}

void ReleaseSharedSpritesheet() {
	if (gSharedSpritesheetRefs <= 0) {
		return;
	}
	--gSharedSpritesheetRefs;
	if (gSharedSpritesheetRefs == 0 && gSharedSpritesheet != nullptr) {
		SDL_DestroyTexture(gSharedSpritesheet);
		gSharedSpritesheet = nullptr;
	}
}

Sint32 ToSdlScalar(float v) {
	if (!std::isfinite(v)) {
		return 0;
	}
	const float c = std::max(-32000.f, std::min(32000.f, v));
	return static_cast<Sint32>(std::lroundf(c));
}

} // namespace

void GameObject::UpdateMovement(float dt) {
	// UPDATE VELOCITY AND ANGULAR VELOCITY:

	linearVelocity = linearVelocity + (linearAcceleration * dt);
	angularVelocity += (angularAcceleration * dt);

	// APPLY DRAG:

	linearVelocity = linearVelocity * (1.0f - linearDrag * dt);
	angularVelocity *= (1.0f - AngularDrag * dt);

	//UPDATE POSITION AND ROTATION:

	position = position + (linearVelocity * dt);
	rotation += angularVelocity * dt;
	if (!std::isfinite(rotation)) {
		rotation = 0.f;
	} else {
		rotation = std::fmod(rotation, 360.f);
		if (rotation < 0.f) {
			rotation += 360.f;
		}
	}
}

GameObject::~GameObject() {
	if (texture != nullptr) {
		ReleaseSharedSpritesheet();
		texture = nullptr;
	}
}

GameObject::GameObject(SDL_Renderer* renderer, Vector2 textSize, Vector2 textPadding) {
	position = Vector2(250.0f, 250.0f);
	rotation = 0.f;
	scale = Vector2(1.f, 1.f);

	texture = AcquireSharedSpritesheet(renderer);
	size = textSize;
	padding = textPadding;
}

void GameObject::Render(SDL_Renderer* renderer) {
	if (renderer == nullptr || texture == nullptr) {
		return;
	}

	const SDL_Rect source{
		ToSdlScalar(padding.x),
		ToSdlScalar(padding.y),
		ToSdlScalar(size.x),
		ToSdlScalar(size.y),
	};

	const float destW = size.x * scale.x;
	const float destH = size.y * scale.y;
	const SDL_Rect destination{
		ToSdlScalar(position.x),
		ToSdlScalar(position.y),
		ToSdlScalar(destW),
		ToSdlScalar(destH),
	};

	if (source.w <= 0 || source.h <= 0 || destination.w <= 0 || destination.h <= 0) {
		return;
	}

	SDL_RenderCopyEx(renderer, texture, &source, &destination, 90.0 + static_cast<double>(rotation), nullptr, SDL_FLIP_NONE);
}

void GameObject::Update(float dt) {
	UpdateMovement(dt);
}

RectF GameObject::WorldBounds() const {
	return RectF{
		position.x,
		position.y,
		size.x * scale.x,
		size.y * scale.y,
	};
}

void GameObject::WrapToroidal(float playfieldW, float playfieldH) {
	if (playfieldW <= 0.f || playfieldH <= 0.f) {
		return;
	}
	position.x = WrapAxis(position.x, playfieldW);
	position.y = WrapAxis(position.y, playfieldH);
}
