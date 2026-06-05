#pragma once

#include <chrono>

using namespace std::chrono_literals;

class fps_meter {
public:
    explicit fps_meter(std::chrono::duration<double> interval = 1.0s)
        : m_interval(interval) {}

    double get() const { return m_fps; }
    bool is_updated() const { return m_updated; }

    void update() {
        ++m_frame_count;
        const auto now = std::chrono::steady_clock::now();
        const std::chrono::duration<double> delta = now - m_last_time;

        if (delta > m_interval) {
            m_fps = static_cast<double>(m_frame_count) / delta.count();
            m_frame_count = 0;
            m_last_time = now;
            m_updated = true;
        } else {
            m_updated = false;
        }
    }

    void reset() {
        m_last_time = std::chrono::steady_clock::now();
        m_frame_count = 0;
        m_updated = false;
        m_fps = 0.0;
    }

    void set_interval(std::chrono::duration<double> interval) {
        m_interval = interval;
    }

private:
    double m_fps{0.0};
    std::chrono::time_point<std::chrono::steady_clock> m_last_time = std::chrono::steady_clock::now();
    std::chrono::duration<double> m_interval{1.0s};
    std::size_t m_frame_count{0};
    bool m_updated{false};
};
