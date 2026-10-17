#include "stdafx.h"
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_LOADER_

#include "CColladaFileLoader.h"
#include "irros.h"
#include "IXMLReader.h"
#include "IDummyTransformationSceneNode.h"
#include "SAnimatedMesh.h"
#include "fast_atof.h"
#include "quaternion.h"
#include "ILightSceneNode.h"
#include "ICameraSceneNode.h"
#include "IMeshManipulator.h"
#include "IReadFile.h"
#include "IAttributes.h"
#include "IMeshCache.h"
#include "IMeshSceneNode.h"
#include "SMeshBufferLightMap.h"
#include "irrMap.h"
#include "CMemoryReadFile.h"
#include "CMorphingMesh.h"
#include "CSkinnedMesh2.h"
#include "CAnimationTrack.h"
#include "CCameraTargetTrackerSceneNode.h"
#include "irrMath.h"
#include "CSkinnedMeshSceneNode.h"
#include "CTimelineController.h"
#include "CEmptySceneNode.h"

//#define COLLADA_READER_DEBUG
namespace irr
{
namespace scene
{
	// currently supported COLLADA tag names

	const core::stringc colladaSectionName =   "COLLADA";
	const core::stringc librarySectionName =   "library";
	const core::stringc libraryNodesSectionName = "library_nodes";
	const core::stringc libraryAnimationsSectionName = "library_animations";
	const core::stringc libraryAnimationClipsSectionName = "library_animation_clips";
	const core::stringc libraryGeometriesSectionName = "library_geometries";
	const core::stringc libraryControllersSectionName = "library_controllers";
	const core::stringc libraryMaterialsSectionName = "library_materials";
	const core::stringc libraryImagesSectionName = "library_images";
	const core::stringc libraryVisualScenesSectionName = "library_visual_scenes";
	const core::stringc libraryCamerasSectionName = "library_cameras";
	const core::stringc libraryLightsSectionName = "library_lights";
	const core::stringc libraryEffectsSectionName = "library_effects";
	const core::stringc assetSectionName =     "asset";
	const core::stringc sceneSectionName =     "scene";
	const core::stringc visualSceneSectionName = "visual_scene";

	const core::stringc lightSectionName =      "light";
	const core::stringc animationSectionName = "animation";
	const core::stringc animationClipSectionName = "animation_clip";
	const core::stringc cameraSectionName =     "camera";
	const core::stringc materialSectionName =  "material";
	const core::stringc geometrySectionName =  "geometry";
	const core::stringc controllerSectionName =  "controller";
	const core::stringc imageSectionName =     "image";
	const core::stringc textureSectionName =   "texture";
	const core::stringc effectSectionName =    "effect";

	const core::stringc ambientNodeName =	   "ambient";
	const core::stringc pointNodeName =		   "point";
	const core::stringc inputSectionName =	   "input";
	const core::stringc targetsSectionName =   "targets";
	const core::stringc idrefArraySectionName = "IDREF_array";
	const core::stringc nameArraySectionName = "Name_array";
	const core::stringc morphSectionName =	   "morph";
	const core::stringc skinSectionName =	   "skin";
	const core::stringc meshSectionName =      "mesh";
	const core::stringc bindShapeMatrixSectionName = "bind_shape_matrix";
	const core::stringc jointsSectionName =	   "joints";
	const core::stringc	vertexWeightsSectionName = "vertex_weights";
	const core::stringc	vcountSectionName =	   "vcount";
	const core::stringc	vSectionName =		   "v";
	const core::stringc sourceSectionName =    "source";
	const core::stringc samplerSectionName =   "sampler";
	const core::stringc channelSectionName =   "channel";
	const core::stringc arraySectionName =     "array";
	const core::stringc floatArraySectionName ="float_array";
	const core::stringc intArraySectionName =  "int_array";
	const core::stringc techniqueCommonSectionName = "technique_common";
	const core::stringc perspectiveSectionName = "perspective";
	const core::stringc opticsSectionName =    "optics";
	const core::stringc targetSectionName =    "target";
	const core::stringc accessorSectionName =  "accessor";
	const core::stringc verticesSectionName =  "vertices";
	const core::stringc inputTagName =         "input";
	const core::stringc polylistSectionName =  "polylist";
	const core::stringc trianglesSectionName = "triangles";
	const core::stringc polygonsSectionName =  "polygons";
	const core::stringc primitivesName =       "p";
	const core::stringc vcountName =           "vcount";

	const core::stringc nodeSectionName =      "node";
	const core::stringc lookatNodeName =       "lookat";
	const core::stringc matrixNodeName =       "matrix";
	const core::stringc perspectiveNodeName =  "perspective";
	const core::stringc rotateNodeName =       "rotate";
	const core::stringc scaleNodeName =        "scale";
	const core::stringc translateNodeName =    "translate";
	const core::stringc skewNodeName =         "skew";
	const core::stringc bboxNodeName =         "boundingbox";
	const core::stringc minNodeName =          "min";
	const core::stringc maxNodeName =          "max";
	const core::stringc instanceName =         "instance";
	const core::stringc instanceCamera =	   "instance_camera";
	const core::stringc instanceControllerName = "instance_controller";
	const core::stringc instanceGeometryName = "instance_geometry";
	const core::stringc instanceSceneName =    "instance_visual_scene";
	const core::stringc instanceEffectName =   "instance_effect";
	const core::stringc setparamName =		   "setparam";
	const core::stringc instanceMaterialName = "instance_material";
	const core::stringc instanceLightName =    "instance_light";
	const core::stringc instanceNodeName =     "instance_node";
	const core::stringc skeletonNodeName =	   "skeleton";
	const core::stringc bindMaterialName =     "bind_material";
	const core::stringc extraNodeName =        "extra";
	const core::stringc techniqueNodeName =    "technique";
	const core::stringc constantAttenuationNodeName = "constant_attenuation";
	const core::stringc linearAttenuationNodeName = "linear_attenuation";
	const core::stringc quadraticAttenuationNodeName = "quadratic_attenuation";
	const core::stringc aspectRatioNodeName =  "aspect_ratio";
	const core::stringc xfovNodeName =		   "xfov";
	const core::stringc znearNodeName =		   "znear";
	const core::stringc zfarNodeName =		   "zfar";
	const core::stringc colorNodeName =        "color";
	const core::stringc floatNodeName =        "float";
	const core::stringc float2NodeName =       "float2";
	const core::stringc float3NodeName =       "float3";

	const core::stringc newParamName =         "newparam";
	const core::stringc paramTagName =         "param";
	const core::stringc initFromName =         "init_from";
	const core::stringc dataName =             "data";

	const core::stringc textureNodeName =      "texture";
	const core::stringc doubleSidedName =      "double_sided";
	const core::stringc userPropertiesNodeName = "user_properties";

	const core::stringc profileCOMMONSectionName = "profile_COMMON";
	const core::stringc irrlichtAdditiveName = "__irrlicht_Additive";

	const char* const inputSemanticNames[] = {"POSITION", "VERTEX", "NORMAL", "TEXCOORD",
		"UV", "TANGENT", "IMAGE", "TEXTURE", "COLOR", 0};

	//! read Collada Name
	core::stringc readName(io::IXMLReaderUTF8* reader)
	{
		core::stringc name = reader->getAttributeValue("name");

		return name;
	}

	class CGeometryPrefab;

	//! following class is for holding and creating instances of library
	//! objects, named prefabs in this loader.
	class CPrefab : public IColladaPrefab
	{
	public:

		CPrefab(const core::stringc& id) : Id(id)
		{
		}

		//! creates an instance of this prefab
		virtual scene::ISceneNode* addInstance(scene::ISceneNode* parent,
			scene::ISceneManager* mgr)
		{
			// empty implementation
			return 0;
		}

		//! returns id of this prefab
		virtual const core::stringc& getId()
		{
			return Id;
		}

		//! returns sId of this prefab
		virtual const core::stringc& getsId()
		{
			return Id;
		}

		//! returns Name of this prefab
		virtual const core::stringc& getName()
		{
			return Id;
		}

		//! returns the ids that need to be resolve before to be able to instanciate the object
		//! 0 indicate that no Id need to be resolved 
		virtual const core::stringc * getUnresolvedPrefabId()
		{
			return 0;
		}

		//! Set a prefabs for a given Id
		virtual void resolvePrefab(const core::stringc *, IColladaPrefab *)
		{

		}

		core::stringc Name;

	protected:

		core::stringc Id;
	};


	//! prefab for a light scene node
	class CLightPrefab : public CPrefab
	{

	public:

		CLightPrefab(const core::stringc& id) : CPrefab(id)
		{
			#ifdef COLLADA_READER_DEBUG
			os::Printer::log("COLLADA: loaded light prefab:", Id.c_str());
			#endif
		}

		video::SLight LightData; // publically accessible

		//! creates an instance of this prefab
		virtual scene::ISceneNode* addInstance(scene::ISceneNode* parent,
			scene::ISceneManager* mgr)
		{
			#ifdef COLLADA_READER_DEBUG
			os::Printer::log("COLLADA: Constructing light instance:", Id.c_str());
			#endif

			scene::ILightSceneNode* l = mgr->addLightSceneNode(parent);
			l->setLightData ( LightData );
			return l;
		}
	};

	


	//! prefab for a mesh scene node
	class CGeometryPrefab : public CPrefab
	{
	public:

		CGeometryPrefab(const core::stringc& id) : CPrefab(id), Mesh(0)
		{
		}

		
		//! Get the create instance
		//! 0 indicate that no instance has been created
		virtual  scene::ISceneNode*	getInstance() const { return 0; } 

		//! Get the create instance
		//! 0 indicate that the instance could not be created
		virtual scene::ISceneNode*	getInstance(scene::ISceneNode* parent,
			scene::ISceneManager* mgr)
		{
			
			m_pInstance = addInstance(parent, mgr);
			return m_pInstance;
		}


		//! creates an instance of this prefab
		virtual scene::ISceneNode* addInstance(scene::ISceneNode* parent,
			scene::ISceneManager* mgr)
		{
			#ifdef COLLADA_READER_DEBUG
			os::Printer::log("COLLADA: Constructing mesh instance:", Id.c_str());
			#endif

			scene::ISceneNode* m = mgr->addMeshSceneNode(Mesh, parent);
			return m;
		}

		virtual IMesh *getMesh() 
		{ 
			return Mesh; 
		}

		virtual void setMesh(IMesh *m) 
		{ 
			Mesh = m;
		}

		virtual const core::array<s32> & getVertexConditionnerMapping(int vbidx) const
		{
			return Mapping[vbidx];
		}

		void addVertexConditionnerMapping(const core::array<s32> &map)
		{
			Mapping.push_back(map);
		}

	protected:
		scene::IMesh* Mesh;
		core::array< core::array<s32> > Mapping;
	};

	class CInstancePrefab : public CPrefab
	{
	public:
		CInstancePrefab(const core::stringc &url) : CPrefab(core::stringc("")), m_url(url), m_pUrlPrefab(0) {};
		core::stringc m_url;
		IColladaPrefab *m_pUrlPrefab;

		virtual const core::stringc * getUnresolvedPrefabId()
		{
			if(m_pUrlPrefab)
				return m_pUrlPrefab->getUnresolvedPrefabId();
			return &m_url;
		}

		//! Set a prefabs for a given Id
		virtual void resolvePrefab(const core::stringc *id, IColladaPrefab *prefab)
		{
			if(m_pUrlPrefab)
			{
				m_pUrlPrefab->resolvePrefab(id, prefab);
			}
			else if(id == &m_url)
			{
				m_pUrlPrefab = prefab;
			}
		}

		virtual scene::ISceneNode* addInstance(scene::ISceneNode* parent,
			scene::ISceneManager* mgr)
		{
			#ifdef COLLADA_READER_DEBUG
			os::Printer::log("COLLADA: Instanciate Controller:", Id.c_str());
			#endif

			return m_pUrlPrefab->getInstance(parent, mgr);
		}
	protected:
	};

	

	class CControllerPrefab : public CGeometryPrefab
	{
	public:
		enum Type
		{
			MORPH,
			SKIN,
		};

		virtual const core::array<s32> & getVertexConditionnerMapping(int vbIdx) const
		{
			switch(type)
			{
			case MORPH:
				return ((CGeometryPrefab *)((SMorph*)pData)->sourcePrefab)->getVertexConditionnerMapping(vbIdx);
			case SKIN:
				return ((CGeometryPrefab *)((SSkin*)pData)->sourcePrefab)->getVertexConditionnerMapping(vbIdx);
			}
			_IRR_DEBUG_BREAK_IF("Should never happen");
		}

		CControllerPrefab(const core::stringc& id, Type t, void *data) 
			: CGeometryPrefab(id), type(t), pData(data)
		{
		}

		virtual IMesh *getMesh() 
		{ 
			if(!Mesh)
				ResolveMesh();
			return Mesh; 
		}

		//! returns the ids that need to be resolve before to be able to instanciate the object
		//! 0 indicate that no Id need to be resolved 
		virtual const core::stringc * getUnresolvedPrefabId()
		{
			switch(type)
			{
			case MORPH:
				{
					SMorph *data = (SMorph *)pData;
					for(u32 i = 0; i < data->sources.size(); i++)
					{
						if(data->sources[i].Array->type == ISourceArray::IDREF)
						{
							SIDRefArray *pArray = static_cast<SIDRefArray *> (data->sources[i].Array);
							if(pArray->Prefabs.size() < pArray->Data.size())
							{
								return &pArray->Data[pArray->Prefabs.size()];
							} else if (data->sourcePrefab == 0) 
							{
								return &data->source;
							}
							break;
						}
					}
					
				}
				break;
			case SKIN:
				{
					SSkin *data = (SSkin *)pData;
					for(u32 i = 0; i < data->sources.size(); i++)
					{
						if(data->sources[i].Array->type == ISourceArray::NAME)
						{
							SNameArray *pArray = static_cast<SNameArray *> (data->sources[i].Array);
							if(pArray->Prefabs.size() < pArray->Data.size())
							{
								return &pArray->Data[pArray->Prefabs.size()];
							} else if (data->sourcePrefab == 0) 
							{
								return &data->source;
							}
							else if(data->sourcePrefab)
							{
								return data->sourcePrefab->getUnresolvedPrefabId();
							}
							break;
						}
					}
				}
				break;
			default:
				#ifdef COLLADA_READER_DEBUG
				os::Printer::log("COLLADA: Constructing unsupported mesh instance:", Id.c_str());
				#endif
				break;
			}

			return 0;
		}

		//! Set a prefabs for a given Id
		virtual void resolvePrefab(const core::stringc *id, IColladaPrefab *prefab)
		{
			switch(type)
			{
			case MORPH:
				{
					SMorph *data = (SMorph *)pData;
					for(u32 i = 0; i < data->sources.size(); i++)
					{
						if(data->sources[i].Array->type == ISourceArray::IDREF)
						{
							SIDRefArray *pArray = (SIDRefArray *)data->sources[i].Array;
							
							if(pArray->Prefabs.size() < pArray->Data.size())
							{
								if(pArray->Data[pArray->Prefabs.size()] == *id)
								{
									pArray->Prefabs.push_back(prefab);
								}
								else
								{
									UNIMPLEMENTED("case were the resolved id isn't the first unresolved");
								}
							}
							else if(data->source == *id)
							{
								data->sourcePrefab = prefab;
							}
							else
							{

								UNIMPLEMENTED("case were the resolved id isn't the first unresolved");
								// TODO:
							}
							
							break;
						}
					}
				}
				break;
			case SKIN:
				{
					SSkin *data = (SSkin *)pData;
					for(u32 i = 0; i < data->sources.size(); i++)
					{
						if(data->sources[i].Array->type == ISourceArray::NAME)
						{
							SNameArray *pArray = (SNameArray *)data->sources[i].Array;
							
							if(pArray->Prefabs.size() < pArray->Data.size())
							{
								if(pArray->Data[pArray->Prefabs.size()] == *id)
								{
									pArray->Prefabs.push_back(prefab);
								}
								else
								{
									UNIMPLEMENTED("case were the resolved id isn't the first unresolved");
								}
							}
							else if(data->source == *id)
							{
								data->sourcePrefab = prefab;
							}
							else if(data->sourcePrefab)
							{
								data->sourcePrefab->resolvePrefab(id, prefab);
							}
							else
							{

								UNIMPLEMENTED("case were the resolved id isn't the first unresolved");
								// TODO:
							}
							
							break;
						}
					}
				}
				break;
			default:
				#ifdef COLLADA_READER_DEBUG
				os::Printer::log("COLLADA: Constructing unsupported mesh instance:", Id.c_str());
				#endif
				break;
			}
		}

