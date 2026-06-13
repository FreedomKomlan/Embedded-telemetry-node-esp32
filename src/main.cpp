#include "drivers/uart_console.hpp"
#include "global.hpp"

UartConsole console(0, 1, 3, 115200);

extern "C" void app_main() {
    console.writeln("Ready for UART communication");

    while (true) {
        console.writeln("Hello from ESP32!");
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
    }
}