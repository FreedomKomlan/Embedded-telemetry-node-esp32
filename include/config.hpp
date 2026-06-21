#ifndef CONFIG_HPP
#define CONFIG_HPP
#include "global.hpp"

constexpr int DHT11_DATA_PIN = 4; // GPIO4 for DHT11 data
constexpr int BUTTON_PIN = 5; // GPIO5 for button input
constexpr int UART_NUMBER = 0; // UART0
constexpr int UART_TX_PIN = 1; // GPIO1 for UART TX
constexpr int UART_RX_PIN = 3; // GPIO3 for UART RX
constexpr int BAUDRATE = 115200; 
constexpr TickType_t SENSOR_READ_INTERVAL_MS = 2000;
constexpr TickType_t DATA_WRITE_INTERVAL_MS = 1000;
constexpr TickType_t EVENT_HANDLING_INTERVAL_MS = 100;

#endif // CONFIG_HPP