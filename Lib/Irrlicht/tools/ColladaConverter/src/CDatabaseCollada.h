#ifndef __CDATABASECOLLADA_H__
#define __CDATABASECOLLADA_H__

#include "irrString.h"
#include "irrArray.h"
#include "matrix4.h"
#include "IXMLReader.h"
#include "resFileWriter.h"
#include "aabbox3d.h"
#include "CColladaParticleSystem.h"



void ChangeExtension(std::string &url);




namespace irr
{
namespace collada
{
namespace converter
{



using namespace particle_system;

struct COffsetPtr
{
	Chunk *pChunk;
	int offset;
};

struct CCollada;
struct CInput;
struct CSource;

struct CImage;
class CInstance;
class CInstanceMaterial;
class CInstanceAnimation;
class CInstanceGeometry;
class CInstanceController;
struct CSource;

struct CColor
	: public COffsetPtr
{
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;
	unsigned char	a;

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);
};

class CInstance
	: public COffsetPtr
{
public:
	enum InstanceType
	{
		ITAnimation,
		ITCamera,
		ITController,
		ITGeometry,
		ITLight,
		ITNode,
		ITVisualScene,
		ITPhysicsScene,
		ITMaterial,
		ITEmitter,
		ITForce,
		ITModularSkin,
	};

	InstanceType		type;

	

	// Geometry
	std::list<CInstanceMaterial *>	m_Material;


	virtual void Read(io::IXMLReaderUTF8 *reader) = 0;
	virtual void Write(Chunk &chk) = 0;
	virtual void ResolveURL(CCollada &) {};
};

class CInstanceAnimation
	: public CInstance
{
public:
	std::string	url;

	virtual void Read(io::IXMLReaderUTF8 *reader);
	virtual void Write(Chunk &chk);
};

struct CBindMaterial
{
	std::list<CInstanceMaterial *> instanceMaterials;

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);

	void ResolveURL(CCollada &);
};

struct CInstanceForce
	: public CInstance
{
	std::string				filename;
	std::string				url;
	std::string				sid;
	//SForce					*pForce;

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
};

struct CInstanceEmitter
	: public CInstance
{
	int							type;
	std::string					filename;
	std::string					url;
	//CEmitter					*pEmitter;
	CBindMaterial				bindMaterial;
	std::vector<std::string>	forceNodes;
	int							drawLayer;

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);
	void ResolveURL(CCollada &data);
};

class CInstanceLight
	: public CInstance
{
public:
	std::string				url;
	int						iLight;

	virtual void Read(io::IXMLReaderUTF8 *reader);
	virtual void Write(Chunk &chk);
	virtual void ResolveURL(CCollada &);
};

class CInstanceGeometry
	: public CInstance
{
public:
	CInstanceGeometry() : drawLayer(0) {}
	std::string	url;
	CBindMaterial bindMaterial;
	int			iGeometry;
	int			drawLayer;

	virtual void Read(io::IXMLReaderUTF8 *reader);
	virtual void Write(Chunk &chk);

	virtual void ResolveURL(CCollada &);
};


class CInstanceCamera
	: public CInstance
{
public:
	std::string				url;
	int						iCamera;

	virtual void Read(io::IXMLReaderUTF8 *reader);
	virtual void Write(Chunk &chk);
	virtual void ResolveURL(CCollada &);
};

class CInstanceController
	: public CInstance
{
public:
	std::string				url;
	CBindMaterial			bindMaterial;
	int						drawLayer;
	int						iGeometry;
	std::list<std::string>	skeletons;

	virtual void Read(io::IXMLReaderUTF8 *reader);
	virtual void Write(Chunk &chk);
	virtual void ResolveURL(CCollada &);
};

class CCategory
{
public:
	std::string						selectedSkin;
	std::string						name;
	std::list<CInstanceController *>	modules;

	virtual void Read(io::IXMLReaderUTF8 *reader);
	virtual void Write(Chunk &chk);
	virtual void ResolveURL(CCollada &);
};

