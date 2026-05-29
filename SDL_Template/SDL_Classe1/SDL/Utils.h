#pragma once

struct  Vector2
{
	float x;
	float y;

	//Constructors
	Vector2() : x(0), y(0) {}
	Vector2(float x, float y) : x(x), y(y) {}
	
	Vector2 operator+(const Vector2& other) const {
		return Vector2(x + other.x, y + other.y);
	}

	Vector2 operator-(const Vector2& other) const {
		return Vector2(x - other.x, y - other.y);
	}

	Vector2 operator*(float other) const {
		return Vector2(x * other, y * other);
	}

};

struct RectF {
	float x = 0.f;
	float y = 0.f;
	float w = 0.f;
	float h = 0.f;
};

inline RectF ShrinkRectCentered(RectF r, float factor) {
	if (factor <= 0.f || factor >= 1.f) {
		return r;
	}
	const float nw = r.w * factor;
	const float nh = r.h * factor;
	const float ox = (r.w - nw) * 0.5f;
	const float oy = (r.h - nh) * 0.5f;
	return RectF{ r.x + ox, r.y + oy, nw, nh };
}

inline bool RectOverlap(const RectF& a, const RectF& b) {
	return a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h && a.y + a.h > b.y;
}