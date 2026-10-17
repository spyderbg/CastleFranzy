#include "CColladaParticleSystem.h"
#include "CDatabaseCollada.h"
#include "CColladaTags.h"
#include "util.h"


using namespace irr;
using namespace collada;
using namespace converter;
using namespace particle_system;

//struct SBox
//{
//	float		length;
//	float		width;
//	float		height;
//};
void 
CBox::Read(irr::io::IXMLReaderUTF8* reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::length == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &length, 1);
			}
			else if(tags::width == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &width, 1);
			}
			else if(tags::height == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &height, 1);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::box == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CBox::Write(Chunk &chk)
{
	chk.WriteFloat(length);
	chk.WriteFloat(width);
	chk.WriteFloat(height);
}


//struct SSphere
//{
//	float		radius;
//};
void 
CSphere::Read(irr::io::IXMLReaderUTF8* reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::radius == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &radius, 1);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::sphere == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CSphere::Write(Chunk &chk)
{
	chk.WriteFloat(radius);
}


//struct SCylinder
//{
//	float		radius;
//	float		height;
//};

void 
CCylinder::Read(irr::io::IXMLReaderUTF8* reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::radius == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &radius, 1);
			} else if(tags::height == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &height, 1);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::cylinder == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CCylinder::Write(Chunk &chk)
{
	chk.WriteFloat(radius);
	chk.WriteFloat(height);
}

//struct STarget
//{
//};

