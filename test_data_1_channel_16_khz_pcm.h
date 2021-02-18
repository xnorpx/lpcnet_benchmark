#pragma once

#include <cstddef>
#include <cstdint>

namespace test_data {
struct k1_channel_16_kHz_pcm {
    static constexpr size_t seconds = 60;
    static constexpr size_t length = 960000;
    static const int16_t data[length];
};
}  // namespace test_data
