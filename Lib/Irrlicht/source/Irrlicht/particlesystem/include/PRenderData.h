#ifndef __IRR_PRENDER_DATA_H__
#define __IRR_PRENDER_DATA_H__

#include "S3DVertex.h"
#include "IMesh.h"
#include "IMeshBuffer.h"

namespace irr
{
namespace ps
{

class PRenderData
{
public:
	typedef scene::IMeshBuffer					PMeshBuffer_t;
	typedef video::S3DVertexComponentArrays	VertexList;
	typedef std::vector<short>					IndexList;

	enum PStreamType
	{
		ST_POSITION = 0,
		ST_NORMAL,
		ST_TEXCOORD0,
		ST_TEXCOORD1,
		ST_COLOR0,
		ST_COLOR1
	};

	PRenderData() 
		: m_vertices()
		, m_indices()
		, m_riStreamCount(0)
		, m_riElementCount(0)
		, m_riSize(0)
	{}

	void setRenderDataInfo(PMeshBuffer_t* meshBuffer) {
		PRenderData::VertexList* vertices = reinterpret_cast<PRenderData::VertexList*>(meshBuffer->getVertices());
		m_riElementCount = meshBuffer->getVertexCount();
		m_riSize = 0;
		m_riStreamsSize = 0;
		m_riStreamCount = 0;
		if (vertices->Position) {
			m_riStreamCount |= 1 << ST_POSITION;
			m_riStreamsSize += sizeof(*m_vertices.Position);
		}
		if (vertices->Normal) {
			m_riStreamCount |= 1 << ST_NORMAL;
			m_riStreamsSize += sizeof(*m_vertices.Normal);
		}
		if (vertices->TexCoord[0].Coord) {
			m_riStreamCount |= 1 << ST_TEXCOORD0;
			m_riStreamsSize += sizeof(*m_vertices.TexCoord[0].Coord);
		}
		if (vertices->TexCoord[1].Coord) {
			m_riStreamCount |= 1 << ST_TEXCOORD1;
			m_riStreamsSize += sizeof(*m_vertices.TexCoord[1].Coord);
		}
		if (vertices->Color0) {
			m_riStreamCount |= 1 << ST_COLOR0;
			m_riStreamsSize += sizeof(*m_vertices.Color0);
		}
		if (vertices->Color1) {
			m_riStreamCount |= 1 << ST_COLOR1;
			m_riStreamsSize += sizeof(*m_vertices.Color1);
		}
		m_riSize = m_riStreamsSize * m_riElementCount;
	}

