#include "CFileSystem.h"
#include "IReadFile.h"
#include "IXMLReader.h"
#include "IXMLWriter.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <vector>

using namespace irr;

// The bundled engine delegates allocation to its embedding application.
void* IrrAlloc(size_t size, irr::MemHint) { return std::malloc(size); }
void* IrrAlloc(size_t size, irr::MemHint hint, int, const char*, int) { return IrrAlloc(size, hint); }
void IrrFree(void* pointer) { std::free(pointer); }

static bool readWave(io::IXMLReader* reader)
{
    if (!reader) return false;
    bool found = false;
    while (reader->read())
    {
        if (reader->getNodeType() == io::EXN_ELEMENT &&
            !std::wcscmp(reader->getNodeName(), L"wave"))
        {
            found = !std::wcscmp(reader->getAttributeValueSafe(L"name"), L"er \u0413 & <") &&
                reader->getAttributeValueAsInt(L"count") == 7;
            break;
        }
    }
    reader->drop();
    return found;
}

int main()
{
    static_assert(sizeof(io::char32) == 4, "UTF-32 code units must be four bytes");
    io::CFileSystem files;
    const char* path = "editor-xml-roundtrip.xml";
    io::IXMLWriter* writer = files.createXMLWriter(path);
    if (!writer) return 1;
    writer->writeXMLHeader();
    writer->writeElement(L"wave", true, L"name", L"er \u0413 & <", L"count", L"7");
    writer->drop();
    const bool roundtrip = readWave(files.createXMLReader(path));
    std::remove(path);
    if (!roundtrip)
    {
        std::fprintf(stderr, "Could not reload XML saved by the editor's writer\n");
        return 1;
    }

    // Exercise both UTF-32 byte orders even where wchar_t/writer uses UTF-16.
    const wchar_t* xml = L"<wave name=\"er \u0413 &amp; &lt;\" count=\"7\" />";
    for (bool bigEndian : {false, true})
    {
        std::vector<char> bytes;
        auto append = [&](std::uint32_t unit) {
            for (int i = 0; i < 4; ++i)
                bytes.push_back(static_cast<char>(unit >> (8 * (bigEndian ? 3 - i : i))));
        };
        append(0xfeff);
        for (const wchar_t* p = xml; *p; ++p) append(*p);
        io::IReadFile* memory = files.createMemoryReadFile(bytes.data(), bytes.size(), "wave.xml");
        const bool decoded = readWave(files.createXMLReader(memory));
        memory->drop();
        if (!decoded)
        {
            std::fprintf(stderr, "Could not read UTF-32 %s XML\n", bigEndian ? "BE" : "LE");
            return 1;
        }
    }
    return 0;
}
