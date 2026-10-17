#ifndef __CCOLLADAPARTICULESYSTEM_H__
#define __CCOLLADAPARTICULESYSTEM_H__

#include "irrString.h"
#include "irrArray.h"
#include "matrix4.h"
#include "IXMLReader.h"
#include "resFileWriter.h"
#include "aabbox3d.h"


namespace irr
{
namespace collada
{
namespace converter
{
struct CCollada;

namespace particle_system
{
struct CStandardParticle;
struct CMetaParticle;
struct CInstancedGeometryParticle;

struct CBox
{
	float		length;
	float		width;
	float		height;

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
};

struct CSphere
{
	float		radius;

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
};

struct CCylinder
{
	float		radius;
	float		height;

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
};

struct CTarget
{

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
};



struct CShape
{
	CShape() : pBox(0), pSphere(0), pCylinder(0), pTarget(0) {}
	~CShape() 
	{
		if(pBox) delete pBox;
		if(pSphere) delete pSphere;
		if(pCylinder) delete pCylinder;
		if(pTarget) delete pTarget;
	}
	enum Type
	{
		SE_BOX = 0,
		SE_SPHERE = 1,
		SE_CYLINDER = 2,
		SE_TARGET = 3,
	};

	int iType;
	//union
	//{
		CBox		*pBox;
		CSphere		*pSphere;
		CCylinder	*pCylinder;
		CTarget		*pTarget;
	//};

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
};


struct CEmission
{
	float	emitStart;
	float	emitStop;
	int		maxParticles;
	float	displayUntil;
	float	birthRate;

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
};

struct CRandomization
{
	int		seed;

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
};

struct CParticleLife
{
	float	life;
	float	lifeVariance;

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
};

struct CParticleSize
{
	float	size;
	float	sizeVariance;
	float	sizeGrowFor;
	float	sizeFadeFor;

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
};



struct CVariableVector
{
	core::vector3df	vector;
	float			variance;

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
};

struct CDirectionTarget
{
	std::string		url;
	float			variance;

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
};

struct CDirection
{
	CDirection() : pVariableVector(0), pDirectionTarget(0) {}
	~CDirection() 
	{
		if(pVariableVector) delete pVariableVector;
		if(pDirectionTarget) delete pDirectionTarget;
	};

	enum Type
	{
		DT_RANDOM_VECTOR = 0,
		DT_VARIABLE_VECTOR = 1,
		DT_DIRECTION_TARGET = 2,
	};
	int			type;
	//union
	//{
		CVariableVector		*pVariableVector;
		CDirectionTarget	*pDirectionTarget;
	//};

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
};

struct CParticleMotion
{
	float		speed;
	float		speedVariance;
	CDirection	direction;

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
};

struct CStandardParticle
{
	enum Type
	{
		SPT_TRIANGLE = 0,
		SPT_CUBE = 1,
		SPT_SPECIAL = 2,
		SPT_FACING = 3,
		SPT_CONSTANT = 4,
		SPT_TETRAHEDRON = 5,
		SPT_SIX_POINTED_STAR = 6,
		SPR_SPHERE = 7,
	};

	int	type;

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
};

struct CMetaParticle
{
	void *to_be_determined;

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
};

struct CInstancedGeometryParticle
{
	void *to_be_determined;

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
};

struct CParticleType
{
	CParticleType() : pStandardParticle(0), pMetaParticle(0), pInstancedGeometryParticle(0) {}
	~CParticleType() 
	{
		if(pStandardParticle) 
			delete pStandardParticle;
		if(pMetaParticle)
			delete pMetaParticle;
		if(pInstancedGeometryParticle)
			delete pInstancedGeometryParticle;
	}
	enum Type
	{
		PT_STANDARD_PARTICULE = 0,
		PT_META_PARTICULE = 1,
		PT_INSTANCED_GEOMETRY_PARTICULE = 2,
	};

	int			type;
	//union
	//{
		CStandardParticle			*pStandardParticle;
		CMetaParticle				*pMetaParticle;
		CInstancedGeometryParticle	*pInstancedGeometryParticle;
	//};

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
};


struct CTravelOrMBlurDir
{
	float stretch;

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
};

struct CAxis
{
	CAxis() : pVariableVector(0), pTravelOrMBlurDir(0) {}
	~CAxis() 
	{
		if(pVariableVector) delete pVariableVector;
		if(pTravelOrMBlurDir) delete pTravelOrMBlurDir;
	}
	enum Type
	{
		AT_RANDOM_VECTOR = 0,
		AT_VARIABLE_VECTOR = 1,
		AT_TRAVEL_OR_MBLUR_DIR = 2,
	};
	int			type;
	//union
	//{
		CVariableVector		*pVariableVector;
		CTravelOrMBlurDir	*pTravelOrMBlurDir;
	//};

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
};

struct CParticleSpin
{
	float				time;
	float				timeVariance;
	float				phase;
	float				phaseVariance;
	CAxis				axis;

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
};

struct CParticleAnimationOffset
{
	enum E_METHOD
	{
		EM_NONE,
		EM_BIRTH,
	};
	E_METHOD			method;
	float				offset;
	float				offsetVariance;

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
};

struct CParticleAnimationScaling
{
	enum E_METHOD
	{
		EM_NONE,
		EM_LIFE,
	};
	E_METHOD			method;
	float				srcLength;
	float				srcVariance;
	float				dstMultiplier;
	float				dstVariance;

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
};

struct CEmitter
{
	std::string					id;
	std::string					name;
	CShape						shape;
	CEmission					emission;
	CRandomization				randomization;
	CParticleLife				particleLife;
	CParticleSize				particleSize;
	CParticleMotion				particleMotion;
	CParticleType				particleType;
	CParticleAnimationOffset	particleAnimationOffset;
	CParticleAnimationScaling	particleAnimationScaling;
	CParticleSpin				particleSpin;

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
};



struct CLibraryEmitters
{
	~CLibraryEmitters();

	std::vector<CEmitter *> emitters;

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
};

struct CWind
{
	float				strength;
	float				decay;
	int					type;
	float				turbulence;
	float				frequency;
	float				scale;

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
};

struct CGravity
{
	float				strength;
	float				decay;
	int					type;

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
};

struct CDeflector
{
	float				bounce;
	float				variation;
	float				chaos;
	float				friction;
	float				inheritVelocity;
	float				width;
	float				height;

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
};

class CForce
{
public:
	enum Type
	{
		FT_GRAVITY = 0,
		FT_WIND = 1,
		FT_DEFLECTOR = 2,
	};

	CForce();
	~CForce();

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);

private:
	CForce(const CForce &cpy);


	std::string			id;
	std::string			name;
	int					type;
	union
	{
		CWind				*pWind;
		CGravity			*pGravity;
		CDeflector			*pDeflector;
	};	
};

class CLibraryForces
{
public:
	~CLibraryForces();

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);

	std::vector<CForce *> forces;
};



};
};
};
};

#endif