#ifndef __IRR_PRENDERDATA_MODEL_H__
#define __IRR_PRENDERDATA_MODEL_H__

#include <algorithm>
#include "ParticleContext.h"
#include "ParameterNames.h"
#include "PSUtils.h"
#include "PRenderData.h"

namespace irr
{
namespace ps
{

template<class PType>
class PRenderDataBillboardModel
	: public virtual IParticleContext<PType>
{
//DECLARE_PARTICLE_CONTEXT_TYPEDEF(PType)
public:
	typedef scene::IMeshBuffer PMeshBuffer_t;
private:
	PMeshBuffer_t*	m_renderMesh;			//Render mesh used to represent each particle
	typename IParticleContext<PType>::PRenderData_t	m_pRenderData;			//Particle render data residing in m_buffer
	bool			m_bInterleave;			//Flag to interleave stream data (i.e.: vertex, normal, texcoord, color)

	core::matrix4	m_viewMat;				//Camera view matrix for billboards

	bool			m_allocateBuffer;		//Flag to tell the model if it needs to allocate the buffer itself
	void*			m_buffer;				//Pointer to the buffer of data
	size_t			m_size;					//Actual current size of the buffer
	unsigned int	m_sizeNeeded;			//Total size needed for the buffer
	int				m_bufferDirty;			//tells if the render data needs to be recalulated, -1 indicates it's dirty			

	void allocateBuffer(size_t size) {
		if (m_size < size) {
			deallocateBuffer();
			m_buffer = PS_NEW(size);
			m_size = size;
			m_pRenderData.setRenderBufferInfo(m_buffer, m_bInterleave);
		}
	}
	void deallocateBuffer() {
		if (m_buffer && m_allocateBuffer) {
			PS_FREE(m_buffer);
			m_buffer = 0;
		}
	}
public:
	PRenderDataBillboardModel()
		: m_renderMesh(0)
		, m_bInterleave(true)
		, m_allocateBuffer(true)
		, m_buffer(0)
		, m_size(0)
		, m_bufferDirty(-1)
	{
		this->registerPParameter(m_allocateBuffer, ALLOCATE_BUFFER_N);
		this->registerPParameter(m_renderMesh, RENDER_MESH_N);
		this->registerPParameter(m_buffer, RENDER_BUFFER_N);
		this->registerPParameter(m_sizeNeeded, RBUFFER_SIZE_NEEDED_N);
		this->registerPParameter(m_bInterleave, INTERLEAVE_N);
		this->registerPParameter(m_viewMat, VIEW_MATRIX_N);
	}
	virtual ~PRenderDataBillboardModel()
	{
		deallocateBuffer();
	}

	typename IParticleContext<PType>::PRenderData_t& getRenderData(int index = 0) 
	{
		if (m_bufferDirty == index)
			return m_pRenderData;

		//Particle iterators
		typename IParticleContext<PType>::ParticleItt itt = this->pBegin();
		typename IParticleContext<PType>::ParticleItt end = this->pEnd();

		//Model render instance data : vertices stream, indices
		//NOTE-FH: Assumes the mesh buffers are of PRenderData::VertexList type (video::S3DVertexComponentArrays) 
		PRenderData::VertexList* mVertices = reinterpret_cast<PRenderData::VertexList*>(m_renderMesh->getVertices());
		u16* mIndices = m_renderMesh->getIndices();

		//Paricles render instance data
		PRenderData::VertexList& pVertices  = m_pRenderData.m_vertices;
		PRenderData::IndexList& pIndices = m_pRenderData.m_indices;

		char* posPtr = reinterpret_cast<char*>(pVertices.Position);
		char* normalPtr = reinterpret_cast<char*>(pVertices.Normal);
		char* texCoord0Ptr = reinterpret_cast<char*>(pVertices.TexCoord[0].Coord);
		char* color0Ptr = reinterpret_cast<char*>(pVertices.Color0);

		pIndices.clear();
		pIndices.reserve(m_pRenderData.m_riElementCount * this->getParticleCount());
		short indexOffset = 0;

		//Calculate vectors for letting particles look to camera
		core::vector3df hor = core::vector3df(m_viewMat[0], m_viewMat[4], m_viewMat[8]) * 0.5f;
		core::vector3df ver = core::vector3df(m_viewMat[1], m_viewMat[5], m_viewMat[9]) * 0.5f;
		core::vector3df viewVector = core::vector3df(-m_viewMat[2], -m_viewMat[6] , -m_viewMat[10]);
		viewVector *= -1.0f;

		// TODO-FH: Spin

		core::vector3df workingVec;
		video::SColor workingColor;
		video::SColor matDiffuseColor = m_renderMesh->getMaterial().getDiffuseColor();
		core::vector3df quad[4] = { ver - hor, -hor - ver, hor - ver, hor + ver }; //TODO-FH: add right hand define

		//For each particle
		for (; itt != end; ++itt)
		{
			//TODO-FH: UV offset
			if (itt->animOffset != 0.0f || itt->animLength != 0) {
				workingColor = itt->color;
			}
			else {
				workingColor = matDiffuseColor;
			}

			//For each element in the render instance, copy stream data -- interleaved
			for (u32 j = 0; j < m_pRenderData.m_riElementCount; ++j) 
			{
				workingVec = itt->position + quad[j] * itt->size;
				memcpy(posPtr, &workingVec, sizeof(core::vector3df));
				posPtr += pVertices.PositionStride;

				memcpy(normalPtr, &viewVector, sizeof(core::vector3df));
				normalPtr += pVertices.NormalStride;


				memcpy(texCoord0Ptr, &itt->transformUV(mVertices->TexCoord[0].Coord[j]), sizeof(core::vector2df));
				texCoord0Ptr += pVertices.TexCoord[0].Stride;
				
				memcpy(color0Ptr, &workingColor, sizeof(video::SColor));
				color0Ptr += pVertices.Color0Stride;
			}
			//Copy index data --- //TODO-FH: This could be done at initialization!!
			std::transform(mIndices, mIndices + m_renderMesh->getIndexCount(), std::back_inserter(pIndices), IndexOffsetFunctor(indexOffset));
			indexOffset += m_pRenderData.m_riElementCount;
		}
		m_bufferDirty = index;
		return m_pRenderData;
	}

	size_t getRenderBufferSizeNeeded() const {
		return m_sizeNeeded;
	}

	void setRenderBuffer(void* buffer, size_t size) {
		if (buffer) {
			deallocateBuffer();
			m_allocateBuffer = false;
			m_buffer = buffer;
			m_size = size;
			m_pRenderData.setRenderBufferInfo(m_buffer, m_bInterleave);
		}
	}
protected:
	void initPRenderDataModel() 
	{
		int size = 0;
		//Calculate the size needed for the render instance, the element count and number of streams
		if (m_renderMesh) {
			m_pRenderData.setRenderDataInfo(m_renderMesh);
		}

		//Multiply by the maximum number of particles at once in the system
		int maxParticles = *reinterpret_cast<int*>(this->getParameter(MAX_PARTICLES_N));

		//Size of the buffer needed for the whole system
		m_sizeNeeded = m_pRenderData.m_riSize * maxParticles;

		//If allocateBuffer is true allocate the buffer
		if (m_allocateBuffer)
			allocateBuffer(m_sizeNeeded);
	}
	void initPRenderData(typename IParticleContext<PType>::ParticleItt p) {}
	void applyPRenderData(typename IParticleContext<PType>::ParticleItt p) {}
	void initPRenderData(typename IParticleContext<PType>::ParticleItt ibegin, typename IParticleContext<PType>::ParticleItt iend) {} 
	void applyPRenderData(typename IParticleContext<PType>::ParticleItt ibegin, typename IParticleContext<PType>::ParticleItt iend) {
		//TODO-FH: Sort for alpha
		//TODO-FH: Calculate Normal? and screen align for billboards?
		m_bufferDirty = -1;
	}
};


////TODO-FH: Unfinished model
//template<class PType>
//class PRenderDataModel
//	: public virtual IParticleContext<PType>
//{
//DECLARE_PARTICLE_CONTEXT_TYPEDEF(PType)
//public:
//	typedef scene::IMesh		PMesh_t;
//private:
//	PMesh_t*		m_renderMesh;			//Render mesh used to represent each particle
//	PRenderData_t	m_pRenderData;			//Particle render data residing in m_buffer
//	bool			m_bInterleave;			//Flag to interleave stream data (i.e.: vertex, normal, texcoord, color)
//
//	core::vector3df m_camPos;				//Camera position for alpha sorting and billboards
//
//	bool			m_allocateBuffer;		//Flag to tell the model if it needs to allocate the buffer itself
//	void*			m_buffer;				//Pointer to the buffer of data
//	size_t			m_size;					//Actual current size of the buffer
//	unsigned int	m_sizeNeeded;			//Total size needed for the buffer
//	int				m_bufferDirty;			//tells if the render data needs to be recalulated, -1 indicates it's dirty			
//
//	void allocateBuffer(size_t size) {
//		if (m_size < size) {
//			deallocateBuffer();
//			m_buffer = PS_NEW(size);
//			m_size = size;
//			m_pRenderData.setRenderBufferInfo(m_buffer, m_bInterleave);
//		}
//	}
//	void deallocateBuffer() {
//		if (m_buffer && m_allocateBuffer) {
//			PS_FREE(m_buffer);
//			m_buffer = 0;
//		}
//	}
//public:
//	struct MInit : public PInit
//	{
//	};
//
//	PRenderDataModel()
//		: m_renderMesh(0)
//		, m_allocateBuffer(false)
//		, m_buffer(0)
//		, m_size(0)
//		, m_bufferDirty(-1)
//	{
//		this->registerPParameter(m_allocateBuffer, ALLOCATE_BUFFER_N);
//		this->registerPParameter(m_renderMesh, RENDER_MESH_N);
//		this->registerPParameter(m_buffer, RENDER_BUFFER_N);
//		this->registerPParameter(m_sizeNeeded, RBUFFER_SIZE_NEEDED_N);
//		this->registerPParameter(m_bInterleave, INTERLEAVE_N);
//	}
//	virtual ~PRenderDataModel()
//	{
//		deallocateBuffer();
//	}
//
//protected:
//	void initPRenderDataModel() 
//	{
//		int size = 0;
//		//Calculate the size needed for one render instance in the mesh, the element count and number of streams
//		if (m_renderMesh && m_renderMesh->getMeshBufferCount()) {
//			m_pRenderData.SetRenderDataInfo(m_renderMesh->getMeshBuffer(0));
//		}
//
//		//Multiply by the maximum number of particles at once in the system
//		int maxParticles = *reinterpret_cast<int*>(this->getParameter(MAX_PARTICLES_N));
//
//		//Size of the buffer needed for the whole system
//		m_sizeNeeded = m_pRenderData.m_riStreamCount * maxParticles;
//
//		//If allocateBuffer is true allocate the buffer
//		if (m_allocateBuffer)
//			allocateBuffer(m_sizeNeeded);
//	}
//	void initPRenderData(ParticleItt p) {}
//	void applyPRenderData(ParticleItt p) {}
//	void initPRenderData(ParticleItt ibegin, ParticleItt iend) {} 
//	void applyPRenderData(ParticleItt ibegin, ParticleItt iend) {
//		//TODO-FH: Sort for alpha
//		//TODO-FH: Calculate Normal? and screen align for billboards?
//		m_bufferDirty = -1;
//	}
//
//	PRenderData_t& getRenderData(int index = 0) 
//	{
//		if (m_bufferDirty == index) //TODO-FH: Recalculate only index in case of multiple buffer sharing the same data
//			return m_pRenderData;
//
//		//Particle iterators
//		ParticleItt itt = pBegin();
//		ParticleItt end = pEnd();
//
//		//Model render instance data : vertices stream, indices
//		//NOTE-FH: Assumes the mesh buffers are of PRenderData::VertexList type (video::S3DVertexComponentArrays) 
//		PRenderData::VertexList* mVertices = reinterpret_cast<PRenderData::VertexList*>(m_renderMesh->getMeshBuffer(index)->getVertices());
//		u16* mIndices = m_renderMesh->getMeshBuffer(index)->getIndices();
//
//		//Paricles render instance data
//		PRenderData::VertexList& pVertices  = m_pRenderData.m_vertices;
//		PRenderData::IndexList& pIndices = m_pRenderData.m_indices;
//
//		char* posPtr = reinterpret_cast<char*>(pVertices.Position);
//		char* normalPtr = reinterpret_cast<char*>(pVertices.Normal);
//		char* texCoord0Ptr = reinterpret_cast<char*>(pVertices.TexCoord[0].Coord);
//		char* texCoord1Ptr = reinterpret_cast<char*>(pVertices.TexCoord[1].Coord);
//		char* color0Ptr = reinterpret_cast<char*>(pVertices.Color0);
//		char* color1Ptr = reinterpret_cast<char*>(pVertices.Color1);
//
//		pIndices.clear();
//		pIndices.reserve(m_pRenderData.m_riElementCount * this->getParticleCount());
//		short indexOffset = 0;
//
//		//For each particle
//		for (; itt != end; ++itt)
//		{
//			//For each element in the render instance, copy stream data
//			for (u32 j = 0; j < m_pRenderData.m_riElementCount; ++j) 
//			{
//				memcpy(posPtr, &mVertices->Position[j * mVertices->PositionStride], sizeof(core::vector3df));
//				posPtr += pVertices.PositionStride;
//				memcpy(normalPtr, &mVertices->Normal[j * mVertices->NormalStride], sizeof(core::vector3df));
//				normalPtr += pVertices.NormalStride;
//				memcpy(texCoord0Ptr, &mVertices->TexCoord[0].Coord[j * mVertices->TexCoord[0].Stride], sizeof(core::vector2df));
//				texCoord0Ptr += pVertices.TexCoord[0].Stride;
//				memcpy(texCoord1Ptr, &mVertices-pTexCoord[1].Coord[j * mVertices->TexCoord[1].Stride], sizeof(core::vector3df));
//				texCoord1Ptr += pVertices.TexCoord[1].Stride;
//				memcpy(color0Ptr, &mVertices->Color0[j * mVertices->Color0Stride], sizeof(video::SColor));
//				color0Ptr += pVertices.Color0.Stride;
//				memcpy(color1Ptr, &mVertices->Color1[j * mVertices->Color1Stride], sizeof(video::SColor));
//				color1Ptr += pVertices.Color1Stride;
//			}
//			//Copy index data --- // The functor is inlined! do not change for a For loop!
//			std::transform(mIndices, mIndices + m_renderMesh->getMeshBuffer(index)->getIndexCount(), std::back_inserter(pIndices), IndexOffsetFunctor(indexOffset));
//			indexOffset += m_pRenderData.m_riSize;
//		}
//		m_bufferDirty = index;
//		return m_pRenderData;
//	}
//
//	size_t getRenderBufferSizeNeeded() const {
//		return m_sizeNeeded;
//	}
//
//	void SetRenderBuffer(void* buffer, size_t size) {
//		if (buffer) {
//			deallocateBuffer();
//			m_allocateBuffer = false;
//			m_buffer = buffer;
//			m_size = size;
//			m_pRenderData.setRenderBufferInfo(m_buffer, m_bInterleave);
//		}
//	}
//};

}; // end namespace ps
}; // end namespace irr

#endif //__IRR_PRENDERDATA_MODEL_H__
