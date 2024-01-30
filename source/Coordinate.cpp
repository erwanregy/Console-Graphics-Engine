#include "pch.hpp"

#include "Coordinate.hpp"


Coordinate() : x(0), y(0) {}
Coordinate(int x, int y) : x(x), y(y) {}
Coordinate(double x, double y) : x(static_cast<int>(x)), y(static_cast<int>(y)) {}
Coordinate(const Coordinate& other) : x(other.x), y(other.y) {}
Coordinate(const std::initializer_list<int>& list) : x(*list.begin()), y(*(list.begin() + 1)) {}
Coordinate(const std::initializer_list<double>& list) : x(static_cast<int>(*list.begin())), y(static_cast<int>(*(list.begin() + 1))) {}

Coordinate& operator=(const Coordinate& other) { x = other.x; y = other.y; return *this; }
Coordinate& operator-() { x = -x; y = -y; return *this; }

Coordinate& operator+=(const Coordinate& other) { return *this = *this + other; }
Coordinate& operator-=(const Coordinate& other) { return *this = *this - other; }
Coordinate& operator*=(const Coordinate& other) { return *this = *this * other; }
Coordinate& operator/=(const Coordinate& other) { return *this = *this / other; }

Coordinate& operator*=(const int other) { return *this = *this * other; }
Coordinate& operator/=(const int other) { return *this = *this / other; }

bool in_bounds(const Coordinate& dimensions) const { return *this >= Coordinate(0, 0) and *this < dimensions; }
size_t to_index(const int width) const { return y * static_cast<size_t>(width) + x; }


Coordinate operator+(const Coordinate& lhs, const Coordinate& rhs) { return { lhs.x + rhs.x, lhs.y + rhs.y }; }
Coordinate operator-(const Coordinate& lhs, const Coordinate& rhs) { return { lhs.x - rhs.x, lhs.y - rhs.y }; }
Coordinate operator*(const Coordinate& lhs, const Coordinate& rhs) { return { lhs.x * rhs.x, lhs.y * rhs.y }; }
Coordinate operator/(const Coordinate& lhs, const Coordinate& rhs) { return { lhs.x / rhs.x, lhs.y / rhs.y }; }

Coordinate operator*(const Coordinate& lhs, const int rhs) { return { lhs.x * rhs, lhs.y * rhs }; }
Coordinate operator*(const int lhs, const Coordinate& rhs) { return rhs * lhs; }
Coordinate operator/(const Coordinate& lhs, const int rhs) { return { lhs.x / rhs, lhs.y / rhs }; }

bool operator< (const Coordinate& lhs, const Coordinate& rhs) { return lhs.x <  rhs.x and lhs.y <  rhs.y; }
bool operator<=(const Coordinate& lhs, const Coordinate& rhs) { return lhs.x <= rhs.x and lhs.y <= rhs.y; }
bool operator==(const Coordinate& lhs, const Coordinate& rhs) { return lhs.x == rhs.x and lhs.y == rhs.y; }
bool operator>=(const Coordinate& lhs, const Coordinate& rhs) { return lhs.x >= rhs.x and lhs.y >= rhs.y; }
bool operator> (const Coordinate& lhs, const Coordinate& rhs) { return lhs.x >  rhs.x and lhs.y >  rhs.y; }

std::wostream& operator<<(std::wostream& stream, const Coordinate& vector2) { return stream << vector2.x << L' ' << vector2.y; }
std::wistream& operator>>(std::wistream& stream, Coordinate& vector2) { return stream >> vector2.x >> vector2.y; }