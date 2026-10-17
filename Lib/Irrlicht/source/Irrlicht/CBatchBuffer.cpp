#include "stdafx.h"
#include "CBatchBuffer.h"
#include "irrGeometryUtil.h"

namespace irr
{
namespace scene
{

//------------------------------------------------------------------------------

namespace
{

struct STransformTexCoordComponent
{
	core::matrix4 Matrix;
	const core::vector3df* Scale;
	const core::vector3df* Offset;

	template <typename T>
	struct SSource
	{
		typedef core::vector2d<T> Type;
	};

	STransformTexCoordComponent(const core::matrix4& matrix,
								const core::vector3df* scale,
								const core::vector3df* offset)
		: Matrix(matrix)
		, Scale(scale)
		, Offset(offset)
	{
	}

	enum HasTrivialConversionDummyEnum { HasTrivialConversion = 0 } ;

	operator bool () const
	{
		return !Matrix.getDefinitelyIdentityMatrix();
	}

	bool isConversionTrivial() const
	{
		return false;
	}

	void setConvertType(void*)
	{
		if (*this)
		{
			_IRR_DEBUG_BREAK_IF(Scale == NULL || Offset == NULL);
			core::vector2df newTr(Offset->X, Offset->Y);
			Matrix.transformTexCoord(newTr);
			Matrix.setTextureTranslate(newTr.X, newTr.Y);
			Matrix.postTextureScale(Scale->X, Scale->Y);
		}
	}

	template <typename T>
	void convertType(core::vector2df& dest,
					 const core::vector2d<T>& src) const
	{
		_IRR_DEBUG_BREAK_IF(Scale == NULL || Offset == NULL);
		dest.X = src.X * float(Scale->X) + Offset->X;
		dest.Y = src.Y * float(Scale->Y) + Offset->Y;
	}

	void transform(core::vector2df& dest,
				   const core::vector2df& src) const
	{
		Matrix.transformTexCoord(dest, src);
	}
};

struct STransformPositionComponent
{
	core::matrix4 Matrix;
	const core::vector3df* Scale;
	const core::vector3df* Offset;

	template <typename T>
	struct SSource
	{
		typedef core::vector3d<T> Type;
	};

	STransformPositionComponent(const core::matrix4& matrix,
								const core::vector3df* scale,
								const core::vector3df* offset)
		: Matrix(matrix)
		, Scale(scale)
		, Offset(offset)
	{
	}

	enum HasTrivialConversionDummyEnum { HasTrivialConversion = 1 } ;

	operator bool () const
	{
		return !Matrix.getDefinitelyIdentityMatrix();
	}

	bool isConversionTrivial() const
	{
		return (Scale == NULL && Offset == NULL);
	}

	void setConvertType(void*)
	{
		if (*this)
		{
			_IRR_DEBUG_BREAK_IF(Offset == NULL || Scale == NULL);
			core::vector3df newTr = *Offset;
			Matrix.transformVect(newTr);
			Matrix.setTranslation(newTr);
			Matrix.postScale(*Scale);
		}
	}

	template <typename T>
	void convertType(core::vector3df& dest,
					 const core::vector3d<T>& src) const
	{
		_IRR_DEBUG_BREAK_IF(Scale == NULL || Offset == NULL);
		dest = core::vector3df(src) * *Scale + *Offset;
	}

	void transform(core::vector3df& dest,
				   const core::vector3df& src) const
	{
		Matrix.transformVect(dest, src);
	}
};

struct STransformNormalComponent
{
	core::matrix4 Matrix;

	template <typename T>
	struct SSource
	{
		typedef core::vector3d<T> Type;
	};

	STransformNormalComponent(const core::matrix4& matrix)
		: Matrix(matrix)
	{
	}

	enum HasTrivialConversionDummyEnum { HasTrivialConversion = 1 } ;

	operator bool () const
	{
		return !Matrix.getDefinitelyIdentityMatrix();
	}

	bool isConversionTrivial() const
	{
		return false;
	}

	void setConvertType(char*)
	{
		const float s = 1.0f / 127;
		Matrix.postScale(core::vector3df(s, s, s));
	}

	void setConvertType(short*)
	{
		const float s = 1.0f / 32767;
		Matrix.postScale(core::vector3df(s, s, s));
	}

	void convertType(core::vector3df& dest,
					 const core::vector3d<char>& src) const
	{	
		dest = core::vector3df(src) * (1.0f / 127);
	}

	void convertType(core::vector3df& dest,
					 const core::vector3d<short>& src) const
	{
		dest = core::vector3df(src) * (1.0f / 32767);
	}

