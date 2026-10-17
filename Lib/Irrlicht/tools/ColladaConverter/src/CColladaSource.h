#ifndef __CCOLLADACONVERTERSOURCE_H__
#define __CCOLLADACONVERTERSOURCE_H__


#include <string>
#include <vector>
#include <list>
#include "vector3d.h"
#include "IXMLReader.h"

struct Chunk;

namespace irr
{

namespace collada
{
namespace converter
{

struct AngleAxe_Quat
{
	core::vector3df axe;
	float angle;
};

class CSource
{
public:
	enum DaeType
	{
		DAE_BYTE					= 0x00,
		DAE_UNSIGNED_BYTE			= 0x01,
		DAE_SHORT					= 0x02,
		DAE_UNSIGNED_SHORT			= 0x03,
		DAE_INT						= 0x04,
		DAE_UNSIGNED_INT			= 0x05,
		DAE_FLOAT					= 0x06,
		DAE_COMPONENT_MASK			= 0x0F,

		DAE_POSITION				= 0x10,		
		DAE_NORMAL					= 0x20,
		DAE_TEXCOORD				= 0x30,
		DAE_TEXCOORD2				= 0x40,
		DAE_TEXCOORD3				= 0x50,
		DAE_COLOR1					= 0x60,
		DAE_COLOR2					= 0x70,
		DAE_COLOR3					= 0x80,
		DAE_COLOR4					= 0x90,

		DAE_QUATERNION				= 0xA0,
		DAE_ANGLE					= 0xB0,
		DAE_ANGLE_AXE				= 0xD0,
		DAE_STRING					= 0xE0,
		DAE_TIME					= 0xF0,

		DAE_TANGENT					= 0x100,
		DAE_BINORMAL				= 0x110,

		DAE_PARAMETER_MASK			= 0xFF0,
		

		DAE_UNKNOWN					= 0xFFF,
	};

	static int s_CTPosition;
	static int s_CTNormal;
	static int s_CTTextureUV;
	static int s_CTColor;

	
	CSource::CSource();
	CSource::CSource(const CSource &cpy);
	CSource::CSource(std::string &id, std::vector<float>		&inputData);
	CSource::CSource(std::string &id, std::vector<std::string*> &inputData);

	CSource::~CSource();

	void							PrepareInputBuffer(int iSrcType, int iSize);
	int								GetInputBufferSize() const;
	
	void							PrepareOutputBuffer(int iTgtType, int iSize);
	int								GetOutputBufferSize() const;

	const std::string &					GetID() const;
	const std::vector<float>&			GetSrcFloat() const;
	const std::vector<std::string*>&	GetSrcString() const;

	void							Read(io::IXMLReaderUTF8 *reader);
	void							Write(Chunk &chk);
	void							Write(Chunk &chk, int iEntry);
	int								Size();
	int								Stride();

	void							ConvertMatrix();
	void							ConvertAngleAxeToQuaternion();
	void							ConvertDegreeToRad();
	void							ConvertColor(bool bAlphaAdd);

	int								GetSourceComponentType() const;
	int								GetSourceParameterType() const;
	int								GetSourceComponentCount() const;

	int								GetTargetComponentType() const;
	int								GetTargetParameterType() const;
	int								GetTargetComponentCount() const;
	int								GetTargetParameterCount() const;

	void							SetOutputParameterType(int iType);
	void							SetOutputComponentsType(int iType);

	void							SetInputParameterType(int iType);
	void							SetInputComponentType(int iType);

	void							Prepare(bool bMeshSource);

	bool							HasScaleAndOffset() { return m_bHasScaleAndOffset; }

	const	core::vector3df&		GetTargetComponentScale() const { return m_TgtScale; }
	const	core::vector3df&		GetTargetComponentOffset() const { return m_TgtOffset; }
protected:
	void							ConvertInput(int iOutputComponentType, bool bScaleAndOffset = true);
	void							ConvertTime();
	void							PrepareOutputBuffer(bool bMeshBuffer);
	bool							ValidateConvertion(float min, float max);

	std::string						m_id;
	
	int								m_iSrcType;
	union
	{
		void *m_pInputData;
		std::vector<float>			*m_pSrcFloat;
		std::vector<AngleAxe_Quat>	*m_pSrcAngleAxe;
		std::vector<std::string*>	*m_pSrcString;
	};

	int								m_iTgtType;
	union
	{
		void *m_pOutputData;
		std::vector<unsigned char>	*m_pTgtU8;
		std::vector<char>			*m_pTgtS8;
		std::vector<unsigned short>	*m_pTgtU16;
		std::vector<short>			*m_pTgtS16;
		std::vector<unsigned int>	*m_pTgtU32;
		std::vector<int>			*m_pTgtS32;
		std::vector<float>			*m_pTgtF32;
		std::vector<std::string*>	*m_pTgtString;
	};

	core::vector3df					m_TgtScale;
	core::vector3df					m_TgtOffset;
	
	int								m_iParamCount;
	bool							m_bConverted;
	bool							m_bHasScaleAndOffset;
	//bool							bInterleave;

	
};

}; // namespace converter
}; // namespace collada
}; // namespace irr
#endif //__CCOLLADACONVERTERSOURCE_H__