		// Temporary for test only
		//! changes the XML URI into an internal id
		void uriToId(core::stringc& str)
		{
			// currently, we only remove the # from the begin if there
			// because we simply don't support referencing other files.
			if (!str.size())
				return;

			if (str[0] == '#')
				str.erase(0);
		}

		IMesh * ResolveSkinIMesh()
		{
			CSkinnedMesh2 *pSkinMesh = irrnew CSkinnedMesh2();
			
			//MeshBuffer->getPosition(
			SSkin *data = (SSkin *)pData;
			CSkin *skin = irrnew CSkin();
			skin->m_bindShapeMtx = data->bindShapeMatrix;
			skin->m_bindShapeMtx.isIdentity();

			// Retrieve bones names
			{ 
				// Find bones name
				const core::stringc *pURI = data->joints.getSourceURI("JOINT");
				_IRR_DEBUG_BREAK_IF(pURI == 0);
				core::stringc id(*pURI);
				uriToId(id);

				// Find bones source
				SSource *pSource = data->sources.getSource(id);
				_IRR_DEBUG_BREAK_IF(pSource == 0);
				_IRR_DEBUG_BREAK_IF(pSource->Array->type != ISourceArray::NAME);

				// Copy nodes name
				skin->m_bonesName = ((SNameArray *)pSource->Array)->Data;
			}
			
			// Retrieve inverse bind matrix (a.k.a. IBM)
			{ 
				// Find IBM url
				const core::stringc *pURI = data->joints.getSourceURI("INV_BIND_MATRIX");
				_IRR_DEBUG_BREAK_IF(pURI == 0);
				core::stringc id(*pURI);
				uriToId(id);

				// Find IBM source
				SSource *pSource = data->sources.getSource(id);
				_IRR_DEBUG_BREAK_IF(pSource == 0);
				_IRR_DEBUG_BREAK_IF(pSource->Array->type != ISourceArray::FLOAT);

				// Copy IBM
				core::array<float> &mtxFloatArray = ((SFloatArray *)pSource->Array)->Data;

				skin->m_invBindMtx.clear();
				skin->m_invBindMtx.reallocate(mtxFloatArray.size() / 16);

				core::matrix4 mtx;
				for(u32 i = 0; i < mtxFloatArray.size() / 16; i++)
				{
					memcpy(mtx.pointer(), &mtxFloatArray[i * 16], 16 * sizeof(f32));
					mtx = mtx.getTransposed();
					skin->m_invBindMtx.push_back(mtx);
				}
			}

			// Retrieve weights 
			{
				// Find weights url
				const core::stringc *pURI = data->vertexWeights.inputs.getSourceURI("WEIGHT");
				_IRR_DEBUG_BREAK_IF(pURI == 0);
				core::stringc id(*pURI);
				uriToId(id);

				// Find weights source
				SSource *pSource = data->sources.getSource(id);
				_IRR_DEBUG_BREAK_IF(pSource == 0);
				_IRR_DEBUG_BREAK_IF(pSource->Array->type != ISourceArray::FLOAT);

				// Copy weights
				skin->m_weights = ((SFloatArray *)pSource->Array)->Data;
			}

			// Conditionne vertex_weight based on the getVertexConditionnerMapping
			{
				core::array<u32> wbTable;
				const core::array<s32> & condMap = getVertexConditionnerMapping(0);
				s32 count = condMap.size();
				
				wbTable.reallocate(data->vertexWeights.count);
				u32 influencesIt = 0;
				for(u32 k = 0; k < data->vertexWeights.count; k++)
				{
					wbTable.push_back(influencesIt);
					influencesIt += data->vertexWeights.vcount[k];
				}

				influencesIt = 0;
				for(int i = 0; i < count; i++)
				{
					// Remap influence count
					u32 nbInfluences = data->vertexWeights.vcount[condMap[i]];
					skin->m_bonesPerVertex.push_back(nbInfluences); //
					influencesIt += nbInfluences;
				}

				skin->m_bonesRefIdPerVertex.reallocate(2 * influencesIt);
				
				for(int j = 0; j < count; j++)
				{
					// Remap influence references
					u32 entry = wbTable[condMap[j]] * 2;
					for(int nbInfluence = skin->m_bonesPerVertex[j]; nbInfluence > 0; --nbInfluence)
					{
						skin->m_bonesRefIdPerVertex.push_back(data->vertexWeights.v[entry]); // bone
						skin->m_bonesRefIdPerVertex.push_back(data->vertexWeights.v[entry + 1]); // weight
						entry+=2;
					}

				}
			}
			
			IMesh *source = ((CGeometryPrefab *)( data->sourcePrefab ))->getMesh();
			pSkinMesh->setSource(source);
			pSkinMesh->setSkin(skin);
			return pSkinMesh;
		}

		IAnimatedMesh * ResolveMorphIMesh()
		{
			CMorphingMesh *pMesh = irrnew CMorphingMesh();

			SMorph *data = (SMorph *)pData;

			CGeometryPrefab *prefab = (CGeometryPrefab *)( data->sourcePrefab );

			pMesh->setSource(prefab->getMesh());
			//pMesh->ReserveTargets(data->sources[0].Array
			SFloatArray *weights;
			SIDRefArray *prefabs;

			for(u32 i = 0; i < data->sources.size(); i++)
			{
				switch(data->sources[i].Array->type)
				{
				case ISourceArray::IDREF :
					prefabs = (SIDRefArray *) data->sources[i].Array;
					break;
				case ISourceArray::FLOAT :
					weights = (SFloatArray *) data->sources[i].Array;
					break;
				}
			}

			for(u32 j = 0; j < weights->Data.size(); j++)
			{
				pMesh->AddTargets(((CGeometryPrefab *)(prefabs->Prefabs[j]))->getMesh(), weights->Data[j]);
			}
			//TODO: Find IMesh in PrefabGeo
			pMesh->Init();
			pMesh->Prepare();

			return pMesh;
		}

		void ResolveMesh()
		{
			if(!Mesh)
			{
				switch(type)
				{
				case MORPH:
					Mesh = ResolveMorphIMesh();
					break;
				case SKIN:
					Mesh = ResolveSkinIMesh();
					break;
				default:
					os::Printer::log("COLLADA: Constructing unsupported mesh instance:", Id.c_str());
					break;
				}
			}
		}

			//! creates an instance of this prefab
		virtual scene::ISceneNode* addInstance(scene::ISceneNode* parent,
			scene::ISceneManager* mgr)
		{
			#ifdef COLLADA_READER_DEBUG
			os::Printer::log("COLLADA: Constructing mesh instance:", Id.c_str());
			#endif

			if(!Mesh)
			{
				ResolveMesh();
			}

			CSkinnedMeshSceneNode *pNode = irrnew CSkinnedMeshSceneNode((IAnimatedMesh *)Mesh);
			pNode->drop();
			return pNode; //(ISceneNode *)mgr->addAnimatedMeshSceneNode((IAnimatedMesh *)Mesh, parent);
		}


		void *pData;
		Type type;
	};

	

	class CInstanceControllerPrefab : public CInstancePrefab
	{
	public:
		CInstanceControllerPrefab(const core::stringc &url) : CInstancePrefab(url) {};
		//! creates an instance of this prefab
		virtual scene::ISceneNode* addInstance(scene::ISceneNode* parent,
			scene::ISceneManager* mgr)
		{
			#ifdef COLLADA_READER_DEBUG
			os::Printer::log("COLLADA: Instanciate Controller:", Id.c_str());
			#endif

			if(m_pUrlPrefab)
			{
				switch(((CControllerPrefab*) m_pUrlPrefab)->type)
				{
				case CControllerPrefab::SKIN:
					{
						CSkinnedMesh2 *pSkin = (CSkinnedMesh2 *)((CControllerPrefab*) m_pUrlPrefab)->getMesh();
						ISceneNode * pSkeletonNode = parent->getSceneNodeFromUID(prefabs[0]->getName().c_str());
						if(pSkeletonNode == 0)
						{
							pSkeletonNode = prefabs[0]->getInstance();
							if(pSkeletonNode == 0)
							{
								pSkeletonNode = prefabs[0]->getInstance(parent, mgr);
							}
						}

						pSkin->setSkeleton(pSkeletonNode);
						//pSkin->skin();
						//mgr->addAnimatedMeshSceneNode(pSkin, parent);
						CSkinnedMeshSceneNode *pNode = irrnew CSkinnedMeshSceneNode((IAnimatedMesh *)pSkin);
						pNode->drop();
						return pNode;
					}
					break;
				case CControllerPrefab::MORPH:
					{
						ISceneNode * pSkeletonNode = m_pUrlPrefab->getInstance();
						
						if(pSkeletonNode == 0)
						{
							pSkeletonNode = m_pUrlPrefab->getInstance(parent, mgr);
						}

						return pSkeletonNode;
					}
				}
				// Forward to the controller prefab
			}


			return 0;
		}

		virtual const core::stringc * getUnresolvedPrefabId()
		{
			if(skeletons.size() == prefabs.size())
			{
				return CInstancePrefab::getUnresolvedPrefabId();
			}
			else
			{
				return &skeletons[prefabs.size()];
			}
		}

		//! Set a prefabs for a given Id
		virtual void resolvePrefab(const core::stringc *id, IColladaPrefab *prefab)
		{
			if(skeletons.size() != prefabs.size())
			{
				if(skeletons[prefabs.size()] == *id)
				{
					prefabs.push_back(prefab);
					return;
				}
			}
			return CInstancePrefab::resolvePrefab(id, prefab);
		}

		core::array<core::stringc> skeletons;
		core::array<IColladaPrefab *> prefabs;
	protected:

	};

	//! prefab for a animations
	class CAnimationPrefab : public CPrefab
	{
	public:
		CAnimationPrefab(const core::stringc& id)
			: CPrefab(id)
		{

		}
		virtual ~CAnimationPrefab()
		{
			for(u32 i = 0; i < sources.size(); i++)
			{
				if(sources[i].Array != 0)
				{
					delete sources[i].Array;
					sources[i].Array = 0;
				}
			}
		}		

		virtual scene::ISceneNode* addInstance(scene::ISceneNode* parent,
			scene::ISceneManager* mgr)
		{
			// Don't know if it really a good idea to do it that way
			return 0;
		}

		SSources sources;
		SSampler sampler;
		SChannel channel;
	};

	//! prefab for a camera scene node
	class CCameraPrefab : public CPrefab
	{
	public:

		CCameraPrefab(const core::stringc& id)
			: CPrefab(id), XFov(core::PI / 2.5f), ZNear(1.0f), ZFar(3000.0f), pTargetPrefab(0)
		{
			#ifdef COLLADA_READER_DEBUG
			os::Printer::log("COLLADA: loaded camera prefab:", Id.c_str());
			#endif
		}

		// publicly accessible data
		f32 XFov;
		f32 AspectRatio;
		f32 ZNear;
		f32 ZFar;
		core::stringc TargetURI;
		IColladaPrefab *pTargetPrefab;

		//! creates an instance of this prefab
		virtual scene::ISceneNode* addInstance(scene::ISceneNode* parent,
			scene::ISceneManager* mgr)
		{
			#ifdef COLLADA_READER_DEBUG
			os::Printer::log("COLLADA: Constructing camera instance:", Id.c_str());
			#endif

			scene::ICameraSceneNode* c = mgr->addCameraSceneNodeTargetTraker(parent);
			c->setName(Name.c_str());
//			c->setUID(Id.c_str());
			c->setFOV(XFov * core::DEGTORAD / AspectRatio);
			c->setNearValue(ZNear);
			c->setFarValue(ZFar);
						
			if(pTargetPrefab)
			{
				ISceneNode *pTarget = pTargetPrefab->getInstance();
				if(pTarget == 0)
				{
					// Temporary set parent to this until his real parent is created.
					pTarget = pTargetPrefab->getInstance(parent, mgr); 
				}
				((CCameraTargetTrackerSceneNode *)c)->setTarget(pTarget);
			}

			return c;
		}

		//! returns the ids that need to be resolve before to be able to instanciate the object
		//! 0 indicate that no Id need to be resolved 
		virtual const core::stringc * getUnresolvedPrefabId()
		{
			if((pTargetPrefab == 0) && (TargetURI != ""))
			{
				return &TargetURI;
			}
			else
			{
				return 0;
			}
		}

		//! Set a prefabs for a given Id
		virtual void resolvePrefab(const core::stringc *stringId, IColladaPrefab *prefab)
		{
			if(&TargetURI == stringId)
			{
				pTargetPrefab = prefab;
			}
		}
	};


	//! prefab for a container scene node
	//! Collects other prefabs and instantiates them upon instantiation
	//! Uses a dummy scene node to return the childs as one scene node
	class CScenePrefab : public CPrefab
	{
	public:
		CScenePrefab(const core::stringc& id) 
			: CPrefab(id)
			, pInstance(0)
		{
			#ifdef COLLADA_READER_DEBUG
			os::Printer::log("COLLADA: loaded scene prefab:", Id.c_str());
			#endif
		}

		//! returns the ids that need to be resolve before to be able to instanciate the object
		//! 0 indicate that no Id need to be resolved 
		virtual const core::stringc * getUnresolvedPrefabId()
		{
			for (u32 i=0; i<Childs.size(); ++i)
			{
				const core::stringc *id = Childs[i]->getUnresolvedPrefabId();
				if(id)
					return id;
			}

			return 0;
		}

		//! Set a prefabs for a given Id
		virtual void resolvePrefab(const core::stringc *id, IColladaPrefab *prefab)
		{
			for (u32 i=0; i<Childs.size(); ++i)
			{
				Childs[i]->resolvePrefab(id, prefab);
			}
		}

		//! creates an instance of this prefab
		virtual scene::ISceneNode* addInstance(scene::ISceneNode* parent,
			scene::ISceneManager* mgr)
		{
			#ifdef COLLADA_READER_DEBUG
			os::Printer::log("COLLADA: Constructing scene instance:", Id.c_str());
			#endif

			//if (Childs.size()==0)
			//	return 0;

			scene::IDummyTransformationSceneNode* c = mgr->addDummyTransformationSceneNode(parent);
			if (c)
			{
				c->setName(Name.c_str());
//				c->setScopeID(sId.c_str());
//				c->setUID(getId().c_str());
				c->setRelativeTransformationMatrix(Transformation);
				c->updateAbsolutePosition();

				for (u32 i=0; i<Childs.size(); ++i)
				{
					scene::ISceneNode* child = Childs[i]->getInstance();
					if(child)
					{
						// set the appropriate parent
						// Bad implementation
						c->addChild(child);
						//child->setParent(c);
					}
					else
					{
						// Instanciate
						Childs[i]->getInstance(c, mgr);
					}
				}
			}

			return c;
		}



		//! returns sId of this prefab
		virtual const core::stringc& getsId()
		{
			return sId;
		}



		scene::ISceneNode* pInstance;
		core::array<IColladaPrefab*> Childs;
		core::matrix4 Transformation;
		core::stringc sId;
	};


//! Constructor
CColladaFileLoader::CColladaFileLoader(scene::ISceneManager* smgr,
		io::IFileSystem* fs)
: SceneManager(smgr), FileSystem(fs), DummyMesh(0)
, FirstLoadedMesh(0), LoadedMeshCount(0), CreateInstances(false)
, m_pAnimationLibrary(0), m_pLibraryAnimationClips(0)
{
	#ifdef _DEBUG
	setDebugName("CColladaFileLoader");
	#endif
}


//! destructor
CColladaFileLoader::~CColladaFileLoader()
{
	if (DummyMesh)
		DummyMesh->drop();

	if (FirstLoadedMesh)
	{
		FirstLoadedMesh->drop();
	}

	if(m_pAnimationLibrary)
	{
		m_pAnimationLibrary->drop();
		m_pAnimationLibrary = 0;
	}
	
	core::map<core::stringc, irr::scene::SAnimatedMesh*>::Iterator it;
	it = SMeshesToBind.getIterator();
	for (int i = 0, sz = SMeshesToBind.size(); i < sz; it++, i++)
	{
		it->getValue()->drop();
	}	
  	
}


//! Returns true if the file maybe is able to be loaded by this class.
/** This decision should be based only on the file extension (e.g. ".cob") */
bool CColladaFileLoader::isALoadableFileExtension(const c8* fileName) const
{
	return strstr(fileName, ".xml") || strstr(fileName, ".dae") || strstr(fileName, ".DAE");
}

ISceneNode *CColladaFileLoader::createScene(io::IReadFile* file)
{
	ISceneNode *pSceneRoot = irrnew scene::CEmptySceneNode( NULL, SceneManager, -1 );

	io::IXMLReaderUTF8* reader = FileSystem->createXMLReaderUTF8(file);

	if (!reader)
	{
		return 0;
	}

	m_pAnimationLibrary = irrnew CSceneNodeAnimatorChannelLibrary();
	m_pLibraryAnimationClips = irrnew CLibraryAnimationClips();

	// Relatif path base
	CurrentlyLoadingMesh = file->getFileName();

	CreateInstances = SceneManager->getParameters()->getAttributeAsBool(
		scene::COLLADA_CREATE_SCENE_INSTANCES);

	// read until COLLADA section, skip other parts
	m_file = file;

	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_ELEMENT)
		{
			if (colladaSectionName == reader->getNodeName())
				readColladaSection(reader, pSceneRoot );
			else
				skipSection(reader, true); // unknown section
		}
	}

