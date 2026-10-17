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

#include "StdAfx.h"
#include "AnimationExporter.h"
#include "DocumentExporter.h"
#include "EntityExporter.h"
#include "GeometryExporter.h"
#include "ControllerExporter.h"
#include "MaterialExporter.h"
#include "NodeExporter.h"
#include "ColladaOptions.h"
#include "XRefFunctions.h"
#include "XRefManager.h"
#include "ColladaIDCreator.h"
#include "ColladaModifiers.h"
#include "Common/MultiMtl.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDAnimated.h"
#include "FCDocument/FCDController.h"
#include "FCDocument/FCDEntityInstance.h"
#include "FCDocument/FCDEntityReference.h"
#include "FCDocument/FCDGeometry.h"
#include "FCDocument/FCDGeometryInstance.h"
#include "FCDocument/FCDGeometryMesh.h"
#include "FCDocument/FCDGeometryPolygons.h"
#include "FCDocument/FCDGeometryPolygonsInput.h"
#include "FCDocument/FCDGeometryPolygonsTools.h"
#include "FCDocument/FCDGeometrySource.h"
#include "FCDocument/FCDGeometrySpline.h"
#include "FCDocument/FCDEmitter.h"
#include "FCDocument/FCDEffect.h"
#include "FCDocument/FCDMaterial.h"
#include "FCDocument/FCDMaterialInstance.h"
#include "FCDocument/FCDLibrary.h"
#include <surf_api.h>

//// debug
//#include "FUtils/FUDaeEnum.h"
//#include <sstream>
//#include <vector>
//#include "maxscrpt/MAXScrpt.h"

#if _DEBUG
#include "maxscrpt/MAXScrpt.h"
#endif // _DEBUG

// ST - TODO: Check where we should be sticking these defines..

// Defined in EditTriObj, in the 3dsMax samples
#define ET_MASTER_CONTROL_REF	0
#define ET_VERT_BASE_REF		1

// Defined in PolyEdit, in the 3dsMax samples
#define EPOLY_PBLOCK 0
#define EPOLY_MASTER_CONTROL_REF  1
#define EPOLY_VERT_BASE_REF 2

// debug
//std::vector<std::string> dbg_strings;

inline bool IsFinite(const Point3& p) { return _finite(p.x) && _finite(p.y) && _finite(p.z); }
inline bool IsFinite(const Point4& p) { return _finite(p.x) && _finite(p.y) && _finite(p.z) && _finite(p.w); }

//
// NormalSpec
//

void NormalSpec::SetTriangles(MeshNormalSpec *t)
{ 
	triangleSpec = t;
	isTriangles = true;
	isEditablePoly = false;
}

void NormalSpec::SetPolygons(MNNormalSpec *p) 
{
	polySpec = p;
	isTriangles = false;
	isEditablePoly = true;
}

void NormalSpec::SetNull()
{
	triangleSpec = NULL;
	polySpec = NULL;
	isTriangles = isEditablePoly = false;
}

bool NormalSpec::IsNull()
{
	return (!isTriangles && !isEditablePoly);
}

// dispatchers
int NormalSpec::GetNumNormals()
{
	if (isEditablePoly) return polySpec->GetNumNormals();
	else if (isTriangles) return triangleSpec->GetNumNormals();
	else return -1;
}

Point3 NormalSpec::GetNormal(int idx)
{
	if (isEditablePoly) 
		return polySpec->Normal(idx);
	else if (isTriangles)
		return triangleSpec->Normal(idx);
	
	return Point3::Origin;
}

int NormalSpec::GetNormalIndex(int faceIdx, int cornerIdx)
{
	if (isEditablePoly) 
		return polySpec->GetNormalIndex(faceIdx, cornerIdx);
	else if (isTriangles)
		return triangleSpec->GetNormalIndex(faceIdx, cornerIdx);
	
	return -1;
}

//
// GeometryExporter
//

GeometryExporter::GeometryExporter(DocumentExporter* _document)
:	BaseExporter(_document)
,	geomObject(NULL), triObject(NULL), polyObject(NULL), deleteObject(false)
{
	currentNormals.SetNull();
}

GeometryExporter::~GeometryExporter()
{
	// Release all the ExportedMaterial information structures.
	CLEAR_POINTER_VECTOR(exportedMaterials);
}

void GetBaseObjectAndID(Object*& object, SClass_ID& sid)
{
	if (object == NULL) return;

	sid = object->SuperClassID();
	if (sid == WSM_DERIVOB_CLASS_ID || sid == DERIVOB_CLASS_ID || sid == GEN_DERIVOB_CLASS_ID)
	{
		IDerivedObject* derivedObject = (IDerivedObject*) object;
		if (derivedObject->NumModifiers() > 0)
		{
			// Remember that 3dsMax has the mod stack reversed in its internal structures.
			// So that evaluating the zero'th modifier implies evaluating the whole modifier stack.
			ObjectState state = derivedObject->Eval(0, 0);
			object = state.obj;
		}
		else
		{
			object = derivedObject->GetObjRef();
		}
		sid = object->SuperClassID();
	}
}

