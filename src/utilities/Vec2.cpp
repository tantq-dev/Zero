#include "Vec2.h"
#include <cmath>

Vec2::Vec2()= default;

Vec2::Vec2(const float xin, const float yin)
	:x(xin),y(yin)
{

}

bool Vec2::operator == (const Vec2& rhs) const {
	return x == rhs.x && y == rhs.y;
}

bool Vec2::operator != (const Vec2& rhs) const {
	return x != rhs.x || y != rhs.y;
}

Vec2 Vec2::operator + (const Vec2& rhs) const {
	return {x+rhs.x,y+rhs.y};
}
Vec2 Vec2::operator - (const Vec2& rhs) const {
	return {x - rhs.x, y - rhs.y};
}
Vec2 Vec2::operator * (const float val) const {
	return {x*val, y*val};
}

Vec2 Vec2::operator / (const float val) const {
	return {x / val, y / val};
}

void Vec2::operator += (const Vec2& rhs) {
	x += rhs.x;
	y += rhs.y;
}
void Vec2::operator-=(const Vec2& rhs) {
	x -= rhs.x;
	y -= rhs.y;
}
void Vec2::operator*=(const float val) {
	x *= val;
	y *= val;
}
void Vec2::operator/=(const float val) {
	x /= val;
	y /= val;
}
float Vec2::dist(const Vec2& rhs) const {
	return sqrtf((rhs.x - x) * (rhs.x - x) + (rhs.y - y) * (rhs.y - y));
}
float Vec2::length() const{
	return sqrtf(x * x + y * y);
}
Vec2& Vec2::normalize()  {
	const float len = length();
	x /= len;
	y /= len;
	return *this;
}
