#include "CColladaSource.h"
#include "quaternion.h"
#include "resFileWriter.h"
#include "IXMLReader.h"
#include "CColladaTags.h"
#include "util.h"

using namespace irr;
using namespace collada;
using namespace converter;

int CSource::s_CTPosition = CSource::DAE_FLOAT;
int CSource::s_CTNormal = CSource::DAE_FLOAT;
int CSource::s_CTTextureUV = CSource::DAE_FLOAT;
int CSource::s_CTColor = CSource::DAE_UNSIGNED_BYTE;



enum SOURCE_PARAM
{
	SP_R = 1,
	SP_G = 2,
	SP_B = 4,
	SP_A = 8,
	SP_S = 16,
	SP_T = 32,
	SP_P = 64,
	SP_X = 128,
	SP_Y = 256,
	SP_Z = 512,
	SP_ANGLE = 1024,
	SP_NAME = 2048,
	SP_RGB = SP_R | SP_G | SP_B,
	SP_RGBA = SP_RGB | SP_A,
	SP_ST = SP_S | SP_T,
	SP_STP = SP_ST | SP_P,
	SP_XYZ = SP_X | SP_Y | SP_Z,
	SP_XYZANGLE = SP_XYZ | SP_ANGLE,

};

CSource::CSource()
: m_bConverted(false)
, m_iSrcType(DAE_UNKNOWN)
, m_iTgtType(DAE_UNKNOWN)
, m_pInputData(0)
, m_pOutputData(0)
, m_iParamCount(0)
, m_bHasScaleAndOffset(false)
{
};

CSource::CSource(std::string &id, std::vector<float>		&inputData)
: m_bConverted(false)
, m_iSrcType(DAE_UNKNOWN)
, m_iTgtType(DAE_UNKNOWN)
, m_pInputData(0)
, m_pOutputData(0)
, m_iParamCount(0)
, m_id(id)
, m_bHasScaleAndOffset(false)
{
	PrepareInputBuffer(DAE_FLOAT, inputData.size());
	*m_pSrcFloat = inputData;
}

CSource::CSource(std::string &id, std::vector<std::string*> &inputData)
: m_bConverted(false)
, m_iSrcType(DAE_UNKNOWN)
, m_iTgtType(DAE_UNKNOWN)
, m_pInputData(0)
, m_pOutputData(0)
, m_iParamCount(0)
, m_id(id)
, m_bHasScaleAndOffset(false)
{
	PrepareInputBuffer(DAE_STRING, inputData.size());
	*m_pSrcString = inputData;
}

CSource::~CSource()
{
	switch(m_iSrcType)
	{
	case DAE_FLOAT:
		delete m_pSrcFloat;
		break;
	case DAE_STRING:
		delete m_pSrcString;
		break;
	case DAE_ANGLE_AXE:
		delete m_pSrcAngleAxe;
		break;
	default:
		_IRR_DEBUG_BREAK_IF(m_pInputData);
		break;
	};

	switch(m_iTgtType & DAE_COMPONENT_MASK)
	{
	case DAE_FLOAT:
		delete m_pTgtF32;
		break;
	case DAE_BYTE:
		delete m_pTgtS8;
		break;
	case DAE_UNSIGNED_BYTE:
		delete m_pTgtU8;
		break;
	case DAE_UNSIGNED_SHORT:
		delete m_pTgtU16;
		break;
	case DAE_SHORT:
		delete m_pTgtS16;
		break;
	case DAE_UNSIGNED_INT:
		delete m_pTgtU32;
		break;
	default:
		_IRR_DEBUG_BREAK_IF(m_pInputData);
		break;
	};
}

CSource::CSource(const CSource &cpy)
: m_iSrcType(DAE_UNKNOWN)
, m_iTgtType(DAE_UNKNOWN)
, m_bConverted(false)
, m_pInputData(0)
, m_pOutputData(0)
, m_iParamCount(0)
, m_bHasScaleAndOffset(false)
{
	m_bHasScaleAndOffset = cpy.m_bHasScaleAndOffset;

	if(cpy.m_pInputData)
	{
		PrepareInputBuffer(cpy.m_iSrcType, cpy.GetInputBufferSize());
	}
	
	if(cpy.m_pOutputData)
	{
		PrepareOutputBuffer(cpy.m_iTgtType, cpy.GetOutputBufferSize());
	}

	m_iSrcType = cpy.m_iSrcType;
	m_iTgtType = cpy.m_iTgtType;
	m_iParamCount = cpy.m_iParamCount;
	m_id = cpy.m_id;
}