class CInstanceModularSkin
	: public CInstanceController
{
public:

	std::list<CCategory *>				categories;
	std::list<CInstanceController *>	orphanModules;

	virtual void Read(io::IXMLReaderUTF8 *reader);
	virtual void Write(Chunk &chk);
	virtual void ResolveURL(CCollada &);
};

class CInstanceMaterial
	: public CInstance
{
public:
	CInstanceMaterial() : iMaterial(0), iEffectComponent(0), iGeometryComponent(0) {}
	std::string	symbol;
	std::string				url;

	int						iMaterial;
	int						iEffectComponent;
	int						iGeometryComponent;

	virtual void Read(io::IXMLReaderUTF8 *reader);
	virtual void Write(Chunk &chk);

	void ResolveURL(CCollada &);
};

class CInstanceVisualScene
	: public CInstance
{
public:
	std::string	url;

	virtual void Read(io::IXMLReaderUTF8 *reader);
	virtual void Write(Chunk &chk);
};

struct CSetParam
{
	std::string	ref;
	void * value;

	void Read(io::IXMLReaderUTF8 *reader) {};
	void Write(Chunk &chk) {};
};

class CInstanceEffect
	: public CInstance
{
public:
	CInstanceEffect() : iEffect(0) {};
	std::string	url;
	std::vector<CSetParam> setParams;
	int iEffect;

	virtual void Read(io::IXMLReaderUTF8 *reader);
	virtual void Write(Chunk &chk);

	void ResolveURL(CCollada &data);
};

struct CCamera
	: public COffsetPtr
{
	std::string		id;
	int				orthographic;
	float			xfov;
	float			aspectRatio;
	float			znear;
	float			zfar;
	std::string		target;

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);
};

struct CLibraryCameras
	: public COffsetPtr
{
	std::list<CCamera>		cameras;

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);
};

struct CSurface
{
	CSurface() : pImage(0), iImage(0) {}

	std::string		sid;
	std::string		initFrom; //image
	CImage *		pImage;
	int				iImage;

	std::string		format;

	void Read(io::IXMLReaderUTF8 *reader);
	void ResolveURL(CCollada &data);
	void Write(Chunk &chk);
};

struct CMaterial
	: public COffsetPtr
{
	CMaterial()
		: additive(0)
		, diffuseSurfaceIndex(-1)
		, envmapSurfaceIndex(-1)
		, envmapIntensity(0)
		, backfaceCulling(1)
		, frontfaceCulling(0)
		, renderSet(-1)
		, renderPriority(128)
		, useIrrlichtMaterial(0)
	{}

	std::string				id;
	std::string				name;
	CInstanceEffect			instanceEffect;

	enum E_ParamID
	{
		DIFFUSE_TEXTURE = 0,
		DIFFUSE_COLOR = 1,
		ENVMAP_TEXTURE = 2,
		ENVMAP_INTENSITY = 3,
		ADDITIVE = 4,
	};

	enum E_RenderSet
	{
		SOLID = 0,
		TRANSLUCENT = 1,
		TRANSPARENT = 2
	};

	int			additive;
	CColor		diffuseColor;

	int			renderSet;
	int			renderPriority;
	int			backfaceCulling;
	int			frontfaceCulling;
	int			useIrrlichtMaterial;

	CSurface	diffuseSurface;
	int			diffuseSurfaceIndex;

	CSurface	envmapSurface;
	int			envmapSurfaceIndex;

	float		envmapIntensity;

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);
	void ResolveURL(CCollada &data);
};

struct CLibraryMaterials
	: public COffsetPtr
{
	std::list<CMaterial>	materials;

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);

	void ResolveURL(CCollada &data);
};



/*struct CSurface
{
	CSurface() : pImage(0), iImage(0) {}

	std::string		sid;
	std::string		initFrom; //image
	CImage *		pImage;
	int				iImage;

	std::string		format;

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);
};*/

struct CSampler2D
{
	CSampler2D() : pSurface(0) {}

