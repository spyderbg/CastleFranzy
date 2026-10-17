#ifndef __RES_VECTOR_H__
#define __RES_VECTOR_H__

#include "resUtils.h"

namespace irr
{
namespace res
{
template<typename T>
struct vector {
	inline const T& operator[](int idx) const {
		_RES_DEBUG_BREAK_IF(idx < 0 || idx >= m_size);
		return m_ptr[idx];
	}

	inline T& operator[](int idx) {
		_RES_DEBUG_BREAK_IF(idx < 0 || idx >= m_size);
		return m_ptr[idx];
	}

	inline const T* operator->() const {
		return m_ptr;
	}

	inline T* operator->() {
		return m_ptr;
	}

	inline const T* ptr() const {
		return m_ptr;
	}

	inline T* ptr() {
		return m_ptr;
	}

	inline const T& ref() const {
		return *ptr(); 
	}

	inline T& ref() {
		return *ptr(); 
	}

	inline bool IsValid() const {
		return m_ptr != 0;
	}

	//operator * ?

	inline bool operator==(const vector& a) const { 
		return ptr() == a.ptr(); 
	}

	inline bool operator!=(const vector& a) const { 
		return ptr() != a.ptr();
	}  

	inline int size() const { return m_size; }
private:
	int m_size;
	union {
		int m_offset;
		T* m_ptr;
	};
};
}; // namespace res
}; // namespace irr

#endif //__RES_VECTOR_H__