void 
CSource::PrepareInputBuffer(int iSrcType, int iSize)
{
	assert(m_iSrcType == DAE_UNKNOWN);

	switch(iSrcType)
	{
	case DAE_FLOAT:
		SetInputComponentType(iSrcType);
		m_pSrcFloat = new std::vector<float>;
		m_pSrcFloat->resize(iSize);
		break;
	case DAE_STRING:
		SetInputParameterType(m_iSrcType);
		m_pSrcString = new std::vector<std::string *>;
		m_pSrcString->resize(iSize);
		break;
	default:
		assert(false);
	}
}

int 
CSource::GetInputBufferSize() const
{
	if(!m_pInputData)
		return 0;

	if(m_iSrcType & DAE_STRING)
	{
		m_pSrcString->size();
	}
	else
	{
		m_pSrcFloat->size();
	}
}

int 
CSource::GetOutputBufferSize() const
{
	if(!m_pOutputData)
		return 0;

	switch(m_iTgtType & DAE_COMPONENT_MASK)
	{
	case DAE_BYTE:
		return m_pTgtS8->size();
	case DAE_UNSIGNED_BYTE:
		return m_pTgtU8->size();
	case DAE_SHORT:
		return m_pTgtS16->size();
	case DAE_UNSIGNED_SHORT:
		return m_pTgtU16->size();
	case DAE_INT:
		return m_pTgtS32->size();
	case DAE_UNSIGNED_INT:
		return m_pTgtU32->size();
	case DAE_FLOAT:
		return m_pTgtF32->size();
	case DAE_STRING:
		return m_pTgtString->size();
	};
	assert(false);
	return 0;
}

bool
CSource::ValidateConvertion(float min, float max)
{
	for(int i; i < m_pSrcFloat->size(); i++)
	{
		float &val = (*m_pSrcFloat)[i];
		if(val < min || val > max)
			return false;
	}
	return true;
};

void 
CSource::ConvertTime()
{
	m_pTgtS32->resize(m_pSrcFloat->size());
	for(int i = 0; i < m_pSrcFloat->size(); i++)
	{
		float &val = (*m_pSrcFloat)[i];
		(*m_pTgtS32)[i] = val * 1000;
	}
}

