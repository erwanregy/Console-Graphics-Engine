#pragma once

#include "pch.hpp"


struct Pixel {

    enum class Colour : WORD {
        Black = 0x0000,
        DarkBlue = 0x0001,
        DarkGreen = 0x0002,
        DarkCyan = 0x0003,
        DarkRed = 0x0004,
        Purple = 0x0005,
        Brown = 0x0006,
        LightGrey = 0x0007,
        DarkGrey = 0x0008,
        Blue = 0x0009,
        Green = 0x000A,
        Cyan = 0x000B,
        Red = 0x000C,
        Magenta = 0x000D,
        Yellow = 0x000E,
        White = 0x000F,
    } colour;

    enum class Shade : WCHAR {
        Empty = 0x0020,
        Quarter = 0x2591,
        Half = 0x2592,
        ThreeQuarters = 0x2593,
        Full = 0x2588,
    } shade;

    Pixel();
    Pixel(const Colour);
    Pixel(const Shade);
    Pixel(const Colour, const Shade);
    Pixel(const Colour foreground, const Colour background, const Shade);
    Pixel(const double luminance);
};

Pixel::Colour background(Pixel::Colour);

std::ostream& operator<<(std::ostream&, const Pixel&);
std::istream& operator>>(std::istream&, Pixel&);

Pixel::Colour operator|(Pixel::Colour, Pixel::Colour);