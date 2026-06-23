#ifndef UART_CONSOLE_HPP
#define UART_CONSOLE_HPP

#include <stdint.h>
// #include <cstddef>
#include <streambuf>


class UartConsole {
    public:
        UartConsole(int uart_num, int tx_pin, int rx_pin, int baudrate);
        ~UartConsole();

        // Avoid duplicating the uart
        UartConsole(const UartConsole&) = delete;
        UartConsole& operator=(const UartConsole&) = delete;

        void write(std::string_view str);
        void writeln(std::string_view str);
        void writeChar(char c);
        int readByte();

    private:
        int uart_num_;
        bool initialized_; // Flag to indicate if the UART has been initialized
        static constexpr size_t buffer_size = 256;
};

#endif // UART_CONSOLE_HPP