void
CSource::ConvertInput(int iOutputComponentType, bool bScaleAndOffset)
{
	int iCmpCnt = GetSourceComponentCount();
	if(iOutputComponentType != DAE_FLOAT)
	{
		core::vector3df min, max;
		for(int i = 0; i < iCmpCnt; i++)
		{
			((float*)&min)[i] = (*m_pSrcFloat)[i];
			((float*)&max)[i] = (*m_pSrcFloat)[i];
		}

		for(int i = 1; i < m_pSrcFloat->size(); i++)
		{
			float &curMin = ((float*)&min)[i % iCmpCnt];
			float &curMax = ((float*)&max)[i % iCmpCnt];

			curMin = curMin > (*m_pSrcFloat)[i] ? (*m_pSrcFloat)[i] : curMin;
			curMax = curMax < (*m_pSrcFloat)[i] ? (*m_pSrcFloat)[i] : curMax; 
		}

		// Compute scale
		switch(iOutputComponentType)
		{
		case DAE_BYTE:
		case DAE_UNSIGNED_BYTE:
			m_TgtScale = (max - min) / core::vector3df(0xFF);
			break;
		case DAE_SHORT:
		case DAE_UNSIGNED_SHORT:
			m_TgtScale = (max - min) / core::vector3df(0xFFFF);
			break;
		case DAE_INT:
		case DAE_UNSIGNED_INT:
			m_TgtScale = (max - min) / core::vector3df(0xFFFFFFFF);
			break;
		}
		// Compute offset

		switch(iOutputComponentType)
		{
		case DAE_BYTE:
		case DAE_SHORT:
		case DAE_INT:
			m_TgtOffset = (max + min) / 2;
			break;
		case DAE_UNSIGNED_BYTE:
		case DAE_UNSIGNED_SHORT:
		case DAE_UNSIGNED_INT:
			m_TgtOffset = min;
			break;
		}

		m_bHasScaleAndOffset = true;
	}

#define CURRSCALE  (((float*)&m_TgtScale)[i % iCmpCnt])
#define CURROFFSET (((float*)&m_TgtOffset)[i % iCmpCnt])
	switch(iOutputComponentType)
	{
	case DAE_BYTE:
		m_pTgtS8->resize(m_pSrcFloat->size());
		for(int i = 0; i < m_pSrcFloat->size(); i++)
		{
			(*m_pTgtS8)[i] = (((((*m_pSrcFloat)[i] + 1.0f) / 2.0f) * 0xFF) - 0x80); // -128 -> 127
		}
		break;
	case DAE_UNSIGNED_BYTE:
		m_pTgtU8->resize(m_pSrcFloat->size());
		for(int i = 0; i < m_pSrcFloat->size(); i++)
		{
			(*m_pTgtU8)[i] = ((*m_pSrcFloat)[i] * 0xFF);
		}
		break;
	case DAE_SHORT:

		m_pTgtS16->resize(m_pSrcFloat->size());
		for(int i = 0; i < m_pSrcFloat->size(); i++)
		{
			(*m_pTgtS16)[i] = (short)(((*m_pSrcFloat)[i] - CURROFFSET) / CURRSCALE);
		}
		break;
	case DAE_UNSIGNED_SHORT:
		m_pTgtU16->resize(m_pSrcFloat->size());
		for(int i = 0; i < m_pSrcFloat->size(); i++)
		{
			(*m_pTgtU16)[i] = ((*m_pSrcFloat)[i] * 0xFFFF);
		}
		break;
	case DAE_INT:
		m_pTgtS32->resize(m_pSrcFloat->size());
		for(int i = 0; i < m_pSrcFloat->size(); i++)
		{
			(*m_pTgtS32)[i] = (((((*m_pSrcFloat)[i] + 1.0f) / 2.0f) * 0xFFFFFFFF) - 0x80000000);
		}
		break;
	case DAE_UNSIGNED_INT:
		m_pTgtU32->resize(m_pSrcFloat->size());
		for(int i = 0; i < m_pSrcFloat->size(); i++)
		{
			(*m_pTgtU32)[i] = ((*m_pSrcFloat)[i] * 0xFFFFFFFF);
		}
		break;
	case DAE_FLOAT:
		(*m_pTgtF32) = (*m_pSrcFloat);
		break;
	};
};

void 
CSource::PrepareOutputBuffer(bool bMeshBuffer)
{
	switch(GetSourceParameterType())
	{
	case DAE_POSITION:
		PrepareOutputBuffer(s_CTPosition, 1);
		ConvertInput(s_CTPosition);
		//*m_pTgtF32 = *m_pSrcFloat;
		SetOutputParameterType(DAE_POSITION);
		break;
	case DAE_NORMAL:
		PrepareOutputBuffer(s_CTNormal, 1);
		ConvertInput(s_CTNormal, false);
		m_bHasScaleAndOffset = false; // Use 1/255 or  1/65535
		//*m_pTgtF32 = *m_pSrcFloat;
		SetOutputParameterType(DAE_NORMAL);
		break;
	case DAE_TEXCOORD:
		PrepareOutputBuffer(s_CTTextureUV, 1);
		ConvertInput(s_CTTextureUV);
		//*m_pTgtF32 = *m_pSrcFloat;
		SetOutputParameterType(DAE_TEXCOORD);
		break;
	case DAE_TEXCOORD2:
		PrepareOutputBuffer(s_CTTextureUV, 1);
		ConvertInput(s_CTTextureUV);
		//*m_pTgtF32 = *m_pSrcFloat;
		SetOutputParameterType(DAE_TEXCOORD2);
		break;
	case DAE_TEXCOORD3:
		PrepareOutputBuffer(s_CTTextureUV, 1);
		ConvertInput(s_CTTextureUV);
		//*m_pTgtF32 = *m_pSrcFloat;
		SetOutputParameterType(DAE_TEXCOORD3);
		break;
	case DAE_COLOR1:
		PrepareOutputBuffer(DAE_UNSIGNED_BYTE, 1);
		ConvertColor(bMeshBuffer);
		SetOutputParameterType(bMeshBuffer ? DAE_COLOR4 : DAE_COLOR1);
		break;
	case DAE_COLOR2:
		PrepareOutputBuffer(DAE_UNSIGNED_BYTE, 1);
		ConvertColor(bMeshBuffer);
		SetOutputParameterType(DAE_COLOR2);
		break;
	case DAE_COLOR3:
		PrepareOutputBuffer(DAE_UNSIGNED_BYTE, 1);
		ConvertColor(bMeshBuffer);
		SetOutputParameterType(bMeshBuffer ? DAE_COLOR4 : DAE_COLOR3);
		break;
	case DAE_COLOR4:
		PrepareOutputBuffer(DAE_UNSIGNED_BYTE, 1);
		ConvertColor(bMeshBuffer);
		SetOutputParameterType(DAE_COLOR4);
		break;
	case DAE_QUATERNION:
		PrepareOutputBuffer(DAE_FLOAT, 1);
		*m_pTgtF32 = *m_pSrcFloat;
		SetOutputParameterType(DAE_QUATERNION);
		break;
	case DAE_ANGLE:
		PrepareOutputBuffer(DAE_FLOAT, 1);
		ConvertDegreeToRad();
		SetOutputParameterType(DAE_ANGLE);
		break;
	case DAE_ANGLE_AXE: // convert to quaternion
		PrepareOutputBuffer(DAE_FLOAT, 1);
		ConvertAngleAxeToQuaternion();
		SetOutputParameterType(DAE_QUATERNION);
		break;
	case DAE_STRING:
		PrepareOutputBuffer(DAE_STRING, 1);
		*m_pTgtString = *m_pSrcString;
		SetOutputParameterType(DAE_STRING);
		break;
	case DAE_TIME:	// convert to ms
		PrepareOutputBuffer(DAE_INT, 1);
		ConvertTime();
		SetOutputParameterType(DAE_TIME);
		break;
	default:
		if(GetSourceComponentType() == DAE_FLOAT)
		{
			PrepareOutputBuffer(DAE_FLOAT, 1);
			*m_pTgtF32 = *m_pSrcFloat;
			SetOutputParameterType(DAE_FLOAT);
		}
		else if(GetSourceComponentType() == DAE_STRING)
		{
			PrepareOutputBuffer(DAE_STRING, 1);
			*m_pTgtString = *m_pSrcString;
			SetOutputParameterType(DAE_STRING);
		}
		else
		{
			//assert(false);
		}
		break;
	}
}

