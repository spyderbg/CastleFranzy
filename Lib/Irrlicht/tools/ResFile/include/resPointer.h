#ifndef IRR_RES_POINTER_H
#define IRR_RES_POINTER_H

#include <cstdint>
#include <type_traits>

namespace irr { namespace res {

// BRES stores four-byte offsets. At load time these become handles to full-width
// native pointers, including textures/animators assigned during post-processing.
// Handles belong to a loaded File and are reclaimed when that File is destroyed.
std::uint32_t StorePointer(const void* field, const void* pointer);
void* ResolvePointer(std::uint32_t handle);

template<typename T>
struct Ptr {
    std::uint32_t handle;

    T* ptr() const { return static_cast<T*>(ResolvePointer(handle)); }
    operator T*() const { return static_cast<T*>(ResolvePointer(handle)); }
    template<typename U>
    explicit operator U*() const { return static_cast<U*>(ResolvePointer(handle)); }
    T* operator->() const { return static_cast<T*>(ResolvePointer(handle)); }
    Ptr& operator=(T* pointer) {
        handle = StorePointer(this, pointer);
        return *this;
    }
    template<typename U = T>
    typename std::enable_if<!std::is_void<U>::value, U&>::type operator*() const {
        return *static_cast<U*>(ResolvePointer(handle));
    }
    template<typename U = T>
    typename std::enable_if<!std::is_void<U>::value, U&>::type operator[](int i) const {
        return static_cast<U*>(ResolvePointer(handle))[i];
    }
};

static_assert(sizeof(Ptr<void>) == 4, "BRES pointer slots must stay four bytes");
static_assert(std::is_trivially_copyable<Ptr<void>>::value, "BRES pointers must be copyable as data");

} }
#endif
