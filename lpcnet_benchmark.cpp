#include <benchmark/benchmark.h>
#include <iostream>
#include <vector>
#include <array>
#include <benchmark/benchmark.h>
#include "test_data_1_channel_16_khz_pcm.h"

extern "C" { 
#include "lpcnet.h"
}

constexpr auto BM_UNITS = benchmark::kMillisecond;

constexpr int kChannels = 1;
constexpr size_t kFrameSizeMS = 20;
constexpr int kFs = 16000;
constexpr int kFrameSizeSamples = kFs / 1000 * kFrameSizeMS;

static void BM_Encode(benchmark::State& state) {
     std::vector<int16_t> pcm_data;
     const size_t test_time_seconds = 10;

     pcm_data.resize(test_data::k1_channel_16_kHz_pcm::length);
     pcm_data.assign(test_data::k1_channel_16_kHz_pcm::data,
                     test_data::k1_channel_16_kHz_pcm::data +
                         test_data::k1_channel_16_kHz_pcm::length /
                             (test_data::k1_channel_16_kHz_pcm::seconds /
                              test_time_seconds));

     std::vector<uint8_t> payload(1500);
     std::array<uint8_t, LPCNET_COMPRESSED_SIZE> buf;

     const auto data_length = pcm_data.size();

     LPCNetEncState *enc_net = lpcnet_encoder_create();

     for (auto _ : state) {
          auto data_ptr = pcm_data.data();
          size_t offset = 0;
          int num_bytes;
          for (; offset < data_length - kFrameSizeSamples + 1;
             offset += kFrameSizeSamples) {
             state.ResumeTiming();
             lpcnet_encode(enc_net, data_ptr, buf.data());
             state.PauseTiming();
             num_bytes = LPCNET_COMPRESSED_SIZE;
             if (num_bytes < 0) {
                 std::cout << "LPCNet encoder failed with: " << num_bytes << '\n';
             }
             benchmark::DoNotOptimize(payload.data());
             std::fill(payload.begin(), payload.end(), static_cast<uint8_t>(0u));
             benchmark::DoNotOptimize(payload.data());
             data_ptr += kFrameSizeSamples;
          }
     }
     lpcnet_encoder_destroy(enc_net);
     state.ResumeTiming();
 }

static void BM_Decode(benchmark::State& state) {
     std::vector<int16_t> pcm_data;
     const size_t test_time_seconds = 10;

     pcm_data.resize(test_data::k1_channel_16_kHz_pcm::length);
     pcm_data.assign(test_data::k1_channel_16_kHz_pcm::data,
                     test_data::k1_channel_16_kHz_pcm::data +
                         test_data::k1_channel_16_kHz_pcm::length /
                             (test_data::k1_channel_16_kHz_pcm::seconds /
                              test_time_seconds));

     std::vector<uint8_t> payload(1500);
     std::array<uint8_t, LPCNET_COMPRESSED_SIZE> buf;
     std::array<int16_t, LPCNET_PACKET_SAMPLES> pcm;

     const auto data_length = pcm_data.size();

     LPCNetEncState *enc_net = lpcnet_encoder_create();
     LPCNetDecState *dec_net = lpcnet_decoder_create();

     for (auto _ : state) {
          auto data_ptr = pcm_data.data();
          size_t offset = 0;
          int num_bytes;
          for (; offset < data_length - kFrameSizeSamples + 1;
             offset += kFrameSizeSamples) {
             lpcnet_encode(enc_net, data_ptr, buf.data());
             state.ResumeTiming();
             lpcnet_decode(dec_net, buf.data(), pcm.data());
             state.PauseTiming();
             num_bytes = LPCNET_COMPRESSED_SIZE;
             if (num_bytes < 0) {
                 std::cout << "LPCNet encoder failed with: " << num_bytes << '\n';
             }
             benchmark::DoNotOptimize(payload.data());
             std::fill(payload.begin(), payload.end(), static_cast<uint8_t>(0u));
             benchmark::DoNotOptimize(payload.data());
             data_ptr += kFrameSizeSamples;
          }
     }
     lpcnet_encoder_destroy(enc_net);
     lpcnet_decoder_destroy(dec_net);
}

BENCHMARK(BM_Encode)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_Decode)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
