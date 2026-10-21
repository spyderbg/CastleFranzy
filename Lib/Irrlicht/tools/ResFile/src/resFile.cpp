#include "resFile.h"
#include <cstring>
#include <limits>
#include <map>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace irr { namespace res {
namespace {
struct PointerOwner {
    std::uintptr_t end;
    std::unordered_map<const void*, std::uint32_t> handles;
};
alignas(std::uint32_t) char emptyString[5] = {}; // Four-byte length followed by the terminating zero.
struct PointerTable {
    std::vector<void*> pointers = {nullptr, emptyString + 4};
    std::vector<std::uint32_t> freeHandles;
    std::map<std::uintptr_t, PointerOwner> owners;
};
PointerTable& table() { static PointerTable value; return value; }

void release(void* buffer) {
    PointerTable& t = table();
    const auto owner = t.owners.find(reinterpret_cast<std::uintptr_t>(buffer));
    if (owner == t.owners.end()) return;
    for (const auto& entry : owner->second.handles) {
        t.pointers[entry.second] = nullptr;
        t.freeHandles.push_back(entry.second);
    }
    t.owners.erase(owner);
}
std::uint32_t word(const char* bytes) {
    std::uint32_t value;
    std::memcpy(&value, bytes, sizeof(value));
    return value;
}
}

std::uint32_t StorePointer(const void* field, const void* pointer) {
    if (!pointer) return 0;
    PointerTable& t = table();
    const std::uintptr_t address = reinterpret_cast<std::uintptr_t>(field);
    auto owner = t.owners.upper_bound(address);
    if (owner == t.owners.begin() || address >= (--owner)->second.end)
        throw std::runtime_error("BRES pointer assignment outside a loaded resource");
    auto& handles = owner->second.handles;
    const auto existing = handles.find(pointer);
    if (existing != handles.end()) return existing->second;
    std::uint32_t handle;
    if (!t.freeHandles.empty()) {
        handle = t.freeHandles.back();
        t.freeHandles.pop_back();
        t.pointers[handle] = const_cast<void*>(pointer);
    } else {
        if (t.pointers.size() >= std::numeric_limits<std::uint32_t>::max())
            throw std::runtime_error("BRES pointer handle table exhausted");
        handle = static_cast<std::uint32_t>(t.pointers.size());
        t.pointers.push_back(const_cast<void*>(pointer));
    }
    handles.emplace(pointer, handle);
    return handle;
}

void* ResolvePointer(std::uint32_t handle) {
    return table().pointers.at(handle);
}

String String::Empty() {
    String value;
    value.ptr.handle = 1;
    return value;
}

File::~File() { release(ptr()); }

int File::Init(FileReader* file) {
    const long size = file->getSize();
    if (size < static_cast<long>(sizeof(FileHeaderData)) ||
        static_cast<unsigned long>(size) > std::numeric_limits<std::uint32_t>::max())
        return 1;
    char* buffer = new char[size];
    if (file->read(buffer, static_cast<unsigned int>(size)) != size ||
        word(buffer + 12) != static_cast<std::uint32_t>(size) || Init(buffer)) {
        delete[] buffer;
        return 1;
    }
    return 0;
}

int File::Init(void* buffer) {
    IsValid = false;
    if (!buffer) return 1;
    char* bytes = static_cast<char*>(buffer);
    FileHeaderData header;
    std::memcpy(&header, buffer, sizeof(header));
    if (std::memcmp(bytes, "BRES\xfe\xff\0\0", 8) != 0 ||
        header.sizeOfHeader != sizeof(header) || header.sizeOfFile < sizeof(header))
        return 1;
    const std::uint32_t offsetTable = word(bytes + 20);
    if (offsetTable < sizeof(header) || offsetTable > header.sizeOfFile ||
        header.numOffsets > (header.sizeOfFile - offsetTable) / 4)
        return 1;
    // Validate before rewriting anything. The table itself remains file offsets.
    std::vector<std::pair<std::uint32_t, std::uint32_t>> relocations;
    for (std::uint32_t i = 0; i < header.numOffsets; ++i) {
        const std::uint32_t field = word(bytes + offsetTable + i * 4);
        if (field > header.sizeOfFile - 4 || field % 4) return 1;
        const std::uint32_t target = word(bytes + field);
        if (!target || target >= header.sizeOfFile) return 1;
        relocations.emplace_back(field, target);
    }
    const std::uintptr_t start = reinterpret_cast<std::uintptr_t>(buffer);
    if (table().owners.count(start)) return 1;
    release(ptr());
    table().owners.emplace(start, PointerOwner{start + header.sizeOfFile, {}});
    try {
        for (const auto& relocation : relocations) {
            const std::uint32_t handle = StorePointer(bytes + relocation.first, bytes + relocation.second);
            std::memcpy(bytes + relocation.first, &handle, sizeof(handle));
        }
    } catch (...) {
        release(buffer);
        throw;
    }
    res::Access<FileHeaderData>::operator=(res::Access<FileHeaderData>(buffer));
    ptr()->version |= 0x8000;
    IsValid = true;
    return 0;
}

} }
