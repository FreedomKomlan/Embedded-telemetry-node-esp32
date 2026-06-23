#include "app/tasks_manager.hpp"
#include <cstdio>


TasksManager::TasksManager(DHT11Driver::Ptr dht, UartConsole& console) : dht_(std::move(dht)), console_(console) {
    button_ = std::make_unique<Button>(BUTTON_PIN, [](void* context) {static_cast<TasksManager*>(context)->onButtonPress();}, this);
}

void TasksManager::start() {
    xTaskCreate(sensorTask, "SensorTask", 2048, this, 2, nullptr); // 4096 -> 16ko let's put 2048
    xTaskCreate(consoleTask, "ConsoleTask", 3072, this, 1, nullptr); // to handle std::string dynamic allocation
    xTaskCreate(eventHandlerTask, "EventHandlerTask", 2048, this, 1, nullptr);
}

void TasksManager::onButtonPress() {
    eventQueue_.push(Event::BUTTON_PRESS);
}

void TasksManager::sensorTask(void* params) {
    TasksManager* manager = static_cast<TasksManager*>(params);
    // Period to read sensor data and print to console
    TickType_t lastAcquisitionTime = xTaskGetTickCount();
    
    while (true) {
        auto data = manager->dht_->read();
        if (data) {
            char buffer[64];
            // snprintf(buffer, sizeof(buffer), "Temp: %.1f C, Humidity: %.1f%%", data->temperature, data->humidity);
            // manager->console_.writeln(buffer);
            manager->dataQueue_.push(*data);
        } else {
            manager->eventQueue_.push(Event::SENSOR_ERROR);
        }
        vTaskDelayUntil(&lastAcquisitionTime, pdMS_TO_TICKS(SENSOR_READ_INTERVAL_MS));
    }
}

void TasksManager::consoleTask(void* params) {
    TasksManager* manager = static_cast<TasksManager*>(params);
    std::string input_line;
    TickType_t lastAcquisitionTime = xTaskGetTickCount();
    TickType_t lastCharTime = xTaskGetTickCount();
    const TickType_t inputTimeout = pdMS_TO_TICKS(200);

    while (true) {

        // get sensor data and display to console
        auto data = manager->dataQueue_.pop();
        if (data) {
            char buffer[64];
            snprintf(buffer, sizeof(buffer), "Console Task - Temp: %.1f C, Humidity: %.1f%%", data->temperature, data->humidity);
            manager->console_.writeln(buffer);
        }

        // Get command lines
        int ch = manager->console_.readByte();
        if (ch > 0) {
            // Echo
            manager->console_.writeChar(static_cast<char>(ch));
            if (ch == '\n' || ch == '\r') {
                // end of line
                std::string_view line(input_line);
                Command cmd = CommandParser::parseCommand(line);
                manager->console_.write("Command : ");
                manager->console_.writeln(line);
                switch (cmd) {
                    case Command::STATUS:
                        // Placeholder for status display
                        manager->console_.writeln("Status: OK");
                        break;

                    case Command::LAST_EVENT:
                        // Placeholder for status display
                        manager->console_.writeln("Last Event: ...");
                        break;

                    case Command::CLEAR:
                        // Placeholder for status display
                        manager->console_.writeln("Clearing...");
                        break;

                    case Command::HELP:
                        // Placeholder for status display
                        manager->console_.writeln("Commands: status, last_event, clear, help");
                        break;
                    
                    default:
                        manager->console_.writeln("Unknown command");
                        break;
                }
                input_line.clear();
            }
        }
        vTaskDelayUntil(&lastAcquisitionTime, pdMS_TO_TICKS(DATA_WRITE_INTERVAL_MS));
    }
}

void TasksManager::eventHandlerTask(void* params) {
    TasksManager* manager = static_cast<TasksManager*>(params);
    TickType_t lastAcquisitionTime = xTaskGetTickCount();

    while(true) {
        auto evt = manager->eventQueue_.pop();
        if (evt) {
            TickType_t now = xTaskGetTickCount();
            switch (*evt)
            {
            case Event::BUTTON_PRESS:
                // manager->console_.write(now);
                manager->console_.writeln("[EVENT] : Button pressed");
                break;

            case Event::SENSOR_ERROR:
                // manager->console_.write(now);
                manager->console_.writeln("[EVENT] : Sensor error");
                break;
            
            default:
                break;
            }
        }
        // vTaskDelayUntil(&now, pdMS_TO_TICKS(EVENT_CHECK_PERIOD));
        // vTaskDelay(pdMS_TO_TICKS(EVENT_HANDLING_INTERVAL_MS));
        vTaskDelayUntil(&lastAcquisitionTime, pdMS_TO_TICKS(EVENT_HANDLING_INTERVAL_MS));

    }
}