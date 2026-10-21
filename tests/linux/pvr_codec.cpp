#include "PVRTDecompress.h"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <stdexcept>
#include <vector>

static void require(bool condition) {
    if (!condition) throw std::runtime_error("64-bit PVR decoder regression");
}
int main() {
    static_assert(sizeof(void*) == 8, "Run this check in a 64-bit process");
    for (int twoBit = 0; twoBit <= 1; ++twoBit) {
        const unsigned width = twoBit ? 16 : 8, height = 8;
        for (unsigned fill : {0u, 255u}) {
            std::vector<std::uint32_t> compressed(8, fill ? UINT32_MAX : 0);
            std::vector<unsigned char> pixels(width * height * 4 + 32, 0xa5);
#ifdef GAME_CODEC
            PVRTDecompressPVRTC(compressed.data(), twoBit, width, height, pixels.data() + 16);
#else
            PVRTCDecompress(compressed.data(), twoBit, width, height, pixels.data() + 16);
#endif
            require(std::all_of(pixels.begin(), pixels.begin() + 16, [](unsigned char p) { return p == 0xa5; }));
            require(std::all_of(pixels.end() - 16, pixels.end(), [](unsigned char p) { return p == 0xa5; }));
            require(std::all_of(pixels.begin() + 16, pixels.end() - 16, [fill](unsigned char p) { return p == fill; }));
        }
    }
#ifdef GAME_CODEC
    const std::uint32_t etc[2] = {};
    std::vector<unsigned char> pixels(4 * 4 * 4 + 32, 0xa5);
    require(PVRTDecompressETC(etc, 4, 4, pixels.data() + 16, 0) == 8);
    for (unsigned i = 0; i < 64; ++i) require(pixels[16 + i] == (i % 4 == 3 ? 255 : 2));
    for (unsigned i = 0; i < 16; ++i) require(pixels[i] == 0xa5 && pixels[80 + i] == 0xa5);
#endif
    std::puts("64-bit texture word sizes and decoded pixels passed");
}
