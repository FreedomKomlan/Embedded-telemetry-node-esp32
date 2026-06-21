#ifndef EVENT_HPP
#define EVENT_HPP

#include <cstdint>

enum class Event : uint8_t {
    BUTTON_PRESS,
    SENSOR_ERROR
    
};

#endif // EVENT_HPP