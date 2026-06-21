#include "drivers/button.hpp"

extern "C" {
    #include "driver/gpio.h"
    #include "esp_log.h"
}

// Constructor for the Button class
Button::Button(int pin, Callback cb, void* context) : pin_(pin), cb_(cb), context_(context) {
    // Configure the GPIO pin for the button as input with puul-up resistor with isr on falling edge
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = 1ULL << pin_;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    
    gpio_config(&io_conf);

    // Install GPIO ISR service    gpio_install_isr_service(0);
    // Attach the ISR handler for the button pin
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    gpio_isr_handler_add(static_cast<gpio_num_t>(pin_), isr_handler, this);
}

Button::~Button() {
    // Remove the ISR handler and uninstall the ISR service
    gpio_isr_handler_remove(static_cast<gpio_num_t>(pin_));
    // gpio_install_isr_service(0);
    gpio_reset_pin(static_cast<gpio_num_t>(pin_));
}

void Button::isr_handler(void* arg) {
    Button* button = static_cast<Button*>(arg);
    if (button && button->cb_) {
        button->cb_(button->context_);
    }
}