void 
CTarget::Read(irr::io::IXMLReaderUTF8* reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			
			break;
		case io::EXN_ELEMENT_END:
			if(tags::target == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CTarget::Write(Chunk &chk)
{

}


//struct SShapeElement
//{
//	enum Type
//	{
//		SE_BOX = 0,
//		SE_SPHERE = 1,
//		SE_CYLINDER = 2,
//		SE_TARGET = 3,
//	};
//
//	int iType;
//	union
//	{
//		CBox		*pBox;
//		CSphere		*pSphere;
//		CCylinder	*pCylindre;
//		CTarget		*pTarget;
//	};
//};


//struct SShape
//{
//	CShapeElement	shapeElement;
//};
void 
CShape::Read(irr::io::IXMLReaderUTF8* reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::box == reader->getNodeName())
			{
				iType = SE_BOX;
				_IRR_DEBUG_BREAK_IF(pBox);
				pBox = new CBox();
				pBox->Read(reader);
			} else if(tags::sphere == reader->getNodeName())
			{
				iType = SE_SPHERE;
				_IRR_DEBUG_BREAK_IF(pSphere);
				pSphere = new CSphere();
				pSphere->Read(reader);
			} else if(tags::cylinder == reader->getNodeName())
			{
				iType = SE_CYLINDER;
				_IRR_DEBUG_BREAK_IF(pCylinder);
				pCylinder = new CCylinder();
				pCylinder->Read(reader);
			} else if(tags::target == reader->getNodeName())
			{
				iType = SE_TARGET;
				_IRR_DEBUG_BREAK_IF(pTarget);
				pTarget = new CTarget();
				pTarget->Read(reader);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::shape == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CShape::Write(Chunk &chk)
{
	chk.Write4u(iType);
	Chunk &inst = chk.AddAndWriteChunkOffset();
	switch(iType)
	{
	case SE_BOX:
		pBox->Write(inst);
		break;
	case SE_SPHERE:
		pSphere->Write(inst);
		break;
	case SE_CYLINDER:
		pCylinder->Write(inst);
		break;
	case SE_TARGET:
		pTarget->Write(inst);
		break;
	}
}


//struct SEmission
//{
//	float	emitStart;
//	float	emitStop;
//	int		maxParticles;
//	float	displayUntil;
//	CBirth	birth;
//};

void 
CEmission::Read(irr::io::IXMLReaderUTF8* reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::emitStart == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &emitStart, 1);
			} else if(tags::emitStop == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &emitStop, 1);
			} else if(tags::maxParticles == reader->getNodeName())
			{
				readIntsInsideElement(reader, &maxParticles, 1);
			} else if(tags::displayUntil == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &displayUntil, 1);
			} else if(tags::birthRate == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &birthRate, 1);
			} 
			break;
		case io::EXN_ELEMENT_END:
			if(tags::emission == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CEmission::Write(Chunk &chk)
{
	chk.WriteFloat(emitStart);
	chk.WriteFloat(emitStop);
	chk.Write4u(maxParticles);
	chk.WriteFloat(displayUntil);
	chk.WriteFloat(birthRate);
}

//struct SRandomization
//{
//	int		seed;
//
//	void Read(irr::io::IXMLReaderUTF8* reader);
//	void Write(Chunk &chk);
//};

void 
CRandomization::Read(irr::io::IXMLReaderUTF8* reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::seed == reader->getNodeName())
			{
				readIntsInsideElement(reader, &seed, 1);
			} 
			break;
		case io::EXN_ELEMENT_END:
			if(tags::randomization == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CRandomization::Write(Chunk &chk)
{
	chk.Write4u(seed);
}

//struct SParticleLife
//{
//	float	life;
//	float	lifeVariance;
//};

void 
CParticleLife::Read(irr::io::IXMLReaderUTF8* reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::life == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &life, 1);
			} else if(tags::lifeVariance == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &lifeVariance, 1);
			} 
			break;
		case io::EXN_ELEMENT_END:
			if(tags::particleLife == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CParticleLife::Write(Chunk &chk)
{
	chk.WriteFloat(life);
	chk.WriteFloat(lifeVariance);
}

//struct SParticleSize
//{
//	float	size;
//	float	sizeVariance;
//	float	sizeGrowFor;
//	float	sizeFadeFor;
//};

void 
CParticleSize::Read(irr::io::IXMLReaderUTF8* reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::size == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &size, 1);
			} else if(tags::sizeVariance == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &sizeVariance, 1);
			} else if(tags::sizeGrowFor == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &sizeGrowFor, 1);
			} else if(tags::sizeFadeFor == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &sizeFadeFor, 1);
			} 
			break;
		case io::EXN_ELEMENT_END:
			if(tags::particleSize == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CParticleSize::Write(Chunk &chk)
{
	chk.WriteFloat(size);
	chk.WriteFloat(sizeVariance);
	chk.WriteFloat(sizeGrowFor);
	chk.WriteFloat(sizeFadeFor);
}

//struct SVariableVector
//{
//	core::vector3df	vector;
//	float			variance;
//};
void 
CVariableVector::Read(irr::io::IXMLReaderUTF8* reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::vector == reader->getNodeName())
			{
				readFloatsInsideElement(reader, (float *)&vector, 3);
			} else if(tags::variance == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &variance, 1);
			} 
			break;
		case io::EXN_ELEMENT_END:
			if(tags::variableVector == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CVariableVector::Write(Chunk &chk)
{
	chk.WriteFloat(vector.X); chk.WriteFloat(vector.Y); chk.WriteFloat(vector.Z);
	chk.WriteFloat(variance);
}


//struct SDirectionTarget
//{
//	std::string		url;
//	float			variance;
//};
void 
CDirectionTarget::Read(irr::io::IXMLReaderUTF8* reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::target == reader->getNodeName())
			{
				_IRR_DEBUG_BREAK_IF("TODO: URL");
			} else if(tags::variance == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &variance, 1);
			} 
			break;
		case io::EXN_ELEMENT_END:
			if(tags::directionTarget == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CDirectionTarget::Write(Chunk &chk)
{
	chk.WriteStr(url.c_str());
	chk.WriteFloat(variance);
}


//struct SDirection
//{
//	enum Type
//	{
//		DT_RANDOM_VECTOR = 0,
//		DT_VARIABLE_VECTOR = 1,
//		DT_DIRECTION_TARGET = 2,
//	};
//	int			type;
//	union
//	{
//		CVariableVector		*pVariableVector;
//		CDirectionTarget	*pDirectionTarget;
//	};
//};

void 
CDirection::Read(irr::io::IXMLReaderUTF8* reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::variableVector == reader->getNodeName())
			{
				type = DT_VARIABLE_VECTOR;
				_IRR_DEBUG_BREAK_IF(pVariableVector);
				pVariableVector = new CVariableVector();
				pVariableVector->Read(reader);
			} else if(tags::directionTarget == reader->getNodeName())
			{
				type = DT_DIRECTION_TARGET;
				_IRR_DEBUG_BREAK_IF(pDirectionTarget);
				pDirectionTarget = new CDirectionTarget();
				pDirectionTarget->Read(reader);
			} else if(tags::randomVector == reader->getNodeName())
			{
				type = DT_RANDOM_VECTOR;
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::direction == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CDirection::Write(Chunk &chk)
{
	chk.Write4u(type);
	
	switch(type)
	{
	case DT_RANDOM_VECTOR:
		chk.Write4u(0);
		break;
	case DT_VARIABLE_VECTOR:
		{
			Chunk &inst = chk.AddAndWriteChunkOffset();
			pVariableVector->Write(inst);
		}
		break;
	case DT_DIRECTION_TARGET:
		{
			Chunk &inst = chk.AddAndWriteChunkOffset();
			pDirectionTarget->Write(inst);
		}
		break;
	}
}

//struct SParticleMotion
//{
//	float		speed;
//	float		speedVariance;
//	CDirection	direction;
//};

void 
CParticleMotion::Read(irr::io::IXMLReaderUTF8* reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::speed == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &speed, 1);
			} else if(tags::speedVariance == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &speedVariance, 1);
			} else if(tags::direction == reader->getNodeName())
			{
				direction.Read(reader);
			} 
			break;
		case io::EXN_ELEMENT_END:
			if(tags::particleMotion == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CParticleMotion::Write(Chunk &chk)
{
	chk.WriteFloat(speed);
	chk.WriteFloat(speedVariance);
	direction.Write(chk);
}

//struct SStandardParticle
//{
//	enum Type
//	{
//		SPT_TRIANGLE = 0,
//		SPT_CUBE = 1,
//		SPT_SPECIAL = 2,
//		SPT_FACING = 3,
//		SPT_CONSTANT = 4,
//		SPT_TETRAHEDRON = 5,
//		SPT_SIX_POINTED_STAR = 6,
//		SPR_SPHERE = 7,
//	};
//
//	int	type;
//};

void 
CStandardParticle::Read(irr::io::IXMLReaderUTF8* reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::stdParticleType == reader->getNodeName())
			{
				readIntsInsideElement(reader, &type, 1);
			} 
			break;
		case io::EXN_ELEMENT_END:
			if(tags::standardParticle == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CStandardParticle::Write(Chunk &chk)
{
	chk.Write4u(type);
}

//struct SMetaParticle
//{
//	void *to_be_determined;
//
//	void Read(irr::io::IXMLReaderUTF8* reader);
//	void Write(Chunk &chk);
//};

void 
CMetaParticle::Read(irr::io::IXMLReaderUTF8* reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			
			break;
		case io::EXN_ELEMENT_END:
			if(tags::metaParticle == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CMetaParticle::Write(Chunk &chk)
{
	chk.Write4u(0);
}

//struct SInstancedGeometryParticle
//{
//	void *to_be_determined;
//
//	void Read(irr::io::IXMLReaderUTF8* reader);
//	void Write(Chunk &chk);
//};

void 
CInstancedGeometryParticle::Read(irr::io::IXMLReaderUTF8* reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			
			break;
		case io::EXN_ELEMENT_END:
			if(tags::metaParticle == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CInstancedGeometryParticle::Write(Chunk &chk)
{
	chk.Write4u(0);
}

//struct SParticleType
//{
//	enum Type
//	{
//		PT_STANDARD_PARTICULE = 0,
//		PT_META_PARTICULE = 1,
//		PT_INSTANCED_GEOMETRY_PARTICULE = 2,
//	};
//
//	int			type;
//	union
//	{
//		CStandardParticle			*pStandardParticle;
//		CMetaParticle				*pMetaParticle;
//		CInstancedGeometryParticle	*pInstancedGeometryParticle;
//	};
//
//	void Read(irr::io::IXMLReaderUTF8* reader);
//	void Write(Chunk &chk);
//};

void 
CParticleType::Read(irr::io::IXMLReaderUTF8* reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::standardParticle == reader->getNodeName())
			{
				type = PT_STANDARD_PARTICULE;
				pStandardParticle = new CStandardParticle();
				pStandardParticle->Read(reader);
			} else if(tags::metaParticle == reader->getNodeName())
			{
				type = PT_META_PARTICULE;
				pMetaParticle = new CMetaParticle();
				pMetaParticle->Read(reader);
			} else if(tags::instancedGeometryParticle == reader->getNodeName())
			{
				type = PT_INSTANCED_GEOMETRY_PARTICULE;
				pInstancedGeometryParticle = new CInstancedGeometryParticle();
				pInstancedGeometryParticle->Read(reader);
			} 
			break;
		case io::EXN_ELEMENT_END:
			if(tags::particleType == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CParticleType::Write(Chunk &chk)
{
	chk.Write4u(type);
	switch(type)
	{
	case PT_STANDARD_PARTICULE:
		{
			Chunk &inst = chk.AddAndWriteChunkOffset();
			pStandardParticle->Write(inst);
		}
		break;
	case PT_META_PARTICULE:
		chk.Write4u(0);
		break;
	case PT_INSTANCED_GEOMETRY_PARTICULE:
		chk.Write4u(0);
		break;
	};
}

//struct STravelOrMBlurDir
//{
//	float stretch;
//
//	void Read(irr::io::IXMLReaderUTF8* reader);
//	void Write(Chunk &chk);
//};

void 
CTravelOrMBlurDir::Read(irr::io::IXMLReaderUTF8* reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::stretch == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &stretch, 1);
			} 
			break;
		case io::EXN_ELEMENT_END:
			if(tags::travelOrMBlurDir == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CTravelOrMBlurDir::Write(Chunk &chk)
{
	chk.WriteFloat(stretch);
}

//struct SAxis
//{
//	enum Type
//	{
//		AT_RANDOM_VECTOR = 0,
//		AT_VARIABLE_VECTOR = 1,
//		AT_TRAVEL_OR_MBLUR_DIR = 2,
//	};
//	int			type;
//	union
//	{
//		SVariableVector		*pVariableVector;
//		STravelOrMBlurDir	*pTravelOrMBlurDir;
//	};
//
//	void Read(irr::io::IXMLReaderUTF8* reader);
//	void Write(Chunk &chk);
//}

void 
CAxis::Read(irr::io::IXMLReaderUTF8* reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::variableVector == reader->getNodeName())
			{
				type = AT_VARIABLE_VECTOR;
				pVariableVector = new CVariableVector();
				pVariableVector->Read(reader);
			} else if(tags::travelOrMBlurDir == reader->getNodeName())
			{
				type = AT_TRAVEL_OR_MBLUR_DIR;
				pTravelOrMBlurDir = new CTravelOrMBlurDir();
				pTravelOrMBlurDir->Read(reader);
			} else if(tags::randomVector == reader->getNodeName())
			{
				type = AT_RANDOM_VECTOR;
			} 
			break;
		case io::EXN_ELEMENT_END:
			if(tags::axis == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CAxis::Write(Chunk &chk)
{
	chk.Write4u(type);
	switch(type)
	{
	case AT_RANDOM_VECTOR:
		chk.Write4u(0);
		break;
	case AT_VARIABLE_VECTOR:
		{
			Chunk &inst = chk.AddAndWriteChunkOffset();
			pVariableVector->Write(inst);
		}
		break;
	case AT_TRAVEL_OR_MBLUR_DIR:
		{
			Chunk &inst = chk.AddAndWriteChunkOffset();
			pTravelOrMBlurDir->Write(inst);
		}
		break;
	};
}

//struct SParticleAnimationOffset
//{
//	E_METHOD			method;
//	float				offset;
//	float				variance;
//
//	void Read(irr::io::IXMLReaderUTF8* reader);
//	void Write(Chunk &chk);
//};

void 
CParticleAnimationOffset::Read(irr::io::IXMLReaderUTF8* reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::method == reader->getNodeName())
			{
				std::string methodStr;
				reader->read();
				methodStr = reader->getNodeData();
				if(methodStr == "BIRTH")
				{
					method = EM_BIRTH;
				}
				else
				{
					method = EM_NONE;
				}
			} else if(tags::offset == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &offset, 1);
			} else if(tags::offsetVariance == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &offsetVariance, 1);
			} 
			break;
		case io::EXN_ELEMENT_END:
			if(tags::particleAnimationOffset == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CParticleAnimationOffset::Write(Chunk &chk)
{
	chk.Write4u(method);
	chk.WriteFloat(offset);
	chk.WriteFloat(offsetVariance);
}

//struct CParticleAnimationScaling
//{
//	E_METHOD			method;
//	float				length;
//	float				lengthVariance;
//	float				multiplier;
//	float				multiplierVariance;
//
//	void Read(irr::io::IXMLReaderUTF8* reader);
//	void Write(Chunk &chk);
//};

void 
CParticleAnimationScaling::Read(irr::io::IXMLReaderUTF8* reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::method == reader->getNodeName())
			{
				std::string methodStr;
				reader->read();
				methodStr = reader->getNodeData();
				if(methodStr == "LIFE")
				{
					method = EM_LIFE;
				}
				else
				{
					method = EM_NONE;
				}
			} else if(tags::srcLength == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &srcLength, 1);
			} else if(tags::srcVariance == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &srcVariance, 1);
			} else if(tags::dstMultiplier == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &dstMultiplier, 1);
			} else if(tags::dstVariance == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &dstVariance, 1);
			} 
			break;
		case io::EXN_ELEMENT_END:
			if(tags::particleAnimationScaling == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CParticleAnimationScaling::Write(Chunk &chk)
{
	chk.Write4u(method);
	chk.WriteFloat(srcLength);
	chk.WriteFloat(srcVariance);
	chk.WriteFloat(dstMultiplier);
	chk.WriteFloat(dstVariance);
}

//struct SParticleSpin
//{
//	float				time;
//	float				timeVariance;
//	float				phase;
//	float				phaseVariance;
//	CAxis				axis;
//
//	void Read(irr::io::IXMLReaderUTF8* reader);
//	void Write(Chunk &chk);
//};

void 
CParticleSpin::Read(irr::io::IXMLReaderUTF8* reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::time == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &time, 1);
			} else if(tags::timeVariance == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &timeVariance, 1);
			} else if(tags::phase == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &phase, 1);
			} else if(tags::phaseVariance == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &phaseVariance, 1);
			} else if(tags::axis == reader->getNodeName())
			{
				axis.Read(reader);
			} 
			break;
		case io::EXN_ELEMENT_END:
			if(tags::particleSpin == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CParticleSpin::Write(Chunk &chk)
{
	chk.WriteFloat(time);
	chk.WriteFloat(timeVariance);
	chk.WriteFloat(phase);
	chk.WriteFloat(phaseVariance);
	axis.Write(chk);
}

//struct SEmitter
//{
//	std::string			id;
//	std::string			name;
//	CShape				shape;
//	CEmission			emission;
//	CRandomization		randomization;
//	CParticleLife		particleLife;
//	CParticleSize		particleSize;
//	CParticleMotion		particleMotion;
//	CParticleType		particleType;
//	CParticleSpin		particleSpin;
//
//	void Read(irr::io::IXMLReaderUTF8* reader);
//	void Write(Chunk &chk);
//};

void 
CEmitter::Read(irr::io::IXMLReaderUTF8* reader)
{
	id = reader->getAttributeValueSafe("id");
	name = reader->getAttributeValueSafe("name");

	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::shape == reader->getNodeName())
			{
				shape.Read(reader);
			} else if(tags::emission == reader->getNodeName())
			{
				emission.Read(reader);
			} else if(tags::randomization == reader->getNodeName())
			{
				randomization.Read(reader);
			} else if(tags::particleLife == reader->getNodeName())
			{
				particleLife.Read(reader);
			} else if(tags::particleSize == reader->getNodeName())
			{
				particleSize.Read(reader);
			} else if(tags::particleMotion == reader->getNodeName())
			{
				particleMotion.Read(reader);
			} else if(tags::particleType == reader->getNodeName())
			{
				particleType.Read(reader);
			} else if(tags::particleSpin == reader->getNodeName())
			{
				particleSpin.Read(reader);
			} else if(tags::particleAnimationOffset == reader->getNodeName())
			{
				particleAnimationOffset.Read(reader);
			} else if(tags::particleAnimationScaling == reader->getNodeName())
			{
				particleAnimationScaling.Read(reader);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::emitter == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CEmitter::Write(Chunk &chk)
{
	chk.WriteStr(id.c_str());
	chk.WriteStr(name.c_str());
	shape.Write(chk);
	emission.Write(chk);
	randomization.Write(chk);
	particleLife.Write(chk);
	particleSize.Write(chk);
	particleMotion.Write(chk);
	particleType.Write(chk);
	particleAnimationOffset.Write(chk);
	particleAnimationScaling.Write(chk);
	particleSpin.Write(chk);
}

//struct SInstanceEmitter
//{
//	std::string				filename;
//	std::string				url;
//	CEmitter				*pEmitter;
//	CBindMaterial			bindMaterial;
//	int						drawLayer;
//
//	void Read(irr::io::IXMLReaderUTF8* reader);
//	void Write(Chunk &chk);
//};



void 
CInstanceEmitter::Read(irr::io::IXMLReaderUTF8* reader)
{
	type = CInstance::ITEmitter;
	url = reader->getAttributeValueSafe("url");
	ChangeExtension(url);

	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::bindMaterial == reader->getNodeName())
			{
				bindMaterial.Read(reader);
			} 
			else if(tags::drawLayer == reader->getNodeName())
			{
				reader->read();
				drawLayer = atoi(reader->getNodeData());
			}
			else if(tags::forceNode == reader->getNodeName())
			{
				reader->read();
				std::string forceNode = reader->getNodeData();
				forceNode = forceNode.substr(0, forceNode.find('/'));
				forceNodes.push_back(forceNode);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::instanceEmitter == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CInstanceEmitter::Write(Chunk &chk)
{
	chk.Write4u(type);
	Chunk &inst = chk.AddAndWriteChunkOffset();
	WriteURL(inst, url);
	inst.Write4u(0);
	bindMaterial.Write(inst);
	inst.Write4s(drawLayer);
	inst.Write(forceNodes);
}

void 
CInstanceEmitter::ResolveURL(CCollada &data)
{
	bindMaterial.ResolveURL(data);
}

//struct SLibraryEmitters
//{
//	std::list<CEmitter *> emitters;
//
//	void Read(irr::io::IXMLReaderUTF8* reader);
//	void Write(Chunk &chk);
//};

CLibraryEmitters::~CLibraryEmitters()
{
	for(int i = 0; i < emitters.size(); i++)
	{
		delete emitters[i];
	}
}

void 
CLibraryEmitters::Read(irr::io::IXMLReaderUTF8* reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::emitter == reader->getNodeName())
			{
				CEmitter *pEmitter = new CEmitter();
				pEmitter->Read(reader);
				emitters.push_back(pEmitter);
			} 
			break;
		case io::EXN_ELEMENT_END:
			if(tags::libraryEmitters == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CLibraryEmitters::Write(Chunk &chk)
{
	chk.Write(emitters, tags::libraryEmitters.c_str());
}

//struct CWind
//{
//	float				strength;
//	float				decay;
//	int					type;
//	float				turbulence;
//	float				frequency;
//	float				scale;
//
//	void Read(irr::io::IXMLReaderUTF8* reader);
//	void Write(Chunk &chk);
//};
void 
CWind::Read(irr::io::IXMLReaderUTF8* reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::forceStrength == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &strength, 1);
			} 
			else if(tags::forceDecay == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &decay, 1);
			}
			else if(tags::forceType == reader->getNodeName())
			{
				readIntsInsideElement(reader, &type, 1);
			}
			else if(tags::windTurbulence == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &turbulence, 1);
			}
			else if(tags::windFrequency == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &frequency, 1);
			}
			else if(tags::windScale== reader->getNodeName())
			{
				readFloatsInsideElement(reader, &scale, 1);
			}

			break;
		case io::EXN_ELEMENT_END:
			if(tags::wind == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CWind::Write(Chunk &chk)
{
	chk.WriteFloat(strength);
	chk.WriteFloat(decay);
	chk.Write4u(type);
	chk.WriteFloat(turbulence);
	chk.WriteFloat(frequency);
	chk.WriteFloat(scale);
}

//struct SDeflector
//{
//	float				bounce;
//	float				variation;
//	float				chaos;
//	float				friction;
//	float				inheritVelocity;
//	float				width;
//	float				height;
//};
void 
CDeflector::Read(irr::io::IXMLReaderUTF8* reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::bounce == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &bounce, 1);
			} 
			else if(tags::variation == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &variation, 1);
			}
			else if(tags::chaos == reader->getNodeName())
			{
				readIntsInsideElement(reader, &chaos, 1);
			}
			else if(tags::friction == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &friction, 1);
			}
			else if(tags::inheritVelocity == reader->getNodeName())
			{
				readIntsInsideElement(reader, &inheritVelocity, 1);
			}
			else if(tags::inheritVelocity == reader->getNodeName())
			{
				readIntsInsideElement(reader, &inheritVelocity, 1);
			}
			else if(tags::width == reader->getNodeName())
			{
				readIntsInsideElement(reader, &width, 1);
			}
			else if(tags::height == reader->getNodeName())
			{
				readIntsInsideElement(reader, &height, 1);
			}

			break;
		case io::EXN_ELEMENT_END:
			if(tags::deflector == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CDeflector::Write(Chunk &chk)
{
	chk.WriteFloat(bounce);
	chk.WriteFloat(variation);
	chk.WriteFloat(chaos);
	chk.WriteFloat(friction);
	chk.WriteFloat(inheritVelocity);
	chk.WriteFloat(width);
	chk.WriteFloat(height);
}


//struct CGravity
//{
//	float				strength;
//	float				decay;
//	int					type;
//
//	void Read(irr::io::IXMLReaderUTF8* reader);
//	void Write(Chunk &chk);
//};
void 
CGravity::Read(irr::io::IXMLReaderUTF8* reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::forceStrength == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &strength, 1);
			} 
			else if(tags::forceDecay == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &decay, 1);
			}
			else if(tags::forceType == reader->getNodeName())
			{
				readIntsInsideElement(reader, &type, 1);
			}

			break;
		case io::EXN_ELEMENT_END:
			if(tags::gravity == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CGravity::Write(Chunk &chk)
{
	chk.WriteFloat(strength);
	chk.WriteFloat(decay);
	chk.Write4u(type);
}


//class CForce
//{
//public:
//	enum Type
//	{
//		FT_GRAVITY = 0,
//		FT_WIND = 1,
//	}
//	void Read(irr::io::IXMLReaderUTF8* reader);
//	void Write(Chunk &chk);
//
//	res::String			id;
//	res::String			name;
//	int					type;
//	union
//	{
//		SWind				*pWind;
//		SGravity			*pGravity;
//	};	
//};

CForce::CForce(const CForce &cpy) 
: type(-1)
, pWind(0)
//, pGravity(0)
{
}

CForce::CForce() 
: type(-1)
, pWind(0)
//, pGravity(0)
{
}

CForce::~CForce()
{
	switch(type)
	{
	case FT_GRAVITY:
		delete pGravity;
		break;
	case FT_WIND:
		delete pWind;
		break;
	case FT_DEFLECTOR:
		delete pDeflector;
		break;
	}
}

void 
CForce::Read(irr::io::IXMLReaderUTF8* reader)
{
	id = reader->getAttributeValueSafe("id");
	name = reader->getAttributeValueSafe("name");

	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::wind == reader->getNodeName())
			{
				_IRR_DEBUG_BREAK_IF(pWind != 0);
				type = FT_WIND;
				pWind = new CWind();
				pWind->Read(reader);
			} 
			else if(tags::gravity == reader->getNodeName())
			{
				_IRR_DEBUG_BREAK_IF(pGravity != 0);
				type = FT_GRAVITY;
				pGravity = new CGravity();
				pGravity->Read(reader);
			}
			else if(tags::deflector == reader->getNodeName())
			{
				_IRR_DEBUG_BREAK_IF(pGravity != 0);
				type = FT_DEFLECTOR;
				pDeflector = new CDeflector();
				pDeflector->Read(reader);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::force == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CForce::Write(Chunk &chk)
{
	chk.WriteStr(id.c_str());
	chk.WriteStr(name.c_str());
	chk.Write4u(type);

	Chunk &inst = chk.AddAndWriteChunkOffset();
	switch(type)
	{
	case FT_GRAVITY:
		pGravity->Write(inst);
		break;
	case FT_WIND:
		pWind->Write(inst);
		break;
	case FT_DEFLECTOR:
		pDeflector->Write(inst);
		break;
	default:
		_IRR_DEBUG_BREAK_IF("Not implemented");
		break;
	}
}

//class CLibraryForces
//{
//public:
//	void Read(irr::io::IXMLReaderUTF8* reader);
//	void Write(Chunk &chk);
//
//	std::vector<CForce *> forces;
//};

CLibraryForces::~CLibraryForces()
{
	for(int i = 0; i < forces.size(); i++)
	{
		delete forces[i];
	}
}

void 
CLibraryForces::Read(irr::io::IXMLReaderUTF8* reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::force == reader->getNodeName())
			{
				CForce *pForce = new CForce();
				pForce->Read(reader);
				forces.push_back(pForce);
			} 
			
			
			break;
		case io::EXN_ELEMENT_END:
			if(tags::libraryForces == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CLibraryForces::Write(Chunk &chk)
{
	chk.Write(forces, tags::libraryForces.c_str());
}

void 
CInstanceForce::Read(irr::io::IXMLReaderUTF8* reader)
{
	type = ITForce;
	sid = reader->getAttributeValueSafe("sid");
	url = reader->getAttributeValueSafe("url");
	ChangeExtension(url);

	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			
			break;
		case io::EXN_ELEMENT_END:
			if(tags::instanceForce == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CInstanceForce::Write(Chunk &chk)
{
	chk.Write4u(type);
	Chunk &inst = chk.AddAndWriteChunkOffset();
	WriteURL(inst, url);
	inst.WriteStr(sid.c_str());
}