	reader->drop();

	// Add animator to the ColladaSceneRoot even if there is no Scene instance
	// to allow the animation sharing.
	if(m_pAnimationLibrary->getChannelAnimatorsCount())
	{
		if(m_pLibraryAnimationClips->clips.size())
		{
		/*	CTimelineController *pTimeCtrl = irrnew CTimelineController;
			pTimeCtrl->setAnimationClipsLibrary(m_pLibraryAnimationClips);
			m_pAnimationLibrary->setTimelineCtrl(pTimeCtrl);
			pTimeCtrl->drop();*/
		}
		
		pSceneRoot->addAnimator(m_pAnimationLibrary);
	}

	// clean up temporary loaded data
	clearData();

	if (FirstLoadedMesh)
	{
		FirstLoadedMesh->drop();
	}

	if (m_pAnimationLibrary)
	{
		m_pAnimationLibrary->drop();
		m_pAnimationLibrary = 0;
	}

	if(m_pLibraryAnimationClips)
	{
		m_pLibraryAnimationClips->drop();
		m_pLibraryAnimationClips = 0;
	}

	FirstLoadedMesh = 0;
	LoadedMeshCount = 0;

	return pSceneRoot;
}

IAnimatedMesh *CColladaFileLoader::createMesh(io::IReadFile* file, ISceneNode *pSceneRoot )
{
	io::IXMLReaderUTF8* reader = FileSystem->createXMLReaderUTF8(file);
	if (!reader)
		return 0;

	m_pAnimationLibrary = irrnew CSceneNodeAnimatorChannelLibrary();
	m_pLibraryAnimationClips = irrnew CLibraryAnimationClips();

	CurrentlyLoadingMesh = file->getFileName();
	CreateInstances = SceneManager->getParameters()->getAttributeAsBool(
		scene::COLLADA_CREATE_SCENE_INSTANCES);

	// read until COLLADA section, skip other parts
	m_file = file;
	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_ELEMENT)
		{
			if (colladaSectionName == reader->getNodeName())
				readColladaSection(reader, pSceneRoot );
			else
				skipSection(reader, true); // unknown section
		}
	}

	reader->drop();

	scene::IAnimatedMesh* returnMesh = DummyMesh;

	// because this loader loads and creates a complete scene instead of
	// a single mesh, return an empty dummy mesh to make the scene manager
	// know that everything went well.
	if (!DummyMesh)
	{
		DummyMesh = irrnew SAnimatedMesh();
		returnMesh = DummyMesh;
	}

	// add the first loaded mesh into the mesh cache too, if more than one
	// meshes have been loaded from the file
	if (LoadedMeshCount>1 && FirstLoadedMesh)
	{
#ifdef COLLADA_READER_DEBUG
		os::Printer::log("Added COLLADA mesh", FirstLoadedMeshName.c_str());
#endif
		SceneManager->getMeshCache()->addMesh(FirstLoadedMeshName.c_str(), FirstLoadedMesh);
	}

	// Add animator to the ColladaSceneRoot even if there is no Scene instance
	// to allow the animation sharing.
	if(m_pAnimationLibrary->getChannelAnimatorsCount())
	{
		if(m_pLibraryAnimationClips->clips.size())
		{
		/*	CTimelineController *pTimeCtrl = irrnew CTimelineController;
			pTimeCtrl->setAnimationClipsLibrary(m_pLibraryAnimationClips);
			m_pAnimationLibrary->setTimelineCtrl(pTimeCtrl);
			pTimeCtrl->drop();*/
		}
		
		pSceneRoot->addAnimator(m_pAnimationLibrary);
	}



	// clean up temporary loaded data
	clearData();

	returnMesh->grab(); // store until this loader is destroyed

	DummyMesh->drop();
	DummyMesh = 0;

	if (FirstLoadedMesh)
	{
		FirstLoadedMesh->drop();
	}

	if (m_pAnimationLibrary)
	{
		m_pAnimationLibrary->drop();
		m_pAnimationLibrary = 0;
	}

	if(m_pLibraryAnimationClips)
	{
		m_pLibraryAnimationClips->drop();
		m_pLibraryAnimationClips = 0;
	}

	FirstLoadedMesh = 0;
	LoadedMeshCount = 0;

	return returnMesh;
}

//! creates/loads an animated mesh from the file.
//! \return Pointer to the created mesh. Returns 0 if loading failed.
//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
//! See IReferenceCounted::drop() for more information.
IAnimatedMesh* CColladaFileLoader::createMesh(io::IReadFile* file )
{
	ISceneNode *pSceneRoot = irrnew scene::CEmptySceneNode( SceneManager->getRootSceneNode(), SceneManager, -1 );
	return createMesh( file, pSceneRoot );
}

//! skips an (unknown) section in the collada document
void CColladaFileLoader::skipSection(io::IXMLReaderUTF8* reader, bool reportSkipping)
{
	#ifdef COLLADA_READER_DEBUG
	if (reportSkipping) // always report in COLLADA_READER_DEBUG mode
		os::Printer::log("COLLADA skipping section", core::stringc(reader->getNodeName()).c_str());
	#endif

	// skip if this element is empty anyway.
	if (reader->isEmptyElement())
		return;

	// read until we've reached the last element in this section
	u32 tagCounter = 1;

	while(tagCounter && reader->read())
	{
		if (reader->getNodeType() == io::EXN_ELEMENT &&
			!reader->isEmptyElement())
		{
			#ifdef COLLADA_READER_DEBUG
			if (reportSkipping)
				os::Printer::log("Skipping COLLADA unknown element:", core::stringc(reader->getNodeName()).c_str());
			#endif // COLLADA_READER_DEBUG

			++tagCounter;
		}
		else
		if (reader->getNodeType() == io::EXN_ELEMENT_END)
			--tagCounter;
	}
}


//! reads the <COLLADA> section and its content
void CColladaFileLoader::readColladaSection(io::IXMLReaderUTF8* reader, ISceneNode *pSceneRoot )
{
	if (reader->isEmptyElement())
		return;

	const f32 version = core::fast_atof(core::stringc(reader->getAttributeValue("version")).c_str());
	Version = core::floor32(version)*10000+core::ceil32(core::fract(version)*1000.0f);
	// Version 1.4 can be checked for by if (Version >= 10400)

	while(reader->read())
	if (reader->getNodeType() == io::EXN_ELEMENT)
	{
		if (assetSectionName == reader->getNodeName())
			readAssetSection(reader);
		else
		if (librarySectionName == reader->getNodeName())
			readLibrarySection(reader, pSceneRoot);
		else
		if (libraryNodesSectionName == reader->getNodeName())
			readLibrarySection(reader, pSceneRoot);
		else
		if (libraryAnimationsSectionName == reader->getNodeName())
			readLibrarySection(reader, pSceneRoot);
		else
		if (libraryAnimationClipsSectionName == reader->getNodeName())
			readLibrarySection(reader, pSceneRoot);
		else
		if (libraryGeometriesSectionName == reader->getNodeName())
			readLibrarySection(reader, pSceneRoot);
		else
		if (libraryControllersSectionName == reader->getNodeName())
			readLibrarySection(reader, pSceneRoot);
		else
		if (libraryMaterialsSectionName == reader->getNodeName())
			readLibrarySection(reader, pSceneRoot);
		else
		if (libraryEffectsSectionName == reader->getNodeName())
			readLibrarySection(reader, pSceneRoot);
		else
		if (libraryImagesSectionName == reader->getNodeName())
			readLibrarySection(reader, pSceneRoot);
		else
		if (libraryCamerasSectionName == reader->getNodeName())
			readLibrarySection(reader, pSceneRoot);
		else
		if (libraryLightsSectionName == reader->getNodeName())
			readLibrarySection(reader, pSceneRoot);
		else
		if (libraryVisualScenesSectionName == reader->getNodeName())
			readVisualSceneLibrary(reader, pSceneRoot);
		else
		if (assetSectionName == reader->getNodeName())
			readAssetSection(reader);
		else
		if (sceneSectionName == reader->getNodeName())
			readSceneSection(reader, pSceneRoot );
		else
		{
			os::Printer::log("COLLADA loader warning: Wrong tag usage found", reader->getNodeName(), ELL_WARNING);
			skipSection(reader, true); // unknown section
		}
	}
}


//! reads a <library> section and its content
void CColladaFileLoader::readLibrarySection(io::IXMLReaderUTF8* reader, ISceneNode *pSceneRoot )
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading library");
	#endif

	if (reader->isEmptyElement())
		return;

	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_ELEMENT)
		{
			if (animationSectionName == reader->getNodeName())
				readAnimationPrefab(reader);
			else
			if (animationClipSectionName == reader->getNodeName())
				readAnimationClipPrefab(reader);
			else
			if (cameraSectionName == reader->getNodeName())
				readCameraPrefab(reader);
			else
			// code section tbd
			if (controllerSectionName == reader->getNodeName())
				readController(reader);
			else
			if (geometrySectionName == reader->getNodeName())
				readGeometry(reader);
			else
			if (imageSectionName == reader->getNodeName())
				readImage(reader);
			else
			if (lightSectionName == reader->getNodeName())
				readLightPrefab(reader);
			else
			if (materialSectionName == reader->getNodeName())
				readMaterial(reader);
			else
			if (nodeSectionName == reader->getNodeName())
			{
				CScenePrefab p("");

				readNodeSection(reader, pSceneRoot, &p);
			}
			else
			if (effectSectionName == reader->getNodeName())
				readEffect(reader);
			else
			// program section tbd
			if (textureSectionName == reader->getNodeName())
				readTexture(reader);
			else
				skipSection(reader, true); // unknown section, not all allowed supported yet
		}
		else
		if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if (libraryAnimationsSectionName == reader->getNodeName())
				break; // end reading.
			if (libraryAnimationClipsSectionName == reader->getNodeName())
				break; // end reading.
			if (librarySectionName == reader->getNodeName())
				break; // end reading.
			if (libraryNodesSectionName == reader->getNodeName())
				break; // end reading.
			if (libraryGeometriesSectionName == reader->getNodeName())
				break; // end reading.
			if (libraryControllersSectionName == reader->getNodeName())
				break; // end reading.
			if (libraryMaterialsSectionName == reader->getNodeName())
				break; // end reading.
			if (libraryEffectsSectionName == reader->getNodeName())
				break; // end reading.
			if (libraryImagesSectionName == reader->getNodeName())
				break; // end reading.
			if (libraryLightsSectionName == reader->getNodeName())
				break; // end reading.
			if (libraryCamerasSectionName == reader->getNodeName())
				break; // end reading.
		}
	}
}


//! reads a <visual_scene> element and stores it as a prefab
void CColladaFileLoader::readVisualSceneLibrary(io::IXMLReaderUTF8* reader, ISceneNode *pSceneRoot )
{
	CScenePrefab* p = 0;
	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_ELEMENT)
		{
			if (visualSceneSectionName == reader->getNodeName())
			{
				p = irrnew CScenePrefab(readId(reader));
				p->Name = readName(reader);
			}
			else
			if (p && nodeSectionName == reader->getNodeName()) // as a child of visual_scene
				readNodeSection(reader, pSceneRoot, p);
			else
			if (assetSectionName == reader->getNodeName())
				readAssetSection(reader);
			else
			if (extraNodeName == reader->getNodeName())
				skipSection(reader, false); // ignore all other sections
			else
			{
				os::Printer::log("COLLADA loader warning: Wrong tag usage found", reader->getNodeName(), ELL_WARNING);
				skipSection(reader, true); // ignore all other sections
			}
		}
		else
		if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if (libraryVisualScenesSectionName == reader->getNodeName())
				return;
			else
			if ((visualSceneSectionName == reader->getNodeName()) && p)
			{
				/*core::matrix4 m;
				m.setRotationDegrees(core::vector3df(-90.0, 0.0, 0.0));
				p->Transformation *= m;*/
				Prefabs.push_back(p);
				p = 0;
			}
		}
	}
}


//! reads a <scene> section and its content
void CColladaFileLoader::readSceneSection(io::IXMLReaderUTF8* reader, ISceneNode *pSceneRoot )
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading scene");
	#endif

	if (reader->isEmptyElement())
		return;

	// read the scene

	core::matrix4 transform; // transformation of this node
	core::aabbox3df bbox;
	scene::IDummyTransformationSceneNode* node = 0;

	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_ELEMENT)
		{
			if (lookatNodeName == reader->getNodeName())
				transform *= readLookAtNode(reader);
			else
			if (matrixNodeName == reader->getNodeName())
				transform *= readMatrixNode(reader);
			else
			if (perspectiveNodeName == reader->getNodeName())
				transform *= readPerspectiveNode(reader);
			else
			if (rotateNodeName == reader->getNodeName())
				transform *= readRotateNode(reader);
			else
			if (scaleNodeName == reader->getNodeName())
				transform *= readScaleNode(reader);
			else
			if (skewNodeName == reader->getNodeName())
				transform *= readSkewNode(reader);
			else
			if (translateNodeName == reader->getNodeName())
				transform *= readTranslateNode(reader);
			else
			if (bboxNodeName == reader->getNodeName())
				readBboxNode(reader, bbox);
			else
			if (nodeSectionName == reader->getNodeName())
			{
				// create dummy node if there is none yet.
				if (!node)
					node = SceneManager->addDummyTransformationSceneNode( pSceneRoot );

				readNodeSection(reader, node);
			}
			else
			if ((instanceSceneName == reader->getNodeName()))
			{
				scene::ISceneNode* pVisualSceneNode = 0;
				readInstanceNode(reader, pSceneRoot, &pVisualSceneNode);
			}
			else
			if (extraNodeName == reader->getNodeName())
				skipSection(reader, false);
			else
			{
				os::Printer::log("COLLADA loader warning: Wrong tag usage found", reader->getNodeName(), ELL_WARNING);
				skipSection(reader, true); // ignore all other sections
			}
		}
		else
		if ((reader->getNodeType() == io::EXN_ELEMENT_END) &&
			(sceneSectionName == reader->getNodeName()))
				break;
	}
	if (node)
	{
		node->setRelativeTransformationMatrix(transform);
		node->updateAbsolutePosition();
	}
}


//! reads a <asset> section and its content
void CColladaFileLoader::readAssetSection(io::IXMLReaderUTF8* reader)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading asset");
	#endif

	// don't need asset data so far, so skip it
	skipSection(reader, false);
}

//! reads a <skeleton> section and its content
void CColladaFileLoader::readSkeletonSection(io::IXMLReaderUTF8* reader, core::stringc &uri)
{
	while(reader->read())
	{
		if(reader->getNodeType() == io::EXN_TEXT)
		{
			uri = reader->getNodeData();
			uriToId(uri);
		}
		else if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if(skeletonNodeName == reader->getNodeName())
			{
				break;
			}
		}
	}
}

//! reads a <instance_camera> section and its content
void CColladaFileLoader::readInstanceCameraSection(io::IXMLReaderUTF8* reader, scene::ISceneNode* parent, CScenePrefab* p)
{
	core::stringc url = reader->getAttributeValue("url");
	uriToId(url);

	CInstancePrefab *prefab = 0;
	prefab = irrnew CInstancePrefab(url);

	p->Childs.push_back(prefab);
}

//! reads a <instance_controller> section and its content
void CColladaFileLoader::readInstanceControllerSection(io::IXMLReaderUTF8* reader, scene::ISceneNode* parent, CScenePrefab* p)
{
	core::stringc url = reader->getAttributeValue("url");
	uriToId(url);

	CInstanceControllerPrefab *prefab = 0;
	prefab = irrnew CInstanceControllerPrefab(url);

	while(reader->read())
	{
		if(reader->getNodeType() == io::EXN_ELEMENT)
		{
			if(skeletonNodeName == reader->getNodeName())
			{
				core::stringc uri;
				readSkeletonSection(reader, uri);
				prefab->skeletons.push_back(uri);
			}
			else if(bindMaterialName == reader->getNodeName())
			{
				
			}
		}
		else if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if(instanceControllerName == reader->getNodeName())
			{
				break;
			}
		}
	}

	p->Childs.push_back(prefab);
}













































