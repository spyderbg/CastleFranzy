#include "stdafx.h"
#include "CColladaAnimationTrackTransparency.h"

namespace irr
{
namespace collada
{
namespace animation_track
{

void CTransparencyEx::applyValue(void* dataPtr, void* outputPtr) const
{
	memcpy(outputPtr, dataPtr, getValueSize());
}

int CTransparencyEx::getValueSize() const
{
	return getValueSizeEx();
}

void CTransparencyEx::getBlendedValue(void* inputsArray,
									  float* weightArray,
									  int size,
									  void* outputPtr) const
{
	getBlendedValueEx(inputsArray, weightArray, size, outputPtr);
}

void CTransparencyEx::getKeyBasedValue(const collada::SAnimation& animation, 
									   int key0,
									   int key1,
									   float ratio,
									   void* outputPtr) const
{
	getKeyBasedValueEx(animation, key0, key1, ratio, outputPtr);
}

void CTransparencyEx::getKeyBasedValue(const collada::SAnimation &animation, 
									   int key0,
									   void* outputPtr) const
{
	getKeyBasedValueEx(animation, key0, outputPtr);
}

const CTransparencyEx CTransparencyEx::Instance;

//------------------------------------------------------------------------------

int CTransparency::getValueSize() const
{
	return CTransparencyEx::getValueSizeEx();
}

void CTransparency::getBlendedValue(void* inputsArray,
									float* weightArray,
									int size,
									void* outputPtr) const
{
	CTransparencyEx::getBlendedValueEx(inputsArray, weightArray, size, outputPtr);
}

void CTransparency::getKeyBasedValue(int key0,
									 int key1,
									 float ratio,
									 void* outputPtr) const
{
	CTransparencyEx::getKeyBasedValueEx(m_Animation, key0, key1, ratio, outputPtr);
}

void CTransparency::getKeyBasedValue(int key0, void* outputPtr) const
{
	CTransparencyEx::getKeyBasedValueEx(m_Animation, key0, outputPtr);
}

const collada::CAnimationTrackEx* CTransparency::getAnimationTrackEx() const
{
	return &CTransparencyEx::Instance;
}

}
}
}






