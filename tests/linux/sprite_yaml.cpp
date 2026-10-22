#include "SpriteYaml.h"
#include <algorithm>
#include <fstream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <cstdio>

static void check(bool ok, const std::string& message)
{
    if (!ok) throw std::runtime_error(message);
}
static std::string read(const char* path)
{
    std::ifstream file(path, std::ios::binary);
    check(bool(file), std::string("Cannot read ") + path);
    return std::string(std::istreambuf_iterator<char>(file), {});
}
static SpriteYamlData compile(const std::string& text)
{
    SpriteYamlData result;
    std::string error;
    check(CompileSpriteYaml(text, result, error), error);
    return result;
}
static void invalid(const std::string& text)
{
    SpriteYamlData result;
    result.binary.push_back(1);
    std::string error;
    check(!CompileSpriteYaml(text, result, error) && !error.empty() && result.binary.empty(),
        "Invalid sprite was accepted or returned partial data");
}
static int word(const SpriteYamlData& data, int at)
{
    return static_cast<short>(data.binary.at(at) | (data.binary.at(at + 1) << 8));
}
static std::string replace(std::string text, const std::string& from, const std::string& to)
{
    const auto at = text.find(from);
    check(at != std::string::npos, "Invalid test replacement");
    text.replace(at, from.size(), to);
    return text;
}
int main(int argc, char** argv)
{
    try
    {
        if (argc > 2 && std::string(argv[1]) == "--validate")
        {
            for (int i = 2; i < argc; ++i)
            {
                compile(read(argv[i]));
                std::printf("Valid sprite YAML: %s\n", argv[i]);
            }
            return 0;
        }
        check(argc == 3, "Expected YAML example and legacy binary paths");
        const auto sample = compile(read(argv[1]));
        const std::string legacy = read(argv[2]);
        // Legacy exports have extra trailing authoring data; compare all runtime bytes.
        check(sample.binary.size() == 335 && legacy.size() >= sample.binary.size() &&
            std::equal(sample.binary.begin(), sample.binary.end(),
                reinterpret_cast<const unsigned char*>(legacy.data())), "Dizzy metadata differs from .bsprite");
        check(sample.image == "./dizzy_effect.tga" && sample.hasTransparent &&
            sample.transparent == 0xff00ff, "Image metadata was lost");

        const std::string fixture = R"(sprite:
  version: "0001"
  images: [{id: "0x00F0", file: "atlas.tga", alphaFile: "alpha.png", transparent: "0xFF00FF"}]
  modules: [{id: "0x1F00", type: MD_IMAGE, image: "0x00F0", x: 4, y: 6, w: 3, h: 5}]
  frames:
    - id: "0x2F01"
      parts: [{module: "0x2F02", x: 10, y: 20, flags: [HYPER_FM, FLIP_X, ROT_90]}]
    - id: "0x2F02"
      parts:
        - module: "0x1F00"
          x: -2
          y: 3
          flags:
            - FLIP_Y
      rects: [{x1: -1, y1: -2, x2: 3, y2: 5}]
  animations:
    - id: "0x3F00"
      frames: [{frame: "0x2F01", time: 255, x: -7, y: 8, flags: [FLIP_X, ROT_90]}]
)";
        const auto result = compile(fixture);
        check(result.alphaImage == "alpha.png", "Alpha image path was lost");
        check(word(result, 19) == 1 && result.binary.at(25) == 21, "Hyperframe IDs/flags not mapped");
        check(word(result, 55) == 2 && word(result, 57) == 19 && word(result, 59) == 5 &&
            word(result, 61) == 3, "Hyperframe transformed bounds are incorrect");
        check(word(result, 35) == -1 && word(result, 37) == -2 && word(result, 39) == 4 &&
            word(result, 41) == 7, "Collision rectangle is incorrect");
        check(result.binary.at(74) == 255 && result.binary.at(75) == 249 &&
            result.binary.at(77) == 5, "Animation timing/offsets/flags are incorrect");

        invalid(replace(fixture, "module: \"0x1F00\"", "module: \"0x1000\""));
        invalid(replace(fixture, "id: \"0x2F02\"", "id: \"0x2F01\""));
        invalid(replace(fixture, "module: \"0x1F00\"", "module: \"0x2F01\""));
        invalid(replace(fixture, "FLIP_Y", "FREE_ROT"));
        invalid(replace(fixture, "w: 3", "w: -1"));
        invalid(replace(fixture, "time: 255", "time: 256"));
        invalid(replace(fixture, "x: -7", "x: -129"));
        invalid(replace(fixture, "x2: 3", "x2: -3"));
        invalid(replace(fixture, "id: \"0x00F0\"", "id: \"0x10000\""));
        invalid(fixture + "---\nsprite: {}\n");
        invalid("sprite: [not: valid");
        const std::string cycle = replace(fixture, "module: \"0x1F00\"", "module: \"0x2F01\"");
        invalid(replace(cycle, "- FLIP_Y", "- HYPER_FM"));

        std::ostringstream extended;
        extended << "sprite:\n  version: \"0001\"\n  images: [{id: \"0x0000\", file: atlas.png}]\n"
            "  modules: [{id: \"0x1000\", type: MD_IMAGE, image: \"0x0000\", x: 0, y: 0, w: 1, h: 1}]\n  frames:\n";
        for (int i = 0; i <= 256; ++i)
            extended << "    - {id: \"0x" << std::hex << (0x2000 + i) << "\", parts: [{module: \"0x1000\"}]}\n";
        extended << "  animations: [{id: \"0x3000\", frames: [{frame: \"0x2100\", time: 1}]}]\n";
        const auto large = compile(extended.str());
        check(large.binary.at(large.binary.size() - 7) == 64, "Extended frame index was not encoded");
        std::puts("Sprite YAML matches binary metadata; validation, IDs, bounds, flags and animations pass");
    }
    catch (const std::exception& error)
    {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
    return 0;
}