void 
CSource::PrepareOutputBuffer(int iTgtType, int iSize) 
{
	assert(m_iTgtType == DAE_UNKNOWN);
	
	/*if(iTgtType & DAE_STRING)
	{
		SetOutputParameterType(iTgtType);
	}
	/*else*/
	if((iTgtType & DAE_STRING) == 0)
	{
		SetOutputComponentsType(iTgtType);
	}
	
	switch(iTgtType)
	{
	case DAE_UNSIGNED_BYTE:
		m_pTgtU8 = new std::vector<unsigned char>;
		m_pTgtU8->reserve(iSize);
		break;
	case DAE_BYTE:
		m_pTgtS8 = new std::vector<char>;
		m_pTgtS8->reserve(iSize);
		break;
	case DAE_UNSIGNED_SHORT:
		m_pTgtU16 = new std::vector<unsigned short>;
		m_pTgtU16->reserve(iSize);
		break;
	case DAE_SHORT:
		m_pTgtS16 = new std::vector<short>;
		m_pTgtS16->reserve(iSize);
		break;
	case DAE_UNSIGNED_INT:
		m_pTgtU32 = new std::vector<unsigned int>;
		m_pTgtU32->reserve(iSize);
		break;
	case DAE_INT:
		m_pTgtS32 = new std::vector<int>;
		m_pTgtS32->reserve(iSize);
		break;
	case DAE_FLOAT:
		m_pTgtF32 = new std::vector<float>;
		m_pTgtF32->reserve(iSize);
		break;
	case DAE_STRING:
		m_pTgtString = new std::vector<std::string *>;
		m_pTgtString->reserve(iSize);
		break;
	default:
		assert(false);
	}
}

const std::string & 
CSource::GetID() const 
{
	return m_id;
}

const std::vector<float>&	
CSource::GetSrcFloat() const
{
	_IRR_DEBUG_BREAK_IF(m_pInputData == 0 || m_iSrcType & DAE_FLOAT == 0);
	return *m_pSrcFloat;
}

const std::vector<std::string*>&	
CSource::GetSrcString() const
{
	_IRR_DEBUG_BREAK_IF(m_pInputData == 0 || m_iSrcType & DAE_STRING == 0);
	return *m_pSrcString;
}


