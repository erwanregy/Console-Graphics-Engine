#include "pch.hpp"

#include "Pixel.hpp"

using enum Pixel::Colour;
using enum Pixel::Shade;


std::array<Pixel, 13> luminance_to_pixel = {
    Pixel(Black),
    Pixel(DarkGrey,  Black,     Quarter),
    Pixel(DarkGrey,  Black,     Half),
    Pixel(DarkGrey,  Black,     ThreeQuarters),
    Pixel(DarkGrey),
    Pixel(LightGrey, DarkGrey,  Quarter),
    Pixel(LightGrey, DarkGrey,  Half),
    Pixel(LightGrey, DarkGrey,  ThreeQuarters),
    Pixel(LightGrey),
    Pixel(White,     LightGrey, Quarter),
    Pixel(White,     LightGrey, Half),
    Pixel(White,     LightGrey, ThreeQuarters),
    Pixel(White),
};


Pixel::Pixel() : colour(Colour::White), shade(Shade::Full) {}

Pixel::Pixel(const Colour colour) : colour(colour), shade(Shade::Full) {}

Pixel::Pixel(const Shade shade) : colour(Colour::White), shade(shade) {}

Pixel::Pixel(const Colour colour, const Shade shade) : colour(colour), shade(shade) {}

Pixel::Pixel(const Colour foreground_colour, const Colour background_colour, const Shade shade) : colour(foreground_colour | background(background_colour)), shade(shade) {}

Pixel::Pixel(const double luminance) {
    if (luminance >= 0.0 and luminance < 1.0) {
        *this = luminance_to_pixel[static_cast<size_t>(luminance * static_cast<double>(luminance_to_pixel.size()))];
    } else {
        shade = Full;
        if (luminance >= 1.0) {
            colour = White;
        } else {
            colour = Black;
        }
    }
}


Pixel::Colour background(Pixel::Colour colour) {
    return static_cast<Pixel::Colour>(static_cast<WORD>(colour) << 4);
}

std::unordered_map<Pixel::Colour, std::string> colour_to_string = {
    { Black, "black" },
    { DarkBlue, "dark_blue" },
    { DarkGreen, "dark_green" },
    { DarkCyan, "dark_cyan" },
    { DarkRed, "dark_red" },
    { Purple, "purple" },
    { Brown, "brown" },
    { LightGrey, "light_grey" },
    { DarkGrey, "dark_grey" },
    { Blue, "blue" },
    { Green, "green" },
    { Cyan, "cyan" },
    { Red, "red" },
    { Magenta, "magenta" },
    { Yellow, "yellow" },
    { White, "white" },
};

std::unordered_map<std::string, Pixel::Colour> string_to_colour = {
    { "black", Black },
    { "dark_blue", DarkBlue },
    { "dark_green", DarkGreen },
    { "dark_cyan", DarkCyan },
    { "dark_red", DarkRed },
    { "purple", Purple },
    { "brown", Brown },
    { "light_grey", LightGrey },
    { "dark_grey", DarkGrey },
    { "blue", Blue },
    { "green", Green },
    { "cyan", Cyan },
    { "red", Red },
    { "magenta", Magenta },
    { "yellow", Yellow },
    { "white", White },
};

std::unordered_map<Pixel::Shade, std::string> shade_to_string = {
    { Empty, "empty" },
    { Quarter, "quarter" },
    { Half, "half" },
    { ThreeQuarters, "three_quarters" },
    { Full, "full" },
};

std::unordered_map<std::string, Pixel::Shade> string_to_shade = {
    { "empty", Empty },
    { "quarter", Quarter },
    { "half", Half },
    { "three_quarters", ThreeQuarters },
    { "full", Full },
};

std::ostream& operator<<(std::ostream& stream, const Pixel& pixel) {
    return stream << colour_to_string[pixel.colour] << ' ' << shade_to_string[pixel.shade];
}

std::istream& operator>>(std::istream& stream, Pixel& pixel) {
    std::string colour, shade;
    stream >> colour >> shade;
    pixel = Pixel(string_to_colour[colour], string_to_shade[shade]);
    return stream;
}


Pixel::Colour operator|(Pixel::Colour lhs, Pixel::Colour rhs) {
    return static_cast<Pixel::Colour>(static_cast<WORD>(lhs) | static_cast<WORD>(rhs));
}