	std::string		sid;
	std::string		source; //surface
	CSurface		*pSurface;
	int				iSurface;
	std::string		wrap_s;
	std::string		wrap_t;
	std::string		minfilter;
	std::string		magfilter;
	std::string		mipfilter;

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);
};

struct CTexture
{
	std::string		texture; //sampler
	CSampler2D		*pSampler;
	int				iSampler;

	std::string		texcoord;

	int mirrorU;
	int mirrorV;
	int wrapU;
	int wrapV;
	float repeatU;
	float repeatV;
	float offsetU;
	float offsetV;
	float rotateUV;

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);

	void ResolveSampler(std::list<CSampler2D> &samplers);
};

struct CCommonColorOrTexture
	: public COffsetPtr
{
	enum Type
	{
		ECOLOR = 0,
		ETEXTURE = 1,
	};

	CCommonColorOrTexture() : eType(ECOLOR) {}

	u8   eType;
	CColor color;
	
	std::vector<CTexture> textures;
	int mirrorU;
	int mirrorV;
	int wrapU;
	int wrapV;
	float repeatU;
	float repeatV;
	float offsetU;
	float offsetV;
	float rotateUV;
	void Read(io::IXMLReaderUTF8 *reader, const core::stringc &endElement);
	void Write(Chunk &chk);
};

struct CNewParam
{
	enum Type
	{
		ESAMPLER2D = 0,
		ESURFACE = 1,
	};

	std::string		sid;
	u8				eType;
	CSampler2D		sampler;
	CSurface		surface;
	void Read(io::IXMLReaderUTF8 *reader);
};

struct CEffect
	: public COffsetPtr
{
	CEffect() 
		: specLevel(0)
		, transparency(1.0)
		, reflectivity(0)
		, shininess(0)
		, doubleSided(0)
	{
	}

	std::string		id;
	std::string		name;

	CCommonColorOrTexture	ambient;
	CCommonColorOrTexture	diffuse;
	CCommonColorOrTexture	specular;
	float					shininess;
	CCommonColorOrTexture	reflective;
	float					reflectivity;
	CCommonColorOrTexture	transparent;
	float					transparency;
	float					specLevel;
	CCommonColorOrTexture	emission;
	int						doubleSided;

	std::list<CSampler2D>	samplers;
	std::list<CSurface>		surfaces;

	void Read(io::IXMLReaderUTF8 *reader);
	void ReadColor(io::IXMLReaderUTF8 *reader, CColor &color, const core::stringc &endNodeName);
	void Write(Chunk &chk);

	void ResolveURL(CCollada &data);
};

struct CLibraryEffects
	: public COffsetPtr
{
	std::list<CEffect>	effects;

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);

	void ResolveURL(CCollada &);
};





struct CLight
	: public COffsetPtr
{
	CLight() : intensity(1.0f) {}
	std::string			id;
	std::string			name;

	enum Type
	{
		LT_AMBIENT,
		LT_POINT,
		LT_SPOT,
		LT_DIRECTIONNAL,
	};

	int					type;

	// Point
	CColor				color;
	float				intensity;

	f32					constantAttenuation;
	f32					linearAttenuation;
	f32					quadraticAttenuation;

	// Spot
	f32					falloffAngle;
	f32					falloffExponent;

	// Directional
	

	void Read(io::IXMLReaderUTF8 *reader);
	void ReadAmbient(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);
};

struct CLibraryLights
	: public COffsetPtr
{
	std::list<CLight> lights;

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);
};

struct CPosition
{
	CPosition() : x(0.0), y(0.0), z(0.0) {};
	f32 x, y, z;
	//void Write(Chunk &chk);
};

struct CQuaternion
{
	CQuaternion() : x(0.0), y(0.0), z(0.0), w(1.0) {};
	f32 x, y, z, w;
	//void Write(Chunk &chk);
};

struct CScale
{
	CScale() : x(1.0), y(1.0), z(1.0) {};
	f32 x, y, z;
	//void Write(Chunk &chk);
};