//! reads a <user_properties> element and returns its content
core::stringc CColladaFileLoader::readUserPropertiesNode(io::IXMLReaderUTF8* reader)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading user properties node");
	#endif

	core::stringc properties("");


	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_TEXT)
		{
			properties = reader->getNodeData();
		}
	
		else if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if (userPropertiesNodeName == reader->getNodeName())
			{
				// end of user properties section reached
				break;
			}
		}
	}
		
	return properties;
}

//! reads a <technique> node section and its content within an <extra> node section
void CColladaFileLoader::readExtraTechnique(io::IXMLReaderUTF8* reader, const core::stringc & id)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading node technique within node extra");
	#endif

	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_ELEMENT)
		{
			if (userPropertiesNodeName == reader->getNodeName())
			{
				if(SMeshesToBind.find(id))
				{
					scene::SAnimatedMesh* tmpMesh = SMeshesToBind[id];
					tmpMesh->userProperties = readUserPropertiesNode(reader);
				}
			}
			else
				skipSection(reader, true); // ignore all other sections
			
		}
		else
		if ((reader->getNodeType() == io::EXN_ELEMENT_END) &&
			(techniqueNodeName == reader->getNodeName()))
		{
				return;
		}
	}
}

//! reads an <extra> node section and its content within a <node> section
void CColladaFileLoader::readExtraNode(io::IXMLReaderUTF8* reader, const core::stringc & id)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading node extra within node section");
	#endif

	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_ELEMENT)
		{
			if (techniqueNodeName == reader->getNodeName())
			{
				readExtraTechnique(reader, id);
			}
			else
				skipSection(reader, true); // ignore all other sections
		}
		else
		if ((reader->getNodeType() == io::EXN_ELEMENT_END) &&
			(extraNodeName == reader->getNodeName()))
				return;
	}
}

//! reads a <node> section and its content
void CColladaFileLoader::readNodeSection(io::IXMLReaderUTF8* reader, scene::ISceneNode* parent, CScenePrefab* p)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading node");
	#endif

	if (reader->isEmptyElement())
		return;

	// find node id
	core::stringc id = readId(reader);

	core::stringc name = readId(reader);
	core::stringc sId = readsId(reader);
	core::stringc realName = readName(reader);

	core::matrix4 transform; // transformation of this node
	core::aabbox3df bbox;
	scene::ISceneNode* node = 0; // instance
	CScenePrefab* nodeprefab = 0; // prefab for library_nodes usage

	if (p)
	{
		nodeprefab = irrnew CScenePrefab(name);
		nodeprefab->Name = realName;
		nodeprefab->sId = sId;

		p->Childs.push_back(nodeprefab);
		Prefabs.push_back(nodeprefab); // in order to delete them later on
	}

	// read the node

	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_ELEMENT)
		{
			if (assetSectionName == reader->getNodeName())
				readAssetSection(reader);
			else
			if (lookatNodeName == reader->getNodeName())
				transform *= readLookAtNode(reader);
			else
			if (matrixNodeName == reader->getNodeName())
				transform *= readMatrixNode(reader);
			else
			if (perspectiveNodeName == reader->getNodeName())
				transform *= readPerspectiveNode(reader);
			else
			if (rotateNodeName == reader->getNodeName())
				transform *= readRotateNode(reader);
			else
			if (scaleNodeName == reader->getNodeName())
				transform *= readScaleNode(reader);
			else
			if (skewNodeName == reader->getNodeName())
				transform *= readSkewNode(reader);
			else
			if (translateNodeName == reader->getNodeName())
				transform *= readTranslateNode(reader);
			else
			if (bboxNodeName == reader->getNodeName())
				readBboxNode(reader, bbox);
			else
			if (instanceControllerName == reader->getNodeName())
			{
				readInstanceControllerSection(reader, parent, nodeprefab);
			}
			else
			if (instanceCamera == reader->getNodeName())
			{
				readInstanceCameraSection(reader, parent, nodeprefab);
			}
			else
			if ((instanceName == reader->getNodeName()) ||
				(instanceNodeName == reader->getNodeName()) ||
				(instanceGeometryName == reader->getNodeName()) ||
				(instanceLightName == reader->getNodeName()))
			{
				scene::ISceneNode* newnode = 0;
				readInstanceNode(reader, parent, &newnode, nodeprefab);

				if (node && newnode)
				{
					// move children from dummy to new node
					core::list<ISceneNode*>::ConstIterator it = node->getChildren().begin();
					for (; it != node->getChildren().end(); it = node->getChildren().begin())
					{
						newnode->addChild(*it);
						//(*it)->setParent(newnode);
					}

					// remove previous dummy node
					node->remove();
				}

				node = newnode;
			}
			else
			if (nodeSectionName == reader->getNodeName())
			{
				// create dummy node if there is none yet.
				if (!node)
				{
					scene::IDummyTransformationSceneNode* dummy =
						SceneManager->addDummyTransformationSceneNode(parent);
					dummy->setName(realName.c_str());
					dummy->setRelativeTransformationMatrix(transform);
					dummy->updateAbsolutePosition();
					node = dummy;

					if ( CreateInstances )
					{
						ToRemove.push_back( dummy );
					}
				}

				// read and add child
				readNodeSection(reader, node, nodeprefab);
			}
			else
			if (extraNodeName == reader->getNodeName())
			{
				// find mesh id corresponding to node id
				core::stringc meshId = id.subString(0, id.find("-node"));
				meshId.append("-mesh");
				readExtraNode(reader, meshId);
			}
			else
				skipSection(reader, true); // ignore all other sections

		} // end if node
		else
		if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if (nodeSectionName == reader->getNodeName())
				break;
		}
	}

	if(nodeprefab)
	{
		nodeprefab->Transformation = transform;
	}
	else if (p)
	{
		p->Transformation = transform;
	}
}


//! reads a <lookat> element and its content and creates a matrix from it
core::matrix4 CColladaFileLoader::readLookAtNode(io::IXMLReaderUTF8* reader)
{
	core::matrix4 mat;
	if (reader->isEmptyElement())
		return mat;

	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading look at node");
	#endif

	f32 floats[9];
	readFloatsInsideElement(reader, floats, 9);

	mat.buildCameraLookAtMatrix(
		core::vector3df(floats[0], floats[1], floats[2]),
		core::vector3df(floats[3], floats[4], floats[5]),
		core::vector3df(floats[6], floats[7], floats[8]));

	return mat;
}


//! reads a <skew> element and its content and creates a matrix from it
core::matrix4 CColladaFileLoader::readSkewNode(io::IXMLReaderUTF8* reader)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading skew node");
	#endif

	core::matrix4 mat;
	if (reader->isEmptyElement())
		return mat;

	f32 floats[7]; // angle rotation-axis translation-axis
	readFloatsInsideElement(reader, floats, 7);

	// build skew matrix from these 7 floats
	core::quaternion q;
	q.fromAngleAxis(floats[0]*core::DEGTORAD, core::vector3df(floats[1], floats[2], floats[3]));
	q.getMatrix(mat);

	if (floats[4]==1.f) // along x-axis
	{
		mat[4]=0.f;
		mat[6]=0.f;
		mat[8]=0.f;
		mat[9]=0.f;
	}
	else
	if (floats[5]==1.f) // along y-axis
	{
		mat[1]=0.f;
		mat[2]=0.f;
		mat[8]=0.f;
		mat[9]=0.f;
	}
	else
	if (floats[6]==1.f) // along z-axis
	{
		mat[1]=0.f;
		mat[2]=0.f;
		mat[4]=0.f;
		mat[6]=0.f;
	}

	return mat;
}


//! reads a <boundingbox> element and its content and stores it in bbox
void CColladaFileLoader::readBboxNode(io::IXMLReaderUTF8* reader,
		core::aabbox3df& bbox)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading boundingbox node");
	#endif

	bbox.reset(core::aabbox3df());

	if (reader->isEmptyElement())
		return;

	f32 floats[3];

	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_ELEMENT)
		{
			if (minNodeName == reader->getNodeName())
			{
				readFloatsInsideElement(reader, floats, 3);
				bbox.MinEdge.set(floats[0], floats[1], floats[2]);
			}
			else
			if (maxNodeName == reader->getNodeName())
			{
				readFloatsInsideElement(reader, floats, 3);
				bbox.MaxEdge.set(floats[0], floats[1], floats[2]);
			}
			else
				skipSection(reader, true); // ignore all other sections
		}
		else
		if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if (bboxNodeName == reader->getNodeName())
				break;
		}
	}
}


//! reads a <matrix> element and its content and creates a matrix from it
core::matrix4 CColladaFileLoader::readMatrixNode(io::IXMLReaderUTF8* reader)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading matrix node");
	#endif

	core::matrix4 mat;
	if (reader->isEmptyElement())
		return mat;

	readFloatsInsideElement(reader, mat.pointer(), 16);
	// put translation into the correct place
	mat = mat.getTransposed();

	return mat;
}


//! reads a <perspective> element and its content and creates a matrix from it
core::matrix4 CColladaFileLoader::readPerspectiveNode(io::IXMLReaderUTF8* reader)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading perspective node");
	#endif

	core::matrix4 mat;
	if (reader->isEmptyElement())
		return mat;

	f32 floats[1];
	readFloatsInsideElement(reader, floats, 1);

	// TODO: build perspecitve matrix from this float

	os::Printer::log("COLLADA loader warning: <perspective> not implemented yet.", ELL_WARNING);

	return mat;
}


//! reads a <rotate> element and its content and creates a matrix from it
core::matrix4 CColladaFileLoader::readRotateNode(io::IXMLReaderUTF8* reader)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading rotate node");
	#endif

	core::matrix4 mat;
	if (reader->isEmptyElement())
		return mat;

	f32 floats[4];
	readFloatsInsideElement(reader, floats, 4);

	if (!core::iszero(floats[3]))
	{
		core::quaternion q;
		q.fromAngleAxis(floats[3]*core::DEGTORAD, core::vector3df(floats[0], floats[1], floats[2]));
		return q.getMatrix();
	}
	else
		return core::IdentityMatrix;
}


//! reads a <scale> element and its content and creates a matrix from it
core::matrix4 CColladaFileLoader::readScaleNode(io::IXMLReaderUTF8* reader)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading scale node");
	#endif

	core::matrix4 mat;
	if (reader->isEmptyElement())
		return mat;

	f32 floats[3];
	readFloatsInsideElement(reader, floats, 3);

	mat.setScale(core::vector3df(floats[0], floats[1], floats[2]));

	return mat;
}


//! reads a <translate> element and its content and creates a matrix from it
core::matrix4 CColladaFileLoader::readTranslateNode(io::IXMLReaderUTF8* reader)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading translate node");
	#endif

	core::matrix4 mat;
	if (reader->isEmptyElement())
		return mat;

	f32 floats[3];
	readFloatsInsideElement(reader, floats, 3);

	mat.setTranslation(core::vector3df(floats[0], floats[1], floats[2]));

	return mat;
}

IColladaPrefab *CColladaFileLoader::findPrefab(const core::stringc *id) const
{
	for (u32 i=0; i<Prefabs.size(); ++i)
	{
		if ((*id == Prefabs[i]->getId())
			|| (*id == Prefabs[i]->getsId())
			|| (*id == Prefabs[i]->getName()))
		{
			return Prefabs[i]; 
		}
	}

	return 0;
}


//! reads any kind of <instance*> node and creates a scene node from it
void CColladaFileLoader::readInstanceNode(io::IXMLReaderUTF8* reader, scene::ISceneNode* parent,
	scene::ISceneNode** outNode, CScenePrefab* p)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading instance");
	#endif

	// find prefab of the specified id
	core::stringc url = reader->getAttributeValue("url");
	uriToId(url);

	if (!reader->isEmptyElement())
	{
		while(reader->read())
		{
			if (reader->getNodeType() == io::EXN_ELEMENT)
			{
				if (skeletonNodeName == reader->getNodeName())
				{
					skipSection(reader, false);
				}
				else if (bindMaterialName == reader->getNodeName())
				{
					readBindMaterialSection(reader,url);
				}
				else
				if (extraNodeName == reader->getNodeName())
					readExtraNode(reader, url);
			}
			else
			if (reader->getNodeType() == io::EXN_ELEMENT_END)
				break;
		}
	}

	for (u32 i=0; i<Prefabs.size(); ++i)
	{
		if (url == Prefabs[i]->getId())
		{
			const core::stringc *id = 0;
			while(id = Prefabs[i]->getUnresolvedPrefabId())
			{
				IColladaPrefab *prefab = findPrefab(id);
				_IRR_DEBUG_BREAK_IF(!prefab);
				if(!prefab)
				{
					
					UNIMPLEMENTED("Unknown prefab");
					break;
				}
				Prefabs[i]->resolvePrefab(id, prefab);
			}

			if (p)
			{
				p->Childs.push_back(Prefabs[i]);
			}
			else if (CreateInstances)
			{
				scene::ISceneNode * newNode
					= Prefabs[i]->getInstance(parent, SceneManager);
				
				if(newNode)
				{
					newNode->setName(readId(reader).c_str());
					//newNode->LogTree();
				}

				if (outNode)
				{
					*outNode = newNode;
				}
			}
			return;
		}
	}
}

//! reads a <animation_clip> element and stores it as prefab
void CColladaFileLoader::readAnimationClipPrefab(io::IXMLReaderUTF8* reader)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading animation clip");
	#endif

	CAnimationClip clip;
	clip.id = readId(reader);
	clip.start = static_cast<int>(reader->getAttributeValueAsFloat("start") * 1000);
	clip.end = static_cast<int>(reader->getAttributeValueAsFloat("end") * 1000);
	m_pLibraryAnimationClips->clips.push_back(clip);

	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if(animationClipSectionName == reader->getNodeName())
			{
				break;
			} 
		}
	}
}

//! reads a <animation> element and stores it as prefab
void CColladaFileLoader::readAnimationPrefab(io::IXMLReaderUTF8* reader)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading animation ");
	#endif

	CAnimationPrefab* prefab = irrnew CAnimationPrefab(readId(reader));

	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_ELEMENT)
		{
			if(sourceSectionName == reader->getNodeName())
			{
				SSource sId;
				readSource(reader, sId);
				prefab->sources.push_back(sId);
			} 
			else if(samplerSectionName == reader->getNodeName())
			{
				readSampler(reader, prefab->sampler);
			}
			else if(channelSectionName == reader->getNodeName())
			{
				prefab->channel.sourceURI = reader->getAttributeValue("source");
				prefab->channel.targetURI = reader->getAttributeValue("target");
			}
		}
		else if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if(animationSectionName == reader->getNodeName())
			{
				break;
			} 
		}
	}

	CAnimationTrack *pAnimationTrack = 0;
	
	
	// Retrieve output arrays
	{
		const core::stringc *outputURI = prefab->sampler.getSourceURI("OUTPUT");
		SSource* source = prefab->sources.getSource(*outputURI);
		_IRR_DEBUG_BREAK_IF(source->Array->type != ISourceArray::FLOAT);

		if(source->Accessors[0].Stride == 1)
		{
			pAnimationTrack = irrnew CFloatLerpAnimationTrack();
			((CFloatLerpAnimationTrack*)pAnimationTrack)->setOutput(((SFloatArray *)source->Array)->Data);
		}
		else if(source->Accessors[0].Stride == 16)
		{
			pAnimationTrack = irrnew CPosDirLerpAnimationTrack();
			((CPosDirLerpAnimationTrack*)pAnimationTrack)->setOutput(((SFloatArray *)source->Array)->Data);
		}
		else
		{
			pAnimationTrack = irrnew CFloatLerpAnimationTrack();
			((CFloatLerpAnimationTrack*)pAnimationTrack)->setOutput(((SFloatArray *)source->Array)->Data);
			//_IRR_DEBUG_BREAK_IF("Not implemented");
		}

		
	}

	//pAnimationTrack->setBindObjURI(prefab->channel.targetURI);

	// Retrive time arrays
	{
		const core::stringc *inputURI = prefab->sampler.getSourceURI("INPUT");
		SSource* source = prefab->sources.getSource(*inputURI);
		_IRR_DEBUG_BREAK_IF(source->Array->type != ISourceArray::FLOAT);
		pAnimationTrack->setInput(((SFloatArray *)source->Array)->Data);
	}

	


	CChannelAnimator *pAnimator = irrnew CChannelAnimator(pAnimationTrack);
	pAnimationTrack->drop();
	

	_IRR_DEBUG_BREAK_IF(m_pAnimationLibrary == 0);

	m_pAnimationLibrary->addAnimator(pAnimator);
	pAnimator->drop();

	Prefabs.push_back(prefab);
}