// This function should only be used with the vertice components interleaving processing
int		
CSource::Stride()
{
	int iFactor = 1;
	
	switch(m_iTgtType & DAE_COMPONENT_MASK)
	{
	case DAE_BYTE:
	case DAE_UNSIGNED_BYTE:
		iFactor *= 1;
		break;
	case DAE_SHORT:
	case DAE_UNSIGNED_SHORT:
		iFactor *= 2;
		break;
	case DAE_INT:
	case DAE_UNSIGNED_INT:
	case DAE_FLOAT:
		iFactor *= 4;
		break;
	default:
		return 0;
	}

	switch(m_iTgtType & DAE_PARAMETER_MASK)
	{
	case DAE_POSITION:
	case DAE_NORMAL:
	case DAE_BINORMAL:
	case DAE_TANGENT:
		iFactor *= 3;
		break;
	case DAE_TEXCOORD:
		iFactor *= 1;
		break;
	case DAE_TEXCOORD2:
		iFactor *= 2;
		break;
	case DAE_TEXCOORD3:
	case DAE_COLOR3:
		iFactor *= 3;
		break;
	case DAE_COLOR4:
		iFactor *= 4;
		break;
	case DAE_QUATERNION:
		iFactor *= 4;
		break;
	case DAE_ANGLE:
		iFactor *= 1;
		break;
	case DAE_ANGLE_AXE:
		iFactor *= 4;
		break;
	case DAE_STRING:
		return 0;
	default:
		return 0;
	};

	return iFactor;
}


// This function should only be used with the vertice components interleaving processing
int		
CSource::Size()
{
	return GetOutputBufferSize();
}

void 
CSource::Prepare(bool bMeshSource)
{
	PrepareOutputBuffer(bMeshSource);
}



void 
CSource::ConvertAngleAxeToQuaternion()
{
	if((m_iSrcType & DAE_ANGLE_AXE) == 0)
	{
		return;
	}

	AngleAxe_Quat *pAaq = &(*m_pSrcAngleAxe)[0];
	core::quaternion rotation;

	m_pTgtF32->resize(m_pSrcAngleAxe->size() * 4);
	core::quaternion *pQuaternion = (core::quaternion *)&(*m_pTgtF32)[0];

	for(int i = 0, sz = m_pSrcAngleAxe->size(); i < sz; i++, pAaq++, pQuaternion++)
	{
		rotation.fromAngleAxis(pAaq->angle * -core::DEGTORAD, pAaq->axe);
		*((core::quaternion *)pQuaternion) = rotation;
	}
}