struct CTransform
{
	CPosition		position;
	CQuaternion		rotation;
	CScale			scale;

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);
};

struct CAnimationClip
	: public COffsetPtr
{
	std::string				id;
	f32						start;
	f32						end;
	std::list<CInstanceAnimation *>	instanceAnimations;

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);
};

struct CLibraryAnimationClips
	: public COffsetPtr
{
	std::list<CAnimationClip>	animationClips;

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);
};

struct CChannel
	: public COffsetPtr
{
	CChannel() : reserved0(-1), reserved1(-1) {}

	enum Type
	{
		CT_TRANSFORM,
		CT_POSITION,
		CT_POSITION_X,
		CT_POSITION_Y,
		CT_POSITION_Z,
		CT_ROTATION,
		CT_ROTATION_X,
		CT_ROTATION_Y,
		CT_ROTATION_Z,
		CT_ROTATION_W,
		CT_SCALE,
		CT_SCALE_X,
		CT_SCALE_Y,
		CT_SCALE_Z,
		CT_MORPHING_WEIGHT,
		CT_IFL,
		CT_LIGHT_COLOR,
		CT_LIGHT_CONSTANT_ATTENUATION,
		CT_LIGHT_LINEAR_ATTENUATION,
		CT_LIGHT_QUADRATIC_ATTENUATION,
		CT_VISIBILITY,
		CT_EFFECT_DIFFUSE,
		CT_EFFECT_DIFFUSE_R,
		CT_EFFECT_DIFFUSE_G,
		CT_EFFECT_DIFFUSE_B,
		CT_EFFECT_DIFFUSE_A,
		CT_EFFECT_TRANSPARENCY,
		CT_EVENTS,
		CT_EMITTER_BIRTH_RATE,
		CT_EMITTER_PARTICLE_SIZE,
		CT_EMITTER_PARTICLE_SIZE_VARIANCE,
		CT_EMITTER_PARTICLE_SPEED,
		CT_EMITTER_PARTICLE_SPEED_VARIANCE,
		CT_EMITTER_PARTICLE_DIRECTION,
		CT_EMITTER_PARTICLE_DIRECTION_VARIANCE,
		CT_EMITTER_PARTICLE_SPIN_TIME,
		CT_EMITTER_PARTICLE_SPIN_TIME_VARIANCE,
		CT_EMITTER_PARTICLE_SPIN_PHASE,
		CT_EMITTER_PARTICLE_SPIN_PHASE_VARIANCE,
		CT_EMITTER_PARTICLE_SPIN_AXIS,
		CT_EMITTER_PARTICLE_SPIN_AXIS_VARIANCE,
		CT_EMITTER_PARTICLE_ANIMATION_OFFSET,
		CT_EMITTER_PARTICLE_ANIMATION_OFFSET_VARIANCE,
		CT_EMITTER_PARTICLE_ANIMATION_SCALING_SRC_LENGTH,
		CT_EMITTER_PARTICLE_ANIMATION_SCALING_SRC_VARIANCE,
		CT_EMITTER_PARTICLE_ANIMATION_SCALING_DST_MULTIPLIER,
		CT_EMITTER_PARTICLE_ANIMATION_SCALING_DST_VARIANCE,
		CT_EFFECT_AMBIENT,
		CT_EFFECT_AMBIENT_R,
		CT_EFFECT_AMBIENT_G,
		CT_EFFECT_AMBIENT_B,
		CT_EFFECT_AMBIENT_A,
		CT_EFFECT_SPECULAR,
		CT_EFFECT_SPECULAR_R,
		CT_EFFECT_SPECULAR_G,
		CT_EFFECT_SPECULAR_B,
		CT_EFFECT_SPECULAR_A,
		CT_EFFECT_EMISSION,
		CT_EFFECT_EMISSION_R,
		CT_EFFECT_EMISSION_G,
		CT_EFFECT_EMISSION_B,
		CT_EFFECT_EMISSION_A,
		CT_EFFECT_TRANSFORM = 0x100,
		CT_EFFECT_TRANSFORM_MASK = 0xFF,
		CT_EFFECT_TRANSFORM_OFFSET_U = CT_EFFECT_TRANSFORM + 1,
		CT_EFFECT_TRANSFORM_OFFSET_V = CT_EFFECT_TRANSFORM + 2,
		CT_EFFECT_TRANSFORM_ROTATE = CT_EFFECT_TRANSFORM + 3,
		CT_EFFECT_TRANSFORM_SCALE_U = CT_EFFECT_TRANSFORM + 4,
		CT_EFFECT_TRANSFORM_SCALE_V = CT_EFFECT_TRANSFORM + 5,
	};

	std::string				source;
	std::string				target;
	int						type;
	unsigned char			reserved0;
	unsigned char			reserved1;
	unsigned char			reserved2;
	unsigned char			reserved3;

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);
};

