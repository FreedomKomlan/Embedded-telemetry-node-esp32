#ifndef TASKS_MANAGER_HPP
#define TASKS_MANAGER_HPP

#include "global.hpp"
#include "drivers/uart_console.hpp"
#include "drivers/dht11.hpp"

class TasksManager {
    public:
        TasksManager(DHT11Driver::Ptr dht, UartConsole& console);
        // ~TasksManager();
        void start();

    private:
        DHT11Driver::Ptr dht_;
        UartConsole& console_;
        static void sensorTask(void* params);
};

#endif // TASKS_MANAGER_HPP