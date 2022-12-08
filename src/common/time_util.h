#pragma once

#include <stdint.h>

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

#define SCOPED_TIMER(timer) RawTimer _##__LINE__(timer)

class RawTimer {
public:
    explicit RawTimer(int64_t& timer) : _timer(timer) { start = std::chrono::steady_clock::now(); }
    ~RawTimer() {
        auto end = std::chrono::steady_clock::now();
        _timer += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    }

private:
    std::chrono::steady_clock::time_point start;
    int64_t& _timer;
};

inline std::string time_to_string(int64_t time, const std::string unit = "auto") {
    std::stringstream ss;
    double d_time = time;
    ss << std::setprecision(3);

    if (unit == "auto") {
        if (time < 1000) {
            ss << d_time << "ns";
        } else if (time < 1000 * 1000) {
            ss << d_time / 1000 << "us";
        } else if (time < 1000 * 1000 * 1000) {
            ss << d_time / (1000 * 1000) << "ms";
        } else {
            ss << d_time / (1000 * 1000 * 1000) << "s";
        }
    } else if (unit == "s") {
        ss << d_time / (1000 * 1000 * 1000) << "s";
    } else if (unit == "ms") {
        ss << d_time / (1000 * 1000) << "ms";
    } else if (unit == "us") {
        ss << d_time / 1000 << "us";
    } else {
        ss << d_time << "ns";
    }
    return ss.str();
}