void GeometryExporter::ClassifyObject(Object* object, bool affectsControllers)
{
	SClass_ID sid;
	GetBaseObjectAndID(object, sid);

	Class_ID id = object->ClassID();

	if (!OPTS->ExportEPolyAsTriangles())
	{
		// Check for the possibility of an editable poly object.
		if (sid == GEOMOBJECT_CLASS_ID && (id == EPOLYOBJ_CLASS_ID || id.PartA() == POLYOBJ_CLASS_ID))
		{
			polyObject = (PolyObject*) object;
		}
	}
	if (polyObject == NULL)
	{
		// Check for the possibility of an editable mesh object.
		if (sid == GEOMOBJECT_CLASS_ID && (id.PartA() == EDITTRIOBJ_CLASS_ID || id.PartA() == TRIOBJ_CLASS_ID))
		{
			triObject = (TriObject*) object;
		}

		if (triObject == NULL)
		{
			// Check for geometry primitives (Box, Cylinder, Torus, etc.) and convert them to TriObject
			// since they don't export well as PolyObject (controllers are messed up)
			if (affectsControllers && 
				(id == Class_ID(BOXOBJ_CLASS_ID, 0) ||
				id == Class_ID(SPHERE_CLASS_ID, 0) ||
				id == Class_ID(CYLINDER_CLASS_ID, 0) ||
#if (MAX_VERSION_MAJOR >= 8)
#ifndef NO_OBJECT_STANDARD_PRIMITIVES
				id == PLANE_CLASS_ID ||
				id == PYRAMID_CLASS_ID ||
				id == GSPHERE_CLASS_ID ||
#endif // NO_OBJECT_STANDARD_PRIMITIVES
#endif // USE_MAX_8
				id == Class_ID(CONE_CLASS_ID, 0) ||
				id == Class_ID(TORUS_CLASS_ID, 0) ||
				id == Class_ID(TUBE_CLASS_ID, 0) ||
				id == Class_ID(HEDRA_CLASS_ID, 0) ||
				id == Class_ID(BOOLOBJ_CLASS_ID, 0)))
			{
				if (object->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
				{
					triObject = (TriObject*) object->ConvertToType(TIME_EXPORT_START, Class_ID(TRIOBJ_CLASS_ID, 0));
					deleteObject = true;
				}
			}

			if (triObject == NULL && !OPTS->ExportEPolyAsTriangles() && object->CanConvertToType(Class_ID(POLYOBJ_CLASS_ID, 0)))
			{
				polyObject = (PolyObject*) object->ConvertToType(TIME_EXPORT_START, Class_ID(POLYOBJ_CLASS_ID, 0));
				deleteObject = true;
			}
			if (triObject == NULL && polyObject == NULL && object->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
			{
				triObject = (TriObject*) object->ConvertToType(TIME_EXPORT_START, Class_ID(TRIOBJ_CLASS_ID, 0));
				// ST - Copy over animated vertices, this is not done by default
				if (id == EPOLYOBJ_CLASS_ID || id.PartA() == POLYOBJ_CLASS_ID) {
					// Copy over any animated vertices, because that is not done by default.
					int numVertControllers = object->NumRefs() - EPOLY_VERT_BASE_REF;
					if (numVertControllers > 0)
					{
						// Initialize our triObj to recieve vert controllers.
						triObject->ReplaceReference(ET_VERT_BASE_REF, NULL);

						// Assert we now have the same count
#ifdef _DEBUG
						assert((triObject->NumRefs() - ET_VERT_BASE_REF) == numVertControllers);
#endif
						for (int i = 0; i < numVertControllers; ++i)
						{
							triObject->AssignController(object->GetReference(EPOLY_VERT_BASE_REF + i), ET_VERT_BASE_REF + i);
						}
					}
				}

				deleteObject = true;
			}
			if (triObject == NULL && OPTS->ExportEPolyAsTriangles())
			{
				// Last possibility: do check for an editable poly type that does not convert to a TriObject on its own.
				//[untested].
				if (object->CanConvertToType(Class_ID(POLYOBJ_CLASS_ID, 0)))
				{
					PolyObject* tempPolyObject = (PolyObject*) object->ConvertToType(TIME_EXPORT_START, Class_ID(POLYOBJ_CLASS_ID, 0));
					triObject = (TriObject*) tempPolyObject->ConvertToType(TIME_EXPORT_START, Class_ID(TRIOBJ_CLASS_ID, 0));
					tempPolyObject->DeleteMe();
					deleteObject = true;
				}
			}
		}
	}

	geomObject = (polyObject != NULL) ? (GeomObject*) polyObject : (GeomObject*) triObject;
}

void GeometryExporter::GetSkins( INode* pNode )
{
	Object *pObject = pNode->GetObjectRef();
	
	ColladaModifierManager::ResolveModifiers( pNode, pObject );

	m_skins.clear();

	for (ColladaModifierStack::iterator itM = modifiers.begin(); itM != modifiers.end(); ++itM)
	{
		ColladaModifier* modifier = (*itM);
		
		if (modifier->GetClassID() == COLLADA_SKIN_CLASS_ID)
		{
			ColladaSkin* skin = (ColladaSkin*) modifier;
			ColladaSkinInterface* iskin = skin->GetInterface( pNode );
			if ( iskin )
			{
				m_skins.push_back( iskin );
			}
		}
	}
}

ExportedMaterial *GeometryExporter::GetExportedMaterial( Mtl* mat, size_t matID, MaterialList &materials, fstring &semantic )
{
	FCDMaterial* blackMtl = NULL;
	size_t numMaterials = materials.size();

	// Attach these polygons to a material
	FCDMaterial* colladaMaterial = NULL;
	if (mat == NULL)
	{
		semantic = FC("ColorMaterial");
	}
	else
	{
		Mtl* subMaterial = materials[matID % numMaterials];
		if (subMaterial != NULL)
		{
			// check for XRefs
			if (XRefFunctions::IsXRefMaterial(subMaterial))
			{
				if (!OPTS->ExportXRefs())
				{
					// resolve the source
					subMaterial = XRefFunctions::GetXRefMaterialSource(subMaterial);
				}
				// else do nothing, this is only a material instance
			}

			// if this is a XRef it'll return NULL
			colladaMaterial = document->GetMaterialExporter()->FindExportedMaterial(subMaterial);

			if (colladaMaterial != NULL) semantic = colladaMaterial->GetDaeId();
			else semantic = ColladaID::Create(subMaterial);
		}
		else
		{
			if (blackMtl == NULL)
			{
				// This is possible. In this case, Max displays black, so lets do that too
				DWORD wireColor = 0;
				blackMtl = document->GetMaterialExporter()->ExportColorMaterial(wireColor);
			}
			colladaMaterial = blackMtl;
			semantic = colladaMaterial->GetDaeId();
		}
	}
	ExportedMaterial* exportedMat = new ExportedMaterial(colladaMaterial, (int)matID);
	exportedMaterials.push_back(exportedMat);
	return exportedMat;
}

void GeometryExporter::GetMaterialIds( const MaterialList &materials, fm::vector<size_t> &matIDs )
{
	size_t numMaterials = materials.size();
	if (numMaterials > 1)
	{
		if (IsEditablePoly())
		{
			MNMesh &mnMesh = polyObject->GetMesh();
			int nbFaces = mnMesh.FNum();
			for (int i = 0; i < nbFaces; ++i) {
				MNFace *face = mnMesh.F(i);
				size_t matid = face->material;
				if (matIDs.find(matid) == matIDs.end())
					matIDs.push_back(matid);
			}
		}
		else
		{
			Mesh& mesh = triObject->GetMesh();
			int nbFaces = mesh.getNumFaces();
			for (int i = 0; i < nbFaces; ++i) {
				Face& face = mesh.faces[i];
				size_t matid = face.getMatID() % numMaterials;
				if (matIDs.find(matid) == matIDs.end())	
					matIDs.push_back(matid);
			}
		}
	} 
	else
	{
		matIDs.push_back(0);
	}
}

FCDEntity* GeometryExporter::ExportMesh(INode* node, Object* object, int flags )
{
	if (node == NULL || object == NULL) return NULL;

	m_exportFlags = flags;

	bool isXRef = false;
	if (XRefFunctions::IsXRefObject(object))
	{
		// Resolve the source
		object = XRefFunctions::GetXRefObjectSource(object);
		if (OPTS->ExportXRefs()) isXRef = true;
	}

	// Retrieve the TriObject or PolyObject representation of this geometric object.
	ClassifyObject(object, TEST_FLAG( m_exportFlags, EXPORT_AFFECTS_CONTROLLERS ));
	if (geomObject == NULL || (triObject == NULL && polyObject == NULL)) return NULL;

	// flag override ==> special case for morph geometries
	if ( IsMorphGeometry( node ) )
	{
		// Setting flags to maintain coherency between morph base and targets
		SET_MORPH_GEOMETRY_EXPORTFLAGS( m_exportFlags );
	}

	currentNormals.SetNull();

	// Create the COLLADA geometry entity and its mesh
	FCDGeometry* colladaGeometry = colladaGeometry = new FCDGeometry(CDOC);

	ENTE->ExportEntity(object, colladaGeometry, (fm::string(node->GetName()) + "-mesh").c_str());
	colladaGeometry->SetName(fm::string(node->GetName()));
	FCDGeometryMesh* colladaMesh = colladaGeometry->CreateMesh();

	// Preprocess required sources
	m_bValidNormals = false;
	if ( OPTS->ExportNormals() )
	{
		if ( IsEditablePoly() )
		{
			m_bValidNormals = PreprocessEditablePolyNormals(colladaMesh);
		}
		else
		{
			m_bValidNormals = PreprocessTriangleObjectNormals(colladaMesh);
		}
	}

	PreprocessTextureChannelSources( colladaMesh );

	// Retrieve the list of materials assigned to the different polygons of this mesh
	Mtl* mat = node->GetMtl();
	MaterialList materials;
	FLATTEN_MATERIALS( mat, materials );

	fm::vector<size_t> matIDs;
	size_t numMaterials = materials.size();
	GetMaterialIds( materials, matIDs );

	for ( size_t matIDIndex = 0; matIDIndex < matIDs.size(); ++matIDIndex )
	{
		size_t matID = matIDs[matIDIndex];
		PreprocessTextureChannelInputs( colladaMesh, numMaterials <= 1 ? -1 : (int)matID, matIDIndex, (int)numMaterials );
	}

	ConvertVertexAlphaValues();
	MergeVertexColorTextureChannels();

	// Create one COLLADA polygon set for each material used in the mesh
	for ( size_t matIDIndex = 0; matIDIndex < matIDs.size(); ++matIDIndex )
	{
		size_t matID = matIDs[matIDIndex];
		fstring semantic;

		// Create the COLLADA polygon set
		// No sources must exist when calling this ( they are added later ot the polygons )
		FCDGeometryPolygons* polygons = colladaMesh->AddPolygons();
		ExportedMaterial *pExportedMaterial = GetExportedMaterial( mat, matID, materials, semantic );

		polygons->SetMaterialSemantic( semantic.c_str() );
		polygons->SetUserHandle(pExportedMaterial);

		// Add the polygons set input and its tessellation indices
		ExportPolygonSet( polygons, numMaterials <= 1 ? -1 : (int)matID, matIDIndex, (int)numMaterials ); // <input>
	}

	GetSkins( node );

	// Export the vertex positions and the per-face-vertex normals
	ExportVertexPositions(colladaMesh);
	if (OPTS->ExportNormals() && m_bValidNormals )
	{
		ExportNormals( colladaMesh );
	}

	ExportTextureChannels( colladaMesh );

	// Translate polygon set indices
	size_t polygonsCount = colladaMesh->GetPolygonsCount();
	for (size_t i = 0; i < polygonsCount; ++i )
	{
		FCDGeometryPolygons* polygons = colladaMesh->GetPolygons( i );
		TranslatePolygonSetIndices(polygons); // <input>
	}

	// For texturing map channels, export the texture tangents and bi-normals, on request
	if (OPTS->ExportNormals() && OPTS->ExportTangents())
	{
		size_t sourceCount = colladaMesh->GetSourceCount();
		for (size_t sourceIndex = 0; sourceIndex < sourceCount; ++sourceIndex)
		{
			FCDGeometrySource* source = colladaMesh->GetSource(sourceIndex);
			if (source->GetType() == FUDaeGeometryInput::TEXCOORD)
			{
				FCDGeometryPolygonsTools::GenerateTextureTangentBasis(colladaMesh, source, true);
			}
		}
	}

	// Reset the store data structures
	currentNormals.SetNull();
	if (deleteObject)
	{
		if (triObject != NULL) triObject->DeleteMe();
		if (polyObject != NULL) polyObject->DeleteMe();
	}
	geomObject = NULL; triObject = NULL; polyObject = NULL;
	deleteObject = false;

	if ( OPTS->DeindexVertexData() && TEST_FLAG( m_exportFlags, EXPORT_DEINDEX_VERTEX_DATA ) )
	{
		FCDGeometryPolygonsTools::GenerateUniqueIndices( colladaMesh, NULL, &colladaGeometry->translationMap, TEST_FLAG( m_exportFlags, EXPORT_ENABLE_NAIVE_DEINDEXATION ) );
	}

	if ( OPTS->ExportEPolyAsTriangleStrips() )
	{
		size_t polygonsCount = colladaMesh->GetPolygonsCount();
		for (size_t i = 0; i < polygonsCount; ++i)
		{
			colladaMesh->GetPolygons(i)->SetPrimitiveType( FCDGeometryPolygons::TRIANGLE_STRIPS );
		}
		FCDGeometryPolygonsTools::Triangulate( colladaMesh );
	}

	bool bValidMesh = ValidateMesh( colladaMesh, node );
	if ( bValidMesh )
	{
		if (!isXRef)
		{
			CDOC->GetGeometryLibrary()->AddEntity( colladaGeometry );
		}
	}
	else
	{
		SAFE_RELEASE( colladaGeometry );
	}

	return colladaGeometry;
}

bool GeometryExporter::ValidateMesh( FCDGeometryMesh *pMesh, INode *pNode )
{
	bool bValidMesh = pMesh->GetPolygonsCount() > 0;

	for ( int p = 0; p < pMesh->GetPolygonsCount(); ++p )
	{
		FCDGeometryPolygons *polygon = pMesh->GetPolygons( p );
		int polygonInputCount = polygon->GetInputCount();
		int indexCountValidator = -1;
		for ( int i = 0; i < polygonInputCount; i++ )
		{
			FCDGeometryPolygonsInput *input = polygon->GetInput( i );
			int indexCount = input->GetIndexCount();

			if ( indexCountValidator != -1 )
			{
				if ( indexCountValidator != indexCount )
				{
					indexCountValidator = -1;
					break;
				}
			}
			else
			{
				indexCountValidator = indexCount;
			}
		}

		if ( indexCountValidator <= 0 )
		{
			LOG( OPTS->m_warningManager, ICWarning::INVALID_GEOMETRY, NULL, pMesh, pNode );
			bValidMesh = false;
			break;
		}
	}

	return bValidMesh;
}

// See http://www.brpreiss.com/books/opus4/html/page217.html
#define HASH_FLOAT( f, exp, mantissa, bitShift ) \
( uint32 ) \
( \
	!f ? 0 : \
	( \
		( ( uint32 ) (( 2 * std::fabs( std::frexp( f, &exp ) ) - 1 ) * ~0U * ( f < 0 ? -1 : 1 ))) << bitShift \
	) \
)

#define SET_HASH_BITSHIFTS( hashBitShifts, srcStride ) \
{ \
	uint32 hashSize = (uint32) srcStride; \
	hashBitShifts.reserve(hashSize); \
	for (uint32 h = 0; h < hashSize; ++h) hashBitShifts.push_back(32 * h / hashSize); \
}

#define VALIDATE_AND_INSERT_IN_HASHMAP( p, i, bIsPosition ) \
bool bIsIndexReferenced = true; \
if ( TEST_FLAG( m_exportFlags, EXPORT_REMOVE_UNREFERENCED_SOURCE_DATA ) ) \
{ \
	bIsIndexReferenced = false; \
	for ( fm::vector<FUParameterBoolList *>::iterator iter = sourceValidityMaps.begin(); iter != sourceValidityMaps.end(); ++iter ) \
	{ \
		if ( (*iter)->at( i ) ) \
		{ \
			bIsIndexReferenced = true; \
			break; \
		} \
	} \
} \
\
if ( bIsIndexReferenced ) \
{ \
	InsertInHashMap( hashMap, hashBitShifts, i, p, stride, bIsPosition ); \
}

void GeometryExporter::InsertInHashMap( VertexHashMap &hashMap, UInt32List &bitShiftList, int oldIndex, const Point4 &p, int stride, bool bIsPosition )
{
	uint32 vectorHashValue = oldIndex;

	if ( TEST_FLAG( m_exportFlags, EXPORT_REMOVE_DUPLICATE_SOURCE_DATA )
//#if _DEBUG		
//		&& !OPTS->DbgSwitch()
//#endif // _DEBUG
		)
	{
		int exp;
		float mantissa;
		vectorHashValue = 0;
		for ( int comp = 0; comp < stride; comp++ )
		{
			vectorHashValue ^= HASH_FLOAT( p[comp], exp, mantissa, bitShiftList[comp] );
		}
	}

	VertexData vertexData;
	vertexData.vertex = FMVector4( p.x, p.y, p.z, p.w );
	vertexData.oldIndex = ( unsigned )oldIndex;

	VertexHashMap::iterator it = hashMap.find( vectorHashValue );
	if ( it == hashMap.end() )
	{
		fm::vector<fm::vector<VertexData>> vertexDataList;
		fm::vector<VertexData> bucket;
		bucket.push_back( vertexData );
		vertexDataList.push_back( bucket );
		
		hashMap.insert( vectorHashValue, vertexDataList );
	}
	else
	{
		fm::vector<fm::vector<VertexData>>::iterator insertIter = it->second.end();
		for ( fm::vector<fm::vector<VertexData>>::iterator validationVertexDataIter = it->second.begin(); validationVertexDataIter != it->second.end(); ++validationVertexDataIter )
		{
			// vertex pos test
			if ( !IsEquivalent( vertexData.vertex, validationVertexDataIter->begin()->vertex ) )
			{
				continue;
			}

			// vertex skin data test
			if ( bIsPosition )
			{
				bool bSkinTestPassed = true;
				int oldIndexBucket = validationVertexDataIter->begin()->oldIndex;

				for ( size_t skinId = 0; skinId < m_skins.size(); ++skinId )
				{
					ColladaSkinInterface *pSkin = m_skins[skinId];

					int numBonesBucket = pSkin->GetNumAssignedBones( oldIndexBucket );
					int numBonesCurrent = pSkin->GetNumAssignedBones( oldIndex );

					if ( numBonesBucket != numBonesCurrent )
					{
						bSkinTestPassed = false;
						break;
					}

					for (int p = 0; p < numBonesBucket; ++p)
					{
						if ( pSkin->GetAssignedBone(oldIndexBucket, p) != pSkin->GetAssignedBone(oldIndex, p) 
							|| !IsEquivalent( pSkin->GetBoneWeight(oldIndexBucket, p), pSkin->GetBoneWeight(oldIndex, p) ) )
						{
							bSkinTestPassed = false;
							break;					
						}
					}

					if ( !bSkinTestPassed )
					{
						break;
					}
				}

				if ( !bSkinTestPassed )
				{
					continue;
				}
			}

			insertIter = validationVertexDataIter;
			break;
		}

		if ( insertIter == it->second.end() )
		{
			fm::vector<VertexData> bucket;
			bucket.push_back( vertexData );
			it->second.push_back( bucket );
		}
		else
		{
			insertIter->push_back( vertexData );
		}
	}
}

void GeometryExporter::ApplyVertexHashMap( FCDGeometrySource* source )
{
	UInt32TransMap &translationMap = source->GetTranslationMapRef();
	VertexHashMap &hashMap = source->GetHashMapRef();

	int newVertexCount = CalculateHashMapVertexCount( hashMap );
	source->SetHashedVertexCount( newVertexCount );

	int stride = source->GetStride();

	source->SetValueCount(newVertexCount);
	float* data = source->GetData();

	uint32 newIndex = 0;
	size_t vertexDataSize = sizeof( float ) * stride;
	for ( VertexHashMap::iterator it = hashMap.begin(); it != hashMap.end(); ++it )
	{
		for ( fm::vector<fm::vector<VertexData>>::iterator vertexDataIter = it->second.begin(); vertexDataIter != it->second.end(); ++vertexDataIter )
		{
			memcpy( data, (float *)vertexDataIter->begin()->vertex, vertexDataSize );
			data += stride;

			for ( fm::vector<VertexData>::iterator bucketIter = vertexDataIter->begin(); bucketIter != vertexDataIter->end(); ++bucketIter )
			{
				translationMap.insert( bucketIter->oldIndex, newIndex );
			}
			++newIndex;
		}
	}
}

int GeometryExporter::CalculateHashMapVertexCount( VertexHashMap &hashMap )
{
	int newVertexCount = 0;

	for ( VertexHashMap::iterator it = hashMap.begin(); it != hashMap.end(); ++it )
	{
		newVertexCount += it->second.size();
	}

	return newVertexCount;
}

fm::vector<FUParameterBoolList *> GetSourceValidityMaps( FCDGeometryMesh* colladaMesh, FCDGeometrySource* source )
{
	fm::vector<FUParameterBoolList *> sourceValidityMaps;
	int polygonsCount = colladaMesh->GetPolygonsCount();
	for ( size_t j = 0; j < polygonsCount; ++j )
	{
		FCDGeometryPolygons *polygons = colladaMesh->GetPolygons( j );
		int inputCount = polygons->GetInputCount();
		for ( size_t k = 0; k < inputCount; ++k )
		{
			FCDGeometryPolygonsInput *input = polygons->GetInput( k );
			if ( input->GetSource() == source )
			{
				FUParameterBoolList *sourceValidityMap = input->GetSourceValidityMap();
				sourceValidityMaps.push_back( sourceValidityMap );
			}
		}
	}

	return sourceValidityMaps;
}

void GeometryExporter::ExportVertexPositions(FCDGeometryMesh* colladaMesh)
{
	// Check for vertex position animation
	MasterPointControl* masterController = NULL;
	int referenceCount = geomObject->NumRefs();
	for (int i = 0; i < referenceCount; ++i)
	{
		ReferenceTarget* reference = geomObject->GetReference(i);
		if (reference->ClassID().PartA() == MASTERPOINTCONT_CLASS_ID)
		{
			masterController = (MasterPointControl*) reference;
			break;
		}
	}

	// Create the data source
	FCDGeometrySource* source = colladaMesh->AddVertexSource(FUDaeGeometryInput::POSITION, false);
	source->SetStride(3);
	source->SetDaeId(colladaMesh->GetDaeId() + "-positions");

	// Set-up hash struct and bitshifts
	VertexHashMap &hashMap = source->GetHashMapRef();
	UInt32List hashBitShifts;
	int stride = source->GetStride();
	SET_HASH_BITSHIFTS( hashBitShifts, stride );

	// cache the source validity maps
	fm::vector<FUParameterBoolList *> sourceValidityMaps = GetSourceValidityMaps( colladaMesh, source );

	int oldVertexCount = 0;
	if (!IsEditablePoly())
	{
		Mesh& triMesh = triObject->GetMesh();
		oldVertexCount = triMesh.getNumVerts();
		for (int i = 0; i < oldVertexCount; ++i)
		{
			Point3& p = triMesh.getVert(i);
			VALIDATE_AND_INSERT_IN_HASHMAP( p, i, true );
		}
	}
	else
	{
		// editable poly. case
		MNMesh& m = polyObject->GetMesh();
		oldVertexCount = m.VNum();
		for (int i = 0; i < oldVertexCount; ++i)
		{
			Point3 &p = m.V(i)->p;
			VALIDATE_AND_INSERT_IN_HASHMAP( p, i, true );
		}
	}

	ApplyVertexHashMap( source );

	// using const ref to make sure the const version of operator[] is used
	const UInt32TransMap &transMap = source->GetTranslationMapConstRef();
	
	// debug code
	//mprintf( "\n" );
	//mprintf( "Vertex HashMap integrity verification\n" );
	//for ( VertexHashMap::iterator it = hashMap.begin(); it != hashMap.end(); ++it )
	//{
	//	mprintf( "[ %u ] ==> \n", it->first );
	//	for ( fm::vector<VertexData>::iterator vertexDataIter = it->second.begin(); vertexDataIter != it->second.end(); ++vertexDataIter )
	//	{
	//		mprintf("\t[ %u ] ==> ", vertexDataIter->oldIndex );
	//		mprintf("(" );
	//		for ( int i = 0; i < 3; i++ )
	//		{
	//			mprintf(" %f", vertexDataIter->vertex[i] );
	//		}
	//		mprintf(" ) \n" );

	//		FUAssert( IsEquivalent( it->second.begin()->vertex, vertexDataIter->vertex ), );
	//	}
	//}

	//mprintf( "\n" );
	//mprintf( "Vertex TranslationMap integrity verification\n" );
	//for ( UInt32TransMap::const_iterator transMapIter = transMap.begin(); transMapIter != transMap.end(); ++transMapIter )
	//{
	//	mprintf( "[ %u ] ==> [ %u ]\n", transMapIter->first, transMapIter->second );
	//}

	// Disabled broken vertex position animations
	// Probably broken because of all the processing we do on the geometry mesh position source ( deindexation, removal of duplicates )

	// Export the master controller information
	//if (masterController != NULL)
	//{
	//	ANIM->SetRelativeAnimationFlag(true);
	//	int vertexCount = masterController->GetNumSubControllers();
	//	for (int index = 0; index < vertexCount; ++index)
	//	{
	//		if ( TEST_FLAG( m_exportFlags, EXPORT_REMOVE_UNREFERENCED_SOURCE_DATA ) )
	//		{
	//			bool bIsIndexReferenced = false;
	//			for ( fm::pvector<FUParameterBoolList>::iterator iter = sourceValidityMaps.begin(); iter != sourceValidityMaps.end(); ++iter )
	//			{
	//				if ( (*iter)->at( index ) )
	//				{
	//					bIsIndexReferenced = true;
	//					break;
	//				}
	//			}

	//			if ( !bIsIndexReferenced )
	//			{
	//				continue;
	//			}
	//		}

	//		int newIndex = transMap[index];
	//		Control* subController = masterController->GetSubController(index);
	//		if (subController != NULL)
	//		{
	//			FMVector3* colladaValue = (FMVector3*) source->GetValue(newIndex);
	//			for (int j = 0; j < 3; ++j)
	//			{
	//				FCDAnimated* animated = source->GetSourceData().GetAnimated(newIndex*3+j);
	//				ANIM->SetControlType((ColladaMaxAnimatable::Type) (ColladaMaxAnimatable::ROTATION_X + j));
	//				ANIM->ExportAnimatedFloat(source->GetDaeId(), subController, animated);
	//				if (animated != NULL) animated->SetRelativeAnimationFlag();
	//			}

	//			// Hack in the relative-to value in the vertex position.
	//			Point3 p; Interval i;
	//			subController->GetValue(OPTS->AnimStart(), &p, i);
	//			colladaValue->x = p.x;
	//			colladaValue->y = p.y;
	//			colladaValue->z = p.z;
	//		}
	//	}
	//	ANIM->SetRelativeAnimationFlag(false);
	//}
}

bool GeometryExporter::PreprocessTriangleObjectNormals(FCDGeometryMesh* colladaMesh)
{
	// Retrieve the 3dsMax normals
	Mesh& triMesh = triObject->GetMesh();

	MeshNormalSpec *norms = triMesh.GetSpecifiedNormals();
	if (norms == NULL)
	{
		triMesh.SpecifyNormals();
		norms = triMesh.GetSpecifiedNormals();
	}
	if (norms->GetNumNormals() == 0)
	{
		norms->SetParent(&triMesh);
		norms->CheckNormals();
	}
	if (norms == NULL) return false;
	currentNormals.SetTriangles(norms);

	if (norms->GetNumNormals() == 0)
	{
		norms->SetParent(&triMesh);
		norms->CheckNormals();
	}

	return true;
}

bool GeometryExporter::PreprocessEditablePolyNormals(FCDGeometryMesh* colladaMesh)
{
	MNMesh &mesh = polyObject->GetMesh();

	MNNormalSpec *mnSpec = mesh.GetSpecifiedNormals();
	if (mnSpec == NULL)
	{
		mesh.SpecifyNormals();
		mnSpec = mesh.GetSpecifiedNormals();
	}
	if (mnSpec == NULL) return false;

	// rebuild as needed
	mnSpec->CheckNormals();

	// final test
	int numNormals = mnSpec->GetNumNormals();
	if (numNormals == 0) return false;

	// set the normals
	currentNormals.SetPolygons(mnSpec);

	return true;
}

TextureChannel *GeometryExporter::AddTextureChannel( int channelId )
{
	TextureChannel *pTextureChannel = NULL;

	if ( channelId == m_channels.GetVertexColorChannel().GetChannelId() )
	{
		pTextureChannel = &m_channels.GetVertexColorChannel();
	}
	else 
	if ( channelId == m_channels.GetVertexAlphaChannel().GetChannelId() )
	{
		pTextureChannel = &m_channels.GetVertexAlphaChannel();
	}
	else 
	if ( channelId == m_channels.GetVertexIllumChannel().GetChannelId() )
	{
		pTextureChannel = &m_channels.GetVertexIllumChannel();
	}
	else
	{
		pTextureChannel = &m_channels.AddGenericChannel( channelId );
	}

	return pTextureChannel;
}

void GeometryExporter::PreprocessTextureChannelSources( FCDGeometryMesh *pGeometryMesh )
{
	m_channels.Clear();

	// Get the texture channels (valid indices are from -2 to 99)
	// Offset the negative map channels indices by MAX_MESHMAPS, in order to bring
	// the -2,-1 channels into non-negative range and not clash with the valid map channels.
	fm::vector<int> channelIds;
	Point4 defaultTv( 0.0f, 0.0f, 0.0f, 0.0f );
	if ( IsEditablePoly() ) 
	{
		MNMesh &mnMesh = polyObject->GetMesh(); 
		int chanNum = mnMesh.MNum();
		for ( int mapId = -NUM_HIDDENMAPS; mapId < chanNum; ++mapId )
		{
			MNMap *map = mnMesh.M( mapId );
			if ( map == NULL )
			{
				continue;
			}

			if ( !map->GetFlag( MN_DEAD ) )
			{
				int numV = map->numv;

				// vertex color
				TextureChannel *pTextureChannel = AddTextureChannel( mapId );
				Point4List &values = pTextureChannel->GetValues();
				values.resize( numV );

				for (int i = 0; i < numV; ++i)
				{
					Point4 tv = Point4( map->V( i ) );
					tv = IsFinite( tv ) ? tv : defaultTv;
					values[i] = tv;
				}
			}
		}
	}
	else // polyObject != NULL...
	{
		Mesh &triMesh = triObject->GetMesh();
		int numMaps = triMesh.getNumMaps(); // does not include hidden maps
		// according to SPARKS... valid maps = 0 (color) to MAX_MESHMAPS
		for (int mapId = -NUM_HIDDENMAPS; mapId < MAX_MESHMAPS && mapId < numMaps; ++mapId)
		{
			if ( triMesh.mapSupport( mapId ) == TRUE )
			{
				MeshMap& tmap = triMesh.Map( mapId );
				int numV = tmap.getNumVerts();

				TextureChannel *pTextureChannel = AddTextureChannel( mapId );
				Point4List &values = pTextureChannel->GetValues();
				values.resize( numV );

				for (int i = 0; i < numV; ++i)
				{
					Point4 tv = Point4( tmap.tv[i] );
					tv = IsFinite( tv ) ? tv : defaultTv;
					values[i] = tv;
				}
			}
		}
	}
}

void GeometryExporter::ExportNormals(FCDGeometryMesh* colladaMesh)
{
	// Create the data source
	FCDGeometrySource* source = colladaMesh->AddSource(FUDaeGeometryInput::NORMAL, 1);
	source->SetStride(3);
	source->SetDaeId(colladaMesh->GetDaeId() + "-normals");

	// Set-up hash struct and bitshifts
	VertexHashMap &hashMap = source->GetHashMapRef();
	UInt32List hashBitShifts;
	int stride = source->GetStride();
	SET_HASH_BITSHIFTS( hashBitShifts, stride );

	// cache the source validity maps
	fm::vector<FUParameterBoolList *> sourceValidityMaps = GetSourceValidityMaps( colladaMesh, source );

	// Fill in the normals source
	int oldNormalCount = currentNormals.GetNumNormals();
	Point3 defaultNormal( 0.0f, 0.0f, 1.0f );
	for (int i = 0; i < oldNormalCount; ++i)
	{
		Point3 n = currentNormals.GetNormal(i);
		n = IsFinite( n ) ? n : defaultNormal;
		VALIDATE_AND_INSERT_IN_HASHMAP( n, i, false );
	}

	ApplyVertexHashMap( source );
}

void GeometryExporter::GetTextureChannelValues( int channelId, fm::vector<UVVert> &values, const UVVert &defaultTv )
{
	if ( IsEditablePoly() )
	{
		MNMesh &mnMesh = polyObject->GetMesh();
		MNMap *mnMap = mnMesh.M( channelId );
		int numV = mnMap->numv;
		for (int i = 0; i < numV; ++i)
		{
			UVVert tv = mnMap->V(i);
			tv = IsFinite( tv ) ? tv : defaultTv;
			values.push_back( tv );
		}
	}
	else
	{
		Mesh& mesh = triObject->GetMesh();
		MeshMap& tmap = mesh.Map( channelId );
		int mapVertexCount = tmap.getNumVerts();
		for (int i = 0; i < mapVertexCount; ++i)
		{
			Point3 tv = tmap.tv[i];
			tv = IsFinite( tv ) ? tv : defaultTv;
			values.push_back( tv );
		}
	}
}

void GeometryExporter::ExportTextureChannels( FCDGeometryMesh* colladaMesh )
{
	int channelIndexIndex = 0;
	if ( IsExportingVertexColor() )
	{
		ExportTextureChannel( colladaMesh, m_channels.GetMergedVertexColorChannel(), channelIndexIndex++ );
	}
	
	size_t genericTextureChannelCount = m_channels.GetGenericChannelCount();
	for ( size_t channelId = 0; channelId < genericTextureChannelCount; ++channelId )
	{
		ExportTextureChannel( colladaMesh, m_channels.GetGenericChannel( channelId ), channelIndexIndex++ );
	}
}

//void GeometryExporter::ExportTextureChannel(FCDGeometryMesh* colladaMesh, size_t channelIndexIndex)
void GeometryExporter::ExportTextureChannel( FCDGeometryMesh* colladaMesh, TextureChannel &textureChannel, size_t channelIndexIndex )
{
	int channelIndex = textureChannel.GetChannelId();

	boolean bIsTexCoord = channelIndex > 0;
	boolean bStripThirdTexCoord = bIsTexCoord && OPTS->StripThirdTextureCoord();
	boolean bInvertVCoord = bIsTexCoord && !_stricmp( OPTS->TextureCoordinatesOrigin().c_str(), GetString( ColladaOptions::s_kTextureCoordinatesOriginStrings[ColladaOptions::TCO_UPPER_LEFT] ));

	size_t sourceCount = 1;
	sourceCount += m_bValidNormals ? 1 : 0;
	size_t firstChannelInputIndex = sourceCount;

	// Create the data source
	FCDGeometrySource* source = colladaMesh->AddSource( !bIsTexCoord ? FUDaeGeometryInput::COLOR : FUDaeGeometryInput::TEXCOORD, firstChannelInputIndex + channelIndexIndex );
	source->SetUserHandle((void*)(size_t) channelIndex);

	// set stride
	if ( bIsTexCoord )
	{
		source->SetStride( bStripThirdTexCoord ? 2 : 3 );
	}
	else
	{
		TextureChannel &vertexAlphaChannel = m_channels.GetVertexAlphaChannel();
		TextureChannel &vertexColorChannel = m_channels.GetVertexColorChannel();

		bool bHasVertexAlphaChannel = vertexAlphaChannel.GetValues().size() > 0;
		bool bHasVertexColorChannel = vertexColorChannel.GetValues().size() > 0;

		size_t stride = 0;
		stride += bHasVertexColorChannel ? 3 : 0;
		stride += bHasVertexAlphaChannel ? 1 : 0;
		source->SetStride( stride );

		if ( bHasVertexAlphaChannel && !bHasVertexColorChannel )
		{
			
		}
	}

	FUSStringBuilder builder(colladaMesh->GetDaeId()); builder.append("-map-channel");
	builder.append( channelIndex );
	source->SetDaeId( builder.ToCharPtr() );

	// Set-up hash struct and bitshifts
	VertexHashMap &hashMap = source->GetHashMapRef();
	UInt32List hashBitShifts;
	int stride = source->GetStride();
	SET_HASH_BITSHIFTS( hashBitShifts, stride );

	// cache the source validity maps
	fm::vector<FUParameterBoolList *> sourceValidityMaps = GetSourceValidityMaps( colladaMesh, source );

	Point4List &values = textureChannel.GetValues();
	int numV = ( int )values.size();
	for (int i = 0; i < numV; ++i)
	{
		Point4 tv = values[i];
		if ( bInvertVCoord )
		{
			tv.y = 1.f - tv.y;
		}
		VALIDATE_AND_INSERT_IN_HASHMAP( tv, i, false );
	}

	ApplyVertexHashMap( source );
}

#undef HASH_FLOAT
#undef SET_HASH_BITSHIFTS
#undef VALIDATE_AND_INSERT_IN_HASHMAP

void GeometryExporter::FlattenMaterials(Mtl* material, MaterialList& materialMap, int materialIndex)
{
	// check for XRefs
	if (material != NULL && XRefFunctions::IsXRefMaterial(material))
	{
		material = XRefFunctions::GetXRefMaterialSource(material);
	}

	// KEEP THE NULL POINTER! Null pointers are actually allowed in max, and we need to 
	// maintain the material list.
	Class_ID matId = (material == NULL) ? 
						Class_ID(STANDIN_CLASS_ID, STANDIN_CLASS_ID) :
						material->ClassID();

	if (matId == Class_ID(MULTI_CLASS_ID, 0))
	{
		if (materialIndex < 0)
		{
			// Read in the sub-materials
			int nbSubMtl = material->NumSubMtls();
			for (int i = 0; i < nbSubMtl; ++i)
			{
				Mtl *subMtl = material->GetSubMtl(i);
				FlattenMaterials(subMtl, materialMap, i);
			}
		}
		else 
		{
			// If we are in a recursive multi-material, read in the correctly indexed sub-material
			Mtl* subMtl = MATE->GetSubMaterialById(material, materialIndex);
			if (subMtl == NULL) subMtl = material->GetSubMtl(0);
			if (subMtl != NULL) FlattenMaterials(subMtl, materialMap);
		}
	}
	else if (matId == Class_ID(BAKE_SHELL_CLASS_ID, 0))
	{
		// Read in the first sub-material only
		FlattenMaterials(material->GetSubMtl(1), materialMap);
	}
	/*else if (XRefFunctions::IsXRefMaterial(material))
	{
		// Add this material as a leaf
		materialMap.push_back(material);
	} */
	else
	{
		// Add this material as a leaf
		materialMap.push_back(material);
	}
}

int GeometryExporter::GetFaceVertexCount( int faceId )
{
	int vertexCount = -1;

	if ( IsEditablePoly() )
	{
		// Calculate the number of vertices in this face
		MNMesh &mnMesh = polyObject->GetMesh();
		MNFace *face = mnMesh.F( faceId );
		if ( face )
		{
			vertexCount = face->deg;
		}
	}
	else
	{
		// Triangles
		vertexCount = 3;
	}

	return vertexCount;
}

void GeometryExporter::PreprocessTextureChannelInputs( FCDGeometryMesh *pColladaMesh, int materialIndex, int materialIndexIndex, int numMaterials )
{
	Int32List faceIndices;
	int faceCount = 0;
	GetFaceIndices( materialIndex, numMaterials, faceIndices, faceCount );

	// get texture channels
	TextureChannelPtrList textureChannels;
	{
		Point4List &alphaChannelValues = m_channels.GetVertexAlphaChannel().GetValues();
		if ( alphaChannelValues.size() > 0)
		{
			textureChannels.push_back( &alphaChannelValues );
		}

		Point4List &colorChannelValues = m_channels.GetVertexColorChannel().GetValues();
		if ( colorChannelValues.size() > 0)
		{
			textureChannels.push_back( &colorChannelValues );
		}

		size_t genericTextureChannelInputCount = m_channels.GetGenericChannelCount();
		for ( size_t i = 0; i < genericTextureChannelInputCount; ++i )
		{
			textureChannels.push_back( &m_channels.GetGenericChannel( i ) );
		}
	}

	FCDGeometryPolygonsInputList inputs;
	size_t textureChannelCount = textureChannels.size();
	for ( int textureChannelId = 0; textureChannelId < textureChannelCount; ++textureChannelId )
	{
		FCDGeometryPolygonsInput *pInput = textureChannels[textureChannelId]->AddInput();
		inputs.push_back( pInput );
	}

	// Add the tessellation indices to the inputs
	for (int i = 0; i < faceCount; ++i)
	{
		int faceIndex = (i < (int) faceIndices.size()) ? faceIndices[i] : i;

		int vertexCount = GetFaceVertexCount( faceIndex );
		if ( vertexCount < 0 )
		{
			continue;
		}

		size_t textureChannelCount = textureChannels.size();
		for ( size_t i = 0; i < textureChannelCount; ++i )
		{
			TextureChannel &textureChannel = *textureChannels[ i ];
			FCDGeometryPolygonsInput *pInput = inputs[ i ];
			int channel = textureChannel.GetChannelId();
			int curIndex = -1;
			
			if (!IsEditablePoly()) // Triangles
			{
				Mesh &mesh = triObject->GetMesh();
				MeshMap& tmap = mesh.Map(channel);
				TVFace& face = tmap.tf[faceIndex];
				for (int v = 0; v < vertexCount; ++v)
				{
					curIndex = face.t[v];
					pInput->AddIndex( curIndex );
				}
			}
			else
			{
				MNMesh &mnMesh = polyObject->GetMesh();
				MNMap *map = mnMesh.M(channel);
				if (map == NULL) break;

				MNMapFace *face = map->F(faceIndex);

				FUAssert(face != NULL,);
				FUAssert(vertexCount == face->deg,);
				for (int v = 0; v < vertexCount; v++)
				{
					curIndex = face->tv[v];
					pInput->AddIndex( curIndex );
				}
			}
		}
	}
}

void GeometryExporter::GetFaceIndices( int materialIndex, int numMaterials, Int32List &faceIndices, int &faceCount )
{
	if (materialIndex != -1)
	{
		// Export only the faces/polygons which use this material
		if (IsEditablePoly())
		{
			MNMesh &mnMesh = polyObject->GetMesh();
			int nbFaces = mnMesh.FNum();
			faceIndices.reserve(nbFaces);
			for (int i = 0; i < nbFaces; ++i)
			{
				MNFace *face = mnMesh.F(i);

				if (face->material % numMaterials == materialIndex)
				{
					faceIndices.push_back(i);
				}
			}
		}
		else
		{
			// Triangles
			Mesh& mesh = triObject->GetMesh();
			int nbFaces = mesh.getNumFaces();
			faceIndices.reserve(nbFaces);
			for (int i = 0; i < nbFaces; ++i)
			{
				if (mesh.getFaceMtlIndex(i) % numMaterials == materialIndex)
				{
					faceIndices.push_back(i);
				}
			}
		}
		
		faceCount = (int) faceIndices.size();
	}
	else
	{
		// Only retrieve the correct face/polygon count.
		if (IsEditablePoly())
		{
			faceCount = polyObject->GetMesh().FNum();
		}
		else
		{
			// Triangles
			faceCount = triObject->GetMesh().getNumFaces();
		}
	}
}

void GeometryExporter::ExportPolygonSet(FCDGeometryPolygons* colladaPolygons, int materialIndex, int materialIndexIndex, int numMaterials )
{
	// Make a list of the IDs of faces to export in this polygons set
	Int32List faceIndices;
	int faceCount = 0;
	GetFaceIndices( materialIndex, numMaterials, faceIndices, faceCount );

	FCDGeometryMesh* colladaMesh = colladaPolygons->GetParent();
	FCDGeometryPolygonsInput *positionInput = NULL;
	FCDGeometryPolygonsInput *normalInput = NULL;
	FCDGeometryPolygonsInput *vertexColorInput = NULL;
	FCDGeometryPolygonsInputList genericTextureChannelInputs;

	// texture channel inputs
	{
		size_t inputId = 0;
		size_t numIndicesToReserve = faceCount * 4;

		// position input
		{
			positionInput = colladaPolygons->AddInput( NULL, inputId++ );
			positionInput->ReserveIndexCount( numIndicesToReserve );
		}

		// normal input
		if ( m_bValidNormals )
		{
			normalInput = colladaPolygons->AddInput( NULL, inputId++ );
			normalInput->ReserveIndexCount( numIndicesToReserve );
		}

		// vertex color, vertex alpha channels
		if ( IsExportingVertexColor() )
		{
			vertexColorInput = colladaPolygons->AddInput( NULL, inputId++ );
			vertexColorInput->SetSet( (int32)MAP_COLOR );
			vertexColorInput->ReserveIndexCount( numIndicesToReserve );
		}

		// generic texture channels
		{
			int exportedChannelsCount = m_channels.GetGenericChannelCount();
			for ( size_t channelIndex = 0; channelIndex < exportedChannelsCount; ++channelIndex )
			{
				FCDGeometryPolygonsInput *input = colladaPolygons->AddInput( NULL, inputId++ );
				input->ReserveIndexCount( numIndicesToReserve );
				genericTextureChannelInputs.push_back( input );
				int channelId = m_channels.GetGenericChannel( channelIndex ).GetChannelId();
				input->SetSet((int32)channelIndex );
			}
		}
	}

	// Set the size of all input source validity maps
	// position input
	if ( positionInput )
	{
		FUParameterBoolList *sourceValidityMap = positionInput->GetSourceValidityMap();
		int numSourceData = 0;

		{
			int oldVertexCount = 0;
			if (!IsEditablePoly())
			{
				Mesh& triMesh = triObject->GetMesh();
				oldVertexCount = triMesh.getNumVerts();
			}
			else
			{
				// editable poly. case
				MNMesh& m = polyObject->GetMesh();
				oldVertexCount = m.VNum();
			}

			numSourceData = oldVertexCount;
		}

		sourceValidityMap->resize( numSourceData );
		FILL_VECTOR( *sourceValidityMap, false );
	}

	// normal input
	if ( normalInput )
	{
		FUParameterBoolList *sourceValidityMap = normalInput->GetSourceValidityMap();
		int numSourceData = 0;
		numSourceData = currentNormals.GetNumNormals();
		sourceValidityMap->resize( numSourceData );
		FILL_VECTOR( *sourceValidityMap, false );
	}

	// vertex color input
	if ( vertexColorInput )
	{
		FUParameterBoolList *sourceValidityMap = vertexColorInput->GetSourceValidityMap();
		int numSourceData = 0;
		numSourceData = ( int )m_channels.GetMergedVertexColorChannel().GetValues().size();
		sourceValidityMap->resize( numSourceData );
		FILL_VECTOR( *sourceValidityMap, false );
	}

	// generic texture channels
	{
		size_t genericTextureChannelInputCount = genericTextureChannelInputs.size();
		for ( size_t i = 0; i < genericTextureChannelInputCount; ++i )
		{
			FCDGeometryPolygonsInput* input = genericTextureChannelInputs[i];
			FUParameterBoolList *sourceValidityMap = input->GetSourceValidityMap();
			int numSourceData = 0;
			numSourceData = ( int )m_channels.GetGenericChannel( i ).GetValues().size();
			sourceValidityMap->resize( numSourceData );
			FILL_VECTOR( *sourceValidityMap, false );
		}
	}

	// Add the tessellation indices to the inputs
	for (int i = 0; i < faceCount; ++i)
	{
		int faceIndex = (i < (int) faceIndices.size()) ? faceIndices[i] : i;

		int vertexCount = GetFaceVertexCount( faceIndex );
		if ( vertexCount < 0 )
		{
			continue;
		}

		colladaPolygons->AddFaceVertexCount( vertexCount );

		// Write out the input indices

		// position
		if ( positionInput )
		{
			FCDGeometryPolygonsInput* input = positionInput;
			FUParameterBoolList *sourceValidityMap = input->GetSourceValidityMap();
			int curIndex = -1;
			if (!IsEditablePoly()) // Triangles
			{
				Mesh& mesh = triObject->GetMesh();
				Face& face = mesh.faces[faceIndex];
				for (int v = 0; v < vertexCount; ++v)
				{
					curIndex = face.v[v];
					input->AddIndex( curIndex );
					(*sourceValidityMap)[curIndex] = true;
				}
			}
			else
			{
				MNMesh &mnMesh = polyObject->GetMesh();
				MNFace *face = mnMesh.F(faceIndex);
				for (int v = 0; v < vertexCount; ++v)
				{
					curIndex = face->vtx[v];
					input->AddIndex( curIndex );
					(*sourceValidityMap)[curIndex] = true;
				}
			}
		}

		// normal
		if ( normalInput )
		{
			FCDGeometryPolygonsInput* input = normalInput;
			FUParameterBoolList *sourceValidityMap = input->GetSourceValidityMap();
			int curIndex = -1;
			for (int v = 0; v < vertexCount; ++v)
			{
				curIndex = currentNormals.GetNormalIndex(faceIndex, v);
				input->AddIndex( curIndex );
				(*sourceValidityMap)[curIndex] = true;
			}
		}
	}

	// vertex color
	if ( vertexColorInput )
	{
		FCDGeometryPolygonsInput *input = vertexColorInput;
		FUParameterBoolList *sourceValidityMap = input->GetSourceValidityMap();
		FCDGeometryPolygonsInput *tmpInput = m_channels.GetMergedVertexColorChannel().GetInput( materialIndexIndex );
		int curIndex = -1;
		int vertexColorCount = tmpInput->GetIndexCount();

		for (int v = 0; v < vertexColorCount; ++v)
		{
			curIndex = tmpInput->GetIndices()[v];
			input->AddIndex( curIndex );
			(*sourceValidityMap)[curIndex] = true;
		}
	}

	// generic texture channels
	{
		size_t genericTextureChannelInputCount = genericTextureChannelInputs.size();
		for ( size_t i = 0; i < genericTextureChannelInputCount; ++i )
		{
			FCDGeometryPolygonsInput* input = genericTextureChannelInputs[i];
			FUParameterBoolList *sourceValidityMap = input->GetSourceValidityMap();
			FCDGeometryPolygonsInput *tmpInput = m_channels.GetGenericChannel( i ).GetInput( materialIndexIndex );
			int curIndex = -1;
			int vertexColorCount = tmpInput->GetIndexCount();

			for (int v = 0; v < vertexColorCount; ++v)
			{
				curIndex = tmpInput->GetIndices()[v];
				input->AddIndex( curIndex );
				(*sourceValidityMap)[curIndex] = true;
			}
		}
	}
}

void GeometryExporter::TranslatePolygonSetIndices(FCDGeometryPolygons* colladaPolygons )
{
	// translate the input indices
	size_t inputCount = colladaPolygons->GetInputCount();
	for (size_t k = 0; k < inputCount; ++k)
	{
		FCDGeometryPolygonsInput* input = colladaPolygons->GetInput( k );

		int indexCount = input->GetIndexCount();
		uint32 *indices = input->GetIndices();
		
		FCDGeometrySource* source = input->GetSource();
		const UInt32TransMap &translationMap =  source->GetTranslationMapConstRef();
		
		for ( size_t i = 0; i < indexCount; ++i )
		{
			indices[i] = translationMap[indices[i]];
		}
	}
}

bool GeometryExporter::IsNURB(Object* object)
{
	if (object == NULL) return false;
	Class_ID objId = object->ClassID();

	if (!(	objId == EDITABLE_CVCURVE_CLASS_ID ||
			objId == EDITABLE_FPCURVE_CLASS_ID ||
			objId == EDITABLE_SURF_CLASS_ID    ||
			objId == FITPOINT_PLANE_CLASS_ID))
	{
		return false;
	}

	return true;
}

bool GeometryExporter::IsLinearSpline( Spline3D *pSpline )
{
	FUAssert( pSpline, return false );
	
	// linear segments in the bezier curve will correspond to one segment in the polyline
	PolyLine polyline;
	pSpline->MakePolyLine( polyline, 2, TRUE );

	int numPolylineSegments = polyline.Segments();
	int numSplineSegments = pSpline->Segments();

	return numPolylineSegments == numSplineSegments;
}

FCDEntity *GeometryExporter::ExportCubicSpline( INode *pNode, bool bCurveClosed, Spline3D *pSpline )
{
	// Create the COLLADA geometry entity and its mesh
	FCDGeometry* colladaGeometry = CDOC->GetGeometryLibrary()->AddEntity();

	FUStringBuilder builder( pNode->GetName() );
	builder.append( "-spline" );
	colladaGeometry->SetDaeId( builder.ToString() );

	// create the Bezier collada pSpline
	FCDGeometrySpline* colladaGeomSpline = colladaGeometry->CreateSpline();
	colladaGeomSpline->SetType(FUDaeSplineType::BEZIER);

	FCDBezierSpline* bezierSpline = (FCDBezierSpline*) colladaGeomSpline->AddSpline(FUDaeSplineType::BEZIER);
	bezierSpline->SetClosed( bCurveClosed );

	// for each knot of the spline3D
	int knots = pSpline->KnotCount();
	for (int j = 0; j < knots; ++j)
	{
		SplineKnot k = pSpline->GetKnot(j);
		Point3 pos;

		// add the knot in-vector as a control vertex for all knots except
		// the first one. Include the first one if the pSpline is closed.
		if (bezierSpline->IsClosed() || j > 0)
		{
			pos = k.InVec();
			bezierSpline->AddCV(ToFMVector3(pos));
		}
		// always add the knot position as a control vertice
		pos = k.Knot();
		bezierSpline->AddCV(ToFMVector3(pos));

		// add the knot out-vector as a control vertex for every knot except
		// the last one. Include the last one if the pSpline is closed.
		if (bezierSpline->IsClosed() || j < knots - 1)
		{
			pos = k.OutVec();
			bezierSpline->AddCV(ToFMVector3(pos));
		}
	}

	return colladaGeometry;
}

FCDEntity *GeometryExporter::ExportLinearSpline( INode *pNode, bool bCurveClosed, Spline3D *pSpline )
{
	// Create the COLLADA geometry entity and its mesh
	FCDGeometry* colladaGeometry = colladaGeometry = CDOC->GetGeometryLibrary()->AddEntity();

	//ENTE->ExportEntity(object, colladaGeometry, (fm::string(node->GetName()) + "-mesh").c_str());
	colladaGeometry->SetName( fm::string( pNode->GetName() ) );
	colladaGeometry->SetDaeId( fm::string( pNode->GetName()) + "-mesh" );
	
	FCDGeometryMesh* colladaMesh = colladaGeometry->CreateMesh();

	FCDGeometryPolygons* polygons = colladaMesh->AddPolygons();
	polygons->SetPrimitiveType( FCDGeometryPolygons::LINE_STRIPS );

	FCDGeometrySource* source = colladaMesh->AddVertexSource(FUDaeGeometryInput::POSITION, true);
	source->SetDaeId( colladaMesh->GetDaeId() + "-positions" );
	source->SetStride( 3 );
	FCDGeometryPolygonsInput* input = polygons->GetInput( 0 );

	int knots = pSpline->KnotCount();
	source->SetValueCount( knots );
	polygons->AddFaceVertexCount( knots );
	for ( int j = 0; j < knots; ++j )
	{
		SplineKnot k = pSpline->GetKnot( j );
		Point3 pos = k.Knot();
		source->SetValue( j, ( float * )&pos );
		input->AddIndex( j );
	}

	// material
	// Retrieve the list of materials assigned to the different polygons of this mesh
	Mtl* mat = pNode->GetMtl();
	MaterialList materials;
	FLATTEN_MATERIALS( mat, materials );

	fm::vector<size_t> matIDs;
	size_t numMaterials = materials.size();

	if (numMaterials > 1)
	{
		MATE->GetSubMaterialIds( mat, matIDs );
	} 
	else
	{
		matIDs.push_back(0);
	}

	for (size_t* it = matIDs.begin(); it != matIDs.end(); ++it)
	{
		size_t matID = *it;
		fstring semantic;

		ExportedMaterial *pExportedMaterial = GetExportedMaterial( mat, matID, materials, semantic );

		polygons->SetMaterialSemantic( semantic.c_str() );
		polygons->SetUserHandle( pExportedMaterial );
	}

	return colladaGeometry;

	// Code to export a linear spline as a <spline> element
	//// Create the COLLADA geometry entity and its mesh
	//FCDGeometry* colladaGeometry = CDOC->GetGeometryLibrary()->AddEntity();

	//FUStringBuilder builder( pNode->GetName() );
	//builder.append( "-mesh" );
	//colladaGeometry->SetDaeId( builder.ToString() );

	//// create the Bezier collada pSpline
	//FCDGeometrySpline* colladaGeomSpline = colladaGeometry->CreateSpline();
	//colladaGeomSpline->SetType(FUDaeSplineType::LINEAR);

	//FCDLinearSpline* linearSpline = (FCDLinearSpline*) colladaGeomSpline->AddSpline(FUDaeSplineType::LINEAR);
	//linearSpline->SetClosed( bCurveClosed );

	//// for each knot of the spline3D
	//int knots = pSpline->KnotCount();
	//for (int j = 0; j < knots; ++j)
	//{
	//	SplineKnot k = pSpline->GetKnot( j );
	//	Point3 pos = k.Knot();
	//	linearSpline->AddCV( ToFMVector3( pos ) );
	//}

	//return colladaGeometry;
}

void GeometryExporter::ExportSpline(INode* node, Object* object, EntityList &entities )
{
	SClass_ID sid;
	GetBaseObjectAndID(object, sid);

	if (sid != SHAPE_CLASS_ID) return;
	if (XRefFunctions::IsXRefObject(object)) return;

	// NURB or spline?
	// doing this test here since spline->InitializeData may CRASH on a NURB object.
	if (IsNURB(object))
	{
		ExportNURBS ( node, object, entities );
		return;
	}

	ShapeObject* maxobj = (ShapeObject*) object;
    if (!maxobj->CanMakeBezier()) return;
	BezierShape shape;
	maxobj->MakeBezier(TIME_EXPORT_START, shape);

	// for each curve
	int splines = min(shape.SplineCount(), maxobj->NumberOfCurves());
	for (int i=0; i < splines; ++i)
	{
		Spline3D* spline = shape.GetSpline(i);
		if (spline == NULL) continue;

		bool bIsLinearSpline = IsLinearSpline( spline );
		bool bCurveClosed = maxobj->CurveClosed( 0, i ) == TRUE;
		if ( bIsLinearSpline && OPTS->ConvertLinearSplinesToLineStrips() )
		{
			ADD_ENTITY( entities, ExportLinearSpline( node, bCurveClosed, spline ) );
		}
		else
		{
			ADD_ENTITY( entities, ExportCubicSpline( node, bCurveClosed, spline ) );
		}
	}
}

void FillNURBSCVCurve(NURBSCVCurve *maxNurbs, FCDNURBSSpline *colladaNURBS)
{
	if (maxNurbs == NULL || colladaNURBS == NULL)
		return;

	TimeValue coreTime = 0;

	// set the nurb degree
	colladaNURBS->SetDegree((uint32)maxNurbs->GetOrder() - 1);

	// is this NURB closed
	colladaNURBS->SetClosed(maxNurbs->IsClosed() == TRUE);

	int numCVs = maxNurbs->GetNumCVs(), 
		numKnots = maxNurbs->GetNumKnots();

	// create the control vertex
	for (int c = 0; c < numCVs; c++)
	{
		NURBSControlVertex *vertex = maxNurbs->GetCV(c);
		float x,y,z,w; x=y=z=w=0.0f;
		vertex->GetPosition(coreTime,x,y,z);
		vertex->GetWeight(coreTime,w);
		colladaNURBS->AddCV(FMVector3(x,y,z), w);
	}

	// create the knots
	for (int k = 0; k < numKnots; k++)
	{
		double knot = maxNurbs->GetKnot(k);
		colladaNURBS->AddKnot(knot);
	}
}

void GeometryExporter::ExportNURBS(INode* node, Object* object, EntityList &entities)
{
	if (node == NULL || !IsNURB(object)) return;

	TimeValue coreTime = 0;

	// the following NURB API calls require the MAX SDK edmodel library
	NURBSSet maxNurbSet;
	BOOL success = GetNURBSSet(object, coreTime, maxNurbSet, FALSE);
	if (!success) return;

	int numNurbs = maxNurbSet.GetNumObjects();
	if (numNurbs == 0) return;

	// for each NURB in the Set
	int exportedNurbsId = 0;
	for (int n = 0; n < numNurbs; n++)
	{
		NURBSObject *maxNurb = NULL;
		maxNurb = maxNurbSet.GetNURBSObject(n);
		if (maxNurb == NULL) continue;

		// single CV Curve NURB
		// this will detect CV and Point NURBS
		if (maxNurb->GetKind() == kNURBSCurve && maxNurb->GetType() == kNCVCurve)
		{
			NURBSCVCurve *maxNurbCurve = static_cast<NURBSCVCurve *>(maxNurb);

			// Create the COLLADA geometry entity and its mesh
			FCDGeometry* colladaGeometry = CDOC->GetGeometryLibrary()->AddEntity();
			
			FUStringBuilder builder( node->GetName() );
			builder.append( "-spline" );
			// Now using FCollada automatic name conflict resolution instead of providing our own id
			//builder.append( exportedNurbsId++ );
			colladaGeometry->SetDaeId( builder.ToString() );

			FCDGeometrySpline* colladaSpline = colladaGeometry->CreateSpline();
			colladaSpline->SetType(FUDaeSplineType::NURBS);

			entities.push_back( colladaGeometry );

			FCDNURBSSpline* nurbSpline = (FCDNURBSSpline*) colladaSpline->AddSpline(FUDaeSplineType::NURBS);
			FillNURBSCVCurve(maxNurbCurve, nurbSpline);
		}
	}
}

void GeometryExporter::ExportInstanceMaterial( FCDGeometryInstance* pInstance, FCDMaterial* pColladaMaterial, Mtl* pBaseMaterial, ExportedMaterial *pExportedMaterial, const fstring& semantic, FCDGeometryPolygons *pPolygons )
{
	FUAssert(pExportedMaterial != NULL, return);
	Mtl *xRefMaterial = NULL;
	
	// Look for the correct material to assign to this polygons set.
	if (pBaseMaterial != NULL)
	{
		Mtl* polyMaterial = pBaseMaterial;

		// Check for XRef material to dereference
		if (polyMaterial != NULL && XRefFunctions::IsXRefMaterial(polyMaterial))
		{
			xRefMaterial = polyMaterial;
			polyMaterial = XRefFunctions::GetXRefMaterialSource(xRefMaterial);
		}

		// For multi-materials: look for the correct sub-material. Multi-material may be hierarchical...
		while (polyMaterial != NULL && polyMaterial->IsMultiMtl())
		{
			polyMaterial = MATE->GetSubMaterialById(polyMaterial, pExportedMaterial->materialId);

			// Check at every level, for a XRef material..
			if (polyMaterial != NULL && XRefFunctions::IsXRefMaterial(polyMaterial))
			{
				xRefMaterial = polyMaterial;
				polyMaterial = XRefFunctions::GetXRefMaterialSource(xRefMaterial);
			}
		}

		if (polyMaterial != NULL && (!OPTS->ExportXRefs() || xRefMaterial == NULL))
		{
			pColladaMaterial = document->GetMaterialExporter()->FindExportedMaterial(polyMaterial);
		}
	}
	if (pColladaMaterial == NULL)
	{
		// If we have not yet found a valid material, we may have a COLLADA material with a color value
		pColladaMaterial = pExportedMaterial->colladaMaterial;
	}

	if (OPTS->ExportXRefs() && xRefMaterial != NULL)
	{
		FCDMaterialInstance* materialInstance = pInstance->AddMaterialInstance();
		materialInstance->SetMaterial(NULL);
		materialInstance->SetSemantic(semantic);
		FCDEntityReference* xref = materialInstance->GetEntityReference();

		if (XRefFunctions::IsXRefCOLLADA(xRefMaterial))
		{
			FUUri uri(XRefFunctions::GetURL(xRefMaterial), ColladaID::Create(xRefMaterial, true));
			xref->SetUri(uri);
		}
		else
		{
			FUUri uri(XRefFunctions::GetURL(xRefMaterial), semantic );
			xref->SetUri(uri);
		}

		XRefFunctions::ReplaceMaxWithDae( xRefMaterial, xref );

		document->GetMaterialExporter()->ExportMaterialInstance(materialInstance, pExportedMaterial, pColladaMaterial);	
	}
	else if (pColladaMaterial != NULL)
	{
		FCDMaterialInstance* materialInstance = pPolygons ? pInstance->AddMaterialInstance(pColladaMaterial, pPolygons) : pInstance->AddMaterialInstance(pColladaMaterial, semantic);
		document->GetMaterialExporter()->ExportMaterialInstance(materialInstance, pExportedMaterial, pColladaMaterial);
	}
}

void GeometryExporter::ExportInstance(INode* node, FCDEntityInstance* _instance)
{
#define GET_COLLADA_MATERIAL \
	if (baseMaterial == NULL) \
	{ \
		/* do not export the XRef color material unless we don't export XRefs or this object isn't an XRef*/ \
		if (!OPTS->ExportXRefs() || !XRefFunctions::IsXRefObject(node->GetObjectRef())) \
		{ \
			DWORD wireColor = node->GetWireColor(); \
			colladaMaterial = document->GetMaterialExporter()->ExportColorMaterial(wireColor); \
		} \
	}

	// Retrieve all the necessary COLLADA interfaces
	if (node == NULL || !_instance->GetObjectType().Includes(FCDGeometryInstance::GetClassType())) return;
	FCDGeometryInstance* instance = (FCDGeometryInstance*) _instance;
	FCDEntity* colladaEntity = instance->GetEntity();
	FCDGeometry* colladaGeometry = NULL;
	if (colladaEntity->GetObjectType().Includes(FCDEmitter::GetClassType()))
	{
		FCDEmitter *pEmitter = ( FCDEmitter * ) colladaEntity;

		// Retrieve the list of materials assigned to the different polygons of this mesh
		Mtl* baseMaterial = node->GetMtl();
		FCDMaterial* colladaMaterial = NULL;
		GET_COLLADA_MATERIAL

		size_t emitterMaterialCount = pEmitter->GetEmitterMaterialCount();
		for (size_t i = 0; i < emitterMaterialCount; ++i)
		{
			EmitterMaterial &emitterMaterial = pEmitter->GetEmitterMaterial( i );
			ExportInstanceMaterial( instance, colladaMaterial, baseMaterial, emitterMaterial.m_exportedMaterial, emitterMaterial.m_semantic );
		}
	}
	else if (colladaEntity->GetObjectType().Includes(FCDGeometry::GetClassType()))
	{
		colladaGeometry = (FCDGeometry*) colladaEntity;
	}
	else if (colladaEntity->GetObjectType().Includes(FCDController::GetClassType()))
	{
		colladaGeometry = ((FCDController*) colladaEntity)->GetBaseGeometry();
	}

	if (colladaGeometry == NULL || (!colladaGeometry->IsMesh() && !colladaGeometry->IsPSurface())) return;
	if (colladaGeometry->IsMesh())
	{
		FCDGeometryMesh* colladaMesh = colladaGeometry->GetMesh();

		// Retrieve the list of materials assigned to the different polygons of this mesh
		Mtl* baseMaterial = node->GetMtl();
		FCDMaterial* colladaMaterial = NULL;
		GET_COLLADA_MATERIAL

		size_t polygonsCount = colladaMesh->GetPolygonsCount();
		for (size_t i = 0; i < polygonsCount; ++i)
		{
			FCDGeometryPolygons* colladaPolygons = colladaMesh->GetPolygons(i);
			ExportedMaterial* exportedMaterial = (ExportedMaterial*) colladaPolygons->GetUserHandle();
			ExportInstanceMaterial( instance, colladaMaterial, baseMaterial, exportedMaterial, colladaPolygons->GetMaterialSemantic(), colladaPolygons );
		}
	}

#undef GET_COLLADA_MATERIAL
}

void GeometryExporter::PreprocessMorphs( ExportNodeMap &exportNodes )
{
	m_morphGeometryObjects.clear();

	// Process the scene nodes to export
	for (ExportNodeMap::iterator it = exportNodes.begin(); it != exportNodes.end(); ++it)
	{
		ExportNode& exportNode = *(*it).second;
		INode *pNode = exportNode.maxNode;
		Object *pObject = pNode->GetObjectRef();
		
		ColladaModifierManager::ResolveModifiers( pNode, pObject );

		for (ColladaModifierStack::iterator itM = modifiers.begin(); itM != modifiers.end(); ++itM)
		{
			ColladaModifier* modifier = (*itM);
			
			if (modifier->GetClassID() == COLLADA_MORPHER_CLASS_ID)
			{
				ColladaMorph* morph = (ColladaMorph*) modifier;

				m_morphGeometryObjects.insert( pNode, true );

				morph->GetModifier();
				MorphR3* imorph = morph->GetInterface();
				size_t targetCount = imorph->chanBank.size();

				for (size_t i = 0; i < targetCount; ++i)
				{
					morphChannel& channel = imorph->chanBank[i];
					if (!channel.mActive || channel.mNumPoints == 0) continue;

					// the target mesh geometry for the morpher
					FCDGeometry* targetMesh = NULL;
					INode* targetMaxNode = channel.mConnection;
					if ( targetMaxNode != NULL )
					{
						m_morphGeometryObjects.insert( targetMaxNode, false );
					}
				}	
			}
		}
	}
}

bool GeometryExporter::IsMorphGeometry( INode *pNode )
{
	bool bIsMorphGeometry = false;

	for ( MorphGeometryObjectMap::const_iterator morphGeomIter = m_morphGeometryObjects.begin(); morphGeomIter != m_morphGeometryObjects.end(); ++morphGeomIter )
	{
		if ( pNode == morphGeomIter->first )
		{
			bIsMorphGeometry = true;
			break;
		}
	}

	return bIsMorphGeometry;
}

void GeometryExporter::ResolveModifiers(INode *pNode, Object* obj, ColladaModifierStack &modifiers )
{
	ControllerExporter::ResolveSkinAndMorphModifiers( pNode, obj, modifiers );
}

void TextureChannels::Clear()
{
	m_vertexColorChannel.Release();
	m_vertexAlphaChannel.Release();
	m_vertexIllumChannel.Release();
	m_mergedVertexColorChannel.Release();
	m_genericChannels.clear();
}

TextureChannel &TextureChannels::AddGenericChannel( int channelId )
{
	m_genericChannels.push_back( TextureChannel( channelId ) );
	return m_genericChannels.back();
}

bool GeometryExporter::IsExportingVertexColor()
{
	return m_channels.GetMergedVertexColorChannel().GetValues().size();
}

size_t GeometryExporter::GetExportedChannelsCount()
{
	return m_channels.GetGenericChannelCount() + ( IsExportingVertexColor() ? 1 : 0 );
}

void GeometryExporter::ConvertVertexAlphaValues()
{
	TextureChannel &vertexAlphaChannel = m_channels.GetVertexAlphaChannel();

	Point4List &values = vertexAlphaChannel.GetValues();
	size_t valueCount = values.size();
	for ( int valueId = 0; valueId < valueCount; ++valueId )
	{
		Point4 &value = values[valueId];
		value.x = ( value.x + value.y + value.z ) / 3.f; // must be in .x position ==> further methods down the pipe assume the alpha value is a that position
	}
}

void GeometryExporter::MergeVertexColorTextureChannels()
{
	TextureChannel &vertexAlphaChannel = m_channels.GetVertexAlphaChannel();	
	TextureChannel &vertexColorChannel = m_channels.GetVertexColorChannel();
	TextureChannel &mergedVertexColorChannel = m_channels.GetMergedVertexColorChannel();

	Point4List &alphaValues = vertexAlphaChannel.GetValues();
	Point4List &colorValues = vertexColorChannel.GetValues();
	Point4List &mergedColorValues = mergedVertexColorChannel.GetValues();

	bool bHasVertexAlphaChannel = alphaValues.size() > 0;
	bool bHasVertexColorChannel = colorValues.size() > 0;

	if ( bHasVertexAlphaChannel ^ bHasVertexColorChannel )
	{
		if ( bHasVertexAlphaChannel )
		{
			vertexAlphaChannel.Clone( &mergedVertexColorChannel );
		}

		if ( bHasVertexColorChannel )
		{
			vertexColorChannel.Clone( &mergedVertexColorChannel );
		}
	}
	else if ( bHasVertexAlphaChannel && bHasVertexColorChannel )
	{
		size_t alphaInputCount = vertexAlphaChannel.GetInputCount();
		size_t colorInputCount = vertexColorChannel.GetInputCount();
		FUAssert( alphaInputCount == colorInputCount, return );

		for ( size_t inputId = 0; inputId < alphaInputCount; ++inputId )
		{
			FCDGeometryPolygonsInput *pVertexAlphaChannelInput = vertexAlphaChannel.GetInput( inputId );
			FCDGeometryPolygonsInput *pVertexColorChannelInput = vertexColorChannel.GetInput( inputId );
			FCDGeometryPolygonsInput *pMergedVertexColorChannelInput = mergedVertexColorChannel.AddInput();

			size_t alphaIndexCount = pVertexAlphaChannelInput->GetIndexCount();
			size_t colorIndexCount = pVertexColorChannelInput->GetIndexCount();
			FUAssert( alphaIndexCount == colorIndexCount, continue );

			for ( size_t indexId = 0; indexId < alphaIndexCount; ++indexId )
			{
				uint32 alphaIndex = pVertexAlphaChannelInput->GetIndices()[indexId];
				Point4 alpha = alphaValues.at( alphaIndex );

				uint32 colorIndex = pVertexColorChannelInput->GetIndices()[indexId];
				Point4 color = colorValues.at( colorIndex );

				mergedColorValues.push_back( Point4( color.x, color.y, color.z, alpha.x ) );
				pMergedVertexColorChannelInput->AddIndex( mergedColorValues.size() - 1 );
			}
		}
	}

	// channel is appended to .dae source name
	mergedVertexColorChannel.SetChannelId( vertexColorChannel.GetChannelId() );
}

TextureChannel::~TextureChannel()
{
	Release();
}

void TextureChannel::Release()
{
	m_values.clear();
	CLEAR_POINTER_VECTOR( m_inputs );
}

FCDGeometryPolygonsInput *TextureChannel::AddInput()
{
	m_inputs.push_back( new FCDGeometryPolygonsInput( NULL, NULL ) );
	return m_inputs.back();
}

TextureChannel *TextureChannel::Clone( TextureChannel *pClone )
{
	FUAssert( pClone, return NULL );

	CLONE_PARAMETER( pClone, m_channelId );
	CLONE_PARAMETER( pClone, m_values );

	size_t inputCount = m_inputs.size();
	for ( size_t inputId = 0; inputId < inputCount; ++inputId )
	{
		FCDGeometryPolygonsInput *pInput = GetInput( inputId );
		uint32 *pIndices = pInput->GetIndices();
		size_t indexCount = pInput->GetIndexCount();
		FCDGeometryPolygonsInput *pClonedInput = pClone->AddInput();
		pClonedInput->SetIndices( pIndices, indexCount );
	}

	return pClone;
}