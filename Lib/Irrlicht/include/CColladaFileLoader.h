// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_COLLADA_MESH_FILE_LOADER_H_INCLUDED__
#define __C_COLLADA_MESH_FILE_LOADER_H_INCLUDED__

#include "ISceneNode.h"
#include "IMeshLoader.h"
#include "IFileSystem.h"
#include "IVideoDriver.h"
#include "irrString.h"
#include "SMesh.h"
#include "SAnimatedMesh.h"
#include "SMeshBuffer.h"
#include "ISceneManager.h"
#include "irrMap.h"
#include "CAttributes.h"
#include "CSceneNodeAnimatorChannelLibrary.h"
#include "SLibraryAnimationClips.h"

namespace irr
{
namespace scene
{

	
#ifdef _DEBUG
//#define COLLADA_READER_DEBUG
#endif

class IColladaPrefab;
class CCameraPrefab;

enum ECOLLADA_PARAM_NAME
{
	ECPN_COLOR = 0,
	ECPN_AMBIENT,
	ECPN_DIFFUSE,
	ECPN_SPECULAR,
	ECPN_SHININESS,
	ECPN_TRANSPARENCY,
	ECPN_YFOV,
	ECPN_ZNEAR,
	ECPN_ZFAR,
	ECPN_MORPH_TARGET,
	ECPN_MORPH_WEIGHT,
	ECPN_COUNT
};

enum ECOLLADA_PARAM_TYPE
{
	ECPT_FLOAT = 0,
	ECPT_FLOAT2,
	ECPT_FLOAT3,
	ECPT_FLOAT4,
	ECPT_IDREF,
	ECPT_COUNT
};

//! Collada Parameter
struct SColladaParam
{
	SColladaParam()
		: Name(ECPN_COUNT), Type(ECPT_COUNT)
	{
		for (int i=0; i<4; ++i) Floats[i] = 0;
	}

	ECOLLADA_PARAM_NAME Name;
	ECOLLADA_PARAM_TYPE Type;

	f32 Floats[4];
};

enum ECOLLADA_INPUT_SEMANTIC
{
	ECIS_POSITION = 0,
	ECIS_VERTEX,
	ECIS_NORMAL,
	ECIS_TEXCOORD,
	ECIS_UV,
	ECIS_TANGENT,
	ECIS_IMAGE,
	ECIS_TEXTURE,
	ECIS_COLOR,

	ECIS_COUNT
};

//! Collada Input
struct SColladaInput
{
	SColladaInput()
		: Semantic(ECIS_COUNT), Data(0), Offset(0), Set(0), Stride(1)
	{
	}

	ECOLLADA_INPUT_SEMANTIC Semantic;
	core::stringc Source;
	f32* Data;
	u32 Offset;
	u32 Set;
	u32 Stride;
};

//! Collada images
struct SColladaImage
{
	core::stringc Id;
	core::stringc Source;
	core::dimension2di Dimension;
	bool SourceIsFilename;
};


//! Collada texture
struct SColladaTexture
{
	video::ITexture* Texture;
	core::stringc Id;
};


//! Collada material
struct SColladaMaterial
{
	video::SMaterial Mat;
	core::stringc Id;
	core::stringc InstanceEffectId;
	f32 Transparency;
	bool Additive;
	
	inline bool operator< (const SColladaMaterial & other) const
	{
		return Id < other.Id;
	}
};

//! Collada effect (materials, shaders, and programs)
struct SColladaEffect
{
	video::SMaterial Mat;
	core::stringc Id;
	f32 Transparency;
	
	inline bool operator< (const SColladaEffect & other) const
	{
		return Id < other.Id;
	}
};


class ISourceArray
{
public:
	enum EIS_TYPE
	{
		NAME,
		IDREF,
		INT,
		FLOAT,
		COUNT
	};
	ISourceArray(EIS_TYPE t) : type(t) {}
	core::stringc Name;
	EIS_TYPE type;
};

class CPrefab;

class SNameArray : public ISourceArray // for storing IDRef arrays
{
public:
	SNameArray() : ISourceArray(ISourceArray::NAME) {};
	core::array<core::stringc> Data;
	core::array<IColladaPrefab *> Prefabs;
};

class SIDRefArray : public ISourceArray // for storing IDRef arrays
{
public:
	SIDRefArray() : ISourceArray(ISourceArray::IDREF) {};
	core::array<core::stringc> Data;
	core::array<IColladaPrefab *> Prefabs;
};

class SIntArray : public ISourceArray // for storing int arrays
{
public:
	SIntArray() : ISourceArray(ISourceArray::INT) {};
	core::array<int> Data;
};

class SFloatArray : public ISourceArray // for storing float arrays
{
public:
	SFloatArray() : ISourceArray(ISourceArray::FLOAT) {};
	core::array<float> Data;
};



struct SAccessor
{
	SAccessor()
		: Count(0), Offset(0), Stride(1) {}
	// I don't store the source of the accessor here because I assume
	// it to use the array of the source this accessor is located in.