struct CSampler
	: public COffsetPtr
{
	enum EInterpolationType
	{
		IT_STEP = 0,
		IT_LINEAR = 1,
		IT_HERMITE = 2,
		IT_UNKNOWN = -1,
	};
	std::string					id;
	std::list<CInput>			inputs;
	std::list<unsigned char>	inputsId;

	int iInput;
	int iOutput;
	int iInterpolationType;
	int iInputTangeant;
	int iOutputTangeant;

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);
	void ResolveURL(std::vector<CSource *> &pSources);
};

struct CAnimation
	: public COffsetPtr
{
	std::string				id;
	enum Type
	{
		AT_MATRIX,
		AT_POSITION,
	};
	std::vector<CSource *>	pSources;
	std::list<CSampler>		samplers;
	std::list<CChannel>		channels;

	struct DefaultValue
	{
		std::string name;
		std::string value;
	};

	std::list<DefaultValue>		defaultValues;

	//TextureTransformDefaultValue
	float DefaultOffsetU;
	float DefaultOffsetV;
	float DefaultRotate;
	float DefaultScaleU;
	float DefaultScaleV;

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);
	void WriteAsEvent(Chunk &chk);
	void ResolveURL(CCollada &data);
	bool MergeEffectTransform(CCollada &data);
	void ReadDefaultValues(io::IXMLReaderUTF8 *reader);
	CAnimation &operator +=(CAnimation &other);
};

struct CLibraryAnimations
	: public COffsetPtr
{
	CLibraryAnimations() : writeEvents(false) {}

	std::list<CAnimation> animations;

	void setEvents(CAnimation &newEvents)
	{
		writeEvents = true;
		events = newEvents;
	}

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);
	void ResolveURL(CCollada &data);

protected:
	CAnimation events;
	bool writeEvents;	
};

struct CMeshBuffer
	: public COffsetPtr
{
	enum E_BufferType
	{
		EBT_Triangles = 0,
		EBT_TriangleStrips = 1,
		EBT_Lines = 2,
		EBT_LineStrips = 3,
		EBT_LineLoop = 4,
	};

	CMeshBuffer() : positionSource(-1), normalSource(-1), binormalSource(-1), tangentSource(-1)
		, texcoord0Source(-1), texbinormal0Source(-1), textangent0Source(-1), texcoord1Source(-1)
		, texbinormal1Source(-1), textangent1Source(-1), color0Source(-1), color1Source(-1)
	{
	}

	E_BufferType			type;
	std::string			material;
	int					count;

	std::list<CInput>	inputs;

	char				positionSource;		//-1 if not used

	char				normalSource;		//-1 if not used
	char				binormalSource;		//-1 if not used
	char				tangentSource;		//-1 if not used

	char				texcoord0Source;	//-1 if not used
	char				texbinormal0Source;	//-1 if not used
	char				textangent0Source;	//-1 if not used

	char				texcoord1Source;	//-1 if not used
	char				texbinormal1Source;	//-1 if not used
	char				textangent1Source;	//-1 if not used

	char				color0Source;		//-1 if not used
	char				color1Source;		//-1 if not used

	std::vector<unsigned short>	indices;

	core::aabbox3df		boundingBox;

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);

	void ResolveSource(int idx, std::string semantic, int set, CSource &source);
	void ComputeBoundingBox(std::vector<CSource *> &sources);
};



