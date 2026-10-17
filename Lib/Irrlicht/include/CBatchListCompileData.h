//-*-c++-*-
#ifndef __C_VIDEO_BATCH_LIST_COMPILE_DATA_H_INCLUDED__
#define __C_VIDEO_BATCH_LIST_COMPILE_DATA_H_INCLUDED__

#include "IVideoDriver.h"

namespace irr
{
namespace scene
{

class IBatchList;

}

namespace video
{

class CBatchListCompileData : public IVideoDriver::ICompileData
{
public:

	// sizes are in bytes
	CBatchListCompileData(scene::IBatchList* batchList,
						  u32 maxVertexBufferSize,
						  u32 maxIndexBufferSize)
		: BatchList(batchList)
		, MaxVertexBufferSize(maxVertexBufferSize)
		, MaxIndexBufferSize(maxIndexBufferSize)
	{
	}

	~CBatchListCompileData();

	virtual E_COMPILE_DATA_TYPE getType() const;

	scene::IBatchList* getBatchList() const
	{
		return BatchList;
	}

	u32 getMaxVertexBufferSize() const
	{
		return MaxVertexBufferSize;
	}

	u32 getMaxIndexBufferSize() const
	{
		return MaxIndexBufferSize;
	}
	
private:

	scene::IBatchList* BatchList;
	u32 MaxVertexBufferSize;
	u32 MaxIndexBufferSize;
};

} // end namespace video
} // end namespace irr

#endif