void 
CSource::ConvertColor(bool bMeshSources)
{
	int iTargetComponentCount = 0;

	switch(GetSourceParameterType())
	{
	case DAE_COLOR1:
		iTargetComponentCount = 1;
		break;
	case DAE_COLOR2:
		iTargetComponentCount = 2;
		break;
	case DAE_COLOR3:
		iTargetComponentCount = 3;
		break;
	case DAE_COLOR4:
		iTargetComponentCount = 4;
		break;
	default:
		return;
	}


#define CLAMPEX(type_cast, entry, remap_to) (type_cast)(core::clamp((*m_pSrcFloat)[entry],0.0f,1.0f) * remap_to)

#define FOR_LOOP_4(target, type_cast, remap_to) \
	for(int i = 0, j = 0, sz = m_pSrcFloat->size(); i < sz; i+=4, j+=4) \
	{	\
		target[j + 0] = CLAMPEX(type_cast, i, remap_to);	\
		target[j + 1] = CLAMPEX(type_cast, i + 1, remap_to);	\
		target[j + 2] = CLAMPEX(type_cast, i + 2, remap_to);	\
		target[j + 3] = CLAMPEX(type_cast, i + 3, remap_to);	\
	}

#define FOR_LOOP_3TO4(target, type_cast, remap_to) \
	for(int i = 0, j = 0, sz = m_pSrcFloat->size(); i < sz; i+=3, j+=4) \
	{	\
		target[j + 0] = CLAMPEX(type_cast, i, remap_to);	\
		target[j + 1] = CLAMPEX(type_cast, i + 1, remap_to);	\
		target[j + 2] = CLAMPEX(type_cast, i + 2, remap_to);	\
		target[j + 3] = remap_to;	\
	}

#define FOR_LOOP_3(target, type_cast, remap_to) \
	for(int i = 0, j = 0, sz = m_pSrcFloat->size(); i < sz; i+=3, j+=3) \
	{	\
		target[j + 0] = CLAMPEX(type_cast, i, remap_to);	\
		target[j + 1] = CLAMPEX(type_cast, i + 1, remap_to);	\
		target[j + 2] = CLAMPEX(type_cast, i + 2, remap_to);	\
	}

#define FOR_LOOP_1(target, type_cast, remap_to) \
	for(int i = 0, j = 0, sz = m_pSrcFloat->size(); i < sz; i+=1, j+=1) \
	{	\
		target[j + 0] = CLAMPEX(type_cast, i, remap_to);	\
	}

#define FOR_LOOP_1TO4_A(target, type_cast, remap_to) \
	for(int i = 0, j = 0, sz = m_pSrcFloat->size(); i < sz; i+=1, j+=4) \
	{	\
		target[j + 0] = remap_to;	\
		target[j + 1] = remap_to;	\
		target[j + 2] = remap_to;	\
		target[j + 3] = CLAMPEX(type_cast, i, remap_to);	\
	}

	if(m_iTgtType == DAE_UNKNOWN)
	{
		PrepareOutputBuffer(DAE_UNSIGNED_BYTE, 0);
	}

	switch(GetSourceParameterType())
	{
	case DAE_COLOR4:
		m_pTgtU8->resize(m_pSrcFloat->size());
		FOR_LOOP_4((*m_pTgtU8), unsigned char, 255);
		break;
	case DAE_COLOR3:
		if(bMeshSources)
		{
			m_pTgtU8->resize(m_pSrcFloat->size() * 4 / 3);
			FOR_LOOP_3TO4((*m_pTgtU8), unsigned char, 255);
		}
		else
		{
			m_pTgtU8->resize(m_pSrcFloat->size());
			FOR_LOOP_3((*m_pTgtU8), unsigned char, 255);
		}
		break;
	case DAE_COLOR1:
		if(bMeshSources)
		{
			m_pTgtU8->resize(m_pSrcFloat->size() * 4);
			FOR_LOOP_1TO4_A((*m_pTgtU8), unsigned char, 255);
		}
		else
		{
			m_pTgtU8->resize(m_pSrcFloat->size());
			FOR_LOOP_1((*m_pTgtU8), unsigned char, 255);
		}
		break;
	default:
		break;
	}
}

void 
CSource::ConvertDegreeToRad()
{
	if(!(m_iSrcType & DAE_ANGLE))
	{
		return;
	}

	m_pTgtF32->resize(m_pSrcFloat->size());
	for(int i = 0, sz = m_pSrcFloat->size(); i < sz; i++)
	{
		(*m_pTgtF32)[i] = (*m_pSrcFloat)[i] * -core::DEGTORAD;
	}
}

void
CSource::SetInputParameterType(int iType)
{
	assert((m_iSrcType & DAE_PARAMETER_MASK) == DAE_PARAMETER_MASK);
	m_iSrcType = (iType & DAE_PARAMETER_MASK) | (m_iSrcType & ~DAE_PARAMETER_MASK);
}

void
CSource::SetInputComponentType(int iType)
{
	assert((m_iSrcType & DAE_COMPONENT_MASK) == DAE_COMPONENT_MASK);
	m_iSrcType = (iType & DAE_COMPONENT_MASK) | (m_iSrcType & ~DAE_COMPONENT_MASK);
}

void
CSource::SetOutputParameterType(int iType)
{
	assert((m_iTgtType & DAE_PARAMETER_MASK) == DAE_PARAMETER_MASK);
	m_iTgtType = (iType & DAE_PARAMETER_MASK) | (m_iTgtType & ~DAE_PARAMETER_MASK);
}

void
CSource::SetOutputComponentsType(int iType)
{
	assert((m_iTgtType & DAE_COMPONENT_MASK) == DAE_COMPONENT_MASK);
	m_iTgtType = (iType & DAE_COMPONENT_MASK) | (m_iTgtType & ~DAE_COMPONENT_MASK);
}

int
CSource::GetTargetParameterCount() const
{
	return GetOutputBufferSize() / GetTargetComponentCount();
}

int
CSource::GetSourceParameterType() const
{
	return m_iSrcType & DAE_PARAMETER_MASK;
}

int
CSource::GetSourceComponentType() const
{
	return m_iSrcType & DAE_COMPONENT_MASK;
};

