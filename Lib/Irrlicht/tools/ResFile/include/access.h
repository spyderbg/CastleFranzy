#ifndef __ACCESS_H__
#define __ACCESS_H__

#include <stdlib.h>

namespace irr
{
namespace res
{

template<typename T>
struct Access {
	Access(){}

	Access(void* p) {
		m_ptr = static_cast<T*>(p);
	}

	Access(const Access& a) {
		m_ptr = a.m_ptr;
	}
	Access& operator=(const Access& a) {
		m_ptr = a.m_ptr;
		return *this;
	}

	T& operator[](int idx) {
		return m_ptr[idx];
	}
	const T& operator[](int idx) const {
		return m_ptr[idx];
	}

	T* operator->() {
		return m_ptr;
	}
	const T* operator->() const {
		return m_ptr;
	}

	T* ptr() {
		return m_ptr;
	}
	const T* ptr() const {
		return m_ptr;
	}

	T& ref() { 
		return *ptr(); 
	}
	const T& ref() const {
		return *ptr(); 
	}

	bool IsValid() const {
		return m_ptr != 0;
	}

	//operator * ?

	bool operator==(const Access& a) const { 
		return ptr() == a.ptr(); 
	} 
	bool operator!=(const Access& a) const { 
		return ptr() != a.ptr();
	}  

	void OffsetToPtr(void* ref)
	{
		m_ptr = reinterpret_cast<T*>(reinterpret_cast<char*>(ref) + m_offset);
	}

	void PtrToOffset(void* ref)
	{
		m_offset = static_cast<char*>(m_ptr) - static_cast<char*>(ref);
	}

	
private:
	union {
		int m_offset;
		T* m_ptr;
	};
};

}; // namespace res
}; // namespace irr

#endif // __ACCESS_H__