	void transform(core::vector3df& dest,
				   const core::vector3df& src) const
	{
		Matrix.rotateVect(dest, src);
	}
};

template <typename CT, typename Transform>
inline
CT*
copyComponent(CT* dest,
			  u32 destStride,
			  const CT* src,
			  u32 srcStride,
			  u16 count,
			  const Transform& transform)
{
	if (transform)
	{
		while (count-- > 0)
		{
			transform.transform(*dest, *src);
			dest = core::stepPointer(dest, destStride);
			src = core::stepPointer(src, srcStride);
		}
	}
	else
	{
		dest = core::copyArray(dest, destStride, src, srcStride, count);
	}
	return dest;
}

template <typename DestCT, typename SrcCT, typename Transform>
inline
DestCT*
copyComponent(DestCT* dest,
			  u32 destStride,
			  const SrcCT* src,
			  u32 srcStride,
			  u16 count,
			  const Transform& transform)
{
	if (transform)
	{
		while (count-- > 0)
		{
			transform.transform(*dest, DestCT(*src));
			dest = core::stepPointer(dest, destStride);
			src = core::stepPointer(src, srcStride);
		}
	}
	else if (Transform::HasTrivialConversion && transform.isConversionTrivial())
	{
		while (count-- > 0)
		{
			*dest = DestCT(*src);
			dest = core::stepPointer(dest, destStride);
			src = core::stepPointer(src, srcStride);
		}
	}
	else
	{
		while (count-- > 0)
		{
			transform.convertType(*dest, *src);
			dest = core::stepPointer(dest, destStride);
			src = core::stepPointer(src, srcStride);
		}
	}
	return dest;
}

#define _IRR_COPY_COMPONENT_CASE_STATEMENT_(typeEnum, type)				\
	case irr::video::S3DVertexComponentArrays::typeEnum :				\
	{																	\
		dest = copyComponent(dest,										\
							 destStride,								\
							 reinterpret_cast<const typename Transform::template SSource<type>::Type*>(src), \
							 srcStride,									\
							 count,										\
							 transform);								\
	}																	\
	break

#define _IRR_COPY_COMPONENT_CVT_CASE_STATEMENT_(typeEnum, type)			\
	case irr::video::S3DVertexComponentArrays::typeEnum :				\
	{																	\
		transform.setConvertType((type*)0);								\
		dest = copyComponent(dest,										\
							 destStride,								\
							 reinterpret_cast<const typename Transform::template SSource<type>::Type*>(src), \
							 srcStride,									\
							 count,										\
							 transform);								\
	}																	\
	break

#define _IRR_COPY_COMPONENT_DEFAULT_STATEMENT_	\
	default :									\
	{											\
		_IRR_DEBUG_BREAK_IF("not supported");	\
	}											\
	break
	
template <typename DestCT, typename Transform>
inline
DestCT*
copyComponentSF(DestCT* dest,
				u32 destStride,
				const u8* src,
				u32 srcStride,
				video::S3DVertexComponentArrays::E_COMPONENT_TYPE srcType,
				u16 count,
				Transform& transform)
{
	switch (srcType)
	{
		_IRR_COPY_COMPONENT_CVT_CASE_STATEMENT_(ECT_SHORT, short);
		_IRR_COPY_COMPONENT_CASE_STATEMENT_(ECT_FLOAT, float);
		_IRR_COPY_COMPONENT_DEFAULT_STATEMENT_;
	}
	return dest;
}
	
template <typename DestCT, typename Transform>
inline
DestCT*
copyComponentBSF(DestCT* dest,
				 u32 destStride,
				 const u8* src,
				 u32 srcStride,
				 video::S3DVertexComponentArrays::E_COMPONENT_TYPE srcType,
				 u16 count,
				 Transform& transform)
{
	switch (srcType)
	{
		_IRR_COPY_COMPONENT_CVT_CASE_STATEMENT_(ECT_BYTE, char);
		_IRR_COPY_COMPONENT_CVT_CASE_STATEMENT_(ECT_SHORT, short);
		_IRR_COPY_COMPONENT_CASE_STATEMENT_(ECT_FLOAT, float);
		_IRR_COPY_COMPONENT_DEFAULT_STATEMENT_;
	}
	return dest;
}

#undef _IRR_COPY_COMPONENT_CVT_CASE_STATEMENT_
#undef _IRR_COPY_COMPONENT_CASE_STATEMENT_
#undef _IRR_COPY_COMPONENT_DEFAULT_STATEMENT_

}

//------------------------------------------------------------------------------

CBatchBuffer::CBatchBuffer(video::IVideoDriver* driver)
	: Driver(driver)
	, RequiredVertexAttributes(video::EVA_POSITION)
	, ChangedID_Vertex(0)
	, MappingHint_Vertex(EHM_NEVER)
{
}

CBatchBuffer::CBatchBuffer(const CBatchBuffer* buffer)
	: Driver(buffer->Driver)
	, Material(buffer->Material)
	, RequiredVertexAttributes(video::EVA_POSITION)
	, BBox(buffer->BBox)
	, ChangedID_Vertex(0)
	, MappingHint_Vertex(EHM_NEVER)
{
	// commit to get proper stride (yeah, this is somewhat lazy implementation,
	// but it works great)
	commitMaterialChanges();

	u16 cnt = buffer->getVertexCount();
	u32 sz = buffer->Buffers.getVertexSize();
	setVertexBuffer(NULL, sz, true);
	u8* vbuf = getAppendBuffer(cnt);
	memcpy(vbuf, buffer->Buffers.getVertexStart(), sz);
	commit(vbuf, cnt);
	_IRR_DEBUG_BREAK_IF(vbuf + sz != Buffers.getVertexEnd());
	_IRR_DEBUG_BREAK_IF(getVertexCount() != buffer->getVertexCount());

	u32 icnt = buffer->Buffers.getIndexSize();
	sz = icnt * sizeof(u16);
	setIndexBuffer(NULL, sz, true);
	u16* ibuf = getIndexAppendBuffer(icnt);
	memcpy(ibuf, buffer->Buffers.getIndexStart(), sz);
	commitIndices(ibuf, icnt);
	_IRR_DEBUG_BREAK_IF(ibuf + icnt != Buffers.getIndexEnd());
	_IRR_DEBUG_BREAK_IF((u8*)ibuf + sz != (u8*)Buffers.getIndexEnd());
	_IRR_DEBUG_BREAK_IF(getIndexCount() != buffer->getIndexCount());
}

CBatchBuffer::~CBatchBuffer()
{
}

// IMeshBuffer interface -------------------------------------------------------

E_PRIMITIVE_TYPE
CBatchBuffer::getPrimitiveType() const
{
	return EPT_TRIANGLES;
}

/** \warning If the material's lighting flag or material type has changed */
video::SMaterial&
CBatchBuffer::getMaterial()
{
	return Material;
}

const video::SMaterial&
CBatchBuffer::getMaterial() const
{
	return Material;
}

video::E_VERTEX_TYPE
CBatchBuffer::getVertexType() const
{
	return video::EVT_COMPONENT_ARRAYS;
}

const void*
CBatchBuffer::getVertices() const
{
	return &Components;
}

void*
CBatchBuffer::getVertices()
{
	return &Components;
}

u32
CBatchBuffer::getVertexCount() const
{
	return Buffers.getVertexSize() / Components.PositionStride;
}

video::E_INDEX_TYPE
CBatchBuffer::getIndexType() const
{
	return video::EIT_16BIT;
}

const u16*
CBatchBuffer::getIndices() const
{
	return Buffers.getIndexStart();
}

u16*
CBatchBuffer::getIndices()
{
	return Buffers.getIndexStart();
}

u32
CBatchBuffer::getIndexCount() const
{
	return Buffers.getIndexSize();
}

const core::aabbox3df&
CBatchBuffer::getBoundingBox() const
{
	return BBox;
}

void
CBatchBuffer::setBoundingBox(const core::aabbox3df& box)
{
	BBox = box;
}

void
CBatchBuffer::recalculateBoundingBox()
{
	getBoundingBox(0, getVertexCount(), BBox);
}

const core::vector3df&
CBatchBuffer::getPosition(u32 i) const
{
	_IRR_DEBUG_BREAK_IF(Buffers.getVertexStart() == 0);
	if (Components.PositionType == video::S3DVertexComponentArrays::ECT_FLOAT)
	{
		return *core::stepPointer(Components.Position,
								  Components.PositionStride * i);
	}
	core::vector3df* dummy = NULL;
	return *dummy;
}

core::vector3df&
CBatchBuffer::getPosition(u32 i)
{
	_IRR_DEBUG_BREAK_IF(Buffers.getVertexStart() == 0);
	if (Components.PositionType == video::S3DVertexComponentArrays::ECT_FLOAT)
	{
		return *core::stepPointer(Components.Position,
								  Components.PositionStride * i);
	}
	core::vector3df* dummy = NULL;
	return *dummy;
}

const core::vector3df&
CBatchBuffer::getNormal(u32 i) const
{
	_IRR_DEBUG_BREAK_IF(Buffers.getVertexStart() == 0
						|| (RequiredVertexAttributes & video::EVA_NORMAL) == 0);
	if (Components.NormalType == video::S3DVertexComponentArrays::ECT_FLOAT)
	{
		return *core::stepPointer(Components.Normal,
								  Components.NormalStride * i);
	}
	core::vector3df* dummy = NULL;
	return *dummy;
}

core::vector3df&
CBatchBuffer::getNormal(u32 i)
{
	_IRR_DEBUG_BREAK_IF(Buffers.getVertexStart() == 0
						|| (RequiredVertexAttributes & video::EVA_NORMAL) == 0);
	if (Components.NormalType == video::S3DVertexComponentArrays::ECT_FLOAT)
	{
		return *core::stepPointer(Components.Normal,
								  Components.NormalStride * i);
	}
	core::vector3df* dummy = NULL;
	return *dummy;
}

const core::vector2df&
CBatchBuffer::getTCoords(u32 i, u32 set) const
{
	_IRR_DEBUG_BREAK_IF(Buffers.getVertexStart() == 0
						|| (RequiredVertexAttributes & (video::EVA_TEXCOORD0 << i)) == 0
						|| set >= video::S3DVertexComponentArrays::MAX_TEXTURES);
	if (Components.TexCoord[i].Type == video::S3DVertexComponentArrays::ECT_FLOAT)
	{
		return *core::stepPointer(Components.TexCoord[set].Coord,
								  Components.TexCoord[set].Stride * i);
	}
	core::vector2df* dummy = NULL;
	return *dummy;
}

core::vector2df&
CBatchBuffer::getTCoords(u32 i, u32 set)
{
	_IRR_DEBUG_BREAK_IF(Buffers.getVertexStart() == 0
						|| (RequiredVertexAttributes & (video::EVA_TEXCOORD0 << i)) == 0
						|| set >= video::S3DVertexComponentArrays::MAX_TEXTURES);
	if (Components.TexCoord[i].Type == video::S3DVertexComponentArrays::ECT_FLOAT)
	{
		return *core::stepPointer(Components.TexCoord[set].Coord,
								  Components.TexCoord[set].Stride * i);
	}
	core::vector2df* dummy = NULL;
	return *dummy;
}

void
CBatchBuffer::append(const void* const vertices,
					 u32 numVertices,
					 const u16* const indices,
					 u32 numIndices)
{
	append(*reinterpret_cast<const video::S3DVertexComponentArrays*>(vertices),
		   indices,
		   0,
		   numVertices,
		   numIndices / 3,
		   scene::EPT_TRIANGLES);
}

void
CBatchBuffer::append(const IMeshBuffer* const other)
{
	if (other->getVertexType() != video::EVT_COMPONENT_ARRAYS)
	{
		video::S3DVertexComponentArrays components;
		components.assign(const_cast<void*>(other->getVertices()),
						  other->getVertexType());
		append(components,
			   other->getIndices(),
			   0,
			   other->getVertexCount(),
			   other->getIndexCount() / 3,
			   scene::EPT_TRIANGLES);
	}
	else
	{
		append(*reinterpret_cast<const video::S3DVertexComponentArrays*>(
				   other->getVertices()
			   ),
			   other->getIndices(),
			   0,
			   other->getVertexCount(),
			   other->getIndexCount() / 3,
			   scene::EPT_TRIANGLES);
	}
}

E_HARDWARE_MAPPING
CBatchBuffer::getHardwareMappingHint_Vertex() const
{
	return MappingHint_Vertex;
}

E_HARDWARE_MAPPING
CBatchBuffer::getHardwareMappingHint_Index() const
{
	return scene::EHM_NEVER;
}

void
CBatchBuffer::setHardwareMappingHint(E_HARDWARE_MAPPING newMappingHint,
									 E_BUFFER_TYPE buffer)
{
	if (buffer == EBT_VERTEX || buffer == EBT_VERTEX_AND_INDEX)
	{
		MappingHint_Vertex = newMappingHint;
	}
}

void
CBatchBuffer::setDirty(E_BUFFER_TYPE buffer)
{
	if (buffer == EBT_VERTEX || buffer == EBT_VERTEX_AND_INDEX)
	{
		++ChangedID_Vertex;
	}
}

u32
CBatchBuffer::getChangedID_Vertex() const
{
	return ChangedID_Vertex;
}

u32
CBatchBuffer::getChangedID_Index() const
{
	// todo
	return 0;
}

// this class ------------------------------------------------------------------

void
CBatchBuffer::setVertexBuffer(void* buffer,
							  u32 capacity,
							  bool takeOwnership,
							  bool retainSize)
{
	u8* bufferToSet = NULL;
	if (buffer)
	{
		bufferToSet = reinterpret_cast<u8*>(buffer);
	}
	else if (takeOwnership && capacity != 0)
	{
		if (!Buffers.ownsVertexBuffer()
			|| Buffers.getVertexMaxSize() != capacity)
		{
			bufferToSet = irrnew u8[capacity];
		}
	}
	Buffers.setVertexBuffer(bufferToSet,
							capacity,
							takeOwnership,
							retainSize);
	u8* oldPos = reinterpret_cast<u8*>(Components.Position);
	Components.Position = reinterpret_cast<core::vector3df*>(
		Buffers.getVertexStart()
	);
	if (retainSize)
	{
		// Ajust pointers; strides should be the same
		Components.Normal = core::stepPointer(
			Components.Position,
			reinterpret_cast<u8*>(Components.Normal) - oldPos
		);
		Components.Color0 = core::stepPointer(
			reinterpret_cast<video::SColor*>(Components.Position),
			reinterpret_cast<u8*>(Components.Color0) - oldPos
		);
		for (u32 i = 0; i < video::S3DVertexComponentArrays::MAX_TEXTURES; ++i)
		{
			Components.TexCoord[i].Coord = core::stepPointer(
				reinterpret_cast<core::vector2df*>(Components.Position),
				reinterpret_cast<u8*>(Components.TexCoord[i].Coord) - oldPos
			);
		}
	}
	else
	{
		// commit to set other pointers to make sure the buffer is always in a
		// valid state
		commitMaterialChanges(RequiredVertexAttributes);
	}
}

void
CBatchBuffer::setIndexBuffer(void* buffer,
							 u32 capacity,
							 bool takeOwnership,
							 bool retainSize)
{
	u16* bufferToSet = NULL;
	// Index buffer is stored in u16 units, not u8
	capacity = (capacity + sizeof(u16) - 1) / sizeof(u16);
	if (buffer)
	{
		bufferToSet = reinterpret_cast<u16*>(buffer);
	}
	else if (takeOwnership && capacity != 0)
	{
		if (!Buffers.ownsIndexBuffer()
			|| Buffers.getIndexMaxSize() != capacity)
		{
			bufferToSet = irrnew u16[capacity];
		}
	}
	Buffers.setIndexBuffer(bufferToSet,
						   capacity,
						   takeOwnership,
						   retainSize);
}

void
CBatchBuffer::getBoundingBox(u32 vertexStart,
							 u32 vertexEnd,
							 core::aabbox3df& bbox) const
{
	if (Buffers.getVertexStart() != 0)
	{
		if (Components.PositionType == video::S3DVertexComponentArrays::ECT_FLOAT)
		{
			core::computeBoundingBox(
				core::stepPointer(Components.Position,
								  Components.PositionStride * vertexStart),
				Components.PositionStride,
				vertexEnd - vertexStart,
				bbox
			);
		}
		else
		{
			core::aabbox3d<short> aabb = core::computeBoundingBox(
				core::stepPointer(Components.PositionS16,
								  Components.PositionStride * vertexStart),
				Components.PositionStride,
				vertexEnd - vertexStart
			);
			bbox.MinEdge = core::vector3df(aabb.MinEdge);
			bbox.MaxEdge = core::vector3df(aabb.MaxEdge);
			if (Components.getPositionScale())
			{
				bbox.MinEdge *= *Components.getPositionScale();
				bbox.MaxEdge *= *Components.getPositionScale();
			}
			if (Components.getPositionOffset())
			{
				bbox.MinEdge += *Components.getPositionOffset();
				bbox.MaxEdge += *Components.getPositionOffset();
			}
		}
	}
}

void
CBatchBuffer::overwrite(const video::S3DVertexComponentArrays& vertices,
						u16 startIndex,
						u16 endIndex,
						u8* dest,
						u32 attributeMask,
						const video::SMaterial* material)
{

	_IRR_DEBUG_BREAK_IF(
		(u32)(endIndex - startIndex)
		> (Buffers.getVertexEndStorage() - dest) / Components.PositionStride
	);

	const u16 count = endIndex - startIndex;
	u32 stride = Components.PositionStride;

	attributeMask &= RequiredVertexAttributes;

	// Copy position
	if (attributeMask & video::EVA_POSITION)
	{
		_IRR_DEBUG_BREAK_IF((RequiredVertexAttributes & video::EVA_POSITION) == 0);
		_IRR_DEBUG_BREAK_IF(vertices.Position == NULL);
		STransformPositionComponent transform(Driver->getTransform(video::ETS_WORLD),
											  vertices.getPositionScale(),
											  vertices.getPositionOffset());
#ifdef _DEBUG
		core::vector3df* vend =
#endif
		copyComponentSF(
			reinterpret_cast<core::vector3df*>(
				dest + (reinterpret_cast<u8*>(Components.Position)
						- Buffers.getVertexStart())
			),
			stride,
			(reinterpret_cast<const u8*>(vertices.Position)
			 + vertices.PositionStride * startIndex),
			vertices.PositionStride,
			static_cast<video::S3DVertexComponentArrays::E_COMPONENT_TYPE>(
				vertices.PositionType
			),
			count,
			transform
		);
		_IRR_DEBUG_BREAK_IF(s32((u8*)vend - dest)
							> (Buffers.getVertexEndStorage() - dest
							   + (reinterpret_cast<u8*>(Components.Position)
								  - Buffers.getVertexStart())));
	}
	// Copy normal
	if ((attributeMask & video::EVA_NORMAL)
		&& vertices.Normal)
	{
		STransformNormalComponent transofrm(Driver->getTransform(video::ETS_WORLD));
#ifdef _DEBUG
		core::vector3df* nend =
#endif
		copyComponentBSF(
			reinterpret_cast<core::vector3df*>(
				dest + (reinterpret_cast<u8*>(Components.Normal)
						- Buffers.getVertexStart())
			),
			stride,
			(reinterpret_cast<const u8*>(vertices.Normal)
			 + vertices.NormalStride * startIndex),
			vertices.NormalStride,
			static_cast<video::S3DVertexComponentArrays::E_COMPONENT_TYPE>(
				vertices.NormalType
			),
			count,
			transofrm
		);
		_IRR_DEBUG_BREAK_IF(s32((u8*)nend - dest)
							> (Buffers.getVertexEndStorage() - dest
							   + (reinterpret_cast<u8*>(Components.Normal)
								  - Buffers.getVertexStart())));
	}

	// Copy/set color
	// Note: Components.Color0Stride is tested instead of Components.Color0
	// incase Components only reccords offsets and Color0 is the first component
	if (attributeMask & video::EVA_COLOR0)
	{
		const video::SMaterial& mat = material ? *material : Material;
		if (vertices.Color0
			&& (!mat.getFlag(video::EMF_LIGHTING)
				|| mat.isPerVertexMaterialColorRequested()))
		{
			_IRR_DEBUG_BREAK_IF(
				vertices.Color0Type
				!= video::S3DVertexComponentArrays::ECT_UNSIGNED_BYTE
			);
#ifdef _DEBUG
			video::SColor* cend =
#endif
			core::copyArray(
				reinterpret_cast<video::SColor*>(
					dest + (reinterpret_cast<u8*>(Components.Color0)
							- Buffers.getVertexStart())
				),
				stride,
				core::stepPointer(vertices.Color0,
								  vertices.Color0Stride * startIndex),
				vertices.Color0Stride,
				count
			);
			_IRR_DEBUG_BREAK_IF(s32((u8*)cend - dest)
								> (Buffers.getVertexEndStorage() - dest
								   + (reinterpret_cast<u8*>(Components.Color0)
									  - Buffers.getVertexStart())));
		}
		else
		{
			// ensure per vertex color is requested
			if (material == &Material)
			{
				const_cast<video::SMaterial&>(mat).setFlag(
					video::EMF_PER_VERTEX_MATERIAL_COLOR,
					true
				);
			}
			for (video::SColor
					 *color = reinterpret_cast<video::SColor*>(
						 dest + (reinterpret_cast<u8*>(Components.Color0)
								 - Buffers.getVertexStart())
					 ),
					 *colorEnd = reinterpret_cast<video::SColor*>(
						 reinterpret_cast<u8*>(color) + count * stride
					 );
				 color != colorEnd;
				 color = core::stepPointer(color, stride))
			{
				_IRR_DEBUG_BREAK_IF(s32((u8*)color - dest) >=
									(Buffers.getVertexEndStorage() - dest
									 + (reinterpret_cast<u8*>(Components.Color0)
										- Buffers.getVertexStart())));
				// TODO: this conversion is conditional on the platform
				video::SColor c;
				mat.getDiffuseColor().toRGBA8(reinterpret_cast<u8*>(&c));
				*color = c;
			}
		}
	}

	// Copy texcoords
	u32 texCoords = attributeMask & video::EVA_TEXCOORDS_MASK;
	for (u32 i = 0; texCoords; ++i)
	{
		const u32 unitMask = video::EVA_TEXCOORD0 << i;
		texCoords &= ~unitMask;
		if ((attributeMask & unitMask) && vertices.TexCoord[i].Coord)
		{
			const video::S3DVertexComponentArrays::STexCoordComponent& texCoord
				= vertices.TexCoord[i];
			STransformTexCoordComponent transform(
				(material
				 ? (material == &Material
					? Driver->getTransform(
						static_cast<video::E_TRANSFORMATION_STATE>(video::ETS_TEXTURE_0 + i)
					)
					: (material->hasTextureMatrix(i)
					   ? material->getTextureMatrix(i)
					   : core::IdentityMatrix))
				 : core::IdentityMatrix),
				vertices.getTexCoordScale(i),
				vertices.getTexCoordOffset(i)
			);
#ifdef _DEBUG
			core::vector2df* tend =
#endif
			copyComponentSF(
				reinterpret_cast<core::vector2df*>(
					dest + (reinterpret_cast<u8*>(Components.TexCoord[i].Coord)
							- Buffers.getVertexStart())
				),
				stride,
				(reinterpret_cast<const u8*>(texCoord.Coord)
				 + texCoord.Stride * startIndex),
				texCoord.Stride,
				static_cast<video::S3DVertexComponentArrays::E_COMPONENT_TYPE>(
					texCoord.Type
				),
				count,
				transform
			);
			_IRR_DEBUG_BREAK_IF(s32((u8*)tend - dest)
								> (Buffers.getVertexEndStorage() - dest
								   + (reinterpret_cast<u8*>(Components.TexCoord[i].Coord)
									  - Buffers.getVertexStart())));
		}
	}
}

u32
CBatchBuffer::append(const video::S3DVertexComponentArrays& vertices,
					 u16 startIndex,
					 u16 endIndex)
{
	_IRR_DEBUG_BREAK_IF(!hasEnoughSpace(endIndex - startIndex, 0));
	_IRR_DEBUG_BREAK_IF((RequiredVertexAttributes & video::EVA_POSITION) == 0);
	_IRR_DEBUG_BREAK_IF(vertices.Position == NULL);

	const u16 count = endIndex - startIndex;

	// get where to append
	u8* appendTo = getAppendBuffer(count);

	// copy
	overwrite(vertices, startIndex, endIndex, appendTo, u32(-1), &Material);

	// done
	return commit(appendTo, count);
}

void
CBatchBuffer::overwrite(const u16* indexList,
						s32 offset,
						u32 count,
						E_PRIMITIVE_TYPE pType,
						u16* dest)
{
	if (offset == 0 && pType == EPT_TRIANGLES)
	{
		memcpy(dest, indexList, count * sizeof(u16));
	}
	else
	{
		switch (pType)
		{
			case EPT_TRIANGLE_STRIP :
			{
				indexList++;
				for(u16 *idx = dest, 
						current = 0;
						current < count - 2;
						current++)
				{
					if (current & 0x01)
					{
						*(idx++) = u16(*indexList + offset);
						*(idx++) = u16(*(--indexList) + offset);
						*(idx++) = u16(*(++(++indexList)) + offset);
					}
					else
					{
						*(idx++) = u16(*(--indexList) + offset);
						*(idx++) = u16(*(++indexList) + offset);
						*(idx++) = u16(*(++indexList) + offset);
					}
				}
			}
			break;

			case EPT_TRIANGLE_FAN :
			{
				u16 v0 = u16(indexList[0] + offset);
				u16 vPrev = u16(indexList[1] + offset);
				indexList += 2;
				for (u16
						 *idx = dest,
						 *idxEnd = idx + count;
					 idx != idxEnd;
					 ++indexList)
				{
					_IRR_DEBUG_BREAK_IF(idx
										>= Buffers.getIndexEndStorage());
					*(idx++) = v0;
					_IRR_DEBUG_BREAK_IF(idx
										>= Buffers.getIndexEndStorage());
					*(idx++) = vPrev;
					u16 vCur = u16(*indexList + offset);
					_IRR_DEBUG_BREAK_IF(idx
										>= Buffers.getIndexEndStorage());
					*(idx++) = vCur;
					vPrev = vCur;
				}
			}
			break;

			case EPT_TRIANGLES :
			{
				for (u16
						 *idx = dest,
						 *idxEnd = idx + count;
					 idx != idxEnd;
					 ++idx, ++indexList)
				{
					_IRR_DEBUG_BREAK_IF(idx
										>= Buffers.getIndexEndStorage());
					*idx = u16(*indexList + offset);
				}
			}
			break;

			default :
			{
				_IRR_DEBUG_BREAK_IF("not supported");
			}
			break;
		}
	} 
}

u32
CBatchBuffer::append(const u16* indexList,
					 u16 startIndex,
					 u32 primitiveCount,
					 E_PRIMITIVE_TYPE pType)
{
	_IRR_DEBUG_BREAK_IF(!hasEnoughSpace(0, primitiveCount * 3)
						|| indexList == NULL);

	u32 count = 0;
	u32 finalCount = 0;
	switch (pType)
	{
		case EPT_TRIANGLE_STRIP :
		case EPT_TRIANGLE_FAN :
		{
			count = primitiveCount + 2;
			finalCount = count * 3;
		}
		break;

		case EPT_TRIANGLES :
		{
			count = primitiveCount * 3;
			finalCount = count;
		}
		break;

		default :
		{
			_IRR_DEBUG_BREAK_IF("not supported");
		}
	}

	// get where to append
	u16* appendTo = getIndexAppendBuffer(finalCount);

	if (count == 0)
	{
		return commitIndices(appendTo, finalCount);
	}

	overwrite(indexList, s32(getVertexCount()) - s32(startIndex), count, pType, appendTo);

	// done
	return commitIndices(appendTo, finalCount);
}

void
CBatchBuffer::commitMaterialChanges(u32 requiredAttribs)
{
	// setup pointers in 

	_IRR_DEBUG_BREAK_IF(
		(requiredAttribs & video::EVA_POSITION) == 0
		|| reinterpret_cast<u8*>(Components.Position) != Buffers.getVertexStart()
	);
	u32 stride = sizeof(core::vector3df);

	if (requiredAttribs & video::EVA_NORMAL)
	{
		Components.Normal
			= reinterpret_cast<core::vector3df*>(Buffers.getVertexStart()
												 + stride);
		stride += sizeof(core::vector3df);
	}

	if (requiredAttribs & video::EVA_COLOR0)
	{
		// special case due to diffuse/ambient color coherence issue
		Components.Color0
			= reinterpret_cast<video::SColor*>(Buffers.getVertexStart() + stride);
		stride += sizeof(video::SColor);
	}
	else
	{
		Components.Color0 = NULL;
	}

	u32 texMask = requiredAttribs & video::EVA_TEXCOORDS_MASK;
	for (u32 i = 0; texMask; ++i)
	{
		u32 unitMask = video::EVA_TEXCOORD0 << i;
		texMask &= ~unitMask;
		if (requiredAttribs & unitMask)
		{
			Components.TexCoord[i].Coord
				= reinterpret_cast<core::vector2df*>(Buffers.getVertexStart()
													 + stride);
			stride += sizeof(core::vector2df);
		}
	}

	// assign strides (never mind if's here, not worth it)
	Components.PositionStride = stride;
	Components.NormalStride = stride;
	Components.Color0Stride = stride;
	for (u32 i = 0; i < video::S3DVertexComponentArrays::MAX_TEXTURES; ++i)
	{
		Components.TexCoord[i].Stride = stride;
	}

	RequiredVertexAttributes = requiredAttribs;
}

void
CBatchBuffer::quantizeComponents(bool normalInShort,
								 bool positionInShort)
{
	_IRR_DEBUG_BREAK_IF(Buffers.getVertexStart() == NULL);

	u32 stride = 0;
	video::S3DVertexComponentArrays newComponents;

	// Position
	if (positionInShort)
	{
		newComponents.PositionType = video::S3DVertexComponentArrays::ECT_SHORT;
		stride = sizeof(core::vector3d<short>);
	}
	else
	{
		stride = sizeof(core::vector3df);
	}

	// Normal
	if (RequiredVertexAttributes & video::EVA_NORMAL)
	{
		newComponents.NormalStride = stride; 
		if (normalInShort)
		{
			newComponents.NormalType = video::S3DVertexComponentArrays::ECT_SHORT;
			stride += sizeof(core::vector3d<short>);
		}
		else
		{
			newComponents.NormalType = video::S3DVertexComponentArrays::ECT_BYTE;
			stride += 4;
		}
	}

	// Color
	bool withColor = ((RequiredVertexAttributes & video::EVA_COLOR0) != 0
					  && Components.Color0 != NULL);
	if (withColor)
	{
		newComponents.Color0Stride = stride; 
		stride += sizeof(video::SColor);
	}

	// Texcoords
	u32 texMask = RequiredVertexAttributes & video::EVA_TEXCOORDS_MASK;
	for (u32 i = 0; texMask; ++i)
	{
		u32 unitMask = video::EVA_TEXCOORD0 << i;
		if (texMask & unitMask)
		{
			newComponents.TexCoord[i].Type
				= video::S3DVertexComponentArrays::ECT_SHORT;
			newComponents.TexCoord[i].Stride = stride;
			stride += sizeof(core::vector2d<short>);
		}
		texMask &= ~unitMask;
	}

	// adjust stride for 4 bytes alignment
	if (stride % 4 != 0)
	{
		stride = (stride * 4 + 3) / 4;
	}

	// Setup pointers

	u32 vertexCount = getVertexCount();
	u32 bufferSize = vertexCount * stride;
	newComponents.Position = reinterpret_cast<core::vector3df*>(
		irrnew u8 [bufferSize]
	);
	newComponents.PositionStride = stride;

	if (RequiredVertexAttributes & video::EVA_NORMAL)
	{
		newComponents.Normal
			= core::stepPointer(newComponents.Position,
								newComponents.NormalStride);
		newComponents.NormalStride = stride;
	}

	if (withColor)
	{
		newComponents.Color0
			= core::stepPointer((video::SColor*)newComponents.Position,
								newComponents.Color0Stride);
		newComponents.Color0Stride = stride;
	}

	texMask = RequiredVertexAttributes & video::EVA_TEXCOORDS_MASK;
	for (u32 i = 0; texMask; ++i)
	{
		u32 unitMask = video::EVA_TEXCOORD0 << i;
		if (texMask & unitMask)
		{
			newComponents.TexCoord[i].Coord
				= core::stepPointer(reinterpret_cast<core::vector2df*>(
										newComponents.Position
									),
									newComponents.TexCoord[i].Stride);
			newComponents.TexCoord[i].Stride = stride;
		}
		texMask &= ~unitMask;
	}

	// Process vertex data

	if (positionInShort)
	{
		core::aabbox3df bbox
			= core::computeBoundingBox(Components.Position,
									   Components.PositionStride,
									   vertexCount);

		newComponents.setPositionOffset((bbox.MinEdge + bbox.MaxEdge) * 0.5f);
		newComponents.setPositionScale((bbox.MaxEdge - bbox.MinEdge) / 0xFFFF);

		const core::vector3df* origPos = Components.Position;
		for (core::vector3d<short>
				 *pos = newComponents.PositionS16,
				 *posEnd = core::stepPointer(pos, bufferSize);
			 pos != posEnd;
			 pos = stepPointer(pos, stride))
		{
			*pos = core::vector3d<short>(
				(*origPos - *newComponents.getPositionOffset())
				* *newComponents.getPositionScale()
			);
			origPos = stepPointer(origPos, Components.PositionStride);
		}
	}
	else
	{
		core::copyArray(newComponents.Position,
						stride,
						Components.Position,
						Components.PositionStride,
						vertexCount);
	}

	if (RequiredVertexAttributes & video::EVA_NORMAL)
	{
		_IRR_DEBUG_BREAK_IF(Components.Normal == NULL);
		const core::vector3df* origNormal = Components.Normal;
		if (normalInShort)
		{
			for (core::vector3d<short>
					 *normal = newComponents.NormalS16,
					 *normalEnd = core::stepPointer(normal, bufferSize);
				 normal != normalEnd;
				 normal = core::stepPointer(normal, stride))
			{
				*normal = core::vector3d<short>(*origNormal * f32(0x7FFF));
				origNormal = core::stepPointer(origNormal,
											   Components.NormalStride);
			}				 
		}
		else
		{
			for (core::vector3d<char>
					 *normal = newComponents.NormalS8,
					 *normalEnd = core::stepPointer(normal, bufferSize);
				 normal != normalEnd;
				 normal = core::stepPointer(normal, stride))
			{
				*normal = core::vector3d<char>(*origNormal * f32(0x7F));
				origNormal = core::stepPointer(origNormal,
											   Components.NormalStride);
			}				 
		}
	}

	if (withColor)
	{
		core::copyArray(newComponents.Color0,
						stride,
						Components.Color0,
						Components.Color0Stride,
						vertexCount);
	}

	texMask = RequiredVertexAttributes & video::EVA_TEXCOORDS_MASK;
	for (u32 i = 0; texMask; ++i)
	{
		u32 unitMask = video::EVA_TEXCOORD0 << i;
		if (texMask & unitMask)
		{
			core::rect<f32> texCoordBBox = core::computeBoundingRect(
				reinterpret_cast<const core::position2d<f32>*>(
					Components.TexCoord[i].Coord
				),
				Components.TexCoord[i].Stride,
				vertexCount
			);

			newComponents.setTexCoordOffset(
				i,
				core::vector3df((texCoordBBox.UpperLeftCorner.X
								 + texCoordBBox.LowerRightCorner.X) * 0.5f,
								(texCoordBBox.UpperLeftCorner.Y
								 + texCoordBBox.LowerRightCorner.Y) * 0.5f,
								0.0f)
			);
			newComponents.setTexCoordScale(
				i,
				core::vector3df((texCoordBBox.LowerRightCorner.X
								 - texCoordBBox.UpperLeftCorner.X) / 0xFFFF,
								(texCoordBBox.LowerRightCorner.Y
								 - texCoordBBox.UpperLeftCorner.Y) / 0xFFFF,
								0.0f)
			);

			const core::vector2df* origTCoord = Components.TexCoord[i].Coord;
			for (core::vector2d<short>
					 *tcoord = newComponents.TexCoord[i].CoordS16,
					 *tcoordEnd = core::stepPointer(tcoord, bufferSize);
				 tcoord != tcoordEnd;
				 tcoord = stepPointer(tcoord, stride))
			{
				tcoord->X = short((origTCoord->X - newComponents.getTexCoordOffset(i)->X)
								  / newComponents.getTexCoordScale(i)->X);
				tcoord->Y = short((origTCoord->Y - newComponents.getTexCoordOffset(i)->Y)
								  / newComponents.getTexCoordScale(i)->Y);
				origTCoord = stepPointer(origTCoord, Components.TexCoord[i].Stride);
			}
		}
		texMask &= ~unitMask;
	}

	Components = newComponents;
	Buffers.setVertexBuffer(reinterpret_cast<u8*>(newComponents.Position),
							bufferSize,
							true,
							false);
	commit(Buffers.getVertexStart(), vertexCount);
}
	
void
CBatchBuffer::transferToHardwareBuffer(E_HARDWARE_MAPPING mappingHint)
{
	setHardwareMappingHint(mappingHint, EBT_VERTEX);
	if (mappingHint != EHM_NEVER)
	{
		_IRR_DEBUG_BREAK_IF(Driver == NULL);
		bool batchingEnabled = Driver->getOption(video::EVDO_BATCHING);
		if (batchingEnabled)
		{
			Driver->setOption(video::EVDO_BATCHING, false);
		}
		Driver->setMaterial(Material);
		Driver->drawMeshBuffer(this);
		if (batchingEnabled)
		{
			Driver->setOption(video::EVDO_BATCHING, true);
		}
		setVertexBuffer(NULL, Buffers.getVertexSize(), false, true);
	}
}

} // end namespace scene
} // end namespace irr
