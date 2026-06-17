#ifndef DHT11_HPP
#define DHT11_HPP

#include <memory>
#include <optional>
struct DHT11_SensorData {
    float temperature;
    float humidity;
};

class DHT11Driver {
public:
    // Alias for unique pointer with custom deleter
    using Ptr = std::unique_ptr<DHT11Driver, void(*)(DHT11Driver*)>;

    static Ptr create(int data_pin);

    std::optional<DHT11_SensorData> read();
    
    ~DHT11Driver();

private:
    int data_pin_;
    DHT11Driver(int data_pin); // Private constructor to enforce factory method usage
    bool init();
    void deinit();
};

#endif // DHT11_HPP