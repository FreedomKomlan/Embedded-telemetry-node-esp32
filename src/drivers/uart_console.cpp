#include "drivers/uart_console.hpp"

extern "C" {
    #include "driver/uart.h"
    #include "esp_log.h"
}

static const char* TAG = "UartConsole";

// Mapping of UART port numbers to their corresponding uart_port_t values
static constexpr uart_port_t UART_PORTS[] = {
    UART_NUM_0,
    UART_NUM_1,
    UART_NUM_2
};

// Constructor
UartConsole::UartConsole(int uart_num, int tx_pin, int rx_pin, int baudrate) : uart_num_(uart_num), initialized_(false) {

    // Validate UART number
    if (uart_num < 0 || uart_num >= sizeof(UART_PORTS)/sizeof(UART_PORTS[0])) {
        ESP_LOGE(TAG, "Invalid UART number: %d", uart_num);
        return;
    }

    // UART configuration parameters
    
    uart_config_t uart_config = {};
    
    uart_config.baud_rate = baudrate;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart_config.rx_flow_ctrl_thresh = 0;
    uart_config.source_clk = UART_SCLK_APB;

    // Install UART driver
    uart_port_t port = UART_PORTS[uart_num];

    esp_err_t err = uart_driver_install(port, buffer_size, 0, 0, NULL, 0);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to install UART driver: %s", esp_err_to_name(err));
            uart_driver_delete(port);
            return;
        }

    // Configure UART parameters
    err = uart_param_config(port, &uart_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure UART parameters: %s", esp_err_to_name(err));
        uart_driver_delete(port);
        return;
    }

    // Set UART pins
    err = uart_set_pin(port, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set UART pins: %s", esp_err_to_name(err));
        uart_driver_delete(port);
        return;
    }

    initialized_ = true;
    ESP_LOGI(TAG, "UART console initialized on UART%d with TX pin %d, RX pin %d, baudrate %d", uart_num, tx_pin, rx_pin, baudrate);
}

// Destructor
UartConsole::~UartConsole() {
    if (initialized_) {
        uart_driver_delete(UART_PORTS[uart_num_]);
        ESP_LOGI(TAG, "UART console on UART%d deleted", uart_num_);
    }
}

// Write a string to the UART console
void UartConsole::write(std::string_view str) {
    if (!initialized_) return;
    uart_write_bytes(UART_PORTS[uart_num_], str.data(), str.size());
}

// Write a string followed by a newline to the UART console
void UartConsole::writeln(std::string_view str) {
    write(str);
    write("\n");
}

int UartConsole::readByte() {
    if (!initialized_) return -1;
    uint8_t ch;
    int len = uart_read_bytes(UART_PORTS[uart_num_], &ch, 1, 0);
    if (len > 0) return ch;
    return -1;
}