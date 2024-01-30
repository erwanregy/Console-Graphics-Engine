#pragma once

#include "pch.hpp"

#include "Coordinate.hpp"
#include "Pixel.hpp"


class Sprite {
public:

    Sprite();
    Sprite(const Coordinate<int>& dimensions);
    Sprite(const std::string& filename);

    const Coordinate<int>& dimensions() const;
    int width() const;
    int height() const;
    Pixel pixel(const Coordinate<int>& coordinate) const;

    void save(const std::string& filename) const;
    void load(const std::string& filename);

private:

    Coordinate<int> dimensions_;
    std::vector<Pixel> texture_;
};
