#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <functional>

class Button {
public:
    using Callback = void(*)(void* context);
    Button(int pin, Callback cb, void* context);
    ~Button();

    Button(const Button&) = delete;
    Button& operator=(const Button&) = delete;

private:
    int pin_;
    Callback cb_;
    void* context_;
    static void isr_handler(void* arg);
};

#endif // BUTTON_HPP