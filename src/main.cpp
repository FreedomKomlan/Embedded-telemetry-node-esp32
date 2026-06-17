#include "global.hpp"
#include "drivers/uart_console.hpp"
#include "drivers/dht11.hpp"

UartConsole console(0, 1, 3, 115200);

extern "C" void app_main() {
    console.writeln("UART Communication initialized successfully!");
    auto dht11 = DHT11Driver::create(4); // Assuming data pin is GPIO4
    if (!dht11) {
        console.writeln("Failed to initialize DHT11 sensor!");
        while (true) {
            vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
        }
    }

    while (true) {
        auto data = dht11->read();
        if (data) {
            char buffer[64];
            snprintf(buffer, sizeof(buffer), "Temp: %.1f C, Humidity: %.1f%%", data->temperature, data->humidity);
            console.writeln(buffer);
        } else {
            console.writeln("Failed to read from DHT11 sensor!");
        }
        // console.writeln("Hello from ESP32!");
        vTaskDelay(pdMS_TO_TICKS(2000)); // Delay for 1 second
    }
}