//! reads a camera <perspective> element and stores it in a Camera prefab
void CColladaFileLoader::readCameraPerspective(io::IXMLReaderUTF8* reader, CCameraPrefab* prefab)
{
	while(reader->read())
	{
		if(reader->getNodeType() == io::EXN_ELEMENT)
		{
			if(xfovNodeName == reader->getNodeName())
			{
				f32 xfov;
				readFloatsInsideElement(reader, &xfov, 1);
				prefab->XFov = xfov;
			} else if(aspectRatioNodeName == reader->getNodeName())
			{
				f32 aspectRatio;
				readFloatsInsideElement(reader, &aspectRatio, 1);
				prefab->AspectRatio = aspectRatio;
			} else if(znearNodeName == reader->getNodeName())
			{
				f32 znear;
				readFloatsInsideElement(reader, &znear, 1);
				prefab->ZNear = znear;
			} else if(zfarNodeName == reader->getNodeName())
			{
				f32 zfar;
				readFloatsInsideElement(reader, &zfar, 1);
				prefab->ZFar = zfar;
			}
		}
		else if(reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if(perspectiveSectionName == reader->getNodeName())
			{
				break;
			}
		}
	}
}

//! reads a camera optics <technique_common> element and stores it in a Camera prefab
void CColladaFileLoader::readCameraTechniqueCommon(io::IXMLReaderUTF8* reader, CCameraPrefab* prefab)
{
	while(reader->read())
	{
		if(reader->getNodeType() == io::EXN_ELEMENT)
		{
			if(perspectiveSectionName == reader->getNodeName())
			{
				readCameraPerspective(reader, prefab);
			}
		}
		else if(reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if(techniqueCommonSectionName == reader->getNodeName())
			{
				break;
			}
		}
	}
}

//! read a <optics> element and stores it in a Camera prefab
void CColladaFileLoader::readCameraOptics(io::IXMLReaderUTF8* reader, CCameraPrefab* prefab)
{
	while(reader->read())
	{
		if(reader->getNodeType() == io::EXN_ELEMENT)
		{
			if(techniqueCommonSectionName == reader->getNodeName())
			{
				readCameraTechniqueCommon(reader, prefab);
			}
		}
		else if(reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if(opticsSectionName == reader->getNodeName())
			{
				break;
			}
		}
	}
}

//! read a <target> element and stores it in a Camera Prefab
void CColladaFileLoader::readCameraTarget(io::IXMLReaderUTF8* reader, CCameraPrefab* prefab)
{
	while(reader->read())
	{
		if(reader->getNodeType() == io::EXN_TEXT)
		{
			prefab->TargetURI = reader->getNodeName();
			uriToId(prefab->TargetURI);
		}
		else if(reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if(targetSectionName == reader->getNodeName())
			{
				break;
			}
		}
	}
}

//! reads a <camera> element and stores it as prefab
void CColladaFileLoader::readCameraPrefab(io::IXMLReaderUTF8* reader)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading camera prefab");
	#endif

	CCameraPrefab* prefab = irrnew CCameraPrefab(readId(reader));

	while(reader->read())
	{
		if(reader->getNodeType() == io::EXN_ELEMENT)
		{
			if(opticsSectionName == reader->getNodeName())
			{
				readCameraOptics(reader, prefab);
			}
			else if(targetSectionName == reader->getNodeName())
			{
				readCameraTarget(reader, prefab);
			}
		}
		else if(reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if(cameraSectionName == reader->getNodeName())
			{
				break;
			}
		}
		
	}

	Prefabs.push_back(prefab);
}


//! reads a <image> element and stores it in the image section
void CColladaFileLoader::readImage(io::IXMLReaderUTF8* reader)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading image");
	#endif

	// add image to list of loaded images.
	Images.push_back(SColladaImage());
	SColladaImage& image=Images.getLast();

	image.Id = readId(reader);
	image.Dimension.Height = (u32)reader->getAttributeValueAsInt("height");
	image.Dimension.Width = (u32)reader->getAttributeValueAsInt("width");

	if (Version >= 10400) // start with 1.4
	{
		while(reader->read())
		{
			if (reader->getNodeType() == io::EXN_ELEMENT)
			{
				if (assetSectionName == reader->getNodeName())
					skipSection(reader, false);
				else
				if (initFromName == reader->getNodeName())
				{
					reader->read();
					image.Source = reader->getNodeData();
					image.Source.trim();
					image.SourceIsFilename=true;
				}
				else
				if (dataName == reader->getNodeName())
				{
					reader->read();
					image.Source = reader->getNodeData();
					image.Source.trim();
					image.SourceIsFilename=false;
				}
				else
				if (extraNodeName == reader->getNodeName())
					skipSection(reader, false);
			}
			else
			if (reader->getNodeType() == io::EXN_ELEMENT_END)
			{
				if (initFromName == reader->getNodeName())
					return;
			}
		}
	}
	else
	{
		image.Source = reader->getAttributeValue("source");
		image.Source.trim();
		image.SourceIsFilename=false;
	}
}


//! reads a <texture> element and stores it in the texture section
void CColladaFileLoader::readTexture(io::IXMLReaderUTF8* reader)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading texture");
	#endif

	// add texture to list of loaded textures.
	Textures.push_back(SColladaTexture());
	SColladaTexture& texture=Textures.getLast();

	texture.Id = readId(reader);

	if (!reader->isEmptyElement())
	{
		readColladaInputs(reader, textureSectionName);
		SColladaInput* input = getColladaInput(ECIS_IMAGE);
		if (input)
		{
			const core::stringc imageName = input->Source;
			texture.Texture = getTextureFromImage(imageName);
		}
	}
}


//! reads a <material> element and stores it in the material section
void CColladaFileLoader::readMaterial(io::IXMLReaderUTF8* reader)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading material");
	#endif

	SColladaMaterial material;
	material.Id = readId(reader);
	material.Additive = false;
	if (Version >= 10400)
	{
		while(reader->read())
		{
			if (reader->getNodeType() == io::EXN_ELEMENT &&
				instanceEffectName == reader->getNodeName())
			{
				material.InstanceEffectId = reader->getAttributeValue("url");
				uriToId(material.InstanceEffectId);
			}
			else
			if (reader->getNodeType() == io::EXN_ELEMENT &&
				setparamName == reader->getNodeName())
			{
				if (irrlichtAdditiveName == reader->getAttributeValue("ref"))
				{
					while(reader->read())
					{
						if (reader->getNodeType() == io::EXN_ELEMENT)
						{
							s32 additive = 0;
							readIntsInsideElement(reader,&additive,1);
							material.Additive = additive;
						}
						else
						if (reader->getNodeType() == io::EXN_ELEMENT_END &&
							setparamName == reader->getNodeName())
						{
							break;
						}
					}
				}
			}
			else
			if (reader->getNodeType() == io::EXN_ELEMENT_END &&
				materialSectionName == reader->getNodeName())
			{
				break;
			}
		} // end while reader->read();
	}
	else
	{
		if (!reader->isEmptyElement())
		{
			readColladaInputs(reader, materialSectionName);
			SColladaInput* input = getColladaInput(ECIS_TEXTURE);
			if (input)
			{
				core::stringc textureName = input->Source;
				uriToId(textureName);
				for (u32 i=0; i<Textures.size(); ++i)
					if (textureName == Textures[i].Id)
					{
						material.Mat.setTexture(0, Textures[i].Texture);
						break;
					}
			}

			//does not work because the wrong start node is chosen due to reading of inputs before
#if 0
			readColladaParameters(reader, materialSectionName);

			SColladaParam* p;

			p = getColladaParameter(ECPN_AMBIENT);
			if (p && p->Type == ECPT_FLOAT3)
				material.Mat.AmbientColor = video::SColorf(p->Floats[0],p->Floats[1],p->Floats[2]).toSColor();
			p = getColladaParameter(ECPN_DIFFUSE);
			if (p && p->Type == ECPT_FLOAT3)
				material.Mat.DiffuseColor = video::SColorf(p->Floats[0],p->Floats[1],p->Floats[2]).toSColor();
			p = getColladaParameter(ECPN_SPECULAR);
			if (p && p->Type == ECPT_FLOAT3)
				material.Mat.DiffuseColor = video::SColorf(p->Floats[0],p->Floats[1],p->Floats[2]).toSColor();
			p = getColladaParameter(ECPN_SHININESS);
			if (p && p->Type == ECPT_FLOAT)
				material.Mat.Shininess = p->Floats[0];
#endif
		}
	}

	// add material to list of loaded materials.
	Materials.push_back(material);
}



void CColladaFileLoader::readEffect(io::IXMLReaderUTF8* reader, SColladaEffect * effect)
{
	#ifdef COLLADA_READER_DEBUG
	if (!effect) os::Printer::log("COLLADA reading effect");
	#endif

	static const core::stringc constantNode("constant");
	static const core::stringc lambertNode("lambert");
	static const core::stringc phongNode("phong");
	static const core::stringc blinnNode("blinn");
	static const core::stringc emissionNode("emission");
	static const core::stringc ambientNode("ambient");
	static const core::stringc diffuseNode("diffuse");
	static const core::stringc specularNode("specular");
	static const core::stringc shininessNode("shininess");
	static const core::stringc reflectiveNode("reflective");
	static const core::stringc reflectivityNode("reflectivity");
	static const core::stringc transparentNode("transparent");
	static const core::stringc transparencyNode("transparency");
	static const core::stringc indexOfRefractionNode("index_of_refraction");

	if (!effect)
	{
		Effects.push_back(SColladaEffect());
		effect = &Effects.getLast();
		effect->Id = readId(reader);
		effect->Transparency = 1.f;
	}
	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_ELEMENT)
		{
			// first come the tags we descend, but ignore the top-levels
			if (!reader->isEmptyElement() && ((profileCOMMONSectionName == reader->getNodeName()) ||
				(techniqueNodeName == reader->getNodeName())))
				readEffect(reader,effect);
			else
			if (newParamName == reader->getNodeName())
				readParameter(reader);
			else
			// these are the actual materials inside technique
			if (constantNode == reader->getNodeName() ||
				lambertNode == reader->getNodeName() ||
				phongNode == reader->getNodeName() ||
				blinnNode == reader->getNodeName())
			{
				#ifdef COLLADA_READER_DEBUG
				os::Printer::log("COLLADA reading effect part", reader->getNodeName());
				#endif
				effect->Mat.setFlag(irr::video::EMF_GOURAUD_SHADING,
					phongNode == reader->getNodeName() ||
					blinnNode == reader->getNodeName());
				while(reader->read())
				{
					if (reader->getNodeType() == io::EXN_ELEMENT)
					{
						const core::stringc node = reader->getNodeName();
						if (emissionNode == node || ambientNode == node ||
							diffuseNode == node || specularNode == node ||
							reflectiveNode == node || transparentNode == node )
						{
							if( transparentNode == node )
							{
								if (core::stringc("RGB_ZERO") == reader->getAttributeValue("opaque"))
								{
									effect->Mat.MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
								}
							}
							// color or texture types
							while(reader->read())
							{
								if (reader->getNodeType() == io::EXN_ELEMENT &&
									colorNodeName == reader->getNodeName())

								{
									const video::SColorf colorf = readColorNode(reader);
									const video::SColor color = colorf.toSColor();
									if (emissionNode == node)
										effect->Mat.EmissiveColor = color;
									else
									if (ambientNode == node)
										effect->Mat.AmbientColor = color;
									else
									if (diffuseNode == node)
										effect->Mat.DiffuseColor = color;
									else
									if (specularNode == node)
										effect->Mat.SpecularColor = color;
									else
									if (transparentNode == node)
										effect->Transparency = colorf.a;
								}
								else
								if (reader->getNodeType() == io::EXN_ELEMENT &&
									textureNodeName == reader->getNodeName() &&
									transparentNode == node)
								{
									const core::stringc tname = reader->getAttributeValue("texture");
									effect->Mat.MaterialType = irr::video::EMT_TRANSPARENT_TEXTURE_VERTEX_ALPHA;
									break;
								}
								else
								if (reader->getNodeType() == io::EXN_ELEMENT &&
									textureNodeName == reader->getNodeName())
								{
									const core::stringc tname = reader->getAttributeValue("texture");
									effect->Mat.setTexture(0, getTextureFromImage(tname));
									break;
								}
								else
								if (reader->getNodeType() == io::EXN_ELEMENT)
									skipSection(reader, false);
								else
								if (reader->getNodeType() == io::EXN_ELEMENT_END &&
									node == reader->getNodeName())
									break;
							}
						}
						else
						if (shininessNode == node || reflectivityNode == node ||
							transparencyNode == node || indexOfRefractionNode == node )
						{
							// float or param types
							while(reader->read())
							{
								if (reader->getNodeType() == io::EXN_ELEMENT &&
									floatNodeName == reader->getNodeName())
								{
									f32 f = readFloatNode(reader);
									if (shininessNode == node)
									{

										float s = 0.0f;

										if (core::abs_(f - 0.415939) > 0.001)
										{									
											f = core::clamp(f, 0.0001f, 1.0f);
											if (f >= 0.36f) s = expf((f - 0.0226f) / -0.157f);
											if (f <= 0.45f)
											{
												float factor = core::max_(0.0f, (f - 0.36f) / (0.45f - 0.36f));
												float p = logf(f / 0.64893f) / -3.3611f;
												s = s * factor + (1.0f - factor) * p;
											}
											// interpolate over opengl range	
											s = static_cast<f32>(core::round32(s * 128.0f));
										}

										effect->Mat.Shininess = s;
										//effect->Mat.Shininess = 0;//f;
										//UNIMPLEMENTED("Something don't work with opengl and shininess");
									}
									else
									if (transparencyNode == node)
										effect->Transparency *= f;
								}
								else
								if (reader->getNodeType() == io::EXN_ELEMENT)
									skipSection(reader, false);
								else
								if (reader->getNodeType() == io::EXN_ELEMENT_END &&
									node == reader->getNodeName())
									break;
							}
						}
						else
							skipSection(reader, true); // ignore all other nodes
					}
					else
					if (reader->getNodeType() == io::EXN_ELEMENT_END && (
						constantNode == reader->getNodeName() ||
						lambertNode == reader->getNodeName() ||
						phongNode == reader->getNodeName() ||
						blinnNode == reader->getNodeName()
						))
						break;
				}
				
			}
			else
			if (!reader->isEmptyElement() && (extraNodeName == reader->getNodeName()))
				readEffect(reader,effect);
			else
			if (doubleSidedName == reader->getNodeName())
			{
				// read the GoogleEarth extra flag for double sided polys
				s32 doubleSided = 0;
				readIntsInsideElement(reader,&doubleSided,1);
				if (doubleSided)
				{
					effect->Mat.setFlag(irr::video::EMF_BACK_FACE_CULLING,false);
				}
			}
			else
				skipSection(reader, true); // ignore all other sections
		}
		else
		if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if (effectSectionName == reader->getNodeName())
				break;
			else
			if (profileCOMMONSectionName == reader->getNodeName())
				break;
			else
			if (techniqueNodeName == reader->getNodeName())
				break;
			else
			if (extraNodeName == reader->getNodeName())
				break;
		}
	}

	if (effect->Mat.AmbientColor == video::SColor(0,0,0,0) &&
		effect->Mat.DiffuseColor != video::SColor(0,0,0,0))
		effect->Mat.AmbientColor = effect->Mat.DiffuseColor;
	if (effect->Mat.DiffuseColor == video::SColor(0,0,0,0) &&
		effect->Mat.AmbientColor != video::SColor(0,0,0,0))
		effect->Mat.DiffuseColor = effect->Mat.AmbientColor;
	if (effect->Transparency != 1.0f)
		effect->Mat.MaterialType = irr::video::EMT_TRANSPARENT_VERTEX_ALPHA;
}


const SColladaMaterial * CColladaFileLoader::findMaterial(const core::stringc & materialName)
{
	// do a quick lookup in the materials
	SColladaMaterial matToFind;
	matToFind.Id = materialName;
	s32 mat = Materials.binary_search(matToFind);
	if (mat == -1)
		return 0;
	// instantiate the material effect if needed
	if (Materials[mat].InstanceEffectId.size() > 0)
	{
		// do a quick lookup in the effects
		SColladaEffect effectToFind;
		effectToFind.Id = Materials[mat].InstanceEffectId;
		s32 effect = Effects.binary_search(effectToFind);
		if (effect != -1)
		{
			// found the effect, instantiate by copying into the material
			Materials[mat].Mat = Effects[effect].Mat;
			Materials[mat].Transparency = Effects[effect].Transparency;
			// and indicate the material is instantiated by removing the effect ref
			Materials[mat].InstanceEffectId = "";

			if (Materials[mat].Additive)
				Materials[mat].Mat.MaterialType = irr::video::EMT_TRANSPARENT_ADD_COLOR;
		}
		else
			return 0;
	}
	return &Materials[mat];
}


