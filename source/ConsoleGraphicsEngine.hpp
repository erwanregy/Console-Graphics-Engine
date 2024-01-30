#pragma once

#include "pch.hpp"

#include "Coordinate.hpp"
#include "Pixel.hpp"
#include "Sprite.hpp"


class Timer {
public:

    Timer();

    void start();
    void stop();

    [[nodiscard]] double elapsed() const;

    void restart();

private:

    struct { std::chrono::high_resolution_clock::time_point start, stop; } time_;
};


class ConsoleGraphicsEngine {
public:

    ConsoleGraphicsEngine() = delete;
    ConsoleGraphicsEngine(const ConsoleGraphicsEngine&) = delete;
    ConsoleGraphicsEngine(ConsoleGraphicsEngine&&) = delete;
    ConsoleGraphicsEngine& operator=(const ConsoleGraphicsEngine&) = delete;
    ConsoleGraphicsEngine& operator=(ConsoleGraphicsEngine&&) = delete;

    explicit ConsoleGraphicsEngine(const Coordinate<int>& screen_dimensions = { 80, 40 }, const Coordinate<int>& font_dimensions = { 8, 8 }, const std::string& title = "Console Graphics Engine");

    virtual ~ConsoleGraphicsEngine();

    void start();

    void run();

protected:

    virtual void initialise();
    virtual void update(double frame_time);
    virtual void close();

    void stop() const;

    enum class ButtonState : char {
        Released,
        Pressed,
        Held,
    };

    // enum class MouseWheelState : char {
    // 	Stationary,
    // 	Up,
    // 	Down,
    // };

    enum class Key : unsigned char {
        Escape = VK_ESCAPE,
        F1 = VK_F1,
        F2 = VK_F2,
        F3 = VK_F3,
        F4 = VK_F4,
        F5 = VK_F5,
        F6 = VK_F6,
        F7 = VK_F7,
        F8 = VK_F8,
        F9 = VK_F9,
        F10 = VK_F10,
        F11 = VK_F11,
        F12 = VK_F12,
        Insert = VK_INSERT,
        PrintScreen = VK_SNAPSHOT,
        Delete = VK_DELETE,
        Minus = VK_OEM_MINUS,
        Equals = VK_OEM_PLUS,
        Backspace = VK_BACK,
        Tab = VK_TAB,
        LeftBracket = VK_OEM_4,
        RightBracket = VK_OEM_6,
        Enter = VK_RETURN,
        CapsLock = VK_CAPITAL,
        Semicolon = VK_OEM_1,
        Apostrophe = VK_OEM_7,
        HashTag = VK_OEM_3,
        Shift = VK_SHIFT,
        LeftShift = VK_LSHIFT,
        BackSlash = VK_OEM_5,
        Comma = VK_OEM_COMMA,
        Period = VK_OEM_PERIOD,
        ForwardSlash = VK_OEM_2,
        RightShift = VK_RSHIFT,
        Control = VK_CONTROL,
        LeftControl = VK_LCONTROL,
        Function = VK_OEM_CLEAR,
        Windows = VK_LWIN,
        Alt = VK_MENU,
        LeftAlt = VK_LMENU,
        Space = VK_SPACE,
        AltGr = VK_RMENU,
        RightControl = VK_RCONTROL,
        Left = VK_LEFT,
        Up = VK_UP,
        Right = VK_RIGHT,
        Down = VK_DOWN,
        Home = VK_HOME,
        End = VK_END,
        PageUp = VK_PRIOR,
        PageDown = VK_NEXT,
        ScrollLock = VK_SCROLL,
        NumLock = VK_NUMLOCK,
    };

    enum class MouseButton : char {
        Left = VK_LBUTTON,
        Right = VK_RBUTTON,
        Middle = VK_MBUTTON,
    };


    [[nodiscard]] const Coordinate<int>& screen_dimensions() const;
    [[nodiscard]] int screen_width() const;
    [[nodiscard]] int screen_height() const;

    [[nodiscard]] static bool is_active();

    [[nodiscard]] static ButtonState key(Key);
    [[nodiscard]] static ButtonState key(char key);

    [[nodiscard]] static ButtonState mouse_button(MouseButton);

    const Coordinate<int>& mouse_position();
    int mouse_x();
    int mouse_y();

    // MouseWheelState mouse_wheel();


    void clear_screen(const Pixel & = Pixel::Colour::Black);

    void draw_character(const Coordinate<int>&, const WCHAR character, const Pixel::Colour = Pixel::Colour::White);

    void draw_pixel(const Coordinate<int>&, const Pixel & = Pixel::Colour::White);

    void draw_sprite(const Coordinate<int>&, const Sprite&, const int scale = 1);

    void draw_string(const Coordinate<int>&, const std::wstring&, Pixel::Colour = Pixel::Colour::White);
    void draw_string(const Coordinate<int>&, const std::string&, Pixel::Colour = Pixel::Colour::White);

    void draw_line(const Coordinate<int>& start, const Coordinate<int>& stop, const Pixel & = Pixel::Colour::White);

    void draw_triangle(const std::array<Coordinate<int>, 3>&, const Pixel & = Pixel::Colour::White);
    void draw_filled_triangle(const std::array<Coordinate<int>, 3>&, const Pixel & = Pixel::Colour::White);

    void draw_circle(const Coordinate<int>& centre, const int radius = 1, const Pixel & = Pixel::Colour::White);
    void draw_filled_circle(const Coordinate<int>& centre, const int radius = 1, const Pixel & = Pixel::Colour::White);

    void draw_rectangle(const Coordinate<int>& top_left, const Coordinate<int>& bottom_right, const Pixel & = Pixel::Colour::White);
    void draw_filled_rectangle(const Coordinate<int>& top_left, const Coordinate<int>& bottom_right, const Pixel & = Pixel::Colour::White);

private:

    Timer timer_ = Timer();
    const struct {
        HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
        HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
        HANDLE original = nullptr;
    } console_;
    Coordinate<int> mouse_position_ = { 0, 0 };

    const Coordinate<int> screen_dimensions_;
    const std::string title_;
    std::vector<CHAR_INFO> buffer_;
    const std::unique_ptr<SMALL_RECT> window_region_;

    inline static std::atomic<bool> active_ = false;
    inline static std::mutex mutex_ = std::mutex();
    inline static std::condition_variable game_finished_ = std::condition_variable();

    void render(double frame_rate) const;

    [[nodiscard]] static ButtonState button(char button);

    [[nodiscard]] std::vector<INPUT_RECORD> input_record() const;

    static BOOL close_handler(DWORD event);
};
