#include "pch.hpp"

#include "Sprite.hpp"


Sprite::Sprite() : dimensions_({ 0, 0 }) {}

Sprite::Sprite(const Coordinate<int>& dimensions) : dimensions_(dimensions), texture_(dimensions.x* dimensions.y) {}

Sprite::Sprite(const std::string& filename) {
    load(filename);
}


const Coordinate<int>& Sprite::dimensions() const {
    return dimensions_;
}

int Sprite::width() const {
    return dimensions_.x;
}

int Sprite::height() const {
    return dimensions_.y;
}

Pixel Sprite::pixel(const Coordinate<int>& coordinate) const {
    if (coordinate.in_bounds(dimensions_)) {
        return texture_[coordinate.to_index(dimensions_.x)];
    } else {
        return { Pixel::Colour::White, Pixel::Shade::Empty };
    }
}


void Sprite::save(const std::string& file_name) const {
    std::ofstream file_stream(file_name, std::ios::out | std::ios::trunc | std::ios::binary);
    if (not file_stream.is_open()) {
        throw std::runtime_error("Unable to open file '" + file_name + "'");
    }
    file_stream << dimensions_;
    for (const Pixel& pixel : texture_) {
        file_stream << ' ' << pixel;
    }
    file_stream.close();
}

void Sprite::load(const std::string& file_name) {
    std::ifstream file_stream(file_name, std::ios::in | std::ios::binary);
    if (not file_stream.is_open()) {
        throw std::runtime_error("Unable to open file '" + file_name + "'");
    }
    file_stream >> dimensions_;
    texture_ = std::vector<Pixel>(dimensions_.x * dimensions_.y);
    for (Pixel& pixel : texture_) {
        file_stream >> pixel;
    }
    file_stream.close();
}