#ifndef __SIRRLICHTBATCH_H__
#define __SIRRLICHTBATCH_H__

#include "IAttributeExchangingObject.h"
#include "SMaterial.h"
#include "matrix4.h"
#include "irrTypes.h"
#include "S3DVertex.h"
#include "IMeshBuffer.h"

namespace irr
{
class CBatch
	: public virtual io::IAttributeExchangingObject
{
public:
	struct SInitData
	{
		video::SMaterial			Material;
		core::matrix4				ProjectionTransform;
		core::matrix4				ViewTransform;
		core::matrix4				WorldTransform;
		const void*					Vertices;
		const void*					IndexList;
		u32							StartIndex;
		u32							EndIndex;
		u32							PrimitiveCount;
		video::E_VERTEX_TYPE		vType;
		scene::E_PRIMITIVE_TYPE		pType;
		video::E_INDEX_TYPE			iType;
		bool						fVBOFlag;
		float						NearRange;
		float						FarRange;
	};

	CBatch(const SInitData &initData);
	
	~CBatch();

	bool						Show;
	video::SMaterial			Material;
	core::matrix4				ProjectionTransform;
	core::matrix4				ViewTransform;
	core::matrix4				WorldTransform;
	video::S3DVertexComponentArrays Vertices;
	u32							StartIndex; 
	u32							EndIndex;
	float						NearRange;
	float						FarRange;
	union
	{
		unsigned short*			IndexListU16;
		unsigned int*			IndexListU32;
	};
	u32							PrimitiveCount;
	video::E_VERTEX_TYPE		vType; 
	scene::E_PRIMITIVE_TYPE		pType;
	video::E_INDEX_TYPE			iType;
	bool						fVBOFlag;

	void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const
	{
		out->addInt("Pimitive Count", PrimitiveCount);
		out->addInt("Start Index", StartIndex);
		out->addInt("End Index", EndIndex);
		
		Material.serializeAttributes(out);
	}

	void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)
	{
		Material.deserializeAttributes(in);
	}

protected:
	void cloneData(const void* vertices,
				 const void* indexList,
				 u32 startIndex,
				 u32 endIndex,
				 u32 primitiveCount,
				 video::E_VERTEX_TYPE vType,
				 scene::E_PRIMITIVE_TYPE pType,
				 video::E_INDEX_TYPE iType,
				 bool fVBOFlag);
};
}

#endif
