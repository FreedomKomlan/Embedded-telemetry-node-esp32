#ifndef TASKS_MANAGER_HPP
#define TASKS_MANAGER_HPP

#include "global.hpp"
#include "drivers/uart_console.hpp"
#include "drivers/dht11.hpp"
#include "middleware/msg_queue.hpp"

class TasksManager {
    public:
        TasksManager(DHT11Driver::Ptr dht, UartConsole& console);
        // ~TasksManager();
        void start();

    private:

        DHT11Driver::Ptr dht_;
        UartConsole& console_;
        MsgQueue<DHT11_SensorData, 10> msg_queue_;
        static void sensorTask(void* params);
        static void consoleTask(void* params);
};

#endif // TASKS_MANAGER_HPP