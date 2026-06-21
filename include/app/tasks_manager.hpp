#ifndef TASKS_MANAGER_HPP
#define TASKS_MANAGER_HPP

#include "config.hpp"
#include "global.hpp"
#include "drivers/uart_console.hpp"
#include "drivers/button.hpp"
#include "drivers/dht11.hpp"
#include "middleware/msg_queue.hpp"
#include "middleware/event.hpp"
#include "middleware/command_parser.hpp"


class TasksManager {
    public:
        TasksManager(DHT11Driver::Ptr dht, UartConsole& console);
        // ~TasksManager();
        void start();

        void onButtonPress();

    private:
        static void sensorTask(void* params);
        static void consoleTask(void* params);
        static void eventHandlerTask(void* params);

        DHT11Driver::Ptr dht_;
        UartConsole& console_;
        std::unique_ptr<Button> button_;

        MsgQueue<DHT11_SensorData, 10> dataQueue_;
        MsgQueue<Event, 10> eventQueue_;
        
};

#endif // TASKS_MANAGER_HPP