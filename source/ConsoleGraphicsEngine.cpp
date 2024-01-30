#include "pch.hpp"

#include "ConsoleGraphicsEngine.hpp"
#include "Coordinate.hpp"


ConsoleGraphicsEngine::ConsoleGraphicsEngine(const Coordinate<int>& screen_dimensions, const Coordinate<int>& font_dimensions, const std::string& title)
    : screen_dimensions_(screen_dimensions), title_(title), buffer_(screen_dimensions.x* screen_dimensions.y), window_region_(std::make_unique<SMALL_RECT>(0, 0, static_cast<SHORT>(screen_dimensions.x - 1), static_cast<SHORT>(screen_dimensions.y - 1))) {
    if (console_.output == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Failed to get output console handle");
    }

    if (console_.input == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Failed to get input console handle");
    }

    CONSOLE_CURSOR_INFO cursor_info;
    if (not GetConsoleCursorInfo(console_.output, &cursor_info)) {
        throw std::runtime_error("Failed to get console cursor info");
    }
    cursor_info.bVisible = FALSE;
    if (not SetConsoleCursorInfo(console_.output, &cursor_info)) {
        throw std::runtime_error("Failed to set console cursor info");
    }

    constexpr SMALL_RECT temp_region = { 0, 0, 1, 1 };
    if (not SetConsoleWindowInfo(console_.output, TRUE, &temp_region)) {
        throw std::runtime_error("Failed to set console window info");
    }

    if (not SetConsoleScreenBufferSize(console_.output, { static_cast<SHORT>(screen_dimensions.x), static_cast<SHORT>(screen_dimensions.y) })) {
        throw std::runtime_error("Failed to set console screen buffer size");
    }

    if (not SetConsoleActiveScreenBuffer(console_.output)) {
        throw std::runtime_error("Failed to set console as active screen buffer");
    }

    CONSOLE_FONT_INFOEX font_info = {
        sizeof(CONSOLE_FONT_INFOEX),
        0,
        { static_cast<SHORT>(font_dimensions.x), static_cast<SHORT>(font_dimensions.y) },
        FF_DONTCARE,
        0,
        L""
    };
    if (not SetCurrentConsoleFontEx(console_.output, FALSE, &font_info)) {
        throw std::runtime_error("Failed to set console font");
    }

    CONSOLE_SCREEN_BUFFER_INFO screen_info;
    if (not GetConsoleScreenBufferInfo(console_.output, &screen_info)) {
        throw std::runtime_error("Failed to get console screen_dimensions buffer info");
    } else {
        if (const Coordinate<int> window_dimensions = { screen_info.dwMaximumWindowSize.X, screen_info.dwMaximumWindowSize.Y };
            screen_dimensions > window_dimensions) {
            throw std::runtime_error(std::format(
                "Screen dimensions ({}, {}) are larger than the maximum window dimensions ({}, {})",
                screen_dimensions.x, screen_dimensions.y,
                window_dimensions.x, window_dimensions.y
            ));
        }
    }

    if (not SetConsoleWindowInfo(console_.output, TRUE, window_region_.get())) {
        throw std::runtime_error("Failed to set console window info");
    }

    if (not SetConsoleMode(console_.output, ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
        throw std::runtime_error("Failed to set output console modes");
    }

    if (not SetConsoleMode(console_.input, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT)) {
        throw std::runtime_error("Failed to set input console modes");
    }

    if (not SetConsoleTitleA(title.c_str())) {
        throw std::runtime_error("Failed to set console title");
    }

    if (not SetConsoleCtrlHandler(reinterpret_cast<PHANDLER_ROUTINE>(close_handler), TRUE)) {
        throw std::runtime_error("Failed to set console control handler");
    }
}

ConsoleGraphicsEngine::~ConsoleGraphicsEngine() {
    stop();
}

void ConsoleGraphicsEngine::start() {
    active_ = true;
    auto thread = std::thread(&ConsoleGraphicsEngine::run, this);
    thread.join();
}

void ConsoleGraphicsEngine::run() {
    timer_.start();

    initialise();

    while (active_) {
        timer_.stop();

        const double frame_time = timer_.elapsed();
        const double frame_rate = 1.0 / frame_time;

        timer_.restart();

        update(frame_time);

        render(frame_rate);
    }

    close();
}



// Overridable functions


void ConsoleGraphicsEngine::initialise() {}

void ConsoleGraphicsEngine::update(const double frame_time) {

    clear_screen();

    std::random_device random_device;

    // Coordinate<int> coordinate;
    // for (coordinate.x = 0; coordinate.x < screen_dimensions_.x; ++coordinate.x) {
    //     for (coordinate.y = 0; coordinate.y < screen_dimensions_.y; ++coordinate.y) {
    //         draw_pixel(coordinate, Pixel(static_cast<Pixel::Colour>(random_device() % static_cast<int>(Pixel::Colour::White) + static_cast<int>(Pixel::Colour::Black))));
    //     }
    // }

    if (key(Key::Escape) == ButtonState::Pressed) {
        stop();
    }
}

void ConsoleGraphicsEngine::close() { std::cout << "Hello world!"; }



void ConsoleGraphicsEngine::render(const double frame_rate) const {
    if (not SetConsoleTitleA((title_ + " - FPS: " + std::to_string(frame_rate)).c_str())) {
        throw std::runtime_error("Failed to set console title");
    }

    if (not WriteConsoleOutputW(console_.output, buffer_.data(), { static_cast<SHORT>(screen_dimensions_.x), static_cast<SHORT>(screen_dimensions_.y) }, { 0, 0 }, window_region_.get())) {
        throw std::runtime_error("Failed to draw to console");
    }
}

void ConsoleGraphicsEngine::stop() const {
    active_ = false;
    SetConsoleActiveScreenBuffer(console_.original);
    game_finished_.notify_one();
}



// Getters


const Coordinate<int>& ConsoleGraphicsEngine::screen_dimensions() const {
    return screen_dimensions_;
}

int ConsoleGraphicsEngine::screen_width() const {
    return screen_dimensions_.x;
}

int ConsoleGraphicsEngine::screen_height() const {
    return screen_dimensions_.y;
}


ConsoleGraphicsEngine::ButtonState ConsoleGraphicsEngine::key(Key key) {
    return button(static_cast<char>(key));
}

ConsoleGraphicsEngine::ButtonState ConsoleGraphicsEngine::key(const char key) {
    if ((key >= 'A' and key <= 'Z') or (key >= '0' and key <= '9')) {
        return button(key);
    } else {
        throw std::invalid_argument("Invalid Key '" + std::string(1, key) + "' requested");
    }
}


ConsoleGraphicsEngine::ButtonState ConsoleGraphicsEngine::mouse_button(const MouseButton mouse_button) {
    return button(static_cast<const char>(mouse_button));
}


const Coordinate<int>& ConsoleGraphicsEngine::mouse_position() {
    for (const auto& input_records = input_record();
        const auto & [event_type, event] : input_records) {
        if (event_type == MOUSE_EVENT and event.MouseEvent.dwEventFlags == MOUSE_MOVED) {
            mouse_position_ = { event.MouseEvent.dwMousePosition.X, event.MouseEvent.dwMousePosition.Y };
        }
    }
    return mouse_position_;
}

int ConsoleGraphicsEngine::mouse_x() {
    return mouse_position().x;
}

int ConsoleGraphicsEngine::mouse_y() {
    return mouse_position().y;
}


// MouseWheelState ConsoleGraphicsEngine::mouse_wheel() {
// 	const auto& input_records = input_record();
// 	for (const auto& input_record : input_records) {
// 		if (input_record.EventType == MOUSE_EVENT and input_record.Event.MouseEvent.dwEventFlags == MOUSE_WHEELED) {
// 			const auto& wheel = input_record.Event.MouseEvent.dwButtonState;
// 			if (HIWORD(wheel) > 0) {
//                 return MouseWheelState::Up;
// 			} else {
//                 return MouseWheelState::Down;
//             }
// 		}
// 	}
// 	return MouseWheelState::Stationary;
// }


bool ConsoleGraphicsEngine::is_active() {
    return GetConsoleWindow() == GetForegroundWindow();
}

ConsoleGraphicsEngine::ButtonState ConsoleGraphicsEngine::button(const char button) {
    if (not is_active()) {
        return ButtonState::Released;
    }
    if (const auto state = GetAsyncKeyState(button); state & 0x8000) {
        if (state & 0x0001) {
            return ButtonState::Pressed;
        } else {
            return ButtonState::Held;
        }
    } else {
        return ButtonState::Released;
    }
}

std::vector<INPUT_RECORD> ConsoleGraphicsEngine::input_record() const {
    DWORD num_events = 0;
    if (not GetNumberOfConsoleInputEvents(console_.input, &num_events)) {
        throw std::runtime_error("Failed to get number of console input events");
    }
    std::vector<INPUT_RECORD> input_records(num_events);
    if (num_events > 0) {
        ReadConsoleInputW(console_.input, input_records.data(), num_events, &num_events);
    }
    return input_records;
}



// Drawing functions


void ConsoleGraphicsEngine::clear_screen(const Pixel& pixel) {
    Coordinate<int> coordinate;
    for (coordinate.x = 0; coordinate.x < screen_dimensions_.x; ++coordinate.x) {
        for (coordinate.y = 0; coordinate.y < screen_dimensions_.y; ++coordinate.y) {
            draw_pixel(coordinate, pixel);
        }
    }
}

void ConsoleGraphicsEngine::draw_character(const Coordinate<int>& coordinate, const WCHAR character, const Pixel::Colour colour) {
    if (coordinate.in_bounds(screen_dimensions_)) {
        const size_t index = coordinate.to_index(screen_dimensions_.x);
        buffer_[index].Char.UnicodeChar = character;
        buffer_[index].Attributes = static_cast<WORD>(colour);
    }
}

void ConsoleGraphicsEngine::draw_pixel(const Coordinate<int>& coordinate, const Pixel& pixel) {
    draw_character(coordinate, static_cast<WCHAR>(pixel.shade), pixel.colour);
}

void ConsoleGraphicsEngine::draw_sprite(const Coordinate<int>& coordinate, const Sprite& sprite, const int scale) {
    const Coordinate<int>& dimensions = sprite.dimensions() * scale;
    Coordinate<int> current;
    for (current.x = 0; current.x < dimensions.x; ++current.x) {
        for (current.y = 0; current.y < dimensions.y; ++current.y) {
            if (const Pixel& pixel = sprite.pixel(current / scale); pixel.shade != Pixel::Shade::Empty) {
                draw_pixel(coordinate + current, pixel);
            }
        }
    }
}

void ConsoleGraphicsEngine::draw_string(const Coordinate<int>& coordinate, const std::wstring& string, Pixel::Colour colour) {
    for (Coordinate<int> current = { 0, 0 }; current.x < static_cast<int>(string.length()); ++current.x) {
        draw_character(coordinate + current, string[current.x], colour);
    }
}

void ConsoleGraphicsEngine::draw_string(const Coordinate<int>& coordinate, const std::string& string, Pixel::Colour colour) {
    for (Coordinate<int> current = { 0, 0 }; current.x < static_cast<int>(string.length()); ++current.x) {
        draw_character(coordinate + current, static_cast<WCHAR>(string[current.x]), colour);
    }
}

void ConsoleGraphicsEngine::draw_line(const Coordinate<int>& start, const Coordinate<int>& end, const Pixel& pixel) {
    Coordinate<int> current = start;
    Coordinate<int> delta = end - start;
    const Coordinate<int> step = { (delta.x > 0) - (delta.x < 0), (delta.y > 0) - (delta.y < 0) };
    auto abs = [](int x) { return x > 0 ? x : -x; };
    delta = { abs(delta.x), abs(delta.y) };

    if (delta.x > delta.y) {
        int error = delta.x / 2;
        while (current.x != end.x) {
            draw_pixel(current, pixel);
            error -= delta.y;
            if (error < 0) {
                current.y += step.y;
                error += delta.x;
            }
            current.x += step.x;
        }
    } else {
        int error = delta.y / 2;
        while (current.y != end.y) {
            draw_pixel(current, pixel);
            error -= delta.x;
            if (error < 0) {
                current.x += step.x;
                error += delta.y;
            }
            current.y += step.y;
        }
    }
}

void ConsoleGraphicsEngine::draw_triangle(const std::array<Coordinate<int>, 3>& vertices, const Pixel& pixel) {
    draw_line(vertices[0], vertices[1], pixel);
    draw_line(vertices[1], vertices[2], pixel);
    draw_line(vertices[2], vertices[0], pixel);
}

void ConsoleGraphicsEngine::draw_filled_triangle(const std::array<Coordinate<int>, 3>& vertices, const Pixel& pixel) {
    // auto swap = [](Coordinate<int>& a, Coordinate<int>& b) {
    //     const Coordinate<int> temp = a;
    // 	a = b;
    // 	b = temp;
    // };
    // auto draw_scan_line = [&](const Coordinate<int>& start, const Coordinate<int>& end) {
    //     for (int x = start.x; x <= end.x; ++x) {
    //         draw_pixel(Coordinate<int>(x, start.y), pixel);
    //     }
    // };

    // std::array<Coordinate<int>, 3> sorted_vertices = vertices;
    // std::sort(sorted_vertices.begin(), sorted_vertices.end(), [](const Coordinate<int>& a, const Coordinate<int>& b) { return a.y < b.y; }

    int x1 = vertices[0].x; int y1 = vertices[0].y;
    int x2 = vertices[1].x; int y2 = vertices[1].y;
    int x3 = vertices[2].x; int y3 = vertices[2].y;

    auto SWAP = [](int& x, int& y) { int t = x; x = y; y = t; };
    auto drawline = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; i++) draw_pixel({ i, ny }, pixel); };

    int t1x, t2x, y, minx, maxx, t1xp, t2xp;
    bool changed1 = false;
    bool changed2 = false;
    int signx1, signx2, dx1, dy1, dx2, dy2;
    int e1, e2;
    // Sort vertices
    if (y1 > y2) { SWAP(y1, y2); SWAP(x1, x2); }
    if (y1 > y3) { SWAP(y1, y3); SWAP(x1, x3); }
    if (y2 > y3) { SWAP(y2, y3); SWAP(x2, x3); }

    t1x = t2x = x1; y = y1;   // Starting points
    dx1 = (int)(x2 - x1); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; } else signx1 = 1;
    dy1 = (int)(y2 - y1);

    dx2 = (int)(x3 - x1); if (dx2 < 0) { dx2 = -dx2; signx2 = -1; } else signx2 = 1;
    dy2 = (int)(y3 - y1);

    if (dy1 > dx1) {   // swap values
        SWAP(dx1, dy1);
        changed1 = true;
    }
    if (dy2 > dx2) {   // swap values
        SWAP(dy2, dx2);
        changed2 = true;
    }

    e2 = (int)(dx2 >> 1);
    // Flat top, just process the second half
    if (y1 == y2) goto next;
    e1 = (int)(dx1 >> 1);

    for (int i = 0; i < dx1;) {
        t1xp = 0; t2xp = 0;
        if (t1x < t2x) { minx = t1x; maxx = t2x; } else { minx = t2x; maxx = t1x; }
        // process first line until y value is about to change
        while (i < dx1) {
            ++i;
            e1 += dy1;
            while (e1 >= dx1) {
                e1 -= dx1;
                if (changed1) t1xp = signx1;
                else          goto next1;
            }
            if (changed1) break;
            else t1x += signx1;
        }
        // Move line
    next1:
        // process second line until y value is about to change
        while (true) {
            e2 += dy2;
            while (e2 >= dx2) {
                e2 -= dx2;
                if (changed2) t2xp = signx2;
                else          goto next2;
            }
            if (changed2)     break;
            else              t2x += signx2;
        }
    next2:
        if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
        if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
        drawline(minx, maxx, y);    // Draw line from min to max points found on the y
                                     // Now increase y
        if (!changed1) t1x += signx1;
        t1x += t1xp;
        if (!changed2) t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y == y2) break;

    }
