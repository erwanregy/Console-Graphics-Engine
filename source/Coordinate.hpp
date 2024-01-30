#pragma once

#include "pch.hpp"


template <typename type>
struct Coordinate {

    type x, y;

    Coordinate() : x(static_cast<type>(0)), y(static_cast<type>(0)) {}
    Coordinate(type x, type y) : x(x), y(y) {}
    Coordinate(const std::initializer_list<type>& list) : x(*list.begin()), y(*(list.begin() + 1)) {}

    Coordinate& operator-() { x = -x; y = -y; return *this; }

    Coordinate& operator+=(const Coordinate& other) { return *this = *this + other; }
    Coordinate& operator-=(const Coordinate& other) { return *this = *this - other; }
    Coordinate& operator*=(const Coordinate& other) { return *this = *this * other; }
    Coordinate& operator/=(const Coordinate& other) { return *this = *this / other; }

    Coordinate& operator*=(const type scalar) { return *this = *this * scalar; }
    Coordinate& operator/=(const type scalar) { return *this = *this / scalar; }

    bool in_bounds(const Coordinate& dimensions) const { return *this >= Coordinate(0, 0) and *this < dimensions; }
    size_t to_index(const int width) const { return y * static_cast<size_t>(width) + x; }

    friend Coordinate operator+(const Coordinate& lhs, const Coordinate& rhs) { return { lhs.x + rhs.x, lhs.y + rhs.y }; }
    friend Coordinate operator-(const Coordinate& lhs, const Coordinate& rhs) { return { lhs.x - rhs.x, lhs.y - rhs.y }; }
    friend Coordinate operator*(const Coordinate& lhs, const Coordinate& rhs) { return { lhs.x * rhs.x, lhs.y * rhs.y }; }
    friend Coordinate operator/(const Coordinate& lhs, const Coordinate& rhs) { return { lhs.x / rhs.x, lhs.y / rhs.y }; }

    friend Coordinate operator*(const Coordinate& lhs, const type rhs) { return { lhs.x * rhs, lhs.y * rhs }; }
    friend Coordinate operator*(const type lhs, const Coordinate& rhs) { return rhs * lhs; }
    friend Coordinate operator/(const Coordinate& lhs, const type rhs) { return { lhs.x / rhs, lhs.y / rhs }; }

    friend bool operator< (const Coordinate& lhs, const Coordinate& rhs) { return lhs.x < rhs.x and lhs.y < rhs.y; }
    friend bool operator<=(const Coordinate& lhs, const Coordinate& rhs) { return lhs.x <= rhs.x and lhs.y <= rhs.y; }
    friend bool operator==(const Coordinate& lhs, const Coordinate& rhs) = default;
    friend bool operator>=(const Coordinate& lhs, const Coordinate& rhs) { return lhs.x >= rhs.x and lhs.y >= rhs.y; }
    friend bool operator> (const Coordinate& lhs, const Coordinate& rhs) { return lhs.x > rhs.x and lhs.y > rhs.y; }

    friend std::ostream& operator<<(std::ostream& stream, const Coordinate& coordinate) { return stream << coordinate.x << ' ' << coordinate.y; }
    friend std::istream& operator>>(std::istream& stream, Coordinate& coordinate) { return stream >> coordinate.x >> coordinate.y; }

    template <typename type2>
    explicit operator Coordinate<type2>() const {
        return { static_cast<type2>(x), static_cast<type2>(y) };
    }
};