void CColladaFileLoader::readBindMaterialSection(io::IXMLReaderUTF8* reader, const core::stringc & id)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading bind material");
	#endif

	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_ELEMENT)
		{
			if (instanceMaterialName == reader->getNodeName())
			{
				// the symbol to retarget, and the target material
				core::stringc materialReference = reader->getAttributeValue("symbol");
				if (materialReference.size()==0)
					continue;
				core::stringc target = reader->getAttributeValue("target");
				uriToId(target);
				if (target.size()==0)
					continue;
				const SColladaMaterial * material = findMaterial(target);
				if (!material)
					continue;
				// bind any pending materials for this node
				materialReference = id+"/"+materialReference;
#ifdef COLLADA_READER_DEBUG
				os::Printer::log((core::stringc("Material binding: ")+materialReference+" "+target).c_str());
#endif
				if (MaterialsToBind.find(materialReference))
				{
					core::array<irr::scene::IMeshBuffer*> & toBind
						= MeshesToBind[MaterialsToBind[materialReference]];
					SMesh tmpmesh;
					for (u32 i = 0; i < toBind.size(); ++i)
					{
						toBind[i]->getMaterial() = material->Mat;
						if (material->Transparency!=1.0f)
							tmpmesh.addMeshBuffer(toBind[i]);
					}
					if (material->Transparency!=1.0f)
					{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA found transparency material", core::stringc(material->Transparency).c_str());
	#endif
						SceneManager->getMeshManipulator()->setVertexColorAlpha(&tmpmesh, core::floor32(material->Transparency*255.0f));
					}
				}
			}
		}
		else
		if (reader->getNodeType() == io::EXN_ELEMENT_END &&
			bindMaterialName == reader->getNodeName())
			break;
	}
}

void CColladaFileLoader::readFloatArray(core::stringc &data, core::array<f32> &outArray, int iCount)
{
	const c8* p = &data[0];

	outArray.reallocate(iCount);
	outArray.set_used(iCount);

	for (u32 i=0; i<outArray.size(); ++i)
	{
		findNextNoneWhiteSpace(&p);
		if (*p)
			outArray[i] = readFloat(&p);
		else
			outArray[i] = 0.0f;
	}
}

void CColladaFileLoader::readIntArray(core::stringc &data, core::array<s32> &outArray, int iCount)
{
	const c8* p = &data[0];

	outArray.reallocate(iCount);
	outArray.set_used(iCount);

	for (u32 i=0; i<outArray.size(); ++i)
	{
		findNextNoneWhiteSpace(&p);
		if (*p)
			outArray[i] = readInt(&p);
		else
			outArray[i] = 0;
	}
}




void CColladaFileLoader::readIdRefArray(core::stringc &data, core::array<core::stringc> &outArray, int iCount)
{
	s32 beginPos = 0;
	s32 nextPos = data.findFirst(' ');

	outArray.reallocate(iCount);
	outArray.set_used(iCount);

	while(nextPos != -1)
	{
		outArray.push_back(data.subString(beginPos, nextPos - beginPos));
		beginPos = nextPos + 1;
		nextPos = data.findNext(' ', beginPos);
	}
}

void CColladaFileLoader::readFloatArray(io::IXMLReaderUTF8* reader, ISourceArray **outArray)
{
	if(*outArray)
	{
		delete outArray;
	}

	SFloatArray *idArray = irrnew SFloatArray();
	*outArray = idArray;

	idArray->Name = readId(reader);

	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_TEXT)
		{
			core::stringc data = reader->getNodeData();
			readFloatArray(data, idArray->Data, reader->getAttributeValueAsInt("count"));

			// TODO: Read SAccessor 
		}
		else if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if (floatArraySectionName == reader->getNodeName())
			{
				// end of geometry section reached, cancel out
				break;
			}
		}
	}
}

void CColladaFileLoader::readIntArray(io::IXMLReaderUTF8* reader, ISourceArray **outArray)
{
	if(*outArray)
	{
		delete outArray;
	}

	SIntArray *idArray = irrnew SIntArray();
	*outArray = idArray;

	idArray->Name = readId(reader);

	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_TEXT)
		{
			core::stringc data = reader->getNodeData();
			readIntArray(data, idArray->Data, reader->getAttributeValueAsInt("count"));

			// TODO: Read SAccessor 
		}
		else if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if (intArraySectionName == reader->getNodeName())
			{
				// end of geometry section reached, cancel out
				break;
			}
		}
	}
}

//! reads a list of Name or ID element and store it in outArray
void CColladaFileLoader::readStringArray(const char* data, core::array<core::stringc> &outArray)
{
	int counts = countTokens(data);

	outArray.clear();
	outArray.reallocate(counts);
	

	const core::stringc str(data);
	s32 beginPos = 0;
	s32 nextPos = str.findFirst(' ');
	int i = 0;

	while(nextPos != -1)
	{
		outArray.push_back(str.subString(beginPos, nextPos - beginPos));
		beginPos = nextPos + 1;
		nextPos = str.findNext(' ', beginPos);
	}

	outArray.push_back(str.subString(beginPos, str.size() - beginPos));

}

//! reads a NAME element from a string and stores it in outArray
void CColladaFileLoader::readNameArray(io::IXMLReaderUTF8* reader, ISourceArray **array)
{
	if(*array)
	{
		delete array;
	}

	SNameArray *idArray = irrnew SNameArray();
	*array = idArray;

	idArray->Name = readId(reader);
	idArray->Data.reallocate(reader->getAttributeValueAsInt("count"));

	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_TEXT)
		{
			readStringArray(reader->getNodeData(), idArray->Data);
		}
		else if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if (nameArraySectionName == reader->getNodeName())
			{
				// end of geometry section reached, cancel out
				break;
			}
		}
	}
}

void CColladaFileLoader::readIdRefArray(io::IXMLReaderUTF8* reader, ISourceArray **array)
{
	if(*array)
	{
		delete array;
	}

	SIDRefArray *idArray = irrnew SIDRefArray();
	*array = idArray;

	idArray->Name = readId(reader);
	idArray->Data.reallocate(reader->getAttributeValueAsInt("count"));

	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_TEXT)
		{
			readStringArray(reader->getNodeData(), idArray->Data);
		}
		else if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if (idrefArraySectionName == reader->getNodeName())
			{
				// end of geometry section reached, cancel out
				break;
			}
		}
	}
}

void CColladaFileLoader::readTargets(io::IXMLReaderUTF8* reader, STargets &target)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading tagets");
	#endif

	// No Ids

	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_ELEMENT)
		{
			if (inputSectionName == reader->getNodeName())
			{
				SInput input;
				input.semantic = reader->getAttributeValue("semantic");
				input.source = reader->getAttributeValue("source");
				target.push_back(input);
			}
		} 
		else if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if (targetsSectionName == reader->getNodeName())
			{
				// end of geometry section reached, cancel out
				break;
			}
		}
	}
}

void CColladaFileLoader::readAccessor(io::IXMLReaderUTF8* reader, SAccessor &accessor)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("Reading accessor");
	#endif
	accessor.Count = reader->getAttributeValueAsInt("count");
	accessor.Offset = reader->getAttributeValueAsInt("offset");
	accessor.Stride = reader->getAttributeValueAsInt("stride");
	if (accessor.Stride == 0)
		accessor.Stride = 1;

	// the accessor contains some information on how to access (boi!) the array,
	// the info is stored in collada style parameters, so just read them.
	//! Dont care for the moment
}

void CColladaFileLoader::readVertexWeights(io::IXMLReaderUTF8* reader, SVertexWeights &vertexWeights)
{
	vertexWeights.count = reader->getAttributeValueAsInt("count");

	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_ELEMENT)
		{
			if (inputSectionName == reader->getNodeName())
			{
				SInput input;
				input.semantic = reader->getAttributeValue("semantic");
				input.source = reader->getAttributeValue("source");
				vertexWeights.inputs.push_back(input);
			}
			else if(vcountSectionName == reader->getNodeName())
			{
				readIntsInsideElement(reader, vertexWeights.vcount);
			}
			else if(vSectionName == reader->getNodeName())
			{
				readIntsInsideElement(reader, vertexWeights.v);
			}
		} 
		else if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if (vertexWeightsSectionName == reader->getNodeName())
			{
				// end of geometry section reached, cancel out
				break;
			}
		}
	}
}


void CColladaFileLoader::readSampler(io::IXMLReaderUTF8* reader, SSampler &sampler)
{
	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_ELEMENT)
		{
			if (inputSectionName == reader->getNodeName())
			{
				SInput input;
				input.semantic = reader->getAttributeValue("semantic");
				input.source = reader->getAttributeValue("source");
				uriToId(input.source);
				sampler.push_back(input);
			}
		} 
		else if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if (samplerSectionName == reader->getNodeName())
			{
				// end of geometry section reached, cancel out
				break;
			}
		}
	}
}


void CColladaFileLoader::readJoints(io::IXMLReaderUTF8* reader, SJoints &joints)
{
	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_ELEMENT)
		{
			if (inputSectionName == reader->getNodeName())
			{
				SInput input;
				input.semantic = reader->getAttributeValue("semantic");
				input.source = reader->getAttributeValue("source");
				joints.push_back(input);
			}
		} 
		else if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if (jointsSectionName == reader->getNodeName())
			{
				// end of geometry section reached, cancel out
				break;
			}
		}
	}
}

void CColladaFileLoader::readSource(io::IXMLReaderUTF8* reader, SSource &source)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading source");
	#endif

	source.Id = readId(reader);

	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_ELEMENT)
		{
			if (idrefArraySectionName == reader->getNodeName())
			{
				SIDRefArray *idArray = 0;
				readIdRefArray(reader, (ISourceArray**)&idArray);
				source.Array = idArray;
			}
			else if (nameArraySectionName == reader->getNodeName())
			{
				SNameArray *idArray = 0;
				readNameArray(reader, (ISourceArray**)&idArray);
				source.Array = idArray;
			}
			else if (floatArraySectionName == reader->getNodeName())
			{
				SFloatArray *idArray = 0;
				readFloatArray(reader, (ISourceArray**)&idArray);
				source.Array = idArray;
			}
			else if (intArraySectionName == reader->getNodeName())
			{
				SIntArray *idArray = 0;
				readIntArray(reader, (ISourceArray**)&idArray);
				source.Array = idArray;
			}
			else if (accessorSectionName == reader->getNodeName())
			{
				SAccessor accessor;
				readAccessor(reader, accessor);
				source.Accessors.push_back(accessor);
			}

			UNIMPLEMENTED("Reading accessor");
		} 
		else if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if (sourceSectionName == reader->getNodeName())
			{
				// end of geometry section reached, cancel out
				break;
			}
		}
	}
}

void CColladaFileLoader::readSkin(io::IXMLReaderUTF8* reader, void **outData)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading skin");
	#endif

	SSkin *pSkin = irrnew SSkin;
	*outData = pSkin;

	pSkin->source = reader->getAttributeValue("source");
	uriToId(pSkin->source);

	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_ELEMENT)
		{
			if (bindShapeMatrixSectionName == reader->getNodeName())
			{
				pSkin->bindShapeMatrix = readMatrixNode(reader);
			}
			else if (sourceSectionName == reader->getNodeName())
			{
				SSource sId;
				readSource(reader, sId);
				pSkin->sources.push_back(sId);
			} 
			else if (jointsSectionName == reader->getNodeName())
			{
				readJoints(reader, pSkin->joints);
			} 
			else if (vertexWeightsSectionName == reader->getNodeName())
			{
				readVertexWeights(reader, pSkin->vertexWeights );
			}
		} 
		else if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if (skinSectionName == reader->getNodeName())
			{
				// end of geometry section reached, cancel out
				break;
			}
		}
	}
}

void CColladaFileLoader::readMorph(io::IXMLReaderUTF8* reader, void **outData)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading morph");
	#endif

	SMorph *pMorph = irrnew SMorph;
	*outData = pMorph;
	pMorph->source = reader->getAttributeValue("source");
	uriToId(pMorph->source);

	pMorph->method = reader->getAttributeValue("method");


	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_ELEMENT)
		{
			if (sourceSectionName == reader->getNodeName())
			{
				SSource sId;
				readSource(reader, sId);
				pMorph->sources.push_back(sId);
			} else if (targetsSectionName == reader->getNodeName())
			{
				readTargets(reader, pMorph->targets );
			}
		} 
		else if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if (morphSectionName == reader->getNodeName())
			{
				// end of geometry section reached, cancel out
				break;
			}
		}
	}
	// [source]*
	
	// [targets::input]*

}

void CColladaFileLoader::readController(io::IXMLReaderUTF8* reader)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading controller");
	#endif

	core::stringc id = readId(reader);
	CControllerPrefab::Type type = CControllerPrefab::MORPH;
	void *data = 0;

	if (!reader->isEmptyElement())
	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_ELEMENT)
		{
			if (morphSectionName == reader->getNodeName())
			{
				readMorph(reader, &data);
				type = CControllerPrefab::MORPH;
			}
			else if (skinSectionName == reader->getNodeName())
			{
				readSkin(reader, &data);
				type = CControllerPrefab::SKIN;
			}
		} 
		else if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if (controllerSectionName == reader->getNodeName())
			{
				// end of geometry section reached, cancel out
				break;
			}
		}
	}

	if(data)
	{
		// create controller prefab
		CControllerPrefab* prefab = irrnew CControllerPrefab(id.c_str(), type, data);
		//prefab->Morph = morph;
		Prefabs.push_back(prefab);
	}
}

//! reads a <geometry> element and stores it as mesh if possible
void CColladaFileLoader::readGeometry(io::IXMLReaderUTF8* reader)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading geometry");
	#endif

	core::array< core::array<s32> > conditionnerVtxMapping;
	core::stringc id = readId(reader);

	core::stringc VertexPositionSource; // each mesh has exactly one <vertex> member, containing
										// a POSITION input. This string stores the source of this input.
	core::array<SSource> sources;
	bool okToReadArray = false;

	SAnimatedMesh* amesh = irrnew SAnimatedMesh();
	scene::SMesh* mesh = irrnew SMesh();
	SMeshesToBind[id] = amesh;
	amesh->grab();
	amesh->addMesh(mesh);

	// handles geometry node and the mesh childs in this loop
	// read sources with arrays and accessor for each mesh
	if (!reader->isEmptyElement())
	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_ELEMENT)
		{
			const char* nodeName = reader->getNodeName();
			if (meshSectionName == nodeName)
			{
				// inside a mesh section. Don't have to do anything here.
			}
			else
			if (sourceSectionName == nodeName)
			{
				// create a new source
				sources.push_back(SSource());
				readSource(reader, sources.getLast());
				//sources.getLast().Id = readId(reader);

				#ifdef COLLADA_READER_DEBUG
				os::Printer::log("Reading source", sources.getLast().Id.c_str());
				#endif
			}
			else
			if (arraySectionName == nodeName || floatArraySectionName == nodeName || intArraySectionName == nodeName)
			{
				// create a new array and read it.
				if (!sources.empty())
				{
					readFloatArray(reader, &sources.getLast().Array);

					#ifdef COLLADA_READER_DEBUG
					os::Printer::log("Read array", sources.getLast().Array->Name.c_str());
					#endif
				}
				#ifdef COLLADA_READER_DEBUG
				else
					os::Printer::log("Warning, array outside source found",
						readId(reader).c_str());
				#endif

			}
			else
			if (accessorSectionName == nodeName) // child of source (below a technique tag)
			{
				#ifdef COLLADA_READER_DEBUG
				os::Printer::log("Reading accessor");
				#endif
				SAccessor accessor;
				accessor.Count = reader->getAttributeValueAsInt("count");
				accessor.Offset = reader->getAttributeValueAsInt("offset");
				accessor.Stride = reader->getAttributeValueAsInt("stride");
				if (accessor.Stride == 0)
					accessor.Stride = 1;

				// the accessor contains some information on how to access (boi!) the array,
				// the info is stored in collada style parameters, so just read them.
				readColladaParameters(reader, accessorSectionName);
				if (!sources.empty())
				{
					sources.getLast().Accessors.push_back(accessor);
					sources.getLast().Accessors.getLast().Parameters = ColladaParameters;
				}
			}
			else
			if (verticesSectionName == nodeName)
			{
				#ifdef COLLADA_READER_DEBUG
				os::Printer::log("Reading vertices");
				#endif
				// read vertex input position source
				readColladaInputs(reader, verticesSectionName);
				SColladaInput* input = getColladaInput(ECIS_POSITION);
				if (input)
					VertexPositionSource = input->Source;
			}
			else
			// lines and linestrips missing
			if (polygonsSectionName == nodeName ||
				polylistSectionName == nodeName ||
				trianglesSectionName == nodeName)
			{
				// read polygons section
				readPolygonSection(reader, VertexPositionSource, sources, mesh, conditionnerVtxMapping, id);
			}
			else
			// trifans, and tristrips missing
			if (extraNodeName == reader->getNodeName())
				skipSection(reader, false);
			else
			if (techniqueCommonSectionName != nodeName) // techniqueCommon must not be skipped
			{
//				os::Printer::log("COLLADA loader warning: Wrong tag usage found", reader->getNodeName(), ELL_WARNING);
				skipSection(reader, true); // ignore all other sections
			}
		} // end if node type is element
		else
		if (reader->getNodeType() == io::EXN_TEXT)
		{
			// read array data
			/*if (okToReadArray && !sources.empty())
			{
				core::array<f32>& a = sources.getLast().Array.Data;
				core::stringc data = reader->getNodeData();
				data.trim();
				const c8* p = &data[0];

				for (u32 i=0; i<a.size(); ++i)
				{
					findNextNoneWhiteSpace(&p);
					if (*p)
						a[i] = readFloat(&p);
					else
						a[i] = 0.0f;
				}
			} */// end reading array

			okToReadArray = false;

		} // end if node type is text
		else
		if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if (geometrySectionName == reader->getNodeName())
			{
				// end of geometry section reached, cancel out
				break;
			}
		}
	} // end while reader->read();

	_IRR_DEBUG_BREAK_IF(geometrySectionName != reader->getNodeName());

	// add mesh as geometry

	mesh->recalculateBoundingBox();
	amesh->recalculateBoundingBox();

	// create virtual file name
	core::stringc filename = CurrentlyLoadingMesh;
	filename += '#';
	filename += id;

	// add to scene manager
	if (LoadedMeshCount)
	{
		SceneManager->getMeshCache()->addMesh(filename.c_str(), amesh);
#ifdef COLLADA_READER_DEBUG
		os::Printer::log("Added COLLADA mesh", filename.c_str());
#endif
	}
	else
	{
		FirstLoadedMeshName = filename;
		FirstLoadedMesh = amesh;
		FirstLoadedMesh->grab();
	}

	++LoadedMeshCount;
	mesh->drop();
	amesh->drop();

	// create geometry prefab
	CGeometryPrefab* prefab = irrnew CGeometryPrefab(id.c_str());
	prefab->setMesh(mesh);
	for(u32 i = 0; i < conditionnerVtxMapping.size(); i++)
	{
		prefab->addVertexConditionnerMapping(conditionnerVtxMapping[i]);
	}
	Prefabs.push_back(prefab);

	// store as dummy mesh if no instances will be created
	if (!CreateInstances && !DummyMesh)
	{
		DummyMesh = amesh;
		DummyMesh->grab();
	}
}