next:
    // Second half
    dx1 = (int)(x3 - x2); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; } else signx1 = 1;
    dy1 = (int)(y3 - y2);
    t1x = x2;

    if (dy1 > dx1) {   // swap values
        SWAP(dy1, dx1);
        changed1 = true;
    } else changed1 = false;

    e1 = (int)(dx1 >> 1);

    for (int i = 0; i <= dx1; ++i) {
        t1xp = 0; t2xp = 0;
        if (t1x < t2x) { minx = t1x; maxx = t2x; } else { minx = t2x; maxx = t1x; }
        // process first line until y value is about to change
        while (i < dx1) {
            e1 += dy1;
            while (e1 >= dx1) {
                e1 -= dx1;
                if (changed1) { t1xp = signx1; break; } else          goto next3;
            }
            if (changed1) break;
            else   	   	  t1x += signx1;
            if (i < dx1) ++i;
        }
    next3:
        // process second line until y value is about to change
        while (t2x != x3) {
            e2 += dy2;
            while (e2 >= dx2) {
                e2 -= dx2;
                if (changed2) t2xp = signx2;
                else          goto next4;
            }
            if (changed2)     break;
            else              t2x += signx2;
        }
    next4:

        if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
        if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
        drawline(minx, maxx, y);
        if (!changed1) t1x += signx1;
        t1x += t1xp;
        if (!changed2) t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y > y3) return;
    }
}

