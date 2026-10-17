#ifndef __SDATAARRAY_H__
#define __SDATAARRAY_H__

#include <resUtils.h>
#include <resFile.h>

/*namespace res
{
	struct Chunk;
};

#include "SDatabaseTypes.h"

namespace irr
{
namespace collada
{

template<typename T>
struct SDataArray
{
	int				m_iSize;
	res::ResData<T>	m_Data;
};

};
};

namespace res
{
template< template<typename T> class U, typename V >
void Write(res::Chunk &chk, U< irr::collada::SDataArray<V> > const &data)
{
	chk << m_iSize;
	chk << m_Data;
};

template< typename V >
void Write(res::Chunk &chk, irr::collada::SDataArray<V> const &data)
{
	chk << m_iSize;
	chk << m_Data;
};



template< typename V >
void Write(res::Chunk &chk, res::ResData<V> const &data)
{
	chk << data.ptr;
	
};
/*
void Write(res::Chunk &chk, irr::collada::SDataArray<int> const &data)
{
	chk.operator <<<int>(data.m_iSize);
	chk << data.m_Data;
};

};*/

#endif