int
CSource::GetSourceComponentCount() const
{
	switch(m_iSrcType & DAE_PARAMETER_MASK)
	{
	case DAE_POSITION:		return 3; //x,y,z
	case DAE_NORMAL:		return 3; //x,y,z
	case DAE_TEXCOORD:		return 1; //s
	case DAE_TEXCOORD2:		return 2; //s,t
	case DAE_TEXCOORD3:		return 3; //s,t,u
	case DAE_COLOR1:		return 1; //a
	case DAE_COLOR3:		return 3; //r,g,b
	case DAE_COLOR4:		return 4; //a,r,g,b
	case DAE_QUATERNION:	return 4; //x,y,z,w
	case DAE_ANGLE:			return 1; //w
	case DAE_ANGLE_AXE:		return 4; //x,y,z,w
	case DAE_STRING:		return 1; //void*
	case DAE_TANGENT:		return 3; //x,y,z
	case DAE_BINORMAL:		return 3; //x,y,z
	default :				return 0;
	}
}

int
CSource::GetTargetParameterType() const
{
	return m_iTgtType & DAE_PARAMETER_MASK;
}

int
CSource::GetTargetComponentType() const
{
	return m_iTgtType & ((int)DAE_COMPONENT_MASK);
};

int
CSource::GetTargetComponentCount() const
{
	switch(m_iTgtType & DAE_PARAMETER_MASK)
	{
	case DAE_POSITION:		return 3; //x,y,z
	case DAE_NORMAL:		return 3; //x,y,z
	case DAE_TEXCOORD:		return 1; //s
	case DAE_TEXCOORD2:		return 2; //s,t
	case DAE_TEXCOORD3:		return 3; //s,t,u
	case DAE_COLOR3:		return 3; //r,g,b
	case DAE_COLOR4:		return 4; //a,r,g,b
	case DAE_QUATERNION:	return 4; //x,y,z,w
	case DAE_ANGLE:			return 1; //w
	case DAE_ANGLE_AXE:		return 4; //x,y,z,w
	case DAE_STRING:		return 1; //void*
	default :				return 0;
	}
}

// This function should only be used with the vertice components interleaving processing
void 
CSource::Write(Chunk &chk, int iEntry)
{
	_IRR_DEBUG_BREAK_IF(m_bConverted);

	int iTargetComponentCount = GetTargetComponentCount();
	iEntry *=  iTargetComponentCount;

	switch(GetTargetComponentType())
	{
	case DAE_STRING:
		for(int i = 0; i < iTargetComponentCount; i++)
			chk.WriteStr(((*m_pTgtString)[iEntry + i])->c_str());
		break;
	case DAE_BYTE:
		for(int i = 0; i < iTargetComponentCount; i++)
			chk.Write1s((*m_pTgtS8)[iEntry + i]);
		break;
	case DAE_UNSIGNED_BYTE:
		for(int i = 0; i < iTargetComponentCount; i++)
			chk.Write1u((*m_pTgtU8)[iEntry + i]);
		break;
	case DAE_SHORT:
		for(int i = 0; i < iTargetComponentCount; i++)
			chk.Write2s((*m_pTgtS16)[iEntry + i]);
		break;
	case DAE_UNSIGNED_SHORT:
		for(int i = 0; i < iTargetComponentCount; i++)
			chk.Write2u((*m_pTgtU16)[iEntry + i]);
		break;
	case DAE_INT:
		for(int i = 0; i < iTargetComponentCount; i++)
			chk.Write4s((*m_pTgtS32)[iEntry + i]);
		break;
	case DAE_UNSIGNED_INT:
		for(int i = 0; i < iTargetComponentCount; i++)
			chk.Write4u((*m_pTgtU32)[iEntry + i]);
		break;
	case DAE_FLOAT:
		for(int i = 0; i < iTargetComponentCount; i++)
			chk.WriteFloat((*m_pTgtF32)[iEntry + i]);
		break;
	}	
}

// [4B] - Array Size
// [4B] - Array Pointer
// [Ext] - Array Data
void 
CSource::Write(Chunk &chk)
{

	chk.Write4s(GetTargetComponentType());
	
	int iCType = GetTargetComponentType();
	int iPType = GetTargetParameterType();

	if(iPType == DAE_STRING)
	{
		chk.Write(*m_pTgtString);
	}
	else
	{
		switch(iCType)
		{
		case DAE_STRING:
			chk.Write(*m_pTgtString);
			break;
		case DAE_BYTE:
			chk.Write(*m_pTgtS8);
			break;
		case DAE_UNSIGNED_BYTE:
			chk.Write(*m_pTgtU8);
			break;
		case DAE_SHORT:
			chk.Write(*m_pTgtS16);
			break;
		case DAE_UNSIGNED_SHORT:
			chk.Write(*m_pTgtU16);
			break;
		case DAE_INT:
			chk.Write(*m_pTgtS32);
			break;
		case DAE_UNSIGNED_INT:
			chk.Write(*m_pTgtU32);
			break;
		case DAE_FLOAT:
			chk.Write(*m_pTgtF32);
			break;
		default:
			chk.Write4u(0);
			chk.Write4u(0);
		}
	}
}