void ConsoleGraphicsEngine::draw_circle(const Coordinate<int>& centre, const int radius, const Pixel& pixel) {
    Coordinate<int> current = { 0, radius };
    int p = 1 - radius;
    while (current.x <= current.y) {
        draw_pixel(centre + current, pixel);
        draw_pixel(centre - current, pixel);
        draw_pixel(centre + Coordinate<int>(current.y, current.x), pixel);
        draw_pixel(centre - Coordinate<int>(current.y, current.x), pixel);
        draw_pixel(centre + Coordinate<int>(-current.x, current.y), pixel);
        draw_pixel(centre - Coordinate<int>(-current.x, current.y), pixel);
        draw_pixel(centre + Coordinate<int>(current.x, -current.y), pixel);
        draw_pixel(centre - Coordinate<int>(current.x, -current.y), pixel);
        if (p < 0) {
            p += 2 * current.x + 3;
        } else {
            p += 2 * (current.x - current.y) + 5;
            --current.y;
        }
        ++current.x;
    }
}

void ConsoleGraphicsEngine::draw_filled_circle(const Coordinate<int>& centre, const int radius, const Pixel& pixel) {
    Coordinate<int> current = { 0, radius };
    int p = 1 - radius;
    while (current.x <= current.y) {
        draw_line(centre + Coordinate<int>(-current.y, current.x), centre + Coordinate<int>(current.y, current.x), pixel);
        draw_line(centre + Coordinate<int>(-current.x, current.y), centre + current, pixel);
        draw_line(centre - current, centre + Coordinate<int>(current.x, -current.y), pixel);
        draw_line(centre + Coordinate<int>(-current.y, -current.x), centre + Coordinate<int>(current.y, -current.x), pixel);
        if (p < 0) {
            p += 2 * current.x + 3;
        } else {
            p += 2 * (current.x - current.y) + 5;
            --current.y;
        }
        ++current.x;
    }
}

