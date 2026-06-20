#include "app/tasks_manager.hpp"
#include <cstdio>
#include "config.hpp"

TasksManager::TasksManager(DHT11Driver::Ptr dht, UartConsole& console) : dht_(std::move(dht)), console_(console) {

}

void TasksManager::start() {
    xTaskCreate(sensorTask, "SensorTask", 4096, this, 2, nullptr);
}

void TasksManager::sensorTask(void* params) {
    TasksManager* manager = static_cast<TasksManager*>(params);
    // Period to read sensor data and print to console
    TickType_t lastAcquisitionTime = xTaskGetTickCount();
    
    while (true) {
        auto data = manager->dht_->read();
        if (data) {
            char buffer[64];
            snprintf(buffer, sizeof(buffer), "Temp: %.1f C, Humidity: %.1f%%", data->temperature, data->humidity);
            manager->console_.writeln(buffer);
        } else {
            manager->console_.writeln("Failed to read from DHT11 sensor!");
        }
        vTaskDelayUntil(&lastAcquisitionTime, pdMS_TO_TICKS(SENSOR_READ_INTERVAL_MS));
    }
}