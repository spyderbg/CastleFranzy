#pragma once

#include <cstdint>
#include <string>
#include <vector>

// Validated GP_Data v5 metadata, consumed by the existing ASprite decoder.
struct SpriteYamlData
{
    std::vector<unsigned char> binary;
    std::string image;
    std::string alphaImage;
    bool hasTransparent = false;
    std::uint32_t transparent = 0;
};

bool CompileSpriteYaml(const std::string& source, SpriteYamlData& output, std::string& error);
