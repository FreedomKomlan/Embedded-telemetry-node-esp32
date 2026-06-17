#include "drivers/dht11.hpp"
#include <new>

extern "C" {
    #include "driver/gpio.h"
    #include "esp_log.h"
    #include "rom/ets_sys.h"
}

static const char* TAG = "DHT11";

// ****************************** Static factory method ******************************
DHT11Driver::Ptr DHT11Driver::create(int data_pin) {
    DHT11Driver* raw = new (std::nothrow) DHT11Driver(data_pin);
    if (raw == nullptr) {
        ESP_LOGE(TAG, "Failed to allocate memory for DHT11Driver");
        return Ptr(nullptr, [](DHT11Driver*){}); // Return a null pointer with a no-op deleter
    }
    if (!raw->init()) {
        ESP_LOGE(TAG, "Failed to initialize DHT11Driver");
        delete raw; // Clean up allocated memory
        return Ptr(nullptr, [](DHT11Driver*){}); // Return a null pointer with a no-op deleter
    }

    return Ptr(raw, [](DHT11Driver* driver) {
        driver->deinit();
        delete driver;
        ESP_LOGI(TAG, "DHT11Driver instance destroyed");
    });
}

// ****************************** Constructor & Destructor ******************************
DHT11Driver::DHT11Driver(int data_pin) : data_pin_(data_pin) {

}

DHT11Driver::~DHT11Driver() {
    // deinit();
}

// ********************************* Initialization *************************************
bool DHT11Driver::init() {
    // Configure the data pin as input with pull-up
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT_OUTPUT_OD;
    io_conf.pin_bit_mask = (1ULL << data_pin_);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;

    esp_err_t err = gpio_config(&io_conf);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure GPIO: %s", esp_err_to_name(err));
        return false;
    }
    ESP_LOGI(TAG, "DHT11Driver initialized on GPIO%d", data_pin_);
    return true;
}

// ********************************* Deinitialization **********************************
void DHT11Driver::deinit() {
    ESP_LOGI(TAG, "Deinitializing DHT11Driver on GPIO%d", data_pin_);
    gpio_reset_pin(static_cast<gpio_num_t>(data_pin_));
}

// DHT11Driver::~DHT11Driver() {
//     // Ensure resources are cleaned up
//     deinit();
// }

// ********************************* bit banging delay *********************************
static int wait_for_level(gpio_num_t pin, int level, int timeout_ms) {
    int elapsed = 0;
    while (gpio_get_level(pin) != level) {
    
        if (elapsed >= timeout_ms) {
            return -1; // Timeout
        }
        ets_delay_us(1);
        elapsed++;
    }
    return elapsed; // Success
}

// ********************************* Read Sensor Data **********************************
std::optional<DHT11_SensorData> DHT11Driver::read() {
    gpio_num_t pin = static_cast<gpio_num_t>(data_pin_);

    // Step 1: Send start signal
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);
    ets_delay_us(20000);
    gpio_set_level(pin, 1);
    ets_delay_us(40);
    gpio_set_direction(pin, GPIO_MODE_INPUT);

    // Step 2: Wait for sensor response
    if (wait_for_level(pin, 0, 80) < 0) {
        ESP_LOGE(TAG, "Sensor did not respond (initial low)");
        return std::nullopt;
    }

    if (wait_for_level(pin, 1, 80) < 0) {
        ESP_LOGE(TAG, "Sensor did not respond (initial high)");
        return std::nullopt;
    }

    // Step 3: Read 40 bits of data
    uint8_t data[5] = {0};
    for (int i = 0; i < 40; ++i) {
        if (wait_for_level(pin, 0, 80) < 0) {
            ESP_LOGE(TAG, "Timeout waiting for bit %d low", i);
            return std::nullopt;
        }

        if (wait_for_level(pin, 1, 80) < 0) {
            ESP_LOGE(TAG, "Timeout waiting for bit %d high", i);
            return std::nullopt;
        }

        ets_delay_us(40); // Wait for 40 microseconds to determine bit value
        int bit = gpio_get_level(pin);
        data[i / 8] = (data[i / 8] << 1) | (bit ? 1 : 0);
        wait_for_level(pin, 0, 80);
    }

    // Step 4: Validate checksum
    uint8_t checksum = data[0] + data[1] + data[2] + data[3];
    if (checksum != data[4]) {
        ESP_LOGE(TAG, "Checksum mismatch: calculated %d, received %d", checksum, data[4]);
        return std::nullopt;
    }

    // Step 5: Parse temperature and humidity
    DHT11_SensorData sensor_data;
    sensor_data.humidity = static_cast<float>(data[0]);
    sensor_data.temperature = static_cast<float>(data[2]);

    ESP_LOGI(TAG, "Read sensor data: Temperature = %.1f C, Humidity = %.1f%%", sensor_data.temperature, sensor_data.humidity);
    return sensor_data;
}