void
CSource::Read(io::IXMLReaderUTF8 *reader)
{
	m_id = reader->getAttributeValueSafe("id");
	
	int iParamMask = 0;
	std::string	paramName;
	m_iSrcType = DAE_UNKNOWN;

	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::floatArray == reader->getNodeName())
			{
				int iCount = reader->getAttributeValueAsInt("count");
				PrepareInputBuffer(DAE_FLOAT, iCount);
				if(iCount)
				{
					readFloatsInsideElement(reader, &(*m_pSrcFloat)[0], iCount);
				}
			} 
			else if(tags::nameArray == reader->getNodeName())
			{
				int iCount = reader->getAttributeValueAsInt("count");
				PrepareInputBuffer(DAE_STRING, iCount);
				
				reader->read();
				readStringArray(reader->getNodeData(), *m_pSrcString, iCount);
			}
			else if(tags::accessor == reader->getNodeName())
			{
				m_iParamCount = reader->getAttributeValueAsInt("stride");
			}
			else if(tags::param == reader->getNodeName())
			{
				paramName = reader->getAttributeValueSafe("name");

				if(paramName == "ANGLE")
				{
					iParamMask |= SP_ANGLE;
				} else if(paramName == "X")
				{
					iParamMask |= SP_X;
				} else if(paramName == "Y")
				{
					iParamMask |= SP_Y;
				} else if(paramName == "Z")
				{
					iParamMask |= SP_Z;
				} else if(paramName == "R")
				{
					iParamMask |= SP_R;
				} else if(paramName == "G")
				{
					iParamMask |= SP_G;
				} else if(paramName == "B")
				{
					iParamMask |= SP_B;
				} else if(paramName == "A")
				{
					iParamMask |= SP_A;
				} else if(paramName == "S")
				{
					iParamMask |= SP_S;
				} else if(paramName == "T")
				{
					iParamMask |= SP_T;
				} else if(paramName == "P")
				{
					iParamMask |= SP_P;
				} else if(paramName == "INTERPOLATION")
				{

				} else if(paramName == "EVENTS")
				{
					iParamMask |= SP_NAME;
				}
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::source == reader->getNodeName())
			{
				switch(iParamMask)
				{
				case SP_ANGLE:
					SetInputParameterType(DAE_ANGLE);
					break;
				case SP_XYZANGLE:
					SetInputParameterType(DAE_ANGLE_AXE);
					break;
				case SP_R:
				case SP_G:
				case SP_B:
				case SP_A:
					SetInputParameterType(DAE_COLOR1);
					break;
				case SP_RGB:
					SetInputParameterType(DAE_COLOR3);
					break;
				case SP_RGBA:
					SetInputParameterType(DAE_COLOR4);
					break;
				case SP_S:
					SetInputParameterType(DAE_TEXCOORD);
					break;
				case SP_ST:
					SetInputParameterType(DAE_TEXCOORD2);
					break;
				case SP_STP:
					SetInputParameterType(DAE_TEXCOORD3);
					break;
				case SP_XYZ:
					if(m_id.find("-positions") != std::string::npos)
					{
						SetInputParameterType(DAE_POSITION);
					}
					else if(m_id.find("-normals") != std::string::npos)
					{
						SetInputParameterType(DAE_NORMAL);
					}
					else if(m_id.find("-tangents") != std::string::npos)
					{
						SetInputParameterType(DAE_TANGENT);
					}
					else if(m_id.find("-binormals") != std::string::npos)
					{
						SetInputParameterType(DAE_BINORMAL);
					}
					else if(m_id.find("-node_translation-output") != std::string::npos)
					{
						SetInputParameterType(DAE_POSITION);
					}
					break;
				case SP_NAME:
					SetInputParameterType(DAE_STRING);
					break;
				}

				//ConvertMatrix();
				//ConvertAngleAxeToQuaternion();
				//ConvertDegreeToRad();
				//ConvertColor();
				return;
			}
			break;
		}
	};
}