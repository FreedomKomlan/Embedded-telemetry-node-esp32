#ifndef MSG_QUEUE_HPP
#define MSG_QUEUE_HPP

#include <array>
#include <optional>
#include <cstddef>
#include "global.hpp"


template<typename T, size_t Capacity>

class MsgQueue {
public:
    MsgQueue() {
        mutex_ = xSemaphoreCreateMutex();
    }

    ~MsgQueue() {
        if (mutex_) vSemaphoreDelete(mutex_);
    }

    // Pas de copie
    MsgQueue(const MsgQueue&) = delete;
    MsgQueue& operator=(const MsgQueue&) = delete;

    bool push(const T& item) {
        if (!mutex_) return false;
        if (xSemaphoreTake(mutex_, portMAX_DELAY) != pdTRUE) return false;

        bool result = false;
        if (count_ < Capacity) {
            buffer_[write_idx_] = item;
            write_idx_ = (write_idx_ + 1) % Capacity;
            ++count_;
            result = true;
        }
        xSemaphoreGive(mutex_);
        return result;
    }

    std::optional<T> pop() {
        if (!mutex_) return std::nullopt;
        if (xSemaphoreTake(mutex_, portMAX_DELAY) != pdTRUE) return std::nullopt;

        std::optional<T> item;
        if (count_ > 0) {
            item = buffer_[read_idx_];
            read_idx_ = (read_idx_ + 1) % Capacity;
            --count_;
        }
        xSemaphoreGive(mutex_);
        return item;
    }

    size_t count() const { return count_; } // pas de mutex ici, juste informatif

private:
    std::array<T, Capacity> buffer_{};
    size_t read_idx_{0};
    size_t write_idx_{0};
    size_t count_{0};
    SemaphoreHandle_t mutex_{nullptr};
};


#endif // MSG_QUEUE_HPP