void ConsoleGraphicsEngine::draw_rectangle(const Coordinate<int>& top_left, const Coordinate<int>& bottom_right, const Pixel& pixel) {
    Coordinate<int> current;
    for (current.x = top_left.x; current.x <= bottom_right.x; ++current.x) {
        for (current.y = top_left.y; current.y <= bottom_right.y; ++current.y) {
            if (current.x == top_left.x or current.x == bottom_right.x or
                current.y == top_left.y or current.y == bottom_right.y) {
                draw_pixel(current, pixel);
            }
        }
    }
}

void ConsoleGraphicsEngine::draw_filled_rectangle(const Coordinate<int>& top_left, const Coordinate<int>& bottom_right, const Pixel& pixel) {
    Coordinate<int> current;
    for (current.x = top_left.x; current.x <= bottom_right.x; ++current.x) {
        for (current.y = top_left.y; current.y <= bottom_right.y; ++current.y) {
            draw_pixel(current, pixel);
        }
    }
}


BOOL ConsoleGraphicsEngine::close_handler(const DWORD event) {
    if (event == CTRL_CLOSE_EVENT) {
        active_ = false;
        std::unique_lock unique_lock(mutex_);
        game_finished_.wait(unique_lock);
        return TRUE;
    } else {
        return FALSE;
    }
}


Timer::Timer() { time_.start = time_.stop = std::chrono::high_resolution_clock::now(); }

void Timer::start() { time_.start = std::chrono::high_resolution_clock::now(); }

void Timer::stop() { time_.stop = std::chrono::high_resolution_clock::now(); }

double Timer::elapsed() const { return std::chrono::duration_cast<std::chrono::duration<double>>(time_.stop - time_.start).count(); }

void Timer::restart() { time_.start = time_.stop; }