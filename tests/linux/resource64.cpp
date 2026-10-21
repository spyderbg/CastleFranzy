#include "resFile.h"
#include "resVector.h"
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <vector>

using namespace irr::res;

static void require(bool condition) {
    if (!condition) throw std::runtime_error("64-bit resource regression");
}
static void put(std::vector<char>& data, unsigned offset, std::uint32_t value) {
    std::memcpy(data.data() + offset, &value, 4);
}
static std::vector<char> fixture() {
    // Header, five relocation entries, vector<int>, runtime pointer, string.
    std::vector<char> bytes(84, 0);
    std::memcpy(bytes.data(), "BRES\xfe\xff\0\0", 8);
    put(bytes, 8, 32); put(bytes, 12, bytes.size()); put(bytes, 16, 5);
    put(bytes, 20, 32); put(bytes, 24, 68); put(bytes, 28, 52);
    const unsigned fields[] = {20, 24, 28, 56, 64};
    for (unsigned i = 0; i < 5; ++i) put(bytes, 32 + i * 4, fields[i]);
    put(bytes, 52, 2); put(bytes, 56, 76); // vector
    put(bytes, 64, 72); // string pointer
    put(bytes, 68, 4); std::memcpy(bytes.data() + 72, "abc", 4);
    put(bytes, 76, 123); put(bytes, 80, 456);
    return bytes;
}
struct Record {
    irr::res::vector<int> numbers;
    Ptr<void> runtime;
    String name;
};
struct MemoryReader : FileReader {
    std::vector<char> bytes;
    unsigned pos = 0;
    MemoryReader() : bytes(fixture()) {}
    int read(void* output, unsigned size) override {
        if (pos + size > bytes.size()) return 0;
        std::memcpy(output, bytes.data() + pos, size); pos += size; return size;
    }
    long seek(long offset, bool relative) override { pos = relative ? pos + offset : offset; return pos; }
    long getSize() override { return bytes.size(); }
};
int main() {
    static_assert(sizeof(void*) == 8, "Run this check in a 64-bit process");
    static_assert(sizeof(Record) == 16, "Serialized record changed size");
    int external = 99;
    require(reinterpret_cast<std::uintptr_t>(&external) > UINT32_MAX);
    require(String::Empty() == "");
    require(String::Empty().GetSize() == 0);
    // Two concurrent resources, runtime addresses above 4 GiB, copies and reloads.
    auto bytes = fixture();
    File first(bytes.data());
    require(first.isValid());
    auto* record = reinterpret_cast<Record*>(first->data.ptr());
    require(record->numbers.size() == 2 && record->numbers[1] == 456);
    require(record->name == "abc" && record->name.GetSize() == 4);
    record->runtime = &external;
    require(static_cast<int*>(record->runtime) == &external);
    const String copy = record->name;
    for (int i = 0; i < 100; ++i) {
        MemoryReader reader;
        File other;
        require(other.Init(&reader) == 0);
        auto* second = reinterpret_cast<Record*>(other->data.ptr());
        second->runtime = &external;
        require(static_cast<int*>(second->runtime) == &external);
        require(second->name == "abc" && record->name == "abc");
        delete[] reinterpret_cast<char*>(other.ptr());
    }
    require(copy == "abc" && static_cast<int*>(record->runtime) == &external);
    record->runtime = nullptr;
    require(static_cast<void*>(record->runtime) == nullptr);
    // Reject a relocation target outside the resource and a mismatched file size.
    MemoryReader invalid;
    put(invalid.bytes, 56, 9999);
    File rejected;
    require(rejected.Init(&invalid) != 0 && !rejected.isValid());
    MemoryReader truncated;
    put(truncated.bytes, 12, 1000);
    require(rejected.Init(&truncated) != 0);
    std::puts("64-bit resource relocation, native pointer mutation and reload checks passed");
}