struct CInput
	: public COffsetPtr
{
	std::string				semantic;
	std::string				source;
	int						offset;
	int						set;

	CInput() : offset(-1), set(-1) {}
	void Read(io::IXMLReaderUTF8 *reader);
};

struct CVertices
	: public COffsetPtr
{
	std::string				id;
	std::list<CInput>		inputs;

	void Read(io::IXMLReaderUTF8 *reader);
};

struct CMesh
{
	enum VertexComponents
	{
		VT_POSITION = 1,
		VT_NORMAL = 2,
		VT_COLOR = 4,
		VT_TEXCOOR = 8
	};

	int							vertexComponentsFlags;
	int							vertexStride;
	
	std::vector<CSource *>		pSources;
	CVertices					vertices;

	int							meshBufferCount;
	std::list<CMeshBuffer>		meshBuffers;
	irr::core::aabbox3df		boundingBox;


	// for interleaving
	int							stride;
	std::vector<int>			offsets; 
	std::vector<int>			types; 
	struct ScaleAndOffset
	{
		core::vector3df scale;
		core::vector3df offset;
	};
	std::vector<bool>			hasScaleAndOffset;
	std::vector<ScaleAndOffset>	scaleAndOffset;

	CMesh() : vertexComponentsFlags(0), vertexStride(0) {}
	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);
	void WriteSources(Chunk &chk, bool interleved);

	void ResolveMeshBufferSource();
};

struct CSpline
{
	CSpline() : pPositions(0), closed(0) {}
	
	int					closed;
	CSource				*pPositions;

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);
};

struct CGeometry
	: public COffsetPtr
{
	enum E_Type
	{
		EGT_MESH = 0,
		EGT_SPLINE = 1,
	};

	std::string			id;
	std::string			name;

	int					type;
	CMesh				mesh;
	CSpline				spline;


	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);
};

struct CLibraryGeometries
	: public COffsetPtr
{
	std::list<CGeometry>	pGeometry;

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);
};


struct CMorph
{
	u32						method;
	std::string				source;
	union
	{
		int					iSource;
		CGeometry			*pSource;
	};

	std::vector<std::string*>	targets;

	std::vector<int>			iTargets;
	std::vector<CGeometry*>		pTargets;
	
	std::vector<float>			weights;

	void Read(io::IXMLReaderUTF8 *reader);
	void ResolveURL(CCollada &data);
	void Write(Chunk &chk);
};

struct CVertexWeights
{
	int count;
	std::list<CInput>	inputs;
	std::vector<unsigned char>	vcount; // number of influence per vertex
	std::vector<unsigned short>	v;		// {[jointId, weight]}

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);
};

struct CJointsBoundingBoxes
{
	CJointsBoundingBoxes() : minSource(0), maxSource(0) {}
	/*CJointsBoundingBoxes(const CJointsBoundingBoxes &cpy);
	~CJointsBoundingBoxes() 
	{ 
		if(minSource) 
			delete minSource;
		minSource = 0;
		if(maxSource) 
			delete maxSource;
		maxSource = 0;
	}*/

	CSource *minSource;
	CSource *maxSource;

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);
};

struct CSkin
{
	std::string				source;

	int						iSource;
	CGeometry				*pSource;
	
	core::matrix4				bindShapeMatrix;

	std::vector<std::string *>	nodes;
	std::vector<float>			bindPoses;
	std::vector<float>			weights;	// weight table
	CVertexWeights				vertexWeights;	

	std::vector<CSource *>		pSources;
	std::list<CInput>			inputs;

	CJointsBoundingBoxes		jointsBoundingBoxes;

	void Read(io::IXMLReaderUTF8 *reader);
	void ReadSource(io::IXMLReaderUTF8 *reader);
	void ReadJoints(io::IXMLReaderUTF8 *reader);
	void ResolveURL(CCollada &data);
	void Write(Chunk &chk);

