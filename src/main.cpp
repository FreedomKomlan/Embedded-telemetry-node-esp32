#include "global.hpp"
#include "drivers/uart_console.hpp"
#include "drivers/dht11.hpp"
#include "app/tasks_manager.hpp"
#include "config.hpp"
#include "middleware/msg_queue.hpp"
#include "app/tasks_manager.hpp"

UartConsole console(UART_NUMBER, UART_TX_PIN, UART_RX_PIN, BAUDRATE);

extern "C" void app_main() {
    console.writeln("UART Communication initialized successfully!");
    auto dht11 = DHT11Driver::create(4); // Assuming data pin is GPIO4
    if (!dht11) {
        console.writeln("Failed to initialize DHT11 sensor!");
        while (true) {
            vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
        }
    }

    TasksManager tasks_manager(std::move(dht11), console);
    tasks_manager.start();

    while (true) {
        vTaskDelay(portMAX_DELAY); // Delay for 1 second
    }
}