	int		Count;
	int		Offset;
	int		Stride;

	core::array<SColladaParam> Parameters; // parameters defining the accessor
};

struct SInput
{
	core::stringc semantic;
	core::stringc source;
};

struct SInputs : public core::array<SInput>
{
	const core::stringc * getSourceURI(const core::stringc &semantic)
	{
		for(u32 i = 0; i < size(); i++)
		{
			if(operator [](i).semantic == semantic)
			{
				return &operator [](i).source;
			}
		}
		return 0;
	}
};

typedef SInputs SSampler;
typedef core::array<SInput> STargets;
typedef SInputs SJoints;


class SChannel
{
public:
	SChannel() {}
	core::stringc sourceURI;
	core::stringc targetURI;
};

class SSource
{
public:
	SSource() : Array(0) {}
	core::stringc Id;
	ISourceArray* Array;
	core::array<SAccessor> Accessors;
};

class SSources : public core::array<SSource>
{
public:
	SSource* getSource(const core::stringc &id)
	{
		for(u32 i = 0; i < size(); i++)
		{
			if(operator [](i).Id == id)
			{
				return &operator [](i);
			}
		}
		return 0;
	}
};


struct SMorph
{
	SMorph() : sourcePrefab(0) {};
	core::stringc method;
	core::stringc source;
	IColladaPrefab *sourcePrefab;
	SSources sources;
	STargets targets;
};

struct SVertexWeights
{
	u32					count;
	SInputs				inputs;
	core::array<u8>		vcount;
	core::array<u8>		v;
};

struct SSkin
{
	SSkin() : sourcePrefab(0) {};
	core::stringc			source;
	core::matrix4			bindShapeMatrix;
	SSources				sources;
	SJoints					joints;
	SVertexWeights			vertexWeights;
	IColladaPrefab *		sourcePrefab;
};

class CScenePrefab;

//! Meshloader capable of loading COLLADA meshes and scene descriptions into Irrlicht.
class CColladaFileLoader : public IMeshLoader
{
public:

	//! Constructor
	CColladaFileLoader(scene::ISceneManager* smgr, io::IFileSystem* fs);

	//! destructor
	virtual ~CColladaFileLoader();

	//! returns true if the file maybe is able to be loaded by this class
	//! based on the file extension (e.g. ".cob")
	virtual bool isALoadableFileExtension(const c8* fileName) const;

	//! creates/loads an animated mesh from the file.
	//! \return Pointer to the created mesh. Returns 0 if loading failed.
	//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
	//! See IReferenceCounted::drop() for more information.
	virtual IAnimatedMesh* createMesh(io::IReadFile* file);
	virtual IAnimatedMesh* createMesh(io::IReadFile* file, ISceneNode *pSceneRoot );
	virtual ISceneNode* createScene(io::IReadFile* file);

private:
	//! retrieve a prefab definition by its id
	IColladaPrefab *findPrefab(const core::stringc *id) const;

	//! skips an (unknown) section in the collada document
	void skipSection(io::IXMLReaderUTF8* reader, bool reportSkipping);

	//! reads the <COLLADA> section and its content
	void readColladaSection(io::IXMLReaderUTF8* reader, ISceneNode *pSceneRoot );

	//! reads a <library> section and its content
	void readLibrarySection(io::IXMLReaderUTF8* reader, ISceneNode *pSceneRoot );

	//! reads a <visual_scene> element and stores it as a prefab
	void readVisualSceneLibrary(io::IXMLReaderUTF8* reader, ISceneNode *pSceneRoot );

	//! reads a <scene> section and its content
	void readSceneSection(io::IXMLReaderUTF8* reader, ISceneNode *pSceneRoot );

	//! reads a <asset> section and its content
	void readAssetSection(io::IXMLReaderUTF8* reader);

	//! reads a <skeleton> section and its content
	void readSkeletonSection(io::IXMLReaderUTF8* reader, core::stringc &uri);

	//! reads a <instance_camera> section and its content
	void readInstanceCameraSection(io::IXMLReaderUTF8* reader, scene::ISceneNode* parent, CScenePrefab* p);

	//! reads a <instance_controller> section and its content
	void readInstanceControllerSection(io::IXMLReaderUTF8* reader, scene::ISceneNode* parent, CScenePrefab* p);

	//! reads a <node> section and its content
	//! if a prefab pointer is passed the nodes are created as scene prefabs childs of that prefab
	void readNodeSection(io::IXMLReaderUTF8* reader, scene::ISceneNode* parent, CScenePrefab* p=0);

	//! reads a <lookat> element and its content and creates a matrix from it
	core::matrix4 readLookAtNode(io::IXMLReaderUTF8* reader);

	//! reads a <matrix> element and its content and creates a matrix from it
	core::matrix4 readMatrixNode(io::IXMLReaderUTF8* reader);

	//! reads a <perspective> element and its content and creates a matrix from it
	core::matrix4 readPerspectiveNode(io::IXMLReaderUTF8* reader);

	//! reads a <rotate> element and its content and creates a matrix from it
	core::matrix4 readRotateNode(io::IXMLReaderUTF8* reader);

	//! reads a <skew> element and its content and creates a matrix from it
	core::matrix4 readSkewNode(io::IXMLReaderUTF8* reader);

	//! reads a <boundingbox> element and its content and stores it in bbox
	void readBboxNode(io::IXMLReaderUTF8* reader, core::aabbox3df& bbox);

	//! reads a <scale> element and its content and creates a matrix from it
	core::matrix4 readScaleNode(io::IXMLReaderUTF8* reader);

	//! reads a <translate> element and its content and creates a matrix from it
	core::matrix4 readTranslateNode(io::IXMLReaderUTF8* reader);

	//! reads a <color> element
	video::SColorf readColorNode(io::IXMLReaderUTF8* reader);

	//! reads a <float> element
	f32 readFloatNode(io::IXMLReaderUTF8* reader);

	//! reads a <instance> node and creates a scene node from it
	void readInstanceNode(io::IXMLReaderUTF8* reader, scene::ISceneNode* parent,
		scene::ISceneNode** outNode, CScenePrefab* p=0);

	//! reads a <light> element and stores it as prefab
	void readLightPrefab(io::IXMLReaderUTF8* reader);

	//! reads a <ambient> element and stores it as prefab
	void readAmbientLight(io::IXMLReaderUTF8* reader, video::SLight &outLight);

	//! reads a <point> element and stores it as prefab
	void readPointLight(io::IXMLReaderUTF8* reader, video::SLight &outLight);

	//! reads a <animation_clip> element and stores it as prefab
	void readAnimationClipPrefab(io::IXMLReaderUTF8* reader);

	//! reads a <animation> element and stores it as prefab
	void readAnimationPrefab(io::IXMLReaderUTF8* reader);

	//! reads a <camera> element and stores it as prefab
	void readCameraPrefab(io::IXMLReaderUTF8* reader);

	//! read a <target> element and stores it in a Camera Prefab
	void readCameraTarget(io::IXMLReaderUTF8* reader, CCameraPrefab* prefab);

	//! reads a camera <perspective> element and stores it in a Camera prefab
	void readCameraPerspective(io::IXMLReaderUTF8* reader, CCameraPrefab* prefab);

	//! reads a camera optics <technique_common> element and stores it in a Camera prefab
	void readCameraTechniqueCommon(io::IXMLReaderUTF8* reader, CCameraPrefab* prefab);

	//! read a <optics> element and stores it in a Camera prefab
	void readCameraOptics(io::IXMLReaderUTF8* reader, CCameraPrefab* prefab);

	//! reads a <image> element and stores it in the image section
	void readImage(io::IXMLReaderUTF8* reader);

	//! reads a <texture> element and stores it in the texture section
	void readTexture(io::IXMLReaderUTF8* reader);

	//! reads a <material> element and stores it in the material section
	void readMaterial(io::IXMLReaderUTF8* reader);

	//! reads a <effect> element and stores it in the effects section
	void readEffect(io::IXMLReaderUTF8* reader, SColladaEffect * effect = 0);

	//! reads a <float_array> element and stores it in outArray
	void readFloatArray(io::IXMLReaderUTF8* reader, ISourceArray **outArray);

	//! reads a float element from a string and stores it in outArray
	void readFloatArray(core::stringc &data, core::array<f32> &outArray, int iCount);

	//! reads a int element from a string and stores it in outArray
	void readIntArray(core::stringc &data, core::array<s32> &outArray, int iCount);

	//! reads a <int_array> element and stores it in outArray
	void readIntArray(io::IXMLReaderUTF8* reader, ISourceArray **outArray);

	//! reads a list of Name or ID element and store it in outArray
	void readStringArray(const char* data, core::array<core::stringc> &outArray);

	//! reads a NAME element from a string and stores it in outArray
	void readNameArray(io::IXMLReaderUTF8* reader, ISourceArray **array);

	//! reads a ID element from a string and stores it in outArray
	void readIdRefArray(core::stringc &data, core::array<core::stringc> &outArray, int iCount);

	//! reads a <targets> element and stores it in the effects section
	void readTargets(io::IXMLReaderUTF8* reader, STargets &target);

	//! reads a <source> element and stores it in the effects section
	void readSource(io::IXMLReaderUTF8* reader, SSource &source);

	//! reads a <sampler> element and stores it in the effects section
	void readSampler(io::IXMLReaderUTF8* reader, SSampler &sampler);

	//! reads a <joints> element and stores it in the effects section
	void readJoints(io::IXMLReaderUTF8* reader, SJoints &joints);

	//! reads a <vertex_weights> element and stores it in the effects section
	void readVertexWeights(io::IXMLReaderUTF8* reader, SVertexWeights& vertexWeight);

	//! reads a <accessor> element and stores it in the effects section
	void readAccessor(io::IXMLReaderUTF8* reader, SAccessor &accessor);

	//! reads a <idref_array> element
	void readIdRefArray(io::IXMLReaderUTF8* reader, ISourceArray **outArray);

	//! reads a <morph> element
	void readSkin(io::IXMLReaderUTF8* reader, void **outData);

	//! reads a <morph> element
	void readMorph(io::IXMLReaderUTF8* reader, void **outData);

	//! reads a <controller> element and stores it as mesh if possible
	void readController(io::IXMLReaderUTF8* reader);

	//! reads a <geometry> element and stores it as mesh if possible
	void readGeometry(io::IXMLReaderUTF8* reader);

	//! parses a float from a char pointer and moves the pointer to
	//! the end of the parsed float
	inline f32 readFloat(const c8** p);

	//! parses an int from a char pointer and moves the pointer to
	//! the end of the parsed float
	inline s32 readInt(const c8** p);

	//! count the tokens separated by white space
	u32 countTokens(const c8* data);

	//! places pointer to next begin of a token
	void findNextNoneWhiteSpace(const c8** p);

	//! reads floats from inside of xml element until end of xml element
	void readFloatsInsideElement(io::IXMLReaderUTF8* reader, f32* floats, u32 count);

	//! reads ints from inside of xml element until end of xml element
	void readIntsInsideElement(io::IXMLReaderUTF8* reader, s32* ints, u32 count);

	template <typename t>
	void readIntsInsideElement(io::IXMLReaderUTF8* reader, core::array<t> &ints);

	//! clears all loaded data
	void clearData();

	//! parses all collada parameters inside an element and stores them in Parameters
	void readColladaParameters(io::IXMLReaderUTF8* reader, const core::stringc& parentName);

	//! returns a collada parameter or none if not found
	SColladaParam* getColladaParameter(ECOLLADA_PARAM_NAME name);

	//! parses all collada inputs inside an element and stores them in Inputs. Reads
	//! until first tag which is not an input tag or the end of the parent is reached
	void readColladaInputs(io::IXMLReaderUTF8* reader, const core::stringc& parentName);

	//! reads a collada input tag and adds it to the input parameter
	void readColladaInput(io::IXMLReaderUTF8* reader);

	//! returns a collada input or none if not found
	SColladaInput* getColladaInput(ECOLLADA_INPUT_SEMANTIC input);

	//! read Collada sId, uses id or name if id is missing
	core::stringc readsId(io::IXMLReaderUTF8* reader);

	//! read Collada Id, uses id or name if id is missing
	core::stringc readId(io::IXMLReaderUTF8* reader);

	//! changes the XML URI into an internal id
	void uriToId(core::stringc& str);

	//! reads a polygons section and creates a mesh from it
	void readPolygonSection(io::IXMLReaderUTF8* reader,
		const core::stringc& vertexPositionSource, core::array<SSource>& sources,
		scene::SMesh* mesh, core::array< core::array<s32> > & conditionnerMapping, const core::stringc& geometryId);
	
	//! finds a material, possible instancing it
	const SColladaMaterial * findMaterial(const core::stringc & materialName);
	
	//! reads and bind materials as given by the symbol->target bind mapping
	void readBindMaterialSection(io::IXMLReaderUTF8* reader, const core::stringc & id);

	//! create an Irrlicht texture from the SColladaImage
	video::ITexture* getTextureFromImage(core::stringc uri);

	//! read a parameter and value
	void readParameter(io::IXMLReaderUTF8* reader);

	//! reads an <extra> node section and its content within a <node> section
	void readExtraNode(io::IXMLReaderUTF8* reader, const core::stringc & id);

	//! reads a <technique> node section and its content within an <extra> node section
	void readExtraTechnique(io::IXMLReaderUTF8* reader, const core::stringc & id);

	//! reads a <user_properties> element and returns its content
	core::stringc readUserPropertiesNode(io::IXMLReaderUTF8* reader);

	scene::ISceneManager* SceneManager;
	io::IFileSystem* FileSystem;

	scene::IAnimatedMesh* DummyMesh;
	core::stringc CurrentlyLoadingMesh;

	scene::IAnimatedMesh* FirstLoadedMesh;
	core::stringc FirstLoadedMeshName;
	s32 LoadedMeshCount;
	u32 Version;

	//core::map<core::stringc, irr::scene::IAnimationTrack*> SAnimationToBind;
	core::array<IColladaPrefab*> Prefabs;
	core::array<SColladaParam> ColladaParameters;
	core::array<SColladaImage> Images;
	core::array<SColladaTexture> Textures;
	core::array<SColladaMaterial> Materials;
	core::array<SColladaInput> Inputs;
	core::array<SColladaEffect> Effects;
	core::map<core::stringc,u32> MaterialsToBind;
	core::map<core::stringc, irr::scene::SAnimatedMesh*> SMeshesToBind;
	core::array< core::array<irr::scene::IMeshBuffer*> > MeshesToBind;
	io::CAttributes Parameters;

	bool CreateInstances;

	irr::io::IReadFile* m_file;

	core::array<ISceneNode *> ToRemove;
	CSceneNodeAnimatorChannelLibrary *m_pAnimationLibrary;
	CLibraryAnimationClips *m_pLibraryAnimationClips;
};



//! following class is for holding and createing instances of library objects,
//! named prefabs in this loader.
class IColladaPrefab : public virtual IReferenceCounted
{
public:
	IColladaPrefab() : m_pInstance(0) {}
	//! returns id of this prefab
	virtual const core::stringc& getId() = 0; // Global ID
	virtual const core::stringc& getsId() = 0; // Scope ID
	virtual const core::stringc& getName() = 0; // Name

	//! returns the ids that need to be resolve before to be able to instanciate the object
	//! 0 indicate that no Id need to be resolved 
	virtual const core::stringc * getUnresolvedPrefabId() = 0;

	//! Set a prefabs for a given Id
	virtual void resolvePrefab(const core::stringc *, IColladaPrefab *) = 0;

	//! Get the create instance
	//! 0 indicate that no instance has been created
	virtual  scene::ISceneNode*	getInstance() const { return m_pInstance; } 

	//! Get the create instance
	//! 0 indicate that the instance could not be created
	virtual scene::ISceneNode*	getInstance(scene::ISceneNode* parent,
		scene::ISceneManager* mgr)
	{
		if(m_pInstance == 0)
		{
			m_pInstance = addInstance(parent, mgr);

		}
		else
		{
			parent->addChild(m_pInstance);
			//m_pInstance->setParent(parent);
		}
		return m_pInstance;
	}

protected:
	//! creates an instance of this prefab
	virtual scene::ISceneNode* addInstance(scene::ISceneNode* parent,
		scene::ISceneManager* mgr) = 0;

	//
	scene::ISceneNode* m_pInstance;
};


} // end namespace scene
} // end namespace irr

#endif

