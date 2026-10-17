// Decode the base level of the legacy PVR v2 textures shipped with this game.
// The bundled PVRTC decoder uses unsigned long for 32-bit words, so this tool
// is built with the same 32-bit CMake toolchain as the game.
#include "PVRT/PVRTDecompress.h"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <stdexcept>
#include <vector>

static_assert(sizeof(unsigned long) == 4, "PVRTC decoding requires a 32-bit build");

int main(int argc, char** argv)
{
    if (argc != 2) { std::fprintf(stderr, "Usage: PvrDecode input.pvr > pixels.rgba\n"); return 2; }
    try
    {
        FILE* file = std::fopen(argv[1], "rb");
        if (!file) throw std::runtime_error("Could not open PVR file");
        std::fseek(file, 0, SEEK_END);
        const long size = std::ftell(file);
        std::rewind(file);
        std::vector<uint32_t> data(size > 0 ? (size + 3) / 4 : 0);
        const size_t read = std::fread(data.data(), 1, size > 0 ? size : 0, file);
        std::fclose(file);
        if (size < 52 || read != size) throw std::runtime_error("Truncated PVR header");
        const uint32_t* h = data.data();
        const uint32_t height = h[1], width = h[2], format = h[4] & 255;
        if (h[0] != 52 || h[11] != 0x21525650 || h[12] != 1 ||
            !width || !height || width > 8192 || height > 8192 || h[5] != size - 52)
            throw std::runtime_error("Invalid or unsupported PVR v2 header");
        size_t required;
        if (format == 0x18 || format == 0x19)
        {
            if ((width & (width - 1)) || (height & (height - 1)))
                throw std::runtime_error("PVRTC dimensions must be powers of two");
            required = std::max(width, format == 0x18 ? 16u : 8u) * std::max(height, 8u) / (format == 0x18 ? 4 : 2);
        }
        else if (format == 0x17) required = width * height * 2;
        else if (format == 0x12) required = width * height * 4;
        else throw std::runtime_error("Unsupported PVR pixel format (expected PVRTC, LA88 or RGBA8888)");
        if (h[5] < required) throw std::runtime_error("Truncated PVR pixels");

        const unsigned char* src = reinterpret_cast<const unsigned char*>(data.data()) + 52;
        std::vector<unsigned char> rgba(width * height * 4);
        if (format == 0x18 || format == 0x19)
            PVRTCDecompress(src, format == 0x18, width, height, rgba.data());
        else if (format == 0x17)
            for (size_t i = 0; i < width * height; ++i)
            {
                rgba[i*4] = rgba[i*4+1] = rgba[i*4+2] = src[i*2];
                rgba[i*4+3] = src[i*2+1];
            }
        else std::copy(src, src + rgba.size(), rgba.begin());
        if (!h[10])
            for (size_t i = 3; i < rgba.size(); i += 4) rgba[i] = 255;
        if (std::fwrite(rgba.data(), 1, rgba.size(), stdout) != rgba.size())
            throw std::runtime_error("Could not write decoded pixels");
    }
    catch (const std::exception& error)
    {
        std::fprintf(stderr, "%s: %s\n", argv[1], error.what());
        return 1;
    }
    return 0;
}
