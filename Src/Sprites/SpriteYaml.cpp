#include "SpriteYaml.h"
#include <yaml-cpp/yaml.h>
#include <algorithm>
#include <array>
#include <functional>
#include <map>
#include <set>
#include <stdexcept>

namespace
{
using YAML::Node;
void require(bool condition, const std::string& message)
{
    if (!condition) throw std::runtime_error(message);
}
void mapping(const Node& node, const char* label)
{
    require(node.IsMap(), std::string(label) + " must be a mapping");
    std::set<std::string> keys;
    for (const auto& field : node)
        require(field.first.IsScalar() && keys.insert(field.first.Scalar()).second,
            std::string(label) + " contains a duplicate or non-scalar key");
}
Node sequence(const Node& node, const char* key, bool required = false)
{
    Node value = node[key];
    if (!value || value.IsNull())
    {
        require(!required, std::string("Missing ") + key);
        return Node(YAML::NodeType::Sequence);
    }
    require(value.IsSequence(), std::string(key) + " must be a sequence");
    return value;
}
int integer(const Node& node, const char* key, int low, int high, bool required = true, int fallback = 0)
{
    Node value = node[key];
    if (!value)
    {
        require(!required, std::string("Missing ") + key);
        return fallback;
    }
    require(value.IsScalar(), std::string(key) + " must be an integer");
    const long long result = value.as<long long>();
    require(result >= low && result <= high, std::string(key) + " is outside runtime limits");
    return static_cast<int>(result);
}
int id(const Node& value)
{
    require(value && value.IsScalar(), "IDs must be hexadecimal strings, e.g. \"0x1000\"");
    const std::string text = value.Scalar();
    require(text.size() >= 3 && text.size() <= 6 && text.substr(0, 2) == "0x" &&
        text.find_first_not_of("0123456789abcdefABCDEF", 2) == std::string::npos,
        "Invalid 16-bit hexadecimal ID: " + text);
    return static_cast<int>(std::stoul(text, nullptr, 16));
}
int flags(const Node& node, bool hyperAllowed)
{
    int bits = 0;
    for (const auto& flag : sequence(node, "flags"))
    {
        const std::string name = flag.as<std::string>();
        if (name == "FLIP_X") bits |= 1;
        else if (name == "FLIP_Y") bits |= 2;
        else if (name == "ROT_90") bits |= 4;
        else if (name == "HYPER_FM" && hyperAllowed) bits |= 16;
        else throw std::runtime_error("Unsupported sprite flag: " + name);
    }
    return bits;
}
int lookup(const std::map<int, int>& ids, int target, const char* label)
{
    const auto found = ids.find(target);
    require(found != ids.end(), std::string("Unknown ") + label + " ID: " + std::to_string(target));
    return found->second;
}
void addId(std::map<int, int>& ids, const Node& node, int index, const char* label)
{
    require(ids.emplace(id(node["id"]), index).second, std::string("Duplicate ") + label + " ID");
}
struct Module { int x, y, w, h; };
struct Part { int target, x, y, flags; };
using Rect = std::array<int, 4>; // x1, y1, x2, y2
struct Frame { std::vector<Part> parts; std::vector<Rect> rects; Rect bounds; };
struct Step { int frame, time, x, y, flags; };
struct Writer
{
    std::vector<unsigned char> bytes;
    void put(int value, int count)
    {
        const std::uint32_t bits = static_cast<std::uint32_t>(value);
        for (int i = 0; i < count; ++i) bytes.push_back(static_cast<unsigned char>(bits >> (8 * i)));
    }
    void rect(const Rect& r)
    {
        require(r[0] >= -32768 && r[0] <= 32767 && r[1] >= -32768 && r[1] <= 32767 &&
            r[2] - r[0] >= 0 && r[2] - r[0] <= 32767 && r[3] - r[1] >= 0 && r[3] - r[1] <= 32767,
            "Frame bounds or collision rectangle exceed runtime limits");
        put(r[0], 2); put(r[1], 2); put(r[2] - r[0], 2); put(r[3] - r[1], 2);
    }
};

SpriteYamlData compile(const std::string& source)
{
    require(source.size() <= 16 * 1024 * 1024, "Sprite YAML exceeds 16 MiB");
    const auto documents = YAML::LoadAll(source);
    require(documents.size() == 1, "Expected one YAML sprite document");
    const Node root = documents[0];
    mapping(root, "sprite document");
    const Node sprite = root["sprite"] ? root["sprite"] : root;
    mapping(sprite, "sprite");
    require(sprite["version"] && sprite["version"].as<std::string>() == "0001",
        "Unsupported sprite version; expected \"0001\"");
    const Node images = sequence(sprite, "images", true);
    const Node sourceModules = sequence(sprite, "modules", true);
    const Node sourceFrames = sequence(sprite, "frames", true);
    const Node sourceAnimations = sequence(sprite, "animations");
    require(images.size() > 0 && sourceModules.size() > 0, "A sprite needs an image and image modules");
    require(images.size() <= 65535 && sourceModules.size() <= 32767 &&
        sourceFrames.size() <= 32767 && sourceAnimations.size() <= 32767,
        "Sprite counts exceed runtime limits");

    std::map<int, int> imageIds, moduleIds, frameIds, animationIds;
    for (std::size_t i = 0; i < images.size(); ++i)
    {
        mapping(images[i], "image");
        addId(imageIds, images[i], i, "image");
        require(images[i]["file"] && images[i]["file"].IsScalar() &&
            !images[i]["file"].as<std::string>().empty(), "Image file must not be empty");
    }
    for (std::size_t i = 0; i < sourceFrames.size(); ++i)
    {
        mapping(sourceFrames[i], "frame");
        addId(frameIds, sourceFrames[i], i, "frame");
    }
    std::vector<Module> modules;
    int atlas = -1;
    for (std::size_t i = 0; i < sourceModules.size(); ++i)
    {
        const Node m = sourceModules[i];
        mapping(m, "module");
        addId(moduleIds, m, i, "module");
        require(m["type"] && m["type"].as<std::string>() == "MD_IMAGE",
            "The game supports MD_IMAGE modules in one external atlas (GP_Data)");
        const int image = lookup(imageIds, id(m["image"]), "image");
        if (atlas < 0) atlas = image;
        require(atlas == image, "The game supports one image atlas per sprite");
        modules.push_back({integer(m, "x", 0, 32767), integer(m, "y", 0, 32767),
            integer(m, "w", 0, 32767), integer(m, "h", 0, 32767)});
    }
    std::vector<Frame> frames(sourceFrames.size());
    int partCount = 0, rectCount = 0;
    for (std::size_t i = 0; i < frames.size(); ++i)
    {
        for (const Node& p : sequence(sourceFrames[i], "parts"))
        {
            mapping(p, "frame part");
            const int bits = flags(p, true);
            const int target = lookup(bits & 16 ? frameIds : moduleIds, id(p["module"]),
                bits & 16 ? "hyperframe" : "module");
            frames[i].parts.push_back({target, integer(p, "x", -32768, 32767, false),
                integer(p, "y", -32768, 32767, false), bits});
        }
        for (const Node& r : sequence(sourceFrames[i], "rects"))
        {
            mapping(r, "collision rectangle");
            frames[i].rects.push_back({integer(r, "x1", -32768, 32767), integer(r, "y1", -32768, 32767),
                integer(r, "x2", -32768, 32767), integer(r, "y2", -32768, 32767)});
        }
        require(frames[i].rects.size() <= 255, "A frame supports at most 255 collision rectangles");
        partCount += frames[i].parts.size();
        rectCount += frames[i].rects.size();
        require(partCount <= 32767 && rectCount <= 65535, "Frame data exceeds runtime limits");
    }

    // Compute the same bounds as AuroraMX, including transformed hyperframes.
    std::vector<int> visited(frames.size(), 0);
    std::function<Rect(int, int)> bounds = [&](int index, int depth) -> Rect {
        require(depth <= 128 && visited[index] != 1, "Cyclic or excessively nested hyperframes");
        if (visited[index] == 2) return frames[index].bounds;
        visited[index] = 1;
        bool first = true;
        Rect box = {{0, 0, 0, 0}};
        for (const Part& p : frames[index].parts)
        {
            Rect part;
            if (p.flags & 16)
            {
                const Rect child = bounds(p.target, depth + 1);
                const int xs[] = {child[0], child[2]}, ys[] = {child[1], child[3]};
                bool firstPoint = true;
                for (int x0 : xs) for (int y0 : ys)
                {
                    int x = p.flags & 1 ? -x0 : x0, y = p.flags & 2 ? -y0 : y0;
                    if (p.flags & 4) { const int oldX = x; x = -y; y = oldX; }
                    x += p.x; y += p.y;
                    if (firstPoint) { part = {{x, y, x, y}}; firstPoint = false; }
                    else { part[0] = std::min(part[0], x); part[1] = std::min(part[1], y);
                           part[2] = std::max(part[2], x); part[3] = std::max(part[3], y); }
                }
            }
            else
            {
                int w = modules[p.target].w, h = modules[p.target].h;
                if (p.flags & 4) std::swap(w, h);
                part = {{p.x, p.y, p.x + w, p.y + h}};
            }
            if (first) { box = part; first = false; }
            else { box[0] = std::min(box[0], part[0]); box[1] = std::min(box[1], part[1]);
                   box[2] = std::max(box[2], part[2]); box[3] = std::max(box[3], part[3]); }
        }
        frames[index].bounds = box;
        visited[index] = 2;
        return box;
    };
    for (std::size_t i = 0; i < frames.size(); ++i) bounds(i, 0);

    std::vector<std::vector<Step>> animations;
    int stepCount = 0;
    for (std::size_t i = 0; i < sourceAnimations.size(); ++i)
    {
        const Node a = sourceAnimations[i];
        mapping(a, "animation");
        addId(animationIds, a, i, "animation");
        std::vector<Step> steps;
        for (const Node& step : sequence(a, "frames", true))
        {
            mapping(step, "animation frame");
            const int frame = lookup(frameIds, id(step["frame"]), "frame");
            require(frame <= 1023, "Animation frame indices are limited to 0..1023");
            steps.push_back({frame, integer(step, "time", 0, 255),
                integer(step, "x", -128, 127, false), integer(step, "y", -128, 127, false), flags(step, false)});
        }
        stepCount += steps.size();
        require(stepCount <= 32767, "Animation data exceeds runtime limits");
        animations.push_back(steps);
    }

    Writer w;
    w.put(0x05df, 2); w.put(0x1853b, 4); // Existing GP_Data profile.
    w.put(modules.size(), 2);
    for (const Module& m : modules)
    {
        w.put(0, 1); w.put(m.x, 2); w.put(m.y, 2); w.put(m.w, 2); w.put(m.h, 2);
    }
    w.put(partCount, 2);
    for (const Frame& f : frames) for (const Part& p : f.parts)
    {
        w.put(p.target, 2); w.put(p.x, 2); w.put(p.y, 2); w.put(p.flags, 1);
    }
    w.put(rectCount, 2);
    for (const Frame& f : frames) for (const Rect& r : f.rects) w.rect(r);
    w.put(frames.size(), 2);
    int offset = 0;
    for (const Frame& f : frames)
    {
        w.put(f.parts.size(), 2); w.put(offset, 2); w.put(f.rects.size(), 1);
        offset += f.parts.size();
    }
    for (const Frame& f : frames) w.rect(f.bounds);
    w.put(stepCount, 2);
    for (const auto& a : animations) for (const Step& step : a)
    {
        w.put(step.frame & 255, 1); w.put(step.time, 1); w.put(step.x, 1); w.put(step.y, 1);
        w.put(step.flags | ((step.frame >> 8) << 6), 1);
    }
    w.put(animations.size(), 2);
    offset = 0;
    for (const auto& a : animations) { w.put(a.size(), 2); w.put(offset, 2); offset += a.size(); }

    SpriteYamlData result;
    result.binary.swap(w.bytes);
    result.image = images[atlas]["file"].as<std::string>();
    if (images[atlas]["alphaFile"]) result.alphaImage = images[atlas]["alphaFile"].as<std::string>();
    if (images[atlas]["transparent"])
    {
        const Node color = images[atlas]["transparent"];
        require(color.IsScalar() && !color.Scalar().empty() && color.Scalar()[0] != '-', "Invalid transparent color");
        std::size_t used = 0;
        const unsigned long long value = std::stoull(color.Scalar(), &used, 0);
        require(used == color.Scalar().size() && value <= 0xffffffffULL, "Invalid transparent color");
        result.hasTransparent = true;
        result.transparent = static_cast<std::uint32_t>(value);
    }
    return result;
}
} // namespace

bool CompileSpriteYaml(const std::string& source, SpriteYamlData& output, std::string& error)
{
    try
    {
        SpriteYamlData result = compile(source);
        output = std::move(result);
        error.clear();
        return true;
    }
    catch (const std::exception& exception)
    {
        output = SpriteYamlData();
        error = exception.what();
        return false;
    }
}
