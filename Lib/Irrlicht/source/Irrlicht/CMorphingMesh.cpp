#include "stdafx.h"

#include "IMeshBuffer.h"
#include "CMorphingMesh.h"
#include "S3DVertex.h"
#include "CMeshBuffer.h"
#include "SMesh.h"

using namespace irr;
using namespace scene;


template<class T> void
AddWeightedVertex(T *working, const T *target, irr::f32 weight, int iCount) 
{
	for(int i = 0; i < iCount; i++)
	{
		working->Pos += target->Pos * weight;
		working->Normal += target->Normal * weight;
		working->TCoords += target->TCoords * weight;
		working->Color += target->Color * weight;
		working++;
		target++;
	};
}

template<class T, class V> void
AddWeightedVertexArrays(T *working, const V **target, irr::f32 *weight, int iVertexPerArrays, int iNumbersOfArrays) 
{
	for(int i = 0; i < iNumbersOfArrays; i++, weight++, target++)
	{
		if(*weight)
		{
			AddWeightedVertex(working, (const T*)(*target)->getMeshBuffer(0)->getVertices(), *weight, iVertexPerArrays);
		}

	};
}

void
CMorphingMesh::init()
{
	if(m_WorkingBuffer)
		delete m_WorkingBuffer;

	switch(m_Source->getMeshBuffer(0)->getVertexType())
	{
	case video::EVT_STANDARD:
		{
			SMeshBuffer *buffer = irrnew scene::SMeshBuffer;
			*buffer = *(SMeshBuffer*)m_Source->getMeshBuffer(0);
			m_WorkingBuffer = buffer;
		}
		break;
	case video::EVT_2TCOORDS:
		{
			scene::SMeshBufferLightMap *buffer = irrnew scene::SMeshBufferLightMap;
			*buffer = *(SMeshBufferLightMap*)m_Source->getMeshBuffer(0);
			m_WorkingBuffer = buffer;
		}
		break;
	case video::EVT_TANGENTS:
		{
			scene::SMeshBufferTangents *buffer = irrnew scene::SMeshBufferTangents;
			*buffer = *(SMeshBufferTangents*)m_Source->getMeshBuffer(0);
			m_WorkingBuffer = buffer;
		}
		break;
	}

}

void
CMorphingMesh::prepare()
{
	if(!m_bIsInDirtyState) // Nothing as changed
		return;

	video::E_VERTEX_TYPE type = m_WorkingBuffer->getVertexType();
	u32 count = m_WorkingBuffer->getVertexCount();
	void *vertices = m_WorkingBuffer->getVertices();
	
	switch(type)
	{
	case video::EVT_STANDARD:
		//*(SMeshBuffer*)m_WorkingBuffer = *(SMeshBuffer*)m_Targets[0]->getMeshBuffer(0);
		memset((video::S3DVertex *)m_WorkingBuffer->getVertices(), 0, sizeof(video::S3DVertex) * m_WorkingBuffer->getVertexCount());
		 AddWeightedVertexArrays(
			 (video::S3DVertex *)m_WorkingBuffer->getVertices(),
			 (const scene::IMesh**)m_Targets.pointer(), m_Weights.pointer(), 
			 m_WorkingBuffer->getVertexCount(), m_Targets.size());
		break;
	case video::EVT_2TCOORDS:
		AddWeightedVertexArrays(
			 (video::S3DVertex2TCoords *)m_WorkingBuffer->getVertices(),
			 (const scene::IMesh**)m_Targets.pointer(), m_Weights.pointer(), 
			 m_WorkingBuffer->getVertexCount(), m_Targets.size());
		break;
	case video::EVT_TANGENTS:
		AddWeightedVertexArrays(
			 (video::S3DVertexTangents *)m_WorkingBuffer->getVertices(),
			 (const scene::IMesh**)m_Targets.pointer(), m_Weights.pointer(), 
			 m_WorkingBuffer->getVertexCount(), m_Targets.size());
		break;
	}

	m_bIsInDirtyState = false;
}

void 
CMorphingMesh::setSource(IMesh* src)
{
	// Keep the source a the back of m_Targets
	if(m_Source)
	{
		m_Source->drop();
	}
	
	m_Source = src;
	m_Source->grab();
	
	if(m_Targets.size())
	{
		m_Targets.getLast() = m_Source;
	}
	else
	{
		m_Weights.push_back(1.0);
		m_Targets.push_back(m_Source);
	}
}

void 
CMorphingMesh::addTargets(IMesh* target, irr::f32 weight)
{
	// Keep the source a the back of those 2 array
	target->grab();
	m_Targets.getLast() = target;
	m_Targets.push_back(m_Source); 

	f32 oldSampleWeight = m_Weights.getLast();
	m_Weights.getLast() = weight;
	m_Weights.push_back(oldSampleWeight - weight);
}

void 
CMorphingMesh::reserveTargets(irr::u32 count)
{
	m_Targets.reallocate(count + 1);
	m_Weights.reallocate(count + 1); // the last weight is the applied to the source
}