struct SPolygon
{
	core::array<s32> Indices;
};

struct SPolygonIndices
{
	SPolygonIndices() : posIndice(-1), normIndice(-1), texCoorIndice(-1)  {};
	bool operator ==(const SPolygonIndices &r2) const
	{
		return ((posIndice == r2.posIndice)
			&& (normIndice == r2.normIndice)
			&& (texCoorIndice == r2.texCoorIndice));
	};

	bool operator <(const SPolygonIndices &r2) const
	{
		// Todo avoid redundancy ==
		return (((posIndice < r2.posIndice)) ||
			((posIndice == r2.posIndice) && (normIndice < r2.normIndice)) ||
			((posIndice == r2.posIndice) && (normIndice == r2.normIndice) && (texCoorIndice < r2.texCoorIndice)));
	};
	s32 posIndice;
	s32 normIndice;
	s32 texCoorIndice;
};

//! reads a polygons section and creates a mesh from it
void CColladaFileLoader::readPolygonSection(io::IXMLReaderUTF8* reader,
	const core::stringc& vertexPositionSource, core::array<SSource>& sources,
	scene::SMesh* mesh, core::array< core::array<s32> > & conditionnerMapping, const core::stringc& geometryId)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading polygon section");
	#endif

	core::stringc materialName = reader->getAttributeValue("material");

	core::stringc polygonType = reader->getNodeName();
	// int polygonCount = reader->getAttributeValueAsInt("count"); // Not useful because it only determines the number of primitives, which have arbitrary vertices n case of polygon
	core::array<SPolygon> polygons;
	core::array<int> vCounts;
	
	bool parsePolygonOK = false;
	bool parseVcountOK = false;
	u32 inputSemanticCount = 0;
	bool unresolvedInput=false;
	u32 maxOffset = 0;
	Inputs.clear();
	core::array<int> conditionner;
	
	// read all <input> and primitives
	if (!reader->isEmptyElement())
	while(reader->read())
	{
		const char* nodeName = reader->getNodeName();

		if (reader->getNodeType() == io::EXN_ELEMENT)
		{
			// polygon node may contain params
			if (inputTagName == nodeName)
			{
				// read input tag
				readColladaInput(reader);

				// resolve input source
				SColladaInput& inp = Inputs.getLast();
				core::stringc sourceArrayURI;

				// get input source array id, if it is a vertex input, take
				// the <vertex><input>-source attribute.
				if (inp.Semantic == ECIS_VERTEX)
					sourceArrayURI = vertexPositionSource;
				else
					sourceArrayURI = inp.Source;

				uriToId(sourceArrayURI);

				// find source array (we'll ignore accessors for this implementation)
				u32 s;
				for (s=0; s<sources.size(); ++s)
				{
					if (sources[s].Id == sourceArrayURI)
					{
						// slot found
						inp.Data = ((SFloatArray *)sources[s].Array)->Data.pointer();
						inp.Stride = sources[s].Accessors[0].Stride;
						break;
					}
				}

				if (s == sources.size())
				{
					os::Printer::log("COLLADA Warning, polygon input source not found",
						sourceArrayURI.c_str());
					unresolvedInput=true;
				}
				else
				{
					#ifdef COLLADA_READER_DEBUG
					// print slot
					core::stringc tmp = "Added slot ";
					tmp += inputSemanticNames[inp.Semantic];
					tmp += " sourceArray:";
					tmp += sourceArrayURI;
					os::Printer::log(tmp.c_str());
					#endif
				}

				maxOffset = core::max_(maxOffset,inp.Offset);
				++inputSemanticCount;
			}
			else
			if (primitivesName == nodeName)
			{
				parsePolygonOK = true;
				polygons.push_back(SPolygon());
			} // end  is polygon node
			else
			if (vcountName == nodeName)
			{
				parseVcountOK = true;
			} // end  is polygon node
		} // end is element node
		else
		if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if (primitivesName == nodeName)
				parsePolygonOK = false; // end parsing a polygon
			else
			if (vcountName == nodeName)
				parseVcountOK = false; // end parsing vcounts
			else
			if (polygonType == nodeName)
				break; // cancel out and create mesh

		} // end is element end
		else
		if (reader->getNodeType() == io::EXN_TEXT)
		{
			if (parseVcountOK)
			{
				core::stringc data = reader->getNodeData();
				data.trim();
				const c8* p = &data[0];
				while(*p)
				{
					findNextNoneWhiteSpace(&p);
					if (*p)
						vCounts.push_back(readInt(&p));
				}
				parseVcountOK = false;
			}
			else
			if (parsePolygonOK && polygons.size())
			{
				core::stringc data = reader->getNodeData();
				data.trim();
				const c8* p = &data[0];
				SPolygon& poly = polygons.getLast();

				if (vCounts.empty())
				{
					while(*p)
					{
						findNextNoneWhiteSpace(&p);
						poly.Indices.push_back(readInt(&p));
					}
				}
				else
				{

					for (u32 i = 0; i < vCounts.size(); i++)
					{
						int polyVCount = vCounts[i];

						core::array<int> polyCorners;

						for (u32 j = 0; j < polyVCount * inputSemanticCount; j++)
						{
							if (!*p)
								break;
							findNextNoneWhiteSpace(&p);
							polyCorners.push_back(readInt(&p));
						}

						while (polyCorners.size() >= 3 * inputSemanticCount)
						{
							// add one triangle's worth of indices
							for (u32 k = 0; k < inputSemanticCount * 3; ++k)
							{
								poly.Indices.push_back(polyCorners[k]);
							}

							// remove one corner from our poly
							polyCorners.erase(inputSemanticCount,inputSemanticCount);
						}
						polyCorners.clear();
					}
					vCounts.clear();
				}
				parsePolygonOK = false;
			}
		}
	} // end while reader->read()

	if (inputSemanticCount == 0 || unresolvedInput)
		return; // we cannot create the mesh if one of the input semantics wasn't found.

	if (!polygons.size())
		return; // cancel if there are no polygons anyway.

	// analyze content of Inputs to create a fitting mesh buffer

	u32 u;
	u32 textureCoordsetCount = 0;
	bool normalSlotCount = false;
	u32 secondTexCoordsetIndex = 0xFFFFFFFF;

	for (u=0; u<Inputs.size(); ++u)
	{
		if (Inputs[u].Semantic == ECIS_TEXCOORD || Inputs[u].Semantic == ECIS_UV )
		{
			++textureCoordsetCount;

			if (textureCoordsetCount==2)
				secondTexCoordsetIndex = u;
		}
		else
		if (Inputs[u].Semantic == ECIS_NORMAL)
			normalSlotCount=true;
	}

	// if there is more than one texture coordinate set, create a lightmap mesh buffer,
	// otherwise use a standard mesh buffer

	scene::IMeshBuffer* buffer = 0;
	++maxOffset; // +1 to jump to the next value

	if ( textureCoordsetCount <= 1 )
	{
		// standard mesh buffer

		scene::SMeshBuffer* mbuffer = irrnew SMeshBuffer();
		buffer = mbuffer;

		core::map<SPolygonIndices, int> vertMap;

		for (u32 i=0; i<polygons.size(); ++i)
		{
			core::array<u16> indices;
			const u32 vertexCount = polygons[i].Indices.size() / maxOffset;
			mbuffer->Vertices.reallocate(mbuffer->Vertices.size()+vertexCount);

			// for all index/semantic groups
			for (u32 v=0; v<polygons[i].Indices.size(); v+=maxOffset)
			{
				video::S3DVertex vtx;
				SPolygonIndices polyIdxs;
				vtx.Color.set(255,255,255,255);
				
				// for all input semantics
				for (u32 k=0; k<Inputs.size(); ++k)
				{
					if (!Inputs[k].Data)
						continue;
					// build vertex from input semantics.

					const u32 objId = polygons[i].Indices[v+Inputs[k].Offset];
					const u32 idx = Inputs[k].Stride*objId;

					switch(Inputs[k].Semantic)
					{
					case ECIS_POSITION:
					case ECIS_VERTEX:
						vtx.Pos.X = Inputs[k].Data[idx+0];
						vtx.Pos.Y = Inputs[k].Data[idx+1];
						vtx.Pos.Z = Inputs[k].Data[idx+2];
						polyIdxs.posIndice = objId;
						break;
					case ECIS_NORMAL:
						vtx.Normal.X = Inputs[k].Data[idx+0];
						vtx.Normal.Y = Inputs[k].Data[idx+1];
						vtx.Normal.Z = Inputs[k].Data[idx+2];
						polyIdxs.normIndice = objId;
						break;
					case ECIS_COLOR:
						vtx.Color.setRed(static_cast<u32>(Inputs[k].Data[idx+0] * 255));
						vtx.Color.setGreen(static_cast<u32>(Inputs[k].Data[idx+1] * 255));
						vtx.Color.setBlue(static_cast<u32>(Inputs[k].Data[idx+2] * 255));
						break;
					case ECIS_TEXCOORD:
					case ECIS_UV:
						vtx.TCoords.X = Inputs[k].Data[idx+0];
						vtx.TCoords.Y = Inputs[k].Data[idx+1];
						//vtx.TCoords.Y = 1 - Inputs[k].Data[idx+1];
						polyIdxs.texCoorIndice = objId;
						break;
					case ECIS_TANGENT:
						break;
					default:
						break;
					}
				}

				//first, try to find this vertex in the mesh
				core::map<SPolygonIndices, int>::Node* n = vertMap.find(polyIdxs);

				if (n)
				{
					indices.push_back(n->getValue());
				}
				else
				{
					indices.push_back(mbuffer->getVertexCount());
					mbuffer->Vertices.push_back(vtx);
					vertMap.insert(polyIdxs, mbuffer->getVertexCount()-1);
					conditionner.push_back(polyIdxs.posIndice);
				}

			} // end for all vertices

			if (polygonsSectionName == polygonType &&
				indices.size() >= 4)
			{
				// need to tesselate for polygons of 4 or more vertices
				// for now we naively turn interpret it as a triangle fan
				// as full tesselation is problematic
				for (u32 i = 0; i+2 < indices.size(); ++i)
				{
					mbuffer->Indices.push_back(indices[0]);
					mbuffer->Indices.push_back(indices[i+1]);
					mbuffer->Indices.push_back(indices[i+2]);
				}
			}
			else
			{
				// it's just triangles
				for (u32 i = 0; i < indices.size(); ++i)
				{
					mbuffer->Indices.push_back(indices[i]);
				}
			}

		} // end for all polygons

		// Trim unused entries
		mbuffer->Indices.reallocate(mbuffer->Indices.size());
		mbuffer->Vertices.reallocate(mbuffer->Vertices.size());
	}
	else
	{
		// lightmap mesh buffer

		scene::SMeshBufferLightMap* mbuffer = irrnew SMeshBufferLightMap();
		buffer = mbuffer;

		for (u32 i=0; i<polygons.size(); ++i)
		{
			const u32 vertexCount = polygons[i].Indices.size() / maxOffset;
			mbuffer->Vertices.reallocate(mbuffer->Vertices.size()+vertexCount);
			// for all vertices in array
			for (u32 v=0; v<polygons[i].Indices.size(); v+=maxOffset)
			{
				video::S3DVertex2TCoords vtx;
				vtx.Color.set(100,255,255,255);

				// for all input semantics
				for (u32 k=0; k<Inputs.size(); ++k)
				{
					// build vertex from input semantics.

					const u32 idx = Inputs[k].Stride*polygons[i].Indices[v+Inputs[k].Offset];

					switch(Inputs[k].Semantic)
					{
					case ECIS_POSITION:
					case ECIS_VERTEX:
						vtx.Pos.X = Inputs[k].Data[idx+0];
						vtx.Pos.Y = Inputs[k].Data[idx+1];
						vtx.Pos.Z = Inputs[k].Data[idx+2];
						break;
					case ECIS_NORMAL:
						vtx.Normal.X = Inputs[k].Data[idx+0];
						vtx.Normal.Y = Inputs[k].Data[idx+1];
						vtx.Normal.Z = Inputs[k].Data[idx+2];
						break;
					case ECIS_TEXCOORD:
					case ECIS_UV:
						if (k==secondTexCoordsetIndex)
						{
							vtx.TCoords2.X = Inputs[k].Data[idx+0];
							vtx.TCoords2.Y = Inputs[k].Data[idx+1];
						}
						else
						{
							vtx.TCoords.X = Inputs[k].Data[idx+0];
							vtx.TCoords.Y = Inputs[k].Data[idx+1];
						}
						break;
					case ECIS_TANGENT:
						break;
					default:
						break;
					}
				}

				mbuffer->Vertices.push_back(vtx);

			} // end for all vertices

			// add vertex indices
			const u32 oldVertexCount = mbuffer->Vertices.size() - vertexCount;
			for (u32 face=0; face<vertexCount-2; ++face)
			{
				mbuffer->Indices.push_back(oldVertexCount + 0);
				mbuffer->Indices.push_back(oldVertexCount + 1 + face);
				mbuffer->Indices.push_back(oldVertexCount + 2 + face);
			}

		} // end for all polygons
	}

	const SColladaMaterial* m = findMaterial(materialName);
	if (m)
	{
		buffer->getMaterial() = m->Mat;
		if (m->Transparency != 1.0f)
		{
			SMesh tmpmesh;
			tmpmesh.addMeshBuffer(buffer);
			SceneManager->getMeshManipulator()->setVertexColorAlpha(&tmpmesh,core::floor32(m->Transparency*255.0f));
		}
	}
	// add future bind reference for the material
	core::stringc materialReference = geometryId+"/"+materialName;
	if (!MaterialsToBind.find(materialReference))
	{
		MaterialsToBind[materialReference] = MeshesToBind.size();
		MeshesToBind.push_back(core::array<irr::scene::IMeshBuffer*>());
	}
	MeshesToBind[MaterialsToBind[materialReference]].push_back(buffer);

	// calculate normals if there is no slot for it

	if (!normalSlotCount)
		SceneManager->getMeshManipulator()->recalculateNormals(buffer);

	// recalculate bounding box
	buffer->recalculateBoundingBox();

	// add mesh buffer
	mesh->addMeshBuffer(buffer);

	buffer->drop();

	conditionnerMapping.push_back(conditionner);
}



//! reads a <ambient> element and stores it as prefab
void CColladaFileLoader::readAmbientLight(io::IXMLReaderUTF8* reader, video::SLight &outLight)
{
	outLight.Type = video::ELT_POINT;
	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_ELEMENT)
		{
			if(colorNodeName == reader->getNodeName())
			{
				readFloatsInsideElement(reader, (f32 *)&outLight.AmbientColor, 3);
				outLight.AmbientColor.a = 1.0;
				outLight.DiffuseColor = video::SColorf(0, 0, 0, 0);
			}
		}
		else if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if(ambientNodeName == reader->getNodeName())
			{
				break;
			}
		}
	}
}

//! reads a <point> element and stores it as prefab
void CColladaFileLoader::readPointLight(io::IXMLReaderUTF8* reader, video::SLight &outLight)
{
	outLight.Type = video::ELT_POINT;
	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_ELEMENT)
		{
			if(colorNodeName == reader->getNodeName())
			{
				readFloatsInsideElement(reader, (f32 *)&outLight.DiffuseColor, 3);
				outLight.DiffuseColor.a = 1.0;
			} 
			else if(constantAttenuationNodeName == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &outLight.Attenuation.X, 1);
			}
			else if(linearAttenuationNodeName == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &outLight.Attenuation.Y, 1);
			}
			else if(quadraticAttenuationNodeName == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &outLight.Attenuation.Z, 1);
			}
			else if(zfarNodeName == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &outLight.Radius, 1);
			}
		}
		else if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if(pointNodeName == reader->getNodeName())
			{
				break;
			}
		}
	}
}

//! reads a <light> element and stores it as prefab
void CColladaFileLoader::readLightPrefab(io::IXMLReaderUTF8* reader)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading light prefab");
	#endif

	CLightPrefab* prefab = irrnew CLightPrefab(readId(reader));
	prefab->Name = readName(reader);

	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_ELEMENT)
		{
			if(ambientNodeName == reader->getNodeName())
			{
				readAmbientLight(reader, prefab->LightData);
			}
			else if(pointNodeName == reader->getNodeName())
			{
				readPointLight(reader, prefab->LightData);
			}
		}
		else if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if(lightSectionName == reader->getNodeName())
			{
				break;
			}
		}
	}


	Prefabs.push_back(prefab);
}


//! returns a collada parameter or none if not found
SColladaParam* CColladaFileLoader::getColladaParameter(ECOLLADA_PARAM_NAME name)
{
	for (u32 i=0; i<ColladaParameters.size(); ++i)
		if (ColladaParameters[i].Name == name)
			return &ColladaParameters[i];

	return 0;
}

//! returns a collada input or none if not found
SColladaInput* CColladaFileLoader::getColladaInput(ECOLLADA_INPUT_SEMANTIC input)
{
	for (u32 i=0; i<Inputs.size(); ++i)
		if (Inputs[i].Semantic == input)
			return &Inputs[i];

	return 0;
}


//! reads a collada input tag and adds it to the input parameter
void CColladaFileLoader::readColladaInput(io::IXMLReaderUTF8* reader)
{
	// parse param
	SColladaInput p;

	// get type
	core::stringc semanticName = reader->getAttributeValue("semantic");
	for (u32 i=0; inputSemanticNames[i]; ++i)
	{
		if (semanticName == inputSemanticNames[i])
		{
			p.Semantic = (ECOLLADA_INPUT_SEMANTIC)i;
			break;
		}
	}

	// get source
	p.Source = reader->getAttributeValue("source");
	p.Offset = (u32)reader->getAttributeValueAsInt("offset");
	p.set = (u32)reader->getAttributeValueAsInt("set");

	// add input
	Inputs.push_back(p);
}

//! parses all collada inputs inside an element and stores them in Inputs
void CColladaFileLoader::readColladaInputs(io::IXMLReaderUTF8* reader, const core::stringc& parentName)
{
	Inputs.clear();

	while(reader->read())
	{
		if (reader->getNodeType() == io::EXN_ELEMENT &&
			inputTagName == reader->getNodeName())
		{
			readColladaInput(reader);
		}
		else
		if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if (parentName == reader->getNodeName())
				return; // end of parent reached
		}

	} // end while reader->read();
}

//! parses all collada parameters inside an element and stores them in Parameters
void CColladaFileLoader::readColladaParameters(io::IXMLReaderUTF8* reader,
		const core::stringc& parentName)
{
	ColladaParameters.clear();

	const char* const paramNames[] = {"COLOR", "AMBIENT", "DIFFUSE",
		"SPECULAR", "SHININESS", "YFOV", "ZNEAR", "ZFAR", 0};

	const char* const typeNames[] = {"float", "float2", "float3", 0};

	while(reader->read())
	{
		const char* nodeName = reader->getNodeName();
		if (reader->getNodeType() == io::EXN_ELEMENT &&
			paramTagName == nodeName)
		{
			// parse param
			SColladaParam p;

			// get type
			u32 i;
			core::stringc typeName = reader->getAttributeValue("type");
			for (i=0; typeNames[i]; ++i)
				if (typeName == typeNames[i])
				{
					p.Type = (ECOLLADA_PARAM_TYPE)i;
					break;
				}

			// get name
			core::stringc nameName = reader->getAttributeValue("name");
			for (i=0; typeNames[i]; ++i)
				if (nameName == paramNames[i])
				{
					p.Name = (ECOLLADA_PARAM_NAME)i;
					break;
				}

			// read parameter data inside parameter tags
			switch(p.Type)
			{
				case ECPT_FLOAT:
				case ECPT_FLOAT2:
				case ECPT_FLOAT3:
				case ECPT_FLOAT4:
					readFloatsInsideElement(reader, p.Floats, p.Type - ECPT_FLOAT + 1);
					break;

				// TODO: other types of data (ints, bools or whatever)
				default:
					break;
			}

			// add param
			ColladaParameters.push_back(p);
		}
		else
		if (reader->getNodeType() == io::EXN_ELEMENT_END)
		{
			if (parentName == reader->getNodeName())
				return; // end of parent reached
		}

	} // end while reader->read();
}


//! parses a float from a char pointer and moves the pointer
//! to the end of the parsed float
inline f32 CColladaFileLoader::readFloat(const c8** p)
{
	f32 ftmp;
	*p = core::fast_atof_move(*p, ftmp);
	return ftmp;
}


//! parses an int from a char pointer and moves the pointer to
//! the end of the parsed float
inline s32 CColladaFileLoader::readInt(const c8** p)
{
	return (s32)readFloat(p);
}


//! places pointer to next begin of a token
void CColladaFileLoader::findNextNoneWhiteSpace(const c8** start)
{
	const c8* p = *start;

	while(*p && (*p==' ' || *p=='\n' || *p=='\r' || *p=='\t'))
		++p;

	// TODO: skip comments <!-- -->

	*start = p;
}

u32 CColladaFileLoader::countTokens(const c8* str)
{
	int count = 0;

	const c8* p = str;

	while(*p)
	{
		count++;
		while(*p && (*p!=' ' && *p!='\n' && *p!='\r' && *p!='\t')) // Skip Current Token
			++p;
		while(*p && (*p==' ' || *p=='\n' || *p=='\r' || *p=='\t')) // Skip Current Whitespace
			++p;
	}

	return count;
}

//! reads floats from inside of xml element until end of xml element
void CColladaFileLoader::readFloatsInsideElement(io::IXMLReaderUTF8* reader, f32* floats, u32 count)
{
	if (reader->isEmptyElement())
		return;

	while(reader->read())
	{
		// TODO: check for comments inside the element
		// and ignore them.

		if (reader->getNodeType() == io::EXN_TEXT)
		{
			// parse float data
			core::stringc data = reader->getNodeData();
			data.trim();
			const c8* p = &data[0];

			for (u32 i=0; i<count; ++i)
			{
				findNextNoneWhiteSpace(&p);
				if (*p)
					floats[i] = readFloat(&p);
				else
					floats[i] = 0.0f;
			}
		}
		else
		if (reader->getNodeType() == io::EXN_ELEMENT_END)
			break; // end parsing text
	}
}

//! reads ints from inside of xml element until end of xml element
template <typename t>
void CColladaFileLoader::readIntsInsideElement(io::IXMLReaderUTF8* reader, core::array<t> &ints)
{
	if (reader->isEmptyElement())
		return;

	while(reader->read())
	{
		// TODO: check for comments inside the element
		// and ignore them.

		if (reader->getNodeType() == io::EXN_TEXT)
		{
			// parse float data
			core::stringc data = reader->getNodeData();
			data.trim();
			const c8* p = &data[0];
			int numEntry = countTokens(p);
			ints.reallocate(numEntry);
			ints.set_used(numEntry);
			for (int i=0; i<numEntry; ++i)
			{
				findNextNoneWhiteSpace(&p);
				if (*p)
					ints[i] = readInt(&p);
				else
					ints[i] = 0;
			}
		}
		else
		if (reader->getNodeType() == io::EXN_ELEMENT_END)
			break; // end parsing text
	}
}

//! reads ints from inside of xml element until end of xml element
void CColladaFileLoader::readIntsInsideElement(io::IXMLReaderUTF8* reader, s32* ints, u32 count)
{
	if (reader->isEmptyElement())
		return;

	while(reader->read())
	{
		// TODO: check for comments inside the element
		// and ignore them.

		if (reader->getNodeType() == io::EXN_TEXT)
		{
			// parse float data
			core::stringc data = reader->getNodeData();
			data.trim();
			const c8* p = &data[0];

			for (u32 i=0; i<count; ++i)
			{
				findNextNoneWhiteSpace(&p);
				if (*p)
					ints[i] = readInt(&p);
				else
					ints[i] = 0;
			}
		}
		else
		if (reader->getNodeType() == io::EXN_ELEMENT_END)
			break; // end parsing text
	}
}


video::SColorf CColladaFileLoader::readColorNode(io::IXMLReaderUTF8* reader)
{
	video::SColorf result;
	if (reader->getNodeType() == io::EXN_ELEMENT &&
		colorNodeName == reader->getNodeName())
	{
		f32 color[4];
		readFloatsInsideElement(reader,color,4);
		result.r = color[0];
		result.g = color[1];
		result.b = color[2];
		result.a = color[3];
	}

	return result;
}


f32 CColladaFileLoader::readFloatNode(io::IXMLReaderUTF8* reader)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading <float>");
	#endif

	f32 result = 0.0f;
	if (reader->getNodeType() == io::EXN_ELEMENT &&
		floatNodeName == reader->getNodeName())
	{
		readFloatsInsideElement(reader,&result,1);
	}

	return result;
}


//! clears all loaded data
void CColladaFileLoader::clearData()
{
	// delete all prefabs

	for (u32 i=0; i<Prefabs.size(); ++i)
		Prefabs[i]->drop();

	Prefabs.clear();

	// clear all parameters
	ColladaParameters.clear();

	// clear all materials
	Images.clear();

	// clear all materials
	Textures.clear();

	// clear all materials
	Materials.clear();

	// clear all inputs
	Inputs.clear();

	// clear all effects
	Effects.clear();

	// clear all the materials to bind
	MaterialsToBind.clear();
	MeshesToBind.clear();

	//clear all meshes to bind
	
	core::map<core::stringc, irr::scene::SAnimatedMesh*>::Iterator it;
	it = SMeshesToBind.getIterator();
	for (int i = 0, sz = SMeshesToBind.size(); i < sz; it++, i++)
	{
		it->getValue()->drop();
	}	
  	
	SMeshesToBind.clear();

	// remove all useless dummy nodes from scene
	for (u32 i = 0; i < ToRemove.size(); i++ )
	{
		ToRemove[i]->grab();
		ToRemove[i]->remove();
	}

	// free all useluess dummy nodes
	for (u32 i = 0; i < ToRemove.size(); i++ )
	{
		ToRemove[i]->drop();
	}

	ToRemove.clear();
}


//! changes the XML URI into an internal id
void CColladaFileLoader::uriToId(core::stringc& str)
{
	// currently, we only remove the # from the begin if there
	// because we simply don't support referencing other files.
	if (!str.size())
		return;

	if (str[0] == '#')
		str.erase(0);
}


//! read Collada Id, uses id or name if id is missing
core::stringc CColladaFileLoader::readsId(io::IXMLReaderUTF8* reader)
{
	core::stringc id = reader->getAttributeValue("sid");

	return id;
}

//! read Collada Id, uses id or name if id is missing
core::stringc CColladaFileLoader::readId(io::IXMLReaderUTF8* reader)
{
	core::stringc id = reader->getAttributeValue("id");

	if (id.size()==0)
		id = reader->getAttributeValue("name");
	return id;
}


//! create an Irrlicht texture from the reference
video::ITexture* CColladaFileLoader::getTextureFromImage(core::stringc uri)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA searching texture", uri.c_str());
	#endif
	video::IVideoDriver* driver = SceneManager->getVideoDriver();
	for (;;) 
	{
		uriToId(uri);
		for (u32 i=0; i<Images.size(); ++i)
		{
			if (uri == Images[i].Id)
			{
				if (Images[i].Source.size() && Images[i].SourceIsFilename)
				{
					const char* filename = Images[i].Source.c_str();
					video::ITexture* tex = 0;

					if(FileSystem->existFile(filename))
						tex = driver->getTexture(filename);
					if(tex) return tex;
					
					tex = driver->getTexture(filename);
					if(tex) return tex;
					
					if(m_file)
					{
						long size;
						void* buffer = m_file->getExternalBuffer(filename, &size);
						
						if(buffer) {
							irr::io::CMemoryReadFile mrf(buffer, size, filename, false);
							tex = driver->getTexture(&mrf);
							if(tex) return tex;
						}
					}
					
					tex = driver->getTexture((FileSystem->getFileDir(CurrentlyLoadingMesh)+"/"+Images[i].Source).c_str());
					return tex;
				}
				else
				if (Images[i].Source.size())
				{
					//const u32 size = Images[i].Dimension.getArea();
					const u32 size = Images[i].Dimension.Width * Images[i].Dimension.Height;;
					u32* data = irrnew u32[size]; // we assume RGBA
					u32* ptrdest = data;
					const c8* ptrsrc = Images[i].Source.c_str();
					for (u32 j=0; j<size; ++j)
					{
						sscanf(ptrsrc, "%x", ptrdest);
						++ptrdest;
						ptrsrc += 4;
					}
					video::IImage* img = driver->createImageFromData(video::ECF_A8R8G8B8, Images[i].Dimension, data, true, true);
					video::ITexture* tex = driver->addTexture((CurrentlyLoadingMesh+"#"+Images[i].Id).c_str(), img);
					img->drop();
					return tex;
				}
				break;
			}
		}
		if (Parameters.getAttributeType(uri.c_str())==io::EAT_STRING)
		{
			uri = Parameters.getAttributeAsString(uri.c_str());
#ifdef COLLADA_READER_DEBUG
			os::Printer::log("COLLADA now searching texture", uri.c_str());
#endif
		}
		else
			break;
	}
	return 0;
}


//! read a parameter and value
void CColladaFileLoader::readParameter(io::IXMLReaderUTF8* reader)
{
	#ifdef COLLADA_READER_DEBUG
	os::Printer::log("COLLADA reading parameter");
	#endif

	// if it's a new parameter
	if (newParamName == reader->getNodeName())
	{
		const core::stringc name = reader->getAttributeValue("sid");
		if (!reader->isEmptyElement())
		{
			while(reader->read())
			{
				if (reader->getNodeType() == io::EXN_ELEMENT)
				{
					if (floatNodeName == reader->getNodeName())
					{
						const f32 f = readFloatNode(reader);
						Parameters.addFloat(name.c_str(), f);
					}
					else
					if (float2NodeName == reader->getNodeName())
					{
						f32 f[2];
					       	readFloatsInsideElement(reader, f, 2);
//						Parameters.addVector2d(name.c_str(), core::vector2df(f[0],f[1]));
					}
					else
					if (float3NodeName == reader->getNodeName())
					{
						f32 f[3];
					       	readFloatsInsideElement(reader, f, 3);
						Parameters.addVector3d(name.c_str(), core::vector3df(f[0],f[1],f[2]));
					}
					else
					if ((initFromName == reader->getNodeName()) ||
						(sourceSectionName == reader->getNodeName()))
					{
						reader->read();
						Parameters.addString(name.c_str(), reader->getNodeData());
					}
				}
				else
				if(reader->getNodeType() == io::EXN_ELEMENT_END)
				{
					if (newParamName == reader->getNodeName())
						break;
				}
			}
		}
	}
}


} // end namespace scene
} // end namespace irr

#endif // _IRR_COMPILE_WITH_COLLADA_LOADER_