	void TransposeMatrixVector();
};

struct CController
{
	enum Type
	{
		ESKIN = 0,
		EMORPH = 1,
	};

	std::string				id;
	int						type;
	
	CMorph				morph;
	CSkin				skin;

	void ResolveURL(CCollada &data);
	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);
};

struct CLibraryControllers
{
	std::list<CController>		controllers;

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);
	void ResolveURL(CCollada &data);
};

struct CBillboard
{
	CBillboard() : type(EBT_NONE), orientation(EBO_NONE), front( 0.f, 0.f, 1.f ), up( 0.f, 1.f, 0.f ) {}

	// Billboard data
	enum E_Billboard_Type
	{
		EBT_NONE,
		EBT_FREE,
		EBT_UP_ONLY,
	};

	enum E_Billboard_Orientation
	{
		EBO_NONE,
		EBO_TOWARD_CAMERA,
		EBT_PARALLEL_TO_CAMERA_AXIS,
	};

	int						type;
	int						orientation;
	core::vector3df			front;
	core::vector3df			up;

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);
};

struct CNode
	: public COffsetPtr
{
	CNode() : drawLayer(0), visibility(1) {}

	std::string				id;	
	std::string				name;
	std::string				sid;

	CScale					scale;
	CQuaternion				rotation;
	CPosition				position;

	float					visibility;

	core::matrix4			transform;
	std::list<CNode>		children;
	std::list<CInstance *>	instances;

	std::string				userProperties;

	int						drawLayer;

	CBillboard				billboard;

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);

	void ResolveURL(CCollada &);
};

struct CVisualScene
	: public COffsetPtr
{
	std::string				id;
	std::string				name;
	std::list<CNode>		nodes;

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);

	void ResolveURL(CCollada &);
};

struct CLibraryVisualScenes
	: public COffsetPtr
{
	std::list<CVisualScene> visualScenes;

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);

	void ResolveURL(CCollada &);
};



struct CScene
	: public COffsetPtr
{
	std::list<CInstance *>	intances;

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);
};

struct CImage
	: public COffsetPtr
{
	enum
	{
		CIT_SINGLE = 0,
		CIT_IFL = 1,
	};
	std::string		id;
	std::string		name;
	std::string		path;
	int				type;

	// IFL support
	float						m_iIflTotalTime;
	std::list<std::string>		fileList;
	std::vector<int>			fileFrameCount;
	std::vector<int>			texturePtrs;
	CAnimation					iflAnimation;

	void Read(io::IXMLReaderUTF8 *reader);
	void ReadInitFrom(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);
	void ReadIFL();
	void WriteIFL(Chunk &chk);
};

struct CLibraryImages
	: public COffsetPtr
{
	std::list<CImage> images;

	void Read(io::IXMLReaderUTF8 *reader);
	void Write(Chunk &chk);
};

struct CConfig
{
	enum E_UpVector
	{
		X_UP = 0,
		Y_UP = 1,
		Z_UP = 2,
	};
	int upVector;
	int iSamplingRate;

	void Write(Chunk &chk);
};

struct CCollada
	: public COffsetPtr
{
	CConfig					configuration;
	CLibraryAnimationClips	libraryAnimationClips;
	CLibraryAnimations		libraryAnimations;
	CLibraryCameras			libraryCameras;
	CLibraryLights			libraryLights;
	CLibraryImages			libraryImages;
	CLibraryEffects			libraryEffects;
	CLibraryMaterials		libraryMaterials;
	CLibraryGeometries		libraryGeometries;
	CLibraryControllers		libraryControllers;
	CLibraryEmitters		libraryEmitters;
	CLibraryForces			libraryForces;
	CLibraryVisualScenes	libraryVisualScene;
	CScene					scene;

	void Read(irr::io::IXMLReaderUTF8* reader);
	void Write(Chunk &chk);

	void ResolveURL();
	void ReadComment(io::IXMLReaderUTF8 *reader);
};

};
};
};



#endif