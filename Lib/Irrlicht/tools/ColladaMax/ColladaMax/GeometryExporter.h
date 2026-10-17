/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/
/*
	Based on the 3dsMax COLLADA Tools:
	Copyright (C) 2005-2006 Autodesk Media Entertainment
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _GEOMETRY_EXPORTER_H_
#define _GEOMETRY_EXPORTER_H_

#ifndef _BASE_EXPORTER_H_
#include "BaseExporter.h"
#endif // _BASE_EXPORTER_H_

#include "ColladaModifierManager.h"

class FCDEntity;
class FCDEntityInstance;
class FCDGeometry;
class FCDGeometryMesh;
class FCDGeometryInstance;
class FCDGeometryPolygons;
class FCDGeometryPolygonsInput;
class FCDGeometrySource;
class FCDMaterial;
class FCDMaterial;
class Mtl;
class MNMesh;
class PolyObject;
class TriObject;
class ExportNode;
struct ExportedMaterial;
class ColladaSkinInterface;

typedef fm::map<Object*, FCDGeometry*> ExportedGeometryMap;
//typedef fm::map<int, Mtl*> MaterialIdMap;
typedef fm::pvector<Mtl> MaterialList;
typedef fm::map<INode*, ExportNode*> ExportNodeMap;
typedef fm::map<INode*, bool> MorphGeometryObjectMap;
typedef fm::pvector<FCDEntity> EntityList;

#define GET_INITIAL_MESH( modifiers, object )( (!modifiers.empty()) ? modifiers.front()->GetInitialPose() : object )

// These settings allow to maintain coherency between morph base and targets
#define SET_MORPH_GEOMETRY_EXPORTFLAGS( flags ) \
{ \
	RESET_FLAG( flags, GeometryExporter::EXPORT_REMOVE_DUPLICATE_SOURCE_DATA ); \
	RESET_FLAG( flags, GeometryExporter::EXPORT_REMOVE_UNREFERENCED_SOURCE_DATA ); \
	SET_FLAG( flags, GeometryExporter::EXPORT_ENABLE_NAIVE_DEINDEXATION ); \
}

#define FLATTEN_MATERIALS( mat, materials ) \
if (mat != NULL) \
{ \
	GEOM->FlattenMaterials(mat, materials); \
}

// since there's no base class defining both MeshNormalSpec & MNNormalSpec
// we have to create this dispatcher class
//
class NormalSpec
{
private:
	MeshNormalSpec *triangleSpec;
	bool isTriangles;
	MNNormalSpec *polySpec;
	bool isEditablePoly;

public:
	NormalSpec():triangleSpec(NULL),polySpec(NULL),isEditablePoly(false),isTriangles(false){}
	~NormalSpec(){ SetNull(); }

	// two main modifiers
	void SetTriangles(MeshNormalSpec *t);
	void SetPolygons(MNNormalSpec *p);

	void SetNull();
	bool IsNull();

	// dispatchers
	int GetNumNormals();
	Point3 GetNormal(int idx);
	int GetNormalIndex(int faceIdx, int cornerIdx);

}; // end NormalSpec class

typedef fm::vector<Point4> Point4List;
typedef fm::pvector<FCDGeometryPolygonsInput> FCDGeometryPolygonsInputList;

class TextureChannel
{
private:
	Point4List m_values;
	int m_channelId;
	FCDGeometryPolygonsInputList m_inputs;

public:
	TextureChannel( int channelId = -1 - NUM_HIDDENMAPS )
		: m_channelId( channelId )
	{}

	~TextureChannel();
	void Release();

	TextureChannel *Clone( TextureChannel *pClone );

	inline int GetChannelId()
	{
		return m_channelId;
	}

	inline void SetChannelId( int channelId )
	{
		m_channelId = channelId;
	}

	inline Point4List &GetValues()
	{
		return m_values;
	}

	FCDGeometryPolygonsInput *AddInput();

	inline FCDGeometryPolygonsInput *GetInput( size_t index )
	{
		FUAssert( index < m_inputs.size(), return NULL );
		return m_inputs[index];
	}

	inline size_t GetInputCount()
	{
		return m_inputs.size();
	}
};

typedef fm::vector<TextureChannel> TextureChannelList;
typedef fm::pvector<TextureChannel> TextureChannelPtrList;

class TextureChannels
{
private:
	TextureChannel m_vertexColorChannel;
	TextureChannel m_vertexAlphaChannel;
	TextureChannel m_vertexIllumChannel;
	TextureChannel m_mergedVertexColorChannel;
	TextureChannelList m_genericChannels;

public:
	TextureChannels()
		: m_vertexColorChannel( MAP_COLOR )
		, m_vertexAlphaChannel( MAP_ALPHA )
		, m_vertexIllumChannel( MAP_SHADING )
		, m_mergedVertexColorChannel( MAP_COLOR )
	{}

	void Clear();
	TextureChannel &AddGenericChannel( int channelId );

	inline TextureChannel &GetVertexColorChannel()
	{
		return m_vertexColorChannel;
	}

	inline TextureChannel &GetVertexAlphaChannel()
	{
		return m_vertexAlphaChannel;
	}

	inline TextureChannel &GetVertexIllumChannel()
	{
		return m_vertexIllumChannel;
	}

	inline TextureChannel &GetMergedVertexColorChannel()
	{
		return m_mergedVertexColorChannel;
	}

	inline TextureChannel &GetGenericChannel( size_t index )
	{
		return m_genericChannels[index];
	}

	inline size_t GetGenericChannelCount()
	{
		return m_genericChannels.size();
	}
};


class GeometryExporter : public BaseExporter, public ColladaModifierManager
{
public:
	enum
	{
		EXPORT_AFFECTS_CONTROLLERS                 = 0x00000001
		, EXPORT_REMOVE_DUPLICATE_SOURCE_DATA      = 0x00000002
		, EXPORT_REMOVE_UNREFERENCED_SOURCE_DATA   = 0x00000004
		, EXPORT_DEINDEX_VERTEX_DATA			   = 0x00000008
		, EXPORT_ENABLE_NAIVE_DEINDEXATION		   = 0x00000010
		, EXPORT_DEFAULTS		                   = EXPORT_REMOVE_DUPLICATE_SOURCE_DATA | EXPORT_REMOVE_UNREFERENCED_SOURCE_DATA | EXPORT_DEINDEX_VERTEX_DATA
	};

public:
	GeometryExporter(DocumentExporter* document);
	virtual ~GeometryExporter();

	FCDEntity* ExportMesh(INode* node, Object* object, int flags = EXPORT_DEFAULTS);
	void ExportSpline( INode* node, Object* object, EntityList &entities );
	void ExportNURBS(INode* node, Object* object, EntityList &entities);

	void ExportInstance(INode* node, FCDEntityInstance* instance);
	void PreprocessMorphs( ExportNodeMap &exportNodes );
	void FlattenMaterials(Mtl* material, MaterialList& mtlMap, int materialIndex = -1);

	// Do not release the returned pointer
	// semantic : out parameter
	ExportedMaterial *GetExportedMaterial( Mtl* mat, size_t matID, MaterialList &materials, fstring &semantic );


private:
	void ExportVertexPositions(FCDGeometryMesh* colladaMesh);
	void ExportNormals(FCDGeometryMesh* colladaMesh);
	void ExportTextureChannel( FCDGeometryMesh* colladaMesh, TextureChannel &textureChannel, size_t channelIndexIndex );
	void ExportTextureChannels(FCDGeometryMesh* colladaMesh );
	void ExportPolygonSet(FCDGeometryPolygons* colladaPolygons, int materialIndex, int materialIndexIndex, int numMaterials );
	void ExportInstanceMaterial( FCDGeometryInstance* pInstance, FCDMaterial* pColladaMaterial, Mtl* pBaseMaterial, ExportedMaterial *pExportedMaterial, const fstring& semantic, FCDGeometryPolygons *pPolygons = NULL );
	void TranslatePolygonSetIndices(FCDGeometryPolygons* colladaPolygons);

	void ApplyVertexHashMap( FCDGeometrySource* source ); // copies hash map values into source data and creates translation map
	void InsertInHashMap( VertexHashMap &hashMap, UInt32List &bitShiftList, int oldIndex, const Point4 &p, int stride, bool bIsPosition );
	int CalculateHashMapVertexCount( VertexHashMap &hashMap ); // assumes hashMap has been properly filled

	bool IsEditablePoly() { return triObject == NULL; }
	bool IsNURB(Object* object);

	void ClassifyObject(Object* mesh, bool affectsControllers);

	bool PreprocessTriangleObjectNormals(FCDGeometryMesh* colladaMesh);
	bool PreprocessEditablePolyNormals(FCDGeometryMesh* colladaMesh);
	void PreprocessTextureChannelSources( FCDGeometryMesh *pGeometryMesh );

	bool IsMorphGeometry( INode *pNode );
	void GetSkins( INode* pNode );

	bool ValidateMesh( FCDGeometryMesh *pMesh, INode *pNode );

	void ResolveModifiers( INode *pNode, Object* obj, ColladaModifierStack &modifiers );

	bool IsLinearSpline( Spline3D *pSpline );
	FCDEntity *ExportLinearSpline( INode *pNode, bool bCurveClosed, Spline3D *pSpline );
	FCDEntity *ExportCubicSpline( INode *pNode, bool bCurveClosed, Spline3D *pSpline );

	void GetTextureChannelValues( int channelId, fm::vector<UVVert> &values, const UVVert &defaultTv = UVVert( 0.f, 0.f, 0.f ) );
	void GetMaterialIds( const MaterialList &materials, fm::vector<size_t> &matIDs );
	int GetFaceVertexCount( int faceId );

	TextureChannel *AddTextureChannel( int channelId );
	size_t GetExportedChannelsCount();
	bool IsExportingVertexColor();
	void PreprocessTextureChannelInputs( FCDGeometryMesh *pColladaMesh, int materialIndex, int materialIndexIndex, int numMaterials );
	void GetFaceIndices( int materialIndex, int numMaterials, Int32List &faceIndices, int &faceCount );
	void MergeVertexColorTextureChannels();
	void ConvertVertexAlphaValues();

	NormalSpec currentNormals;

	// Buffered and possibly created geometric object.
	GeomObject* geomObject;
	TriObject* triObject;
	PolyObject* polyObject;
	bool deleteObject;

	fm::pvector<ExportedMaterial> exportedMaterials;
	MorphGeometryObjectMap m_morphGeometryObjects;

	// used during ExportMesh
	TextureChannels m_channels;
	bool m_bValidNormals;
	int m_exportFlags;
	fm::pvector<ColladaSkinInterface> m_skins;
};

// structure used to link together the COLLADA material and the 3dsMax MatID.
struct ExportedMaterial
{
	class FCDMaterial* colladaMaterial;
	int materialId;

	ExportedMaterial(FCDMaterial* _colladaMaterial, int _materialId)
		:	colladaMaterial(_colladaMaterial), materialId(_materialId) {}
};

#endif // _GEOMETRY_EXPORTER_H_