	//Set pointer of each stream inside the buffer as well as their stride
	void setRenderBufferInfo(void* buffer, bool interleave)
	{
		if (buffer == 0)
			return;
		unsigned int offset = 0;
		if (interleave) {
			if (m_riStreamCount & 1 << ST_POSITION) {
				m_vertices.Position = reinterpret_cast<core::vector3df*>(buffer);
				m_vertices.PositionStride = m_riStreamsSize;
				offset += sizeof(*m_vertices.Position);
			}
			if (m_riStreamCount & 1 << ST_NORMAL) {
				m_vertices.Normal = reinterpret_cast<core::vector3df*>(reinterpret_cast<char*>(buffer) + offset);
				m_vertices.NormalStride = m_riStreamsSize;
				offset += sizeof(*m_vertices.Normal);
			}
			if (m_riStreamCount & 1 << ST_TEXCOORD0) {
				m_vertices.TexCoord[0].Coord = reinterpret_cast<core::vector2df*>(reinterpret_cast<char*>(buffer) + offset);
				m_vertices.TexCoord[0].Stride = m_riStreamsSize;
				offset += sizeof(*m_vertices.TexCoord[0].Coord);
			}
			if (m_riStreamCount & 1 << ST_TEXCOORD1) {
				m_vertices.TexCoord[1].Coord = reinterpret_cast<core::vector2df*>(reinterpret_cast<char*>(buffer) + offset);
				m_vertices.TexCoord[1].Stride = m_riStreamsSize;
				offset += sizeof(*m_vertices.TexCoord[1].Coord);
			}
			if (m_riStreamCount & 1 << ST_COLOR0) {
				m_vertices.Color0 = reinterpret_cast<video::SColor*>(reinterpret_cast<char*>(buffer) + offset);
				m_vertices.Color0Stride = m_riStreamsSize;
				offset += sizeof(*m_vertices.Color0);
			}

			if (m_riStreamCount & 1 << ST_COLOR1) {
				m_vertices.Color1 = reinterpret_cast<video::SColor*>(reinterpret_cast<char*>(buffer) + offset);
				m_vertices.Color1Stride = m_riStreamsSize;
			}
		}
		else {
			if (m_riStreamCount & 1 << ST_POSITION) {
				m_vertices.Position = reinterpret_cast<core::vector3df*>(buffer);
			}
			if (m_riStreamCount & 1 << ST_NORMAL) {
				m_vertices.Normal = reinterpret_cast<core::vector3df*>(reinterpret_cast<char*>(buffer) + sizeof(*m_vertices.Position) * m_riElementCount);
			}
			if (m_riStreamCount & 1 << ST_TEXCOORD0) {
				m_vertices.TexCoord[0].Coord = reinterpret_cast<core::vector2df*>(reinterpret_cast<char*>(buffer)
																				  + sizeof(*m_vertices.Position) * m_riElementCount
																				  + sizeof(*m_vertices.Normal) * m_riElementCount);
			}
			if (m_riStreamCount & 1 << ST_TEXCOORD1) {
				m_vertices.TexCoord[1].Coord = reinterpret_cast<core::vector2df*>(reinterpret_cast<char*>(buffer)
																				  + sizeof(*m_vertices.Position) * m_riElementCount
																				  + sizeof(*m_vertices.Normal) * m_riElementCount
																				  + sizeof(*m_vertices.TexCoord[0].Coord) * m_riElementCount);
			}
			if (m_riStreamCount & 1 << ST_COLOR0) {
				m_vertices.Color0 = reinterpret_cast<video::SColor*>(reinterpret_cast<char*>(buffer)
																	 + sizeof(*m_vertices.Position) * m_riElementCount
																	 + sizeof(*m_vertices.Normal) * m_riElementCount
																	 + sizeof(*m_vertices.TexCoord[0].Coord) * m_riElementCount
																	 + sizeof(*m_vertices.TexCoord[1].Coord) * m_riElementCount);
			}

			if (m_riStreamCount & 1 << ST_COLOR1) {
				m_vertices.Color1 = reinterpret_cast<video::SColor*>(reinterpret_cast<char*>(buffer)
																	 + sizeof(*m_vertices.Position) * m_riElementCount
																	 + sizeof(*m_vertices.Normal) * m_riElementCount
																	 + sizeof(*m_vertices.TexCoord[0].Coord) * m_riElementCount
																	 + sizeof(*m_vertices.TexCoord[1].Coord) * m_riElementCount
																	 + sizeof(*m_vertices.Color0) * m_riElementCount);
			}
		}
	}

	VertexList		m_vertices;
	IndexList		m_indices;

	unsigned int	m_riStreamCount;		//Flag of streams used by the render instance (i.e.: vertex, normal, texcoord, color)
	unsigned int	m_riStreamsSize;		//Size taken by one element for all streams
	unsigned int	m_riElementCount;		//Number of elements in each stream used by the render instance
	unsigned int	m_riSize;				//The actual size of the renderInstance
};


//Particle system dummy mesh buffer to make quads
class CPSQuadMeshBuffer : public scene::IMeshBuffer
{
public:
	CPSQuadMeshBuffer()
		: m_Material(0)
	{
		//TODO-FH: Particle Type
		m_texCoord0[0] = core::vector2df(0.0f, 0.0f);
		m_texCoord0[1] = core::vector2df(0.0f, 1.0f);
		m_texCoord0[2] = core::vector2df(1.0f, 1.0f);
		m_texCoord0[3] = core::vector2df(1.0f, 0.0f);
		m_Vertices.Position = &m_dummyVectorComponentArray;
		m_Vertices.Normal = &m_dummyVectorComponentArray;
		m_Vertices.Color0 = &m_dummyColorComponentArray;
		m_Vertices.TexCoord[0].Coord = &m_texCoord0[0];
		m_Indices.push_back(0);
		m_Indices.push_back(1);
		m_Indices.push_back(2);
		m_Indices.push_back(0);
		m_Indices.push_back(2);
		m_Indices.push_back(3);
		//m_Material.setFlag(video::EMF_BACK_FACE_CULLING, false);
		//m_Material.setFlag(video::EMF_FRONT_FACE_CULLING, false);
		//m_Material.Lighting = false;
	}

	virtual scene::E_PRIMITIVE_TYPE getPrimitiveType() const
	{
		return scene::EPT_TRIANGLES;
	}

	void setMaterial(video::SMaterial& mat) {
		m_Material = &mat;
	}
	virtual video::SMaterial& getMaterial() {
		return *m_Material;
	}
	virtual const video::SMaterial& getMaterial() const {
		return *m_Material;
	}
	virtual video::E_VERTEX_TYPE getVertexType() const {
		return video::EVT_COMPONENT_ARRAYS;
	}
	virtual const void* getVertices() const {
		return &m_Vertices;
	}
	virtual void* getVertices() {
		return &m_Vertices;
	}
	virtual u32 getVertexCount() const {
		return 4;
	}
	virtual video::E_INDEX_TYPE getIndexType() const {
		return video::EIT_16BIT;
	}
	virtual const u16* getIndices() const {
		return m_Indices.const_pointer();
	}
	virtual u16* getIndices() {
		return m_Indices.pointer();
	}
	virtual u32 getIndexCount() const {
		return m_Indices.size();
	}
	virtual const core::aabbox3df& getBoundingBox() const {
		return m_box;
	}
	virtual void setBoundingBox(const core::aabbox3df& box) {}
	virtual void recalculateBoundingBox() {}
	virtual const core::vector3df& getPosition(u32 i) const { return m_dummyVectorComponentArray; }
	virtual core::vector3df& getPosition(u32 i) { return m_dummyVectorComponentArray; }
	virtual const core::vector3df& getNormal(u32 i) const { return m_dummyVectorComponentArray; }
	virtual core::vector3df& getNormal(u32 i) { return m_dummyVectorComponentArray; }
	virtual const core::vector2df& getTCoords(u32 i, u32 set = 0) const 
	{ 
		_IRR_DEBUG_BREAK_IF(set != 0);
		return m_texCoord0[0]; 
	}
	virtual core::vector2df& getTCoords(u32 i, u32 set = 0) 
	{ 
		_IRR_DEBUG_BREAK_IF(set != 0);
		return m_texCoord0[0]; 
	}
	virtual void append(const void* const vertices, u32 numVertices, const u16* const indices, u32 numIndices) {}
	virtual void append(const IMeshBuffer* const other) {}
	virtual scene::E_HARDWARE_MAPPING getHardwareMappingHint_Vertex() const {
		return scene::EHM_STREAM;
	}
	virtual scene::E_HARDWARE_MAPPING getHardwareMappingHint_Index() const {
		return scene::EHM_STREAM;
	}
	virtual void setHardwareMappingHint( scene::E_HARDWARE_MAPPING NewMappingHint, scene::E_BUFFER_TYPE Buffer=scene::EBT_VERTEX_AND_INDEX ){}
	virtual void setDirty(scene::E_BUFFER_TYPE Buffer=scene::EBT_VERTEX_AND_INDEX) {}
	virtual u32 getChangedID_Vertex() const {
		return 0;
	}
	virtual u32 getChangedID_Index() const {
		return 0;
	}
	video::SMaterial*					m_Material;
	video::S3DVertexComponentArrays		m_Vertices;
	core::array<u16>					m_Indices;
	core::aabbox3d<f32>					m_box;

	core::vector3df						m_dummyVectorComponentArray;
	video::SColor						m_dummyColorComponentArray;
	core::vector2df						m_texCoord0[4];
};

}; //end namespace ps
}; //end namespace irr

#endif
