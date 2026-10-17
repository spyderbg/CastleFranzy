#include "CDatabaseCollada.h"
#include "CColladaTags.h"
#include "quaternion.h"
#include "SDatabaseCollada_Version.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include "CColladaSource.h"
#include "util.h"

using namespace irr;
using namespace collada;
using namespace converter;

extern void EchoDependencies(const char* filename);

std::string g_Source = "Unknown";
std::string g_Author = "Unknown";
std::string g_irrColladaVersion = "Unknown";
bool g_bQuit = false;


void
Quit()
{
	std::cout << "*************************************************" << std::endl;
	std::cout << "**            Convertion failed                **" << std::endl;
	std::cout << "** author : " << g_Author << std::endl;
	std::cout << "** source : " << g_Source << std::endl;
	std::cout << "** irrColladaVersion : " << g_irrColladaVersion << std::endl;
	std::cout << "*************************************************" << std::endl;
	exit(-1);
}

void 
WriteMtx(Chunk &chk, core::matrix4 &mtx)
{
	for(int i = 0; i < 16; i++)
	{
		chk.WriteFloat(mtx[i]);
	}
};













void 
CConfig::Write(Chunk &chk)
{
	chk.Write4u(upVector);
	chk.Write4u(0);
	chk.Write4u(iSamplingRate);
}

void 
CCollada::Write(Chunk &chk)
{
	chk.WriteStr(STRFILEVER);
	configuration.Write(chk);
	libraryAnimations.Write(chk);
	libraryAnimationClips.Write(chk);
	libraryCameras.Write(chk);
	libraryLights.Write(chk);
	libraryImages.Write(chk);
	libraryEffects.Write(chk);
	libraryMaterials.Write(chk);
	libraryGeometries.Write(chk);
	libraryControllers.Write(chk);
	libraryEmitters.Write(chk);
	libraryForces.Write(chk);
	libraryVisualScene.Write(chk);
	scene.Write(chk);
}

void
CCollada::ResolveURL()
{
	libraryAnimations.ResolveURL(*this);
	libraryEffects.ResolveURL(*this);
	libraryMaterials.ResolveURL(*this);
	libraryControllers.ResolveURL(*this);
	libraryVisualScene.ResolveURL(*this);
}

int getValue(std::string &comment, const char *pValue)
{
	int iPos = comment.find(pValue);
	if(iPos == std::string::npos)
	{
		return -1;
	}

	iPos = comment.find("=", iPos);
	if(iPos == std::string::npos)
	{
		return -1;
	}

	return atoi(&comment.at(iPos + 1));
}

int getValue(std::string &comment, const char *pValue, std::string &outValue)
{
	int iPos = comment.find(pValue);
	if(iPos == std::string::npos)
	{
		return -1;
	}

	iPos = comment.find("=", iPos);
	if(iPos == std::string::npos)
	{
		return -1;
	}

	int iFind = comment.find(';', iPos + 1);
	if(iFind != std::string::npos)
	{
		outValue = comment.substr(iPos + 1, comment.find(';', iPos + 1) - (iPos + 1));
	}
	else
	{
		outValue = comment.substr(iPos + 1);
	}
	return 0;
}

int ConvertComponentType(std::string &type)
{
	if(type == "BYTE") return CSource::DAE_BYTE;
	else if(type == "UNSIGNED_BYTE") return CSource::DAE_UNSIGNED_BYTE;
	else if(type == "SHORT") return CSource::DAE_SHORT;
	else if(type == "UNSIGNED_SHORT") return CSource::DAE_UNSIGNED_SHORT;
	else if(type == "INT") return CSource::DAE_INT;
	else if(type == "UNSIGNED_INT") return CSource::DAE_UNSIGNED_INT;
	else if(type == "FLOAT") return CSource::DAE_FLOAT;
	
	return -1;
}

void
CCollada::ReadComment(io::IXMLReaderUTF8 *reader)
{
	bool bBreak = false;
	std::string comment;

	while(reader->read() && !bBreak)
	{
		switch(reader->getNodeType())
		{
		case io::EXN_TEXT:
			comment = reader->getNodeData();
			break;
		case io::EXN_ELEMENT_END:
			if(tags::comments == reader->getNodeName())
			{
				bBreak = true;
			}
			break;
		}
	}

	if(getValue(comment, "ExportEPolyAsTriangles") != 1)
	{
		std::cout << "Polygones are not exported as Triangles" << std::endl;
		g_bQuit = true;
	}

	if(getValue(comment, "DeindexVertexData") != 1)
	{
		std::cout << "Vertex data is not deindexed" << std::endl;
		g_bQuit = true;
	}

	if(getValue(comment, "BakeMatrices") != 1)
	{
		std::cout << "Matrix are not baked" << std::endl;
		g_bQuit = true;
	}	

	if(getValue(comment, "SampleAnim") != 1)
	{
		std::cout << "Animation data is not sampled" << std::endl;
		configuration.iSamplingRate = 0;
	}
	else
	{
		configuration.iSamplingRate = getValue(comment, "SampleRate");
	}

	std::string paramValue;
	if(getValue(comment, "GeometryPositionFormat", paramValue) != -1)
	{
		int iValue = ConvertComponentType(paramValue);
		if(iValue != -1)
		{
			CSource::s_CTPosition	= iValue;
		}
	}

	if(getValue(comment, "GeometryNormalFormat", paramValue) != -1)
	{
		int iValue = ConvertComponentType(paramValue);
		if(iValue != -1)
		{
			CSource::s_CTNormal	= iValue;
		}
	}

	if(getValue(comment, "GeometryTextureFormat", paramValue) != -1)
	{
		int iValue = ConvertComponentType(paramValue);
		if(iValue != -1)
		{
			CSource::s_CTTextureUV	= iValue;
		}
	}

	if(getValue(comment, "GeometryColorFormat", paramValue) != -1)
	{
		int iValue = ConvertComponentType(paramValue);
		if(iValue != -1)
		{
			CSource::s_CTColor	= iValue;
		}
	}
}

void
CCollada::Read(io::IXMLReaderUTF8 *reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::author == reader->getNodeName())
			{
				reader->read();
				g_Author = reader->getNodeData();
			}
			else if(tags::sourceData == reader->getNodeName())
			{
				reader->read();
				g_Source = reader->getNodeData();
			}
			else if(tags::authoringTool == reader->getNodeName())
			{
				const char *pVersion = reader->getAttributeValueSafe("irr_colllada_version");
				if(pVersion)
				{
					g_irrColladaVersion = pVersion;
				}
			}
			else if(tags::comments == reader->getNodeName())
			{
				ReadComment(reader);
			}
			else if(tags::upAxis == reader->getNodeName())
			{
				reader->read();
				if(strcmp("X_UP", reader->getNodeData()) == 0)
				{
					configuration.upVector = CConfig::X_UP;
				}
				else if(strcmp("Y_UP", reader->getNodeData()) == 0)
				{
					configuration.upVector = CConfig::Y_UP;
				}	
				else if(strcmp("Z_UP", reader->getNodeData()) == 0)
				{
					configuration.upVector = CConfig::Z_UP;
				}
				else
				{
					configuration.upVector = -1;
				}
			}
			else if(tags::scene == reader->getNodeName())
			{
				scene.Read(reader);
			}
			else if(tags::libraryVisualScenes == reader->getNodeName())
			{
				libraryVisualScene.Read(reader);
			}
			else if(tags::libraryControllers == reader->getNodeName())
			{
				libraryControllers.Read(reader);
			}
			else if(tags::libraryGeometries == reader->getNodeName())
			{
				libraryGeometries.Read(reader);
			}
			else if(tags::libraryAnimations == reader->getNodeName())
			{
				libraryAnimations.Read(reader);
			}
			else if(tags::libraryAnimationClips == reader->getNodeName())
			{
				libraryAnimationClips.Read(reader);
			}
			else if(tags::libraryImages == reader->getNodeName())
			{
				libraryImages.Read(reader);
			}
			else if(tags::libraryLights == reader->getNodeName())
			{
				libraryLights.Read(reader);
			}
			else if(tags::libraryEffects == reader->getNodeName())
			{
				libraryEffects.Read(reader);
			}
			else if(tags::libraryMaterials == reader->getNodeName())
			{
				libraryMaterials.Read(reader);
			}
			else if(tags::libraryCameras == reader->getNodeName())
			{
				libraryCameras.Read(reader);
			}
			else if(tags::libraryEmitters == reader->getNodeName())
			{
				libraryEmitters.Read(reader);
			}
			else if(tags::libraryForces == reader->getNodeName())
			{
				libraryForces.Read(reader);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::asset == reader->getNodeName())
			{
				if(g_bQuit)
				{
					Quit();
				}
			} else if(tags::collada == reader->getNodeName())
			{
				ResolveURL();
				if(g_bQuit)
				{
					Quit();
				}
				return;
			}
			break;
		}
	};
	assert(false);
}



void 
CLibraryVisualScenes::ResolveURL(CCollada &data)
{
	std::list<CVisualScene>::iterator visualSceneIt = visualScenes.begin();
	for(int i = 0, sz = visualScenes.size(); i < sz; i++)
	{
		visualSceneIt->ResolveURL(data);
		visualSceneIt++;
	}
}
//! [4B] visualScenesArrayCount
//! [4B] visualScenesArray pointer
void 
CLibraryVisualScenes::Write(Chunk &chk)
{
	chk.Write(visualScenes, tags::libraryVisualScenes.c_str());
}

void
CLibraryVisualScenes::Read(io::IXMLReaderUTF8 *reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::visualScene == reader->getNodeName())
			{
				CVisualScene vs;
				vs.Read(reader);
				visualScenes.push_back(vs);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::libraryVisualScenes == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
	assert(false);
}

void 
CVisualScene::ResolveURL(CCollada &data)
{
	std::list<CNode>::iterator nodeIt = nodes.begin();
	for(int i = 0, sz = nodes.size(); i < sz; i++)
	{
		nodeIt->ResolveURL(data);
		nodeIt++;
	}
}

//! [4B] id ptr
//! [4B] name ptr
//! [4B] node count
//! [4B] node array ptr
void 
CVisualScene::Write(Chunk &chk)
{
	chk.WriteStr(id.c_str());
	chk.WriteStr(name.c_str());
	chk.Write(nodes);
}

void
CVisualScene::Read(io::IXMLReaderUTF8 *reader)
{
	id = reader->getAttributeValueSafe("id");
	
	name = reader->getAttributeValueSafe("name");
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::node == reader->getNodeName())
			{
				CNode node;
				node.Read(reader);
				nodes.push_back(node);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::visualScene == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void
CBillboard::Read(io::IXMLReaderUTF8 *reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::type == reader->getNodeName())
			{
				reader->read();
				if(tags::upOnly == reader->getNodeName())
				{
					type = EBT_UP_ONLY;
				}
				else if(tags::free == reader->getNodeName())
				{
					type = EBT_FREE;
				}
				else
				{
					type = EBT_NONE;
				}
			} else if(tags::orientation == reader->getNodeName())
			{
				reader->read();
				if(tags::towardCamera == reader->getNodeName())
				{
					orientation = EBO_TOWARD_CAMERA;
				}
				else if(tags::parallelCamAxis == reader->getNodeName())
				{
					orientation = EBT_PARALLEL_TO_CAMERA_AXIS;
				}
				else
				{
					orientation = EBO_NONE;
				}
			} else if( tags::front == reader->getNodeName() )
			{
				readFloatsInsideElement( reader, (irr::f32 *)&front, 3 );
			} else if( tags::up == reader->getNodeName() )
			{
				readFloatsInsideElement( reader, (irr::f32 *)&up, 3 );
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::billboard == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
	assert(false);
}

void
CBillboard::Write(Chunk &chk)
{
	chk.Write4s(type);
	chk.Write4s(orientation);
	chk.WriteFloat(front.X); chk.WriteFloat(front.Y); chk.WriteFloat(front.Z);
	chk.WriteFloat(up.X); chk.WriteFloat(up.Y); chk.WriteFloat(up.Z);
}

void 
CNode::ResolveURL(CCollada &data)
{
	std::list<CNode>::iterator nodeIt = children.begin();
	for(int i = 0, sz = children.size(); i < sz; i++)
	{
		nodeIt->ResolveURL(data);
		nodeIt++;
	}

	std::list<CInstance *>::iterator instanceIt = instances.begin();
	for(int i = 0, sz = instances.size(); i < sz; i++)
	{
		(*instanceIt)->ResolveURL(data);
		instanceIt++;
	}
}

extern void WriteUserProperties(Chunk &chk, std::string &userProperties);

// [4B] id ptr
// [4B] name ptr
// [4B] sid ptr
// [16 * 4B] transform matrix ptr
// [4B] node count + [4B] node array ptr
// [4B] instance count + [4B] instance array ptr
void 
CNode::Write(Chunk &chk)
{
	chk.WriteStr(id.c_str());
	chk.WriteStr(name.c_str());
	chk.WriteStr(sid.c_str());
	chk.WriteFloat(position.x); chk.WriteFloat(position.y); chk.WriteFloat(position.z);
	chk.WriteFloat(rotation.x); chk.WriteFloat(rotation.y); chk.WriteFloat(rotation.z); chk.WriteFloat(rotation.w);
	chk.WriteFloat(scale.x); chk.WriteFloat(scale.y); chk.WriteFloat(scale.z);
	chk.Write4u(visibility == 1 ? 1 : 0);
//	WriteMtx(chk, transform);
	chk.Write(children);
	chk.Write(instances);
	
	if(userProperties != "")
	{
		Chunk &userProp = chk.AddAndWriteChunkOffset();
		WriteUserProperties(userProp, userProperties);
	}
	else
	{
		chk.Write4u(0);
	}

	if(billboard.type != CBillboard::EBT_NONE)
	{
		Chunk &extraData = chk.AddAndWriteChunkOffset();
		billboard.Write(extraData);
	}
	else
	{
		chk.Write4u(0);
	}
}

void
ConvertUserProperties(std::string &userProps)
{
	std::string newProps;
	
	int iPos = userProps.find("&#");
	int iLastPos = 0;
	while(iPos != std::string::npos)
	{
		std::string sub = userProps.substr(iLastPos, iPos - iLastPos);
		newProps += sub;
		char newChar = atoi(&userProps[iPos+2]);
		newProps += newChar;
		iLastPos = userProps.find(";", iPos) + 1;
		iPos = userProps.find("&#", iLastPos);
	}
	newProps += userProps.substr(iLastPos, iPos);
	userProps = newProps;
}



void
CNode::Read(io::IXMLReaderUTF8 *reader)
{
	id = reader->getAttributeValueSafe("id");
	
	name = reader->getAttributeValueSafe("name");
	sid = reader->getAttributeValueSafe("sid");

	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::matrix == reader->getNodeName())
			{
				//core::matrix4 transform, transposed;
				readFloatsInsideElement(reader, transform.pointer(), 16);
				transform = transform.getTransposed();
				
				core::vector3df pos = transform.getTranslation();
				position.x = pos.X;
				position.y = pos.Y;
				position.z = pos.Z;
				
				core::quaternion quat = transform;
				quat.normalize();
				rotation.w = quat.W;
				rotation.x = quat.X;
				rotation.y = quat.Y;
				rotation.z = quat.Z;
				
				core::vector3df scl = transform.getScale();
				scale.x = scl.X;
				scale.y = scl.Y;
				scale.z = scl.Z;
				
			}
			else if(tags::translate == reader->getNodeName())
			{
				readFloatsInsideElement(reader, (irr::f32 *)&position, 3);
			}
			else if(tags::rotate == reader->getNodeName())
			{
				readFloatsInsideElement(reader, (irr::f32 *)&rotation, 4);
				core::quaternion quat;

				quat.fromAngleAxis(rotation.w * -core::DEGTORAD, core::vector3df(rotation.x, rotation.y, rotation.z));
				
				quat.normalize();
				rotation.w = quat.W;
				rotation.x = quat.X;
				rotation.y = quat.Y;
				rotation.z = quat.Z;
			}
			else if(tags::scale == reader->getNodeName())
			{
				readFloatsInsideElement(reader, (irr::f32 *)&scale, 3);
			}
			else if(tags::visibility == reader->getNodeName())
			{
				readFloatsInsideElement(reader, (irr::f32 *)&visibility, 1);
			}
			else if(tags::instanceCamera == reader->getNodeName())
			{
				CInstanceCamera *inst = new CInstanceCamera();
				inst->Read(reader);
				instances.push_back(inst);
			}
			else if(tags::instanceLight == reader->getNodeName())
			{
				CInstanceLight *inst = new CInstanceLight();
				inst->Read(reader);
				instances.push_back(inst);
			}
			else if(tags::instanceGeometry == reader->getNodeName())
			{
				CInstanceGeometry *inst = new CInstanceGeometry();
				inst->Read(reader);
				instances.push_back(inst);
			}
			else if(tags::instanceController == reader->getNodeName())
			{
				CInstanceController *inst = new CInstanceController();
				inst->Read(reader);
				instances.push_back(inst);
			}
			else if(tags::instanceModularSkinMaster == reader->getNodeName())
			{
				CInstanceController *inst = new CInstanceModularSkin();
				inst->Read(reader);
				instances.push_back(inst);
			}
			else if(tags::instanceEmitter == reader->getNodeName())
			{
				CInstanceEmitter *inst = new CInstanceEmitter();
				inst->Read(reader);
				instances.push_back(inst);
			}
			else if(tags::instanceForce == reader->getNodeName())
			{
				CInstanceForce *inst = new CInstanceForce();
				inst->Read(reader);
				instances.push_back(inst);
			}
			else if(tags::node == reader->getNodeName())
			{
				CNode node;
				node.Read(reader);
				children.push_back(node);
			}
			else if(tags::userProperties == reader->getNodeName())
			{
				reader->read();
				userProperties = reader->getNodeData();
				ConvertUserProperties(userProperties);
			}
			else if(tags::billboard ==  reader->getNodeName())
			{
				billboard.Read(reader);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::node == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
	assert(false);
}

// [4B] geometry count + [4B] geometry ptr
void 
CLibraryGeometries::Write(Chunk &chk)
{
	chk.Write(pGeometry, tags::libraryGeometries.c_str());
}

void
CLibraryGeometries::Read(io::IXMLReaderUTF8 *reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::geometry == reader->getNodeName())
			{
				CGeometry geo;
				geo.Read(reader);
				pGeometry.push_back(geo);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::libraryGeometries == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}


void 
CMorph::ResolveURL(CCollada &data)
{
	for(int i = 0; i < targets.size(); i++)
	{
		std::list<CGeometry>::iterator geoIt = data.libraryGeometries.pGeometry.begin();
		for(int j = 0; geoIt != data.libraryGeometries.pGeometry.end(); j++, geoIt++)
		{
			if(*(targets[i]) == geoIt->id)
			{
				iTargets.push_back(j);
				break;
			}
		}
	}
}

// [4B] controller count + [4B] controller ptr
void 
CMorph::Write(Chunk &chk)
{
	chk.WriteStr(source.c_str());
	chk.Write4s(method);
	chk.Write(targets);
	chk.Write(iTargets);
	chk.Write(weights);
	// Bind shape matrix


}

void
CMorph::Read(io::IXMLReaderUTF8 *reader)
{
	source = reader->getAttributeValueSafe("source");
	std::string sMethod = reader->getAttributeValueSafe("method");
	method = (sMethod == "NORMALIZED") ? 0 : 1;

	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::idrefArray == reader->getNodeName())
			{
				int iCount = reader->getAttributeValueAsInt("count");
				targets.resize(iCount);

				if(iCount == 0)
				{
					std::cout << "Morph must have at less 1 target" << std::endl;
					//g_bQuit = true;
					continue;
				}

				reader->read();
				readStringArray(reader->getNodeData(), targets);
			}
			else if(tags::floatArray == reader->getNodeName())
			{
				int iCount = reader->getAttributeValueAsInt("count");
				weights.resize(iCount);
				if(iCount)
				{
					readFloatsInsideElement(reader, &weights.at(0), iCount);
				}
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::morph == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

/*CJointsBoundingBoxes::CJointsBoundingBoxes(const CJointsBoundingBoxes &cpy)
: minSource(0)
, maxSource(0)
{
	if(cpy.minSource)
	{
		minSource = new CSource(*cpy.minSource);
	}
	if(cpy.maxSource)
	{
		maxSource = new CSource(*cpy.maxSource);
	}
}*/

void 
CJointsBoundingBoxes::Read(io::IXMLReaderUTF8 *reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::source == reader->getNodeName())
			{
				CSource *pSource = new CSource();
				pSource->Read(reader);
				if(pSource->GetID().find("-bounding_box_min") != std::string::npos)
				{
					minSource = pSource;
					minSource->SetInputParameterType(CSource::DAE_FLOAT);
				}
				else if(pSource->GetID().find("-bounding_box_max") != std::string::npos)
				{
					maxSource = pSource;
					maxSource->SetInputParameterType(CSource::DAE_FLOAT);
				}
				else
				{
					delete pSource;
				}
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::jointsBoundingBoxes == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CJointsBoundingBoxes::Write(Chunk &chk)
{
	if(minSource && maxSource)
	{
		// Write a res::vector<core::aabb3df>
		int size = minSource->GetSrcFloat().size();
		chk.Write4u(size / 3);
		Chunk &inst = chk.AddAndWriteChunkOffset();
		for(int i = 0; i < size; i+=3)
		{
			inst.WriteFloat(minSource->GetSrcFloat()[i]);
			inst.WriteFloat(minSource->GetSrcFloat()[i+1]);
			inst.WriteFloat(minSource->GetSrcFloat()[i+2]);
			inst.WriteFloat(maxSource->GetSrcFloat()[i]);
			inst.WriteFloat(maxSource->GetSrcFloat()[i+1]);
			inst.WriteFloat(maxSource->GetSrcFloat()[i+2]);
		}
	}
	else
	{
		// Write a res::vector<core::aabb3df> {size / void *}
		chk.Write4u(0);
		chk.Write4u(0);
	}
}

void
CSkin::TransposeMatrixVector()
{
	core::matrix4 mat;

	for(int i = 0, sz = bindPoses.size(); i < sz; i+=16)
	{
		memcpy(mat.pointer(), &bindPoses[i], 16 * sizeof(float));
		mat = mat.getTransposed();
		memcpy(&bindPoses[i], mat.pointer(), 16 * sizeof(float));
	}
}

void
CSkin::ResolveURL(CCollada &data)
{
	{ // Resolve INV_BIND_MATRIX
		std::list<CInput>::iterator inputIt = inputs.begin();
		for(; inputIt != inputs.end(); inputIt++)
		{
			if(inputIt->semantic == "INV_BIND_MATRIX")
			{
				//std::list<CSource>::iterator sourceIt = sources.begin();
				for(int i = 0; i < pSources.size(); i++)
				{
					assert(inputIt->source.at(0) == '#');
					if(pSources[i]->GetID() == inputIt->source.c_str() + 1)
					{
						bindPoses = pSources[i]->GetSrcFloat();
						TransposeMatrixVector();
						break;
					}
				}
			}
		}
	}

	{ // Resolve WEIGHT
		std::list<CInput>::iterator vInputIt = vertexWeights.inputs.begin();
		for(; vInputIt != vertexWeights.inputs.end(); vInputIt++)
		{
			if(vInputIt->semantic == "WEIGHT")
			{
				//std::list<CSource>::iterator sourceIt = sources.begin();
				for(int i = 0; i < pSources.size(); i++)
				{
					assert(vInputIt->source.at(0) == '#');
					if(pSources[i]->GetID() == vInputIt->source.c_str() + 1)
					{
						weights = pSources[i]->GetSrcFloat();
						break;
					}
				}
			}
		}
	}
}

// [4B] controller count + [4B] controller ptr
void 
CSkin::Write(Chunk &chk)
{
	chk.WriteStr(source.c_str());
	// Bind shape matrix
	WriteMtx(chk, bindShapeMatrix);
	
	chk.Write(nodes);		// Name_array
	chk.Write(bindPoses);	// Bind poses
	chk.Write(weights);		// Weights

	chk.Write(vertexWeights.vcount);		// VCount
	chk.Write(vertexWeights.v);			// v*/
	jointsBoundingBoxes.Write(chk);
}

void
CSkin::ReadSource(io::IXMLReaderUTF8 *reader)
{
	std::string id = reader->getAttributeValueSafe("id");
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::nameArray == reader->getNodeName())
			{
				int iCount = reader->getAttributeValueAsInt("count");
				reader->read();
				readStringArray(reader->getNodeData(), nodes);
			}
			else if(tags::floatArray == reader->getNodeName())
			{
				int iCount = reader->getAttributeValueAsInt("count");
				std::vector<float> floatArray;
				floatArray.resize(iCount);

				readFloatsInsideElement(reader, &floatArray.at(0), iCount);
				CSource *pSource = new CSource(id, floatArray);
				pSources.push_back(pSource);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::source == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void
CSkin::ReadJoints(io::IXMLReaderUTF8 *reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::input == reader->getNodeName())
			{
				CInput input;
				input.Read(reader);
				inputs.push_back(input);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::joints == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}
void
CSkin::Read(io::IXMLReaderUTF8 *reader)
{
	source = reader->getAttributeValueSafe("source");

	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::bindShapeMatrix == reader->getNodeName())
			{
				readFloatsInsideElement(reader, bindShapeMatrix.pointer(), 16);
				bindShapeMatrix = bindShapeMatrix.getTransposed();
			}
			else if(tags::source == reader->getNodeName())
			{
				ReadSource(reader);
			}
			else if(tags::joints == reader->getNodeName())
			{
				ReadJoints(reader);
			}
			else if(tags::vertexWeights == reader->getNodeName())
			{
				vertexWeights.Read(reader);
			}
			else if(tags::jointsBoundingBoxes == reader->getNodeName())
			{
				jointsBoundingBoxes.Read(reader);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::skin == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CVertexWeights::Write(Chunk &chk)
{

}

void 
CVertexWeights::Read(io::IXMLReaderUTF8 *reader)
{
	count = reader->getAttributeValueAsInt("count");

	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::vcount == reader->getNodeName())
			{
				readIntsInsideElement(reader, vcount);
			}
			else if(tags::input == reader->getNodeName())
			{
				CInput input;
				input.Read(reader);
				inputs.push_back(input);
			}
			else if(tags::v == reader->getNodeName())
			{
				readIntsInsideElement(reader, v);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::vertexWeights == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}


void
CController::ResolveURL(CCollada &data)
{
	switch(type)
	{
	case ESKIN:
		skin.ResolveURL(data);
		break;
	case EMORPH:
		morph.ResolveURL(data);
		break;
	default:
		break;
	};
}

// [4B] id ptr
// [4B] type
// [4B] ptr
void 
CController::Write(Chunk &chk)
{
	chk.Write4s(type);
	chk.WriteStr(id.c_str());
	switch(type)
	{
	case ESKIN:
		chk.WriteExternal(skin);
		break;
	case EMORPH:
		chk.WriteExternal(morph);
		break;
	default:
		chk.Write4s(0);
		break;
	};
}



void
CController::Read(io::IXMLReaderUTF8 *reader)
{
	id = reader->getAttributeValueSafe("id");
	

	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::skin == reader->getNodeName())
			{
				type = ESKIN;
				skin.Read(reader);
			}
			else if(tags::morph == reader->getNodeName())
			{
				type = EMORPH;
				morph.Read(reader);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::controller == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}


void 
CLibraryControllers::ResolveURL(CCollada &data)
{
	std::list<CController>::iterator controllerIt = controllers.begin();
	for(int i = 0, sz = controllers.size(); i < sz; i++)
	{
		controllerIt->ResolveURL(data);
		controllerIt++;
	}
}

// [4B] controller count + [4B] controller ptr
void 
CLibraryControllers::Write(Chunk &chk)
{
	chk.Write(controllers, tags::libraryControllers.c_str());
}

void
CLibraryControllers::Read(io::IXMLReaderUTF8 *reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::controller == reader->getNodeName())
			{
				CController ctrl;
				ctrl.Read(reader);
				controllers.push_back(ctrl);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::libraryControllers == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CSpline::Read(io::IXMLReaderUTF8 *reader)
{
	closed = reader->getAttributeValueAsInt("closed");

	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::source == reader->getNodeName())
			{
				//assert(pPositions == 0);
				pPositions = new CSource();
				pPositions->Read(reader);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::spline == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void 
CSpline::Write(Chunk &chk)
{
	chk.Write4u(closed);
	pPositions->Write(chk);
}

//! [4B] geometry id
//! [4B] geometry name
void 
CGeometry::Write(Chunk &chk)
{
	chk.WriteStr(id.c_str());
	chk.WriteStr(name.c_str());
	chk.Write4u(type);
	
	Chunk &inst = chk.AddAndWriteChunkOffset();
	switch(type)
	{
	case EGT_MESH:
		mesh.Write(inst);
		break;
	case EGT_SPLINE:
		spline.Write(inst);
		break;
	default:
		assert(false);
	}
}

void
CGeometry::Read(io::IXMLReaderUTF8 *reader)
{
	id = reader->getAttributeValueSafe("id");
	
	name = reader->getAttributeValueSafe("name");
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::mesh == reader->getNodeName())
			{
				type = EGT_MESH;
				mesh.Read(reader);
				if(mesh.meshBuffers.size() == 0)
				{
					std::cout << "Error : " << id << " - Empty mesh are not allowed" << std::endl; 
					g_bQuit = true;
				}
			}
			else if(tags::spline == reader->getNodeName())
			{
				type = EGT_SPLINE;
				spline.Read(reader);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::geometry == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

bool PrepareSource(std::vector<CSource *> &pSources, bool bMeshSources)
{
	for(int i = 0; i < pSources.size(); i++)
	{	
		pSources[i]->Prepare(bMeshSources);
	}
	return 0;
}


//! [4B] ComponentsFlags
//! [4B] VertexStride
//  [4B] VertexDataSize + [4B] VertexDataPtr
//  [4B] MeshBufferCount + [4B] MeshBufferPtr
//! [Ext] Source Array
//! [Ext] Meshbuffer Array;
void 
CMesh::Write(Chunk &chk)
{
	PrepareSource(pSources, true);
	WriteSources(chk, true);					// VertexDataSize
	chk.Write(meshBuffers);				// meshBuffer count + ptr [4B+4B]

	

	chk.WriteFloat(boundingBox.MinEdge.X);
	chk.WriteFloat(boundingBox.MinEdge.Y);
	chk.WriteFloat(boundingBox.MinEdge.Z);
	chk.WriteFloat(boundingBox.MaxEdge.X);
	chk.WriteFloat(boundingBox.MaxEdge.Y);
	chk.WriteFloat(boundingBox.MaxEdge.Z);
}

void
CMesh::WriteSources(Chunk &chk, bool interleved)
{	
	if(interleved && pSources.size() > 0)
	{
		std::vector<int> iSrcWritingOrder;
		int	iTgtWritingPading = 0;

		chk.Write4u(1); // Interleaved
		int vertexCount = pSources[0]->GetTargetParameterCount();
		chk.Write4u(vertexCount);
		{	
			Chunk &inst = chk.AddAndWriteChunkOffset();
			stride = 0;
			for(int i = 0; i < pSources.size(); i++)
			{
				offsets.push_back(stride);
				stride += pSources[i]->Stride();
				
				types.push_back(pSources[i]->GetTargetComponentType());
			}

			// Change Writing Order to ensure that all the data are properly align
		
			int totalSize = 0;
			int totalLargestComponent = 0;
			while(iSrcWritingOrder.size() != pSources.size())
			{
				int largerSize = -1;
				int largerIdx = -1;
				

				for(int i = 0; i < pSources.size(); i++)
				{
					bool bFound = false;
					for(int j = 0; j < iSrcWritingOrder.size(); j++)
					{
						if(i == iSrcWritingOrder[j])
						{
							bFound = true;
							break;
						}						
					}

					if(!bFound && largerSize < pSources[i]->Stride() / pSources[i]->GetSourceComponentCount())
					{
						largerSize = pSources[i]->Stride() / pSources[i]->GetSourceComponentCount();
						largerIdx = i;
					}
				};



				totalLargestComponent = totalLargestComponent < largerSize ? largerSize : totalLargestComponent;
				iSrcWritingOrder.push_back(largerIdx);
				offsets[largerIdx] = totalSize;
				totalSize += pSources[largerIdx]->Stride();
			}

			iTgtWritingPading = (totalLargestComponent - (stride % totalLargestComponent)) % totalLargestComponent;
			stride += iTgtWritingPading;

			inst.Write4u(stride);
			inst.Write(offsets);
			inst.Write(types);

			
			// Write data scale and offsets
			inst.Write4u(pSources.size());
			if(pSources.size())
			{
				Chunk &arr = inst.AddAndWriteChunkOffset();
				for(int i = 0; i < pSources.size(); i++)
				{
					if(pSources[i]->HasScaleAndOffset())
					{
						Chunk &inst2 = arr.AddAndWriteChunkOffset();
						core::vector3df scale = pSources[i]->GetTargetComponentScale();
						inst2.WriteFloat(scale.X);
						inst2.WriteFloat(scale.Y);
						inst2.WriteFloat(scale.Z);
						core::vector3df offset = pSources[i]->GetTargetComponentOffset();
						inst2.WriteFloat(offset.X);
						inst2.WriteFloat(offset.Y);
						inst2.WriteFloat(offset.Z);
					}
					else
					{
						arr.Write4u(0);
					}
				}
			}
			else
			{
				inst.Write4u(0);
			}

			Chunk &data = inst.AddAndWriteChunkOffset();
			
			for(int i = 0; i < vertexCount; i++)
			{
				for(int j = 0; j < iSrcWritingOrder.size(); j++)
				{
					pSources[iSrcWritingOrder[j]]->Write(data, i);
				}

				// Write padding
				for(int j = 0; j < iTgtWritingPading; j++)
				{
					data.Write1u(0);
				}
			}
		}
	}
	else
	{
		chk.Write4u(0);
		chk.Write(pSources);					// VertexDataSize
	}
}

void
CMesh::ResolveMeshBufferSource()
{
	std::list<CMeshBuffer>::iterator meshBuffer = meshBuffers.begin();
	for(; meshBuffer != meshBuffers.end(); meshBuffer++)
	{
		std::list<CInput>::iterator input = (*meshBuffer).inputs.begin();
		for(; input != (*meshBuffer).inputs.end(); input++)
		{
			if(input->semantic == "VERTEX")
			{
				
				std::list<CInput>::iterator vInput = vertices.inputs.begin();
				for(; vInput != vertices.inputs.end(); vInput++)
				{
					int idx = -1;
					for(int i = 0; i < pSources.size(); i++)
					{
						assert(vInput->source[0] == '#');
						if(strcmp(pSources[i]->GetID().c_str(), vInput->source.c_str() + 1) == 0)
						{
							idx = i;
							break;
						}
					}
					meshBuffer->ResolveSource(idx, vInput->semantic, vInput->set, *pSources[idx]);
				}
			}
			else
			{
				int idx = -1;
				for(int i = 0; i < pSources.size(); i++)
				{
					assert(input->source[0] == '#');
					if(strcmp(pSources[i]->GetID().c_str(), input->source.c_str() + 1) == 0)
					{
						idx = i;
						break;
					}
				}
				meshBuffer->ResolveSource(idx, input->semantic, input->set, *pSources[idx]);
			}
		}
		meshBuffer->ComputeBoundingBox(pSources);
	}

	// Compute bounding box
	if(meshBuffers.size() > 0)
	{
		std::list<CMeshBuffer>::iterator it = meshBuffers.begin();
		boundingBox.reset(it->boundingBox);
		for(; it != meshBuffers.end(); it++)
		{
			boundingBox.addInternalBox(it->boundingBox);
		}
	}
}


void
CMesh::Read(io::IXMLReaderUTF8 *reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::source == reader->getNodeName())
			{
				CSource *pSource = new CSource();
				pSource->Read(reader);
				pSources.push_back(pSource);
			}
			else if(tags::vertices == reader->getNodeName())
			{
				vertices.Read(reader);
			}
			else if(tags::triangles == reader->getNodeName())
			{
				CMeshBuffer meshBuffer;
				meshBuffer.Read(reader);
				meshBuffers.push_back(meshBuffer);
			}
			else if(tags::linestrips == reader->getNodeName())
			{
				CMeshBuffer meshBuffer;
				meshBuffer.Read(reader);
				meshBuffers.push_back(meshBuffer);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::mesh == reader->getNodeName())
			{
				ResolveMeshBufferSource();
				return;
			}
			break;
		}
	};
}




void
CVertices::Read(io::IXMLReaderUTF8 *reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::input == reader->getNodeName())
			{
				CInput input;
				input.Read(reader);
				inputs.push_back(input);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::vertices == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void
CInput::Read(io::IXMLReaderUTF8 *reader)
{
	semantic = reader->getAttributeValueSafe("semantic");
	source = reader->getAttributeValueSafe("source");
	offset = reader->getAttributeValueAsInt("offset");
	set = reader->getAttributeValueAsInt("set");
}

// [4B] - Type
// [4B] - Material string - could be changed for an offset to a material
// [4B] - Primitive count
// [1B] - Position Source ID
// [1B] - Normal  Source ID
// [1B] - Binormal Source ID
// [1B] - Tangeant Source ID
// [1B] - Texture0 Coord
// [1B] - Texture0 Binromal
// [1B] - Texture0 Tangeant
// [1B] - Texture1 Coord
// [1B] - Texture1 Binromal
// [1B] - Texture1 Tangeant
// [1B] - Color0
// [1B] - Color1
// [4B] - Indices count
// [4B] - Indices ptr
		
void 
CMeshBuffer::Write(Chunk &chk)
{
	chk.Write4u(type); // Type
	chk.WriteStr(material.c_str()); // Material
	chk.Write4u(count); // VertexStride

	chk.Write1s(positionSource);	// Position Source ID
	chk.Write1s(normalSource);		// Normal Source ID
	chk.Write1s(binormalSource);	// Binormal Source ID
	chk.Write1s(tangentSource);		// Tangeant Source ID

	chk.Write1s(texcoord0Source);	// Texture0 Coord Source ID
	chk.Write1s(texbinormal0Source);// Texture0 Binormal Source ID
	chk.Write1s(textangent0Source);	// Texture0 Tangeant Source ID
	chk.Write1s(texcoord1Source);	// Texture1 Coord Source ID
	
	chk.Write1s(texbinormal1Source);// Texture1 Binormal Source ID
	chk.Write1s(textangent1Source);	// Texture1 Tangeant Source ID
	chk.Write1s(color0Source);		// Color0 Source ID
	chk.Write1s(color1Source);		// Color1 Source ID

	chk.Write(indices); // Indices count + ptr [4B+4B]

	int min = 65535;
	int max = 0;
	for(int i = 0; i < indices.size(); i++)
	{
		min = min < indices[i] ? min : indices[i];
		max = max > indices[i] ? max : indices[i];
	}
	chk.Write2s(min);
	chk.Write2s(max);

	chk.WriteFloat(boundingBox.MinEdge.X);
	chk.WriteFloat(boundingBox.MinEdge.Y);
	chk.WriteFloat(boundingBox.MinEdge.Z);
	chk.WriteFloat(boundingBox.MaxEdge.X);
	chk.WriteFloat(boundingBox.MaxEdge.Y);
	chk.WriteFloat(boundingBox.MaxEdge.Z);
}


void 
CMeshBuffer::ResolveSource(int idx, std::string semantic, int set, CSource &source)
{
	if(idx != -1)
	{
		if(semantic == "BINORMAL")
		{
			binormalSource = idx;
		}
		else if(semantic == "COLOR")
		{
			if(set == 0)
			{
				color0Source = idx;
			}
			else if(set == 1)
			{
				color1Source = idx;
			}
		}
		else if(semantic == "NORMAL")
		{
			normalSource = idx;
		}
		else if(semantic == "POSITION")
		{
			positionSource = idx;
		}
		else if(semantic == "TANGENT")
		{
			tangentSource = idx;
		}
		else if(semantic == "TEXBINORMAL")
		{
			if(set == 0)
			{
				texbinormal0Source = idx;
			}
			else if(set == 1)
			{
				texbinormal1Source = idx;
			}
		}
		else if(semantic == "TEXCOORD")
		{
			if(set == 0)
			{
				texcoord0Source = idx;
			}
			else if(set == 1)
			{
				texcoord1Source = idx;
			}
		}
		else if(semantic == "TEXTANGENT")
		{
			if(set == 0)
			{
				textangent0Source = idx;
			}
			else if(set == 1)
			{
				textangent1Source = idx;
			}
		}					
	}
}

void 
CMeshBuffer::ComputeBoundingBox(std::vector<CSource *> &pSources)
{
	assert(positionSource > -1);

	// Find the source
	CSource &source = *pSources[positionSource];

	// Cast the source
	int iSize = source.GetSrcFloat().size() / 3;
	core::vector3df *pVectorArray = (core::vector3df *)&source.GetSrcFloat().at(0);
	
	// Compute the bounding box
	boundingBox.reset(*pVectorArray);
	pVectorArray++;
	for(int j = 1; j < iSize; j++)
	{
		boundingBox.addInternalBox(*pVectorArray);
		pVectorArray++;
	}
}

void
CMeshBuffer::Read(io::IXMLReaderUTF8 *reader)
{
	const core::stringc *pOutTags = 0;

	if(tags::triangles == reader->getNodeName())
	{
		pOutTags = &tags::triangles;
		type = EBT_Triangles;
	}
	else if(tags::linestrips == reader->getNodeName())
	{
		pOutTags = &tags::linestrips;
		type = EBT_LineStrips;
	}
	else
	{
		return;
	}

	material = reader->getAttributeValueSafe("material");
	count = reader->getAttributeValueAsInt("count");
	
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::input == reader->getNodeName())
			{
				CInput input;
				input.Read(reader);
				inputs.push_back(input);
			}
			else if(tags::p == reader->getNodeName())
			{
				/*if(type == EBT_Triangles)
				{
					int indicesCount = inputs.size() * count * 3;
					indices.resize(indicesCount);
					readIntsInsideElement(reader, &indices[0], indicesCount);
				}
				else*/
				{
					readIntsInsideElement(reader, indices);
				}
			}
			break;
		case io::EXN_ELEMENT_END:
			if(*pOutTags == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
}

void 
CSampler::ResolveURL(std::vector<CSource *> &pSources)
{
	iInput = -1;
	iOutput = -1;
	iInputTangeant = -1;
	iOutputTangeant = -1;

	std::list<CInput>::iterator inputIt = inputs.begin();
	for(int i = 0; inputIt != inputs.end(); i++, inputIt++)
	{
		if(inputIt->semantic == "INPUT")
		{
			for(int j = 0; j < pSources.size(); j++)
			{
				if(pSources[j]->GetID() == inputIt->source.c_str() + 1)
				{
					pSources[j]->SetInputParameterType(CSource::DAE_TIME);
					iInput = j;
				}
			}
		}
		else if(inputIt->semantic == "OUTPUT")
		{
			for(int j = 0; j < pSources.size(); j++)
			{
				if(pSources[j]->GetID() == inputIt->source.c_str() + 1)
				{
					iOutput = j;
				}
			}
		}
		else if(inputIt->semantic == "INTERPOLATION")
		{
			for(int j = 0; j < pSources.size(); j++)
			{
				if(pSources[j]->GetID() == inputIt->source.c_str() + 1)
				{
					if(*pSources[j]->GetSrcString()[0] == "STEP")
					{
						iInterpolationType = IT_STEP;
					} else if(*pSources[j]->GetSrcString()[0] == "LINEAR")
					{
						iInterpolationType = IT_LINEAR;
					} else if(*pSources[j]->GetSrcString()[0] == "HERMITE")
					{
						iInterpolationType = IT_HERMITE;
					} else
					{
						iInterpolationType = IT_UNKNOWN;
					}
				}
			}
		}
		if(inputIt->semantic == "IN_TANGENT")
		{
			for(int j = 0; j < pSources.size(); j++)
			{
				if(pSources[j]->GetID() == inputIt->source.c_str() + 1)
				{
					iInputTangeant = j;
				}
			}
		}
		else if(inputIt->semantic == "OUT_TANGENT")
		{
			for(int j = 0; j < pSources.size(); j++)
			{
				if(pSources[j]->GetID() == inputIt->source.c_str() + 1)
				{
					iOutputTangeant = j;
				}
			}
		}
	}
}

// [1B] - INPUT source index
// [1B] - OUTPUT source index
// [1B] - INPUT_TANGEANT source index
// [1B] - OUTPUT_TANGEANT source index
void 
CSampler::Write(Chunk &chk)
{
	chk.Write4s(iInterpolationType);//INTERPOLATION TYPE
	chk.Write4s(iInput);			//INPUT
	chk.Write4s(iOutput);			//OUTPUT
	chk.Write4s(iInputTangeant);	//INPUT_TANGEANT
	chk.Write4s(iOutputTangeant);	//OUTPUT_TANGEANT
}

void
CSampler::Read(io::IXMLReaderUTF8 *reader)
{	
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::input == reader->getNodeName())
			{
				CInput input;
				input.Read(reader);
				inputs.push_back(input);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::sampler == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
}

// [4B+4B] - animation count + ptr
void 
CChannel::Write(Chunk &chk)
{
	chk.Write4u(-1);				// Sampler index
	chk.WriteStr(target.c_str()); // Target string
	chk.Write4s(type);
	chk.Write1u(reserved0);
	chk.Write1u(reserved1);
	chk.Write1u(reserved2);
	chk.Write1u(reserved3);
}

void
CChannel::Read(io::IXMLReaderUTF8 *reader)
{	
	source = reader->getAttributeValueSafe("source");
	target = reader->getAttributeValueSafe("target");

	type = 0;

#define MAP_TYPE(string_id, type_id) \
	if(target.find(string_id) != std::string::npos) \
	{ \
		unsigned int iIndex = target.find(string_id); \
		type = type_id; \
		target.resize(iIndex); \
	}

	if(target.find("/transform") != std::string::npos)
	{
		if(target.rfind("(") == std::string::npos) // dont support field animation
		{
			unsigned int iIndex = target.find("/transform");
			type = CT_ROTATION;
			target.resize(iIndex);
		}
	}

	MAP_TYPE("/translation.X", CT_POSITION_X);
	MAP_TYPE("/translation.Y", CT_POSITION_Y);
	MAP_TYPE("/translation.Z", CT_POSITION_Z);
	MAP_TYPE("/translation", CT_POSITION);

	MAP_TYPE("/rotation.X", CT_ROTATION_X);
	MAP_TYPE("/rotation.Y", CT_ROTATION_Y);
	MAP_TYPE("/rotation.Z", CT_ROTATION_Z);
	MAP_TYPE("/rotation.ANGLE", CT_ROTATION_W);
	MAP_TYPE("/rotation", CT_ROTATION);

	MAP_TYPE("/scale.X", CT_SCALE_X);
	MAP_TYPE("/scale.Y", CT_SCALE_Y);
	MAP_TYPE("/scale.Z", CT_SCALE_Z);
	MAP_TYPE("/scale", CT_SCALE);


	if(target.find("-morph_weights") != std::string::npos)
	{ //
		type = CT_MORPHING_WEIGHT;
		unsigned int iIndex = target.find("-morph_weights");
		unsigned int iIndexStart = target.rfind("(");
		unsigned int iIndexEnd = target.rfind(")");
		reserved0 = atoi(&target[iIndexStart + 1]);
		target.resize(iIndex);
	}

	MAP_TYPE("/common/ambient.R", CT_EFFECT_AMBIENT_R);
	MAP_TYPE("/common/ambient.G", CT_EFFECT_AMBIENT_G);
	MAP_TYPE("/common/ambient.B", CT_EFFECT_AMBIENT_B);
	MAP_TYPE("/common/ambient.A", CT_EFFECT_AMBIENT_A);
	MAP_TYPE("/common/ambient", CT_EFFECT_AMBIENT);

	MAP_TYPE("/common/diffuse.R", CT_EFFECT_DIFFUSE_R);
	MAP_TYPE("/common/diffuse.G", CT_EFFECT_DIFFUSE_G);
	MAP_TYPE("/common/diffuse.B", CT_EFFECT_DIFFUSE_B);
	MAP_TYPE("/common/diffuse.A", CT_EFFECT_DIFFUSE_A);
	MAP_TYPE("/common/diffuse", CT_EFFECT_DIFFUSE);

	MAP_TYPE("/common/emission.R", CT_EFFECT_EMISSION_R);
	MAP_TYPE("/common/emission.G", CT_EFFECT_EMISSION_G);
	MAP_TYPE("/common/emission.B", CT_EFFECT_EMISSION_B);
	MAP_TYPE("/common/emission.A", CT_EFFECT_EMISSION_A);
	MAP_TYPE("/common/emission", CT_EFFECT_EMISSION);

	MAP_TYPE("/common/specular.R", CT_EFFECT_SPECULAR_R);
	MAP_TYPE("/common/specular.G", CT_EFFECT_SPECULAR_G);
	MAP_TYPE("/common/specular.B", CT_EFFECT_SPECULAR_B);
	MAP_TYPE("/common/specular.A", CT_EFFECT_SPECULAR_A);
	MAP_TYPE("/common/specular", CT_EFFECT_SPECULAR);

	MAP_TYPE("/common/repeatU", CT_EFFECT_TRANSFORM_SCALE_U);
	MAP_TYPE("/common/repeatV", CT_EFFECT_TRANSFORM_SCALE_V);
	MAP_TYPE("/common/offsetU", CT_EFFECT_TRANSFORM_OFFSET_U);
	MAP_TYPE("/common/offsetV", CT_EFFECT_TRANSFORM_OFFSET_V);
	MAP_TYPE("/common/rotateUV", CT_EFFECT_TRANSFORM_ROTATE);
	MAP_TYPE("/common/transparency", CT_EFFECT_TRANSPARENCY);

	//MAP_TYPE("-light/color", CT_LIGHT_COLOR);
	if(target.find("-light/color") != std::string::npos)
	{
		unsigned int iIndex = target.find("/color");
		type = CT_LIGHT_COLOR;
		target.resize(iIndex);
	}

	if(target.find("/__irrlicht_Diffuse_color.A") != std::string::npos)
	{
		type = CT_EFFECT_DIFFUSE_A;
		unsigned int iIndex = target.find("/__irrlicht_Diffuse_color.A");
		target.resize(iIndex);
		target += "-fx";
	} else if(target.find("/__irrlicht_Diffuse_color.R") != std::string::npos)
	{
		type = CT_EFFECT_DIFFUSE_R;
		unsigned int iIndex = target.find("/__irrlicht_Diffuse_color.R");
		target.resize(iIndex);
		target += "-fx";
	} else if(target.find("/__irrlicht_Diffuse_color.G") != std::string::npos)
	{
		type = CT_EFFECT_DIFFUSE_G;
		unsigned int iIndex = target.find("/__irrlicht_Diffuse_color.G");
		target.resize(iIndex);
		target += "-fx";
	} else if(target.find("/__irrlicht_Diffuse_color.B") != std::string::npos)
	{
		type = CT_EFFECT_DIFFUSE_B;
		unsigned int iIndex = target.find("/__irrlicht_Diffuse_color.B");
		target.resize(iIndex);
		target += "-fx";
	} else if(target.find("/__irrlicht_Diffuse_color") != std::string::npos)
	{
		type = CT_EFFECT_DIFFUSE;
		unsigned int iIndex = target.find("/__irrlicht_Diffuse_color");
		target.resize(iIndex);
		target += "-fx";
	}

	

	MAP_TYPE("/visibility", CT_VISIBILITY);
	if(target.size() == 0)
	{
		type = CT_EVENTS;
	}

	MAP_TYPE("/emission/birth_rate", CT_EMITTER_BIRTH_RATE);

	MAP_TYPE("/particle_size/size_variance", CT_EMITTER_PARTICLE_SIZE_VARIANCE);
	MAP_TYPE("/particle_size/size", CT_EMITTER_PARTICLE_SIZE);

	MAP_TYPE("/particle_motion/speed_variance", CT_EMITTER_PARTICLE_SPEED_VARIANCE);
	MAP_TYPE("/particle_motion/speed", CT_EMITTER_PARTICLE_SPEED);

	MAP_TYPE("/particle_motion/direction/variable_vector/variance", CT_EMITTER_PARTICLE_DIRECTION_VARIANCE);
	MAP_TYPE("/particle_motion/direction/variable_vector/vector", CT_EMITTER_PARTICLE_DIRECTION);
	
	MAP_TYPE("/particle_spin/time_variance", CT_EMITTER_PARTICLE_SPIN_TIME_VARIANCE);
	MAP_TYPE("/particle_spin/time", CT_EMITTER_PARTICLE_SPIN_TIME);
	
	MAP_TYPE("/particle_spin/phase_variance", CT_EMITTER_PARTICLE_SPIN_PHASE_VARIANCE);
	MAP_TYPE("/particle_spin/phase", CT_EMITTER_PARTICLE_SPIN_PHASE);
	
	MAP_TYPE("/particle_spin/axis/variable_vector/variance", CT_EMITTER_PARTICLE_SPIN_AXIS_VARIANCE);
	MAP_TYPE("/particle_spin/axis/variable_vector/vector", CT_EMITTER_PARTICLE_SPIN_AXIS);
	
	MAP_TYPE("/particle_animation_offset/offset", CT_EMITTER_PARTICLE_ANIMATION_OFFSET);
	MAP_TYPE("/particle_animation_offset/offset_variance", CT_EMITTER_PARTICLE_ANIMATION_OFFSET_VARIANCE);

	MAP_TYPE("/particle_animation_scaling/src_length", CT_EMITTER_PARTICLE_ANIMATION_SCALING_SRC_LENGTH);
	MAP_TYPE("/particle_animation_scaling/src_variance", CT_EMITTER_PARTICLE_ANIMATION_SCALING_SRC_VARIANCE);

	MAP_TYPE("/particle_animation_scaling/dst_multiplier", CT_EMITTER_PARTICLE_ANIMATION_SCALING_DST_MULTIPLIER);
	MAP_TYPE("/particle_animation_scaling/dst_variance", CT_EMITTER_PARTICLE_ANIMATION_SCALING_DST_VARIANCE);
}


CAnimation &
CAnimation::operator +=(CAnimation &other)
{
	pSources.insert(pSources.end(), other.pSources.begin(), other.pSources.end());
	samplers.insert(samplers.end(), other.samplers.begin(), other.samplers.end());
	channels.insert(channels.end(), other.channels.begin(), other.channels.end());
	return *this;
}

bool
CAnimation::MergeEffectTransform(CCollada &data)
{
	// Append transform that target the same texture matrix.
	if(((int)channels.front().type) & ((int)CChannel::CT_EFFECT_TRANSFORM))
	{
		std::list<CAnimation>::iterator animationIt = data.libraryAnimations.animations.begin();
		for(; animationIt != data.libraryAnimations.animations.end(); animationIt++)
		{
			if((animationIt->channels.begin()->type & CChannel::CT_EFFECT_TRANSFORM)
				&& (animationIt->channels.begin()->target == channels.front().target)
				&& (&*animationIt != this))
			{
				(*animationIt) += *this;
				return true;
			}
		}

		std::list<CEffect>::iterator effectIt = data.libraryEffects.effects.begin();
		for(; effectIt != data.libraryEffects.effects.end(); effectIt++)
		{
			if(effectIt->id == channels.front().target)
			{
				DefaultOffsetU = effectIt->diffuse.textures[0].offsetU;
				DefaultOffsetV = effectIt->diffuse.textures[0].offsetV;
				DefaultRotate = effectIt->diffuse.textures[0].rotateUV;
				DefaultScaleU = effectIt->diffuse.textures[0].repeatU;
				DefaultScaleV = effectIt->diffuse.textures[0].repeatV;
			}
		}
	}
	return false;
}

void 
CAnimation::ResolveURL(CCollada &data)
{
	std::list<CSampler>::iterator samplerIt = samplers.begin();
	for(; samplerIt != samplers.end(); samplerIt++)
	{
		std::list<CInput>::iterator inputIt = samplerIt->inputs.begin();
		for(; inputIt != samplerIt->inputs.end(); inputIt++)
		{
			int index = findEntryNumber(pSources, inputIt->source.c_str() + 1);
			samplerIt->inputsId.push_back(index);
		}
		samplerIt->ResolveURL(pSources);
	}

	
}

// [4B+4B] - animation count + ptr
void 
CAnimation::Write(Chunk &chk)
{
	PrepareSource(pSources, false);
	chk.WriteStr(id.c_str());
	chk.Write(pSources);
	chk.Write(samplers);
	chk.Write(channels);
	chk.Write4u(0);
	if(channels.front().type & CChannel::CT_EFFECT_TRANSFORM)
	{
		Chunk &inst = chk.AddAndWriteChunkOffset();
		inst.WriteFloat(DefaultOffsetU);
		inst.WriteFloat(DefaultOffsetV);
		inst.WriteFloat(DefaultRotate);
		inst.WriteFloat(DefaultScaleU);
		inst.WriteFloat(DefaultScaleV);
	}
	else
	{
		// Write others default values.
		std::list<CSampler>::iterator samplerIt = samplers.begin();
		int iSrcParamType = pSources[samplerIt->iOutput]->GetSourceParameterType();
		switch(iSrcParamType)
		{
		case CSource::DAE_POSITION:
		case CSource::DAE_NORMAL:
		case CSource::DAE_TEXCOORD:
		case CSource::DAE_TEXCOORD2:
		case CSource::DAE_TEXCOORD3:
			{
				Chunk &inst = chk.AddAndWriteChunkOffset();
				std::list<DefaultValue>::iterator defaultValueIt = defaultValues.begin();
				for(; defaultValueIt != defaultValues.end(); defaultValueIt++)
				{
					inst.WriteFloat(atof(defaultValueIt->value.c_str()));
				}
			}
			break;
		case CSource::DAE_COLOR1:
		case CSource::DAE_COLOR2:
		case CSource::DAE_COLOR3:
		case CSource::DAE_COLOR4:
			{
				Chunk &inst = chk.AddAndWriteChunkOffset();
				std::list<DefaultValue>::iterator defaultValueIt = defaultValues.begin();
				for(; defaultValueIt != defaultValues.end(); defaultValueIt++)
				{
					inst.Write1u(atof(defaultValueIt->value.c_str()) * 255);
				}
			}
			break;
		default:
			chk.Write4u(0);
			break;
		}	
	}
}

struct TimeAndName
{
	float time;
	std::string* name;

	bool operator< (TimeAndName b)
	{
		return time < b.time;
	}
};

void
CAnimation::WriteAsEvent(Chunk &chk)
{
	int iTime = samplers.begin()->iInput;
	int iStrings = samplers.begin()->iOutput;

	// --- sort events by time ---
	{
		std::vector<float>& times = const_cast<std::vector<float>&>(pSources[iTime]->GetSrcFloat());
		std::vector<std::string*>& strings = const_cast<std::vector<std::string*>&>(pSources[iStrings]->GetSrcString());
		std::vector<TimeAndName> timesAndNames;

		for (int i = 0; i < times.size(); ++i)
		{
			TimeAndName timeAndName = { times[i], strings[i] };
			timesAndNames.push_back(timeAndName);
		}
		std::sort(timesAndNames.begin(), timesAndNames.end());
		for (int i = 0; i < times.size(); ++i)
		{
			times[i] = timesAndNames[i].time;
			strings[i] = timesAndNames[i].name;
		}
	}

	PrepareSource(pSources, false);
	pSources[iTime]->Write(chk);

	int iEvents = pSources[iStrings]->GetSrcString().size();
	chk.Write4u(iEvents);
	
	Chunk& inst = chk.AddAndWriteChunkOffset();
	for(int i = 0; i < iEvents; i++)
	{
		std::string*pStr = pSources[iStrings]->GetSrcString()[i];
		int iSubEvents = 1;
		for(int i = 0; i < pStr->size(); i++)
		{
			if(pStr->at(i) == ';')
			{
				iSubEvents++;
			}
		}
		
		inst.Write4u(iSubEvents);
		Chunk &subInst = inst.AddAndWriteChunkOffset();

		int iLastEvent = 0;
		for(int i = 0; i < pStr->size(); i++)
		{
			if(pStr->at(i) == ';')
			{
				subInst.WriteStr(&pStr->at(iLastEvent), i - iLastEvent);
				iLastEvent = i+1;
			}
		}
		subInst.WriteStr(&pStr->at(iLastEvent));

	}
	//pSources[iStrings]->Write(chk);
}


void
CAnimation::ReadDefaultValues(io::IXMLReaderUTF8 *reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::param == reader->getNodeName())
			{
				DefaultValue defaultValue;
				defaultValue.name = reader->getAttributeValueSafe("name");
				reader->read();
				defaultValue.value = reader->getNodeData();
				defaultValues.push_back(defaultValue);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::defaultValues == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
}

void
CAnimation::Read(io::IXMLReaderUTF8 *reader)
{	
	id = reader->getAttributeValueSafe("id");
	
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::source == reader->getNodeName())
			{
				CSource *pSource = new CSource();
				pSource->Read(reader);
				pSources.push_back(pSource);
			}
			else if(tags::sampler == reader->getNodeName())
			{
				CSampler sampler;
				sampler.Read(reader);
				samplers.push_back(sampler);
			}
			else if(tags::channel == reader->getNodeName())
			{
				CChannel channel;
				channel.Read(reader);
				channels.push_back(channel);
			}
			else if(tags::defaultValues == reader->getNodeName())
			{
				ReadDefaultValues(reader);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::animation == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
}

// [4B+4B] - animation count + ptr
void 
CLibraryAnimations::ResolveURL(CCollada &data)
{
	std::list<CAnimation>::iterator animIt = animations.begin();
	for(; animIt != animations.end(); )
	{
		if(animIt->MergeEffectTransform(data))
		{
			animIt = animations.erase(animIt);
		}
		else
		{
			animIt++;
		}
	}

	animIt = animations.begin();
	for(; animIt != animations.end(); animIt++)
	{
		animIt->ResolveURL(data);
	}

	if(writeEvents)
	{
		events.ResolveURL(data);
	}
}

// [4B+4B] - animation count + ptr
void 
CLibraryAnimations::Write(Chunk &chk)
{
	chk.Write(animations, tags::libraryAnimations.c_str());
	if(writeEvents)
	{
		Chunk &inst = chk.AddAndWriteChunkOffset();
		events.WriteAsEvent(inst);
	}
	else
	{
		chk.Write4u(0);
	}
}


void
CLibraryAnimations::Read(io::IXMLReaderUTF8 *reader)
{	
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::animation == reader->getNodeName())
			{
				CAnimation animation;
				animation.Read(reader);

				if(animation.channels.begin()->type == CChannel::CT_EVENTS)
				{
					setEvents(animation);
				}
				else
				{
					animations.push_back(animation);
				}
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::libraryAnimations == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
}

// [4B+4B] - animation count + ptr
void 
CAnimationClip::Write(Chunk &chk)
{
	chk.WriteStr(id.c_str());
	chk.Write4u((unsigned int)(start * 1000));
	chk.Write4u((unsigned int)(end * 1000));
}

void
CAnimationClip::Read(io::IXMLReaderUTF8 *reader)
{	
	id = reader->getAttributeValueSafe("id");
	
	start = reader->getAttributeValueAsFloat("start");
	end = reader->getAttributeValueAsFloat("end");

	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::animation == reader->getNodeName())
			{
			/*	CInstanceAnimation *pInstance = new CInstanceAnimation();
				pInstance->Read(reader);
				instanceAnimations.push_back(pInstance);*/
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::animationClip == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
}

// [4B+4B] - animation count + ptr
void 
CLibraryAnimationClips::Write(Chunk &chk)
{
	chk.Write(animationClips, tags::libraryAnimationClips.c_str());
}

void
CLibraryAnimationClips::Read(io::IXMLReaderUTF8 *reader)
{	
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::animationClip == reader->getNodeName())
			{
				CAnimationClip animationClip;
				animationClip.Read(reader);
				animationClips.push_back(animationClip);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::libraryAnimationClips == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
}

// [4B+4B] - animation count + ptr
void 
CTransform::Write(Chunk &chk)
{
/*	scale.Write(chk);
	rotation.Write(chk);
	position.Write(chk);*/
}

void
CTransform::Read(io::IXMLReaderUTF8 *reader)
{	
	core::matrix4 transform;


	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT_END:
			if(tags::matrix == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
}

void
CImage::WriteIFL(Chunk &chk)
{
	chk.Write4s(int(m_iIflTotalTime));
	iflAnimation.Write(chk);
	/*chk.Write(fileList);
	chk.Write(fileFrameCount);
	chk.Write(texturePtrs);*/
}

// [4B+4B] - images count + ptr
void 
CImage::Write(Chunk &chk)
{
	
	chk.WriteStr(id.c_str());
	chk.WriteStr(name.c_str());
	chk.WriteStr(path.c_str());
	chk.Write4s(type);
	if(type == CIT_IFL)
	{
		Chunk &iflChunk = chk.AddAndWriteChunkOffset();
		WriteIFL(iflChunk);
	}
	else
	{
		EchoDependencies(path.c_str());
		chk.Write4s(0); // ITexture * - Resolved Load Time
	}
}

void
CImage::Read(io::IXMLReaderUTF8 *reader)
{	
	id = reader->getAttributeValueSafe("id");
	
	name = reader->getAttributeValueSafe("name");

	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::initFrom == reader->getNodeName())
			{
				ReadInitFrom(reader);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::image == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
}



void
CImage::ReadIFL()
{
	std::ifstream fileIn(path.c_str());
	if(!fileIn.is_open())
	{
		std::cout << path.c_str() << " MUST be in the same directory" << std::endl;
		g_bQuit = true;
		return;
	}
	char filename[1024];
	int frameCount;
	m_iIflTotalTime = 0.0f;


	std::string					timeID;
	std::vector<float>			timeData;

	std::string					pathID;
	std::vector<std::string *>	pathData;

	std::string					textureID;
	std::vector<float>			textureData;
	
	while(!fileIn.eof() || !fileIn.fail())
	{
		fileIn >> filename;
		fileIn >> frameCount;
		if(strcmp(filename, "") != 0)
		{
			fileList.push_back(filename);
			frameCount *= 33; // 33 millis per frame

			timeData.push_back(m_iIflTotalTime);
			textureData.push_back(0);
			pathData.push_back(new std::string(filename));
			EchoDependencies(filename);
			m_iIflTotalTime += (frameCount / 1000.0f);
		}
	};

	
	timeData.push_back(m_iIflTotalTime);
	textureData.push_back(0);
	pathData.push_back(new std::string(*pathData[0]));

	CSource *pTimeSource = new CSource(timeID, timeData);
	pTimeSource->SetInputParameterType(CSource::DAE_TIME);
	//pTimeSource->SetOutputComponentsType(CSource::DAE_UNSIGNED_INT);
	iflAnimation.pSources.push_back(pTimeSource);				// input = 0

	CSource *pTextureSource = new CSource(textureID, textureData);
	pTextureSource->SetInputParameterType(CSource::DAE_UNKNOWN);
	//pTextureSource->SetOutputComponentsType(CSource::DAE_FLOAT);
	iflAnimation.pSources.push_back(pTextureSource);				// output = 1

	CSource *pPathSource = new CSource(pathID, pathData);
	pPathSource->SetInputParameterType(CSource::DAE_STRING);
	//pPathSource->SetOutputComponentsType(CSource::DAE_STRING);
	iflAnimation.pSources.push_back(pPathSource);				// filename = 2

	CSampler sampler;
	sampler.iInput = 0;
	sampler.iInputTangeant = -1;
	sampler.iOutput = 1;
	sampler.iOutputTangeant = -1;
	iflAnimation.samplers.push_back(sampler);
	
	CChannel channel;
	channel.target = id;
	channel.type = CChannel::CT_IFL;
	iflAnimation.channels.push_back(channel);
}

void
CImage::ReadInitFrom(io::IXMLReaderUTF8 *reader)
{	
	type = CIT_SINGLE;

	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_TEXT:
			path = reader->getNodeData();
			if(path.find("file:///") == 0)
			{
				path = path.substr(8);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::initFrom == reader->getNodeName())
			{
				if(path.find(".ifl") != std::string::npos)
				{
					type = CIT_IFL;
					ReadIFL();
				}
				return;
			}
			break;
		}
	}
}


// [4B+4B] - images count + ptr
void 
CLibraryImages::Write(Chunk &chk)
{
	chk.Write(images, tags::libraryImages.c_str());
}

void
CLibraryImages::Read(io::IXMLReaderUTF8 *reader)
{	
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::image == reader->getNodeName())
			{
				CImage image;
				image.Read(reader);
				images.push_back(image);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::libraryImages == reader->getNodeName())
			{
				if(g_bQuit)
				{
					Quit();
				}
				return;
			}
			break;
		}
	}
}

// [4B+4B] - images count + ptr
void 
CLight::Write(Chunk &chk)
{
	chk.WriteStr(id.c_str());
	chk.WriteStr(name.c_str());
	chk.Write4u(type);
	color.Write(chk);
	chk.WriteFloat(intensity);	
	switch(type)
	{
	case LT_AMBIENT:
	case LT_DIRECTIONNAL:
		chk.Write4u(0);
		break;
	case LT_POINT:
		{
		Chunk &extData = chk.AddAndWriteChunkOffset();
		extData.WriteFloat(constantAttenuation);
		extData.WriteFloat(linearAttenuation);
		extData.WriteFloat(quadraticAttenuation);
		}
		break;
	case LT_SPOT:
		{
		Chunk &extData = chk.AddAndWriteChunkOffset();
		extData.WriteFloat(constantAttenuation);
		extData.WriteFloat(linearAttenuation);
		extData.WriteFloat(quadraticAttenuation);
		extData.WriteFloat(falloffAngle);
		extData.WriteFloat(falloffExponent);
		}
		break;
	}
}


void
CLight::Read(io::IXMLReaderUTF8 *reader)
{	
	id = reader->getAttributeValueSafe("id");
	
	name = reader->getAttributeValueSafe("name");

	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::ambient == reader->getNodeName())
			{
				type = LT_AMBIENT;
			} 
			else if(tags::point == reader->getNodeName())
			{
				type = LT_POINT;
			}
			else if(tags::spot == reader->getNodeName())
			{
				type = LT_SPOT;
			}
			else if(tags::directional == reader->getNodeName())
			{
				type = LT_DIRECTIONNAL;
			}
			else if(tags::color == reader->getNodeName())
			{
				color.Read(reader);
			}
			else if(tags::intensity == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &intensity, 1);
			}
			else if(tags::constantAttenuation == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &constantAttenuation, 1);
			}
			else if(tags::linearAttenuation == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &linearAttenuation, 1);
			}
			else if(tags::quadraticAttenuation == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &quadraticAttenuation, 1);
			}
			else if(tags::falloffAngle == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &falloffAngle, 1);
			}
			else if(tags::falloffExponent == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &falloffExponent, 1);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::light == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
}

void
CLight::ReadAmbient(io::IXMLReaderUTF8 *reader)
{	
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_TEXT:
			if(tags::color == reader->getNodeName())
			{
				color.Read(reader);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::ambient == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
}

// [4B+4B] - images count + ptr
void 
CLibraryLights::Write(Chunk &chk)
{
	chk.Write(lights, tags::libraryLights.c_str());
}

void
CLibraryLights::Read(io::IXMLReaderUTF8 *reader)
{	
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::light == reader->getNodeName())
			{
				CLight light;
				light.Read(reader);
				lights.push_back(light);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::libraryLights == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
}

// [4B+4B] - images count + ptr
void 
CColor::Write(Chunk &chk)
{
	chk.Write1u(r);
	chk.Write1u(g);
	chk.Write1u(b);
	chk.Write1u(a);
}

void
CColor::Read(io::IXMLReaderUTF8 *reader)
{	
	float color[3];
	readFloatsInsideElement(reader, color, 3);
	r = (unsigned char)(color[0] * 255);
	g = (unsigned char)(color[1] * 255);
	b = (unsigned char)(color[2] * 255);
	a = 255;

	//while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT_END:
			if(tags::color == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
	assert(false);
}

// [4B+4B] - images count + ptr
void 
CEffect::Write(Chunk &chk)
{
	chk.WriteStr(id.c_str());
	chk.WriteStr(name.c_str());
	ambient.Write(chk);
	diffuse.Write(chk);
	specular.Write(chk);
	emission.Write(chk);
	chk.WriteFloat(shininess);
	reflective.Write(chk);
	chk.WriteFloat(reflectivity);
	transparent.Write(chk);
	chk.WriteFloat(transparency);
	chk.WriteFloat(specLevel);
	chk.Write4u(doubleSided);
	chk.Write(samplers);
	chk.Write(surfaces);
}

void
CEffect::ResolveURL(CCollada &data)
{
	// Resolve surface URL
	{
		std::list<CSurface>::iterator surfaceIt = surfaces.begin();
		for(int j = 0, sz = surfaces.size(); j < sz; j++)
		{
			std::list<CImage>::iterator imageIt = data.libraryImages.images.begin();
			for(int i = 0, sz = data.libraryImages.images.size(); i < sz; i++)
			{
				if(imageIt->id == surfaceIt->initFrom)
				{
					surfaceIt->iImage = i;
					break;
				}
				imageIt++;
			}
			 
			surfaceIt++;
		}
	}

	// Resolve sampler URL
	{
		std::list<CSampler2D>::iterator sampleIt = samplers.begin();
		for(int i = 0, sz = samplers.size(); i < sz; i++)
		{
			std::list<CSurface>::iterator surfaceIt = surfaces.begin();
			for(int j = 0, sz = surfaces.size(); j < sz; j++)
			{
				if(surfaceIt->sid == sampleIt->source)
				{
					sampleIt->iSurface = j;
					break;
				}
				surfaceIt++;
			}
			sampleIt++;
		}
	}

	// Resolve texture URL
	if(ambient.eType == CCommonColorOrTexture::ETEXTURE)
	{
		for(int i = 0, sz = ambient.textures.size(); i < sz; i++)
			ambient.textures[i].ResolveSampler(samplers);
	}

	if(diffuse.eType == CCommonColorOrTexture::ETEXTURE)
	{
		for(int i = 0, sz = diffuse.textures.size(); i < sz; i++)
			diffuse.textures[i].ResolveSampler(samplers);
	}

	if(specular.eType == CCommonColorOrTexture::ETEXTURE)
	{
		for(int i = 0, sz = specular.textures.size(); i < sz; i++)
			specular.textures[i].ResolveSampler(samplers);
	}

	if(reflective.eType == CCommonColorOrTexture::ETEXTURE)
	{
		for(int i = 0, sz = reflective.textures.size(); i < sz; i++)
			reflective.textures[i].ResolveSampler(samplers);
	}

	if(transparent.eType == CCommonColorOrTexture::ETEXTURE)
	{
		for(int i = 0, sz = transparent.textures.size(); i < sz; i++)
			transparent.textures[i].ResolveSampler(samplers);
	}

	
}

/*void
CTextureUnit::Read(io::IXMLReaderUTF8 *reader)
{

}

void
CProfileGLES::Read(io::IXMLReaderUTF8 *reader)
{

}
*/
void
CEffect::Read(io::IXMLReaderUTF8 *reader)
{	
	id = reader->getAttributeValueSafe("id");
	
	name = reader->getAttributeValueSafe("name");

	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::newparam == reader->getNodeName())
			{
				CNewParam newParam;
				newParam.Read(reader);
				switch(newParam.eType)
				{
				case CNewParam::ESAMPLER2D:
					newParam.sampler.sid = newParam.sid;
					samplers.push_back(newParam.sampler);
					break;
				case CNewParam::ESURFACE:
					newParam.surface.sid = newParam.sid;
					surfaces.push_back(newParam.surface);
					break;
				};				
			}
			else if(tags::ambient == reader->getNodeName())
			{
				ambient.Read(reader, tags::ambient);
			}
			else if(tags::diffuse == reader->getNodeName())
			{
				diffuse.Read(reader, tags::diffuse);
			}
			else if(tags::specular == reader->getNodeName())
			{
				specular.Read(reader, tags::specular);
			}
			else if(tags::shininess == reader->getNodeName())
			{
				
				while(tags::_float != reader->getNodeName()) 
				{
					reader->read();
				}
				readFloatsInsideElement(reader, &shininess, 1);

				float s = 0.0f;

				if (core::abs_(shininess - 0.415939) > 0.001)
				{									
					shininess = core::clamp(shininess, 0.0001f, 1.0f);
					if (shininess >= 0.36f) s = expf((shininess - 0.0226f) / -0.157f);
					if (shininess <= 0.45f)
					{
						float factor = core::max_(0.0f, (shininess - 0.36f) / (0.45f - 0.36f));
						float p = logf(shininess / 0.64893f) / -3.3611f;
						s = s * factor + (1.0f - factor) * p;
					}
					// interpolate over opengl range	
					s = float(core::round32(s * 128.0f));
				}

				shininess = s;
			}
			else if(tags::reflective == reader->getNodeName())
			{
				reflective.Read(reader, tags::reflective);
			}
			else if(tags::reflectivity == reader->getNodeName())
			{
				while(tags::_float != reader->getNodeName()) 
				{
					reader->read();
				}
				readFloatsInsideElement(reader, &reflectivity, 1);
			}
			else if(tags::transparent == reader->getNodeName())
			{
				transparent.Read(reader, tags::transparent);
			}
			else if(tags::transparency == reader->getNodeName())
			{
				while(tags::_float != reader->getNodeName()) 
				{
					reader->read();
				}
				readFloatsInsideElement(reader, &transparency, 1);
			}
			else if(tags::specLevel == reader->getNodeName())
			{
				while(tags::_float != reader->getNodeName()) 
				{
					reader->read();
				}
				readFloatsInsideElement(reader, &specLevel, 1);
			}
			else if(tags::emission == reader->getNodeName())
			{
				emission.Read(reader, tags::emission);
			}
			else if(tags::emissionLevel == reader->getNodeName())
			{
				float emissionLevel;
				while(tags::_float != reader->getNodeName()) 
				{
					reader->read();
				}
				readFloatsInsideElement(reader, &emissionLevel, 1);
				if(emission.eType == CCommonColorOrTexture::ECOLOR)
				{
					emission.color.r = emissionLevel * 255.0f;
					emission.color.g = emissionLevel * 255.0f;
					emission.color.b = emissionLevel * 255.0f;
					emission.color.a = 1.0;
				}				
			}
			else if(tags::doubleSided == reader->getNodeName())
			{
				readIntsInsideElement(reader, &doubleSided, 1);
			}

			break;
		case io::EXN_ELEMENT_END:
			if(tags::effect == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}

	assert(false);
}

void
CEffect::ReadColor(io::IXMLReaderUTF8 *reader, CColor &color, const core::stringc &endNodeName)
{	
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_TEXT:
			if(tags::color == reader->getNodeName())
			{
				color.Read(reader);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(endNodeName == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
}

void
CLibraryEffects::ResolveURL(CCollada &data)
{
	std::list<CEffect>::iterator effectIt = effects.begin();

	for(int i = 0, sz = effects.size(); i < sz; i++)
	{
		effectIt->ResolveURL(data);
		effectIt++;
	}
}

// [4B+4B] - images count + ptr
void 
CLibraryEffects::Write(Chunk &chk)
{
	chk.Write(effects, tags::libraryEffects.c_str());
}

void
CLibraryEffects::Read(io::IXMLReaderUTF8 *reader)
{	
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::effect == reader->getNodeName())
			{
				CEffect effect;
				effect.Read(reader);
				effects.push_back(effect);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::libraryEffects == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
	assert(false);
}


void 
CMaterial::ResolveURL(CCollada &data)
{
	instanceEffect.ResolveURL(data);
	diffuseSurface.ResolveURL(data);
	envmapSurface.ResolveURL(data);
}

// [4B+4B] - material count + ptr
void 
CMaterial::Write(Chunk &chk)
{
	chk.WriteStr(id.c_str());
	chk.WriteStr(name.c_str());
	instanceEffect.Write(chk);
	chk.Write4u(additive);
	diffuseColor.Write(chk);
	chk.Write4u(diffuseSurface.iImage);
	chk.Write4u(envmapSurface.iImage);
	chk.WriteFloat(envmapIntensity);
	chk.Write4u(renderSet);
	chk.Write4u(renderPriority);
	chk.Write4u(backfaceCulling);
	chk.Write4u(frontfaceCulling);
}

void
CMaterial::Read(io::IXMLReaderUTF8 *reader)
{	
	id = reader->getAttributeValueSafe("id");
	
	name = reader->getAttributeValueSafe("name");

	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::instanceEffect == reader->getNodeName())
			{
				instanceEffect.Read(reader);
			}
			else if(tags::setparam == reader->getNodeName())
			{
				std::string ref = reader->getAttributeValueSafe("ref");
				if(ref == "__irrlicht_Additive")
				{
					reader->read();
					readIntsInsideElement(reader, &additive, 1);
				}
				else if(ref == "__irrlicht_Rendering_Set")
				{
					reader->read();
					readIntsInsideElement(reader, &renderSet, 1);
				}
				else if(ref == "__irrlicht_Render_Priority")
				{
					reader->read();
					readIntsInsideElement(reader, &renderPriority, 1);
				}
				else if(ref == "__irrlicht_Backface_Culling")
				{
					reader->read();
					readIntsInsideElement(reader, &backfaceCulling, 1);
				}
				else if(ref == "__irrlicht_Frontface_Culling")
				{
					reader->read();
					readIntsInsideElement(reader, &frontfaceCulling, 1);
				}
				else if(ref == "__irrlicht_Diffuse_color")
				{
					reader->read();
					float color[4];
					readFloatsInsideElement(reader, color, 4);
					diffuseColor.r = color[0] * 255;
					diffuseColor.g = color[1] * 255;
					diffuseColor.b = color[2] * 255;
					diffuseColor.a = color[3] * 255;
				}
				else if(ref == "__irrlicht_EnvironmentMap_intensity")
				{
					reader->read();
					readFloatsInsideElement(reader, &envmapIntensity, 1);
				}
				else if(ref == "__irrlicht_Diffuse_texture")
				{
					diffuseSurface.Read(reader);
				}
				else if(ref == "__irrlicht_EnvironmentMap_texture")
				{
					envmapSurface.Read(reader);
				}

				
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::material == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
	assert(false);
}


void 
CLibraryMaterials::ResolveURL(CCollada &data)
{
	std::list<CMaterial>::iterator materialIt = materials.begin();
	for(int i = 0, sz = materials.size(); i < sz; i++)
	{
		materialIt->ResolveURL(data);
		materialIt++;
	}
}

// [4B+4B] - material count + ptr
void 
CLibraryMaterials::Write(Chunk &chk)
{
	chk.Write(materials, tags::libraryMaterials.c_str());
}

void
CLibraryMaterials::Read(io::IXMLReaderUTF8 *reader)
{	
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::material == reader->getNodeName())
			{
				CMaterial material;
				material.Read(reader);
				materials.push_back(material);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::libraryMaterials == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
	assert(false);
}



// [4B+4B] - camera count + ptr
void 
CCamera::Write(Chunk &chk)
{
	chk.WriteStr(id.c_str());
	chk.Write4u(orthographic);
	chk.WriteFloat(xfov);
	chk.WriteFloat(aspectRatio);
	chk.WriteFloat(znear);
	chk.WriteFloat(zfar);
	chk.WriteStr(target.c_str());
}

void
CCamera::Read(io::IXMLReaderUTF8 *reader)
{	
	id = reader->getAttributeValueSafe("id");
	orthographic = 0;

	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::xfov == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &xfov, 1);
			}
			else if(tags::xmag == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &xfov, 1);
			}
			else if(tags::aspectRatio == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &aspectRatio, 1);
			}
			else if(tags::orthographic == reader->getNodeName())
			{
				orthographic = 1;
			}
			else if(tags::znear == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &znear, 1);
			}
			else if(tags::zfar == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &zfar, 1);
			}
			else if(tags::target == reader->getNodeName())
			{
				reader->read();
				target = reader->getNodeData();
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::camera == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
	assert(false);
}

// [4B+4B] - camera count + ptr
void 
CLibraryCameras::Write(Chunk &chk)
{
	chk.Write(cameras, tags::libraryCameras.c_str());
}

void
CLibraryCameras::Read(io::IXMLReaderUTF8 *reader)
{	
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::camera == reader->getNodeName())
			{
				CCamera camera;
				camera.Read(reader);
				cameras.push_back(camera);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::libraryCameras == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
	assert(false);
}

void
CCommonColorOrTexture::Write(Chunk &chk)
{
	chk.Write1s(eType);
	chk.Write1s(0);	// Reserved
	chk.Write1s(0); // Reserved
	chk.Write1s(0); // Reserved

	Chunk & inst = chk.AddAndWriteChunkOffset();
	switch(eType)
	{
	case ECOLOR:
		color.Write(inst);
		break;
	case ETEXTURE:
		inst.Write(textures);
		
		break;
	};
}

void
CCommonColorOrTexture::Read(io::IXMLReaderUTF8 *reader, const core::stringc &endElement)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::color == reader->getNodeName())
			{
				eType = ECOLOR;
				color.Read(reader);
			}
			else if(tags::texture == reader->getNodeName())
			{
				eType = ETEXTURE;
				CTexture texture;
				texture.Read(reader);
				textures.push_back(texture);
			}
			else
			{
				assert(false);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(endElement == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
	assert(false);
}

void
CTexture::ResolveSampler(std::list<CSampler2D> &samplers)
{
	std::list<CSampler2D>::iterator sampleIt = samplers.begin();
	for(int i = 0, sz = samplers.size(); i < sz; i++)
	{
		if(sampleIt->sid == texture)
		{
			iSampler = i;
			break;
		}
		sampleIt++;
	}
}

void
CTexture::Write(Chunk &chk)
{
	chk.Write4s(iSampler);
	chk.WriteStr(texcoord.c_str());

	chk.WriteFloat(offsetU);
	chk.WriteFloat(offsetV);
	chk.WriteFloat(rotateUV);
	chk.WriteFloat(repeatU);
	chk.WriteFloat(repeatV);
}

void
CTexture::Read(io::IXMLReaderUTF8 *reader)
{
	texture = reader->getAttributeValueSafe("texture");
	texcoord = reader->getAttributeValueSafe("texcoord");
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::mirrorU == reader->getNodeName())
			{
				readIntsInsideElement(reader, &mirrorU, 1);
			}
			else if(tags::mirrorV == reader->getNodeName())
			{
				readIntsInsideElement(reader, &mirrorV, 1);
			}
			else if(tags::wrapU == reader->getNodeName())
			{
				readIntsInsideElement(reader, &wrapU, 1);
			}
			else if(tags::wrapV == reader->getNodeName())
			{
				readIntsInsideElement(reader, &wrapV, 1);
			}
			else if(tags::repeatU == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &repeatU, 1);
			}
			else if(tags::repeatV == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &repeatV, 1);
			}
			else if(tags::offsetU == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &offsetU, 1);
			}
			else if(tags::offsetV == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &offsetV, 1);
			}
			else if(tags::rotateUV == reader->getNodeName())
			{
				readFloatsInsideElement(reader, &rotateUV, 1);
			}
		case io::EXN_ELEMENT_END:
			if(tags::texture == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
	assert(false);
}



void
CSampler2D::Write(Chunk &chk)
{
	chk.Write4s(iSurface);
}

void
CSampler2D::Read(io::IXMLReaderUTF8 *reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::source == reader->getNodeName())
			{
				reader->read();
				assert(reader->getNodeType() == io::EXN_TEXT);
				source = reader->getNodeData();
			} 
			else if(tags::wrapS == reader->getNodeName())
			{
				reader->read();
				assert(reader->getNodeType() == io::EXN_TEXT);
				wrap_s = reader->getNodeData();
			}
			else if(tags::wrapT == reader->getNodeName())
			{
				reader->read();
				assert(reader->getNodeType() == io::EXN_TEXT);
				wrap_t = reader->getNodeData();
			}
			else if(tags::minfilter == reader->getNodeName())
			{
				reader->read();
				assert(reader->getNodeType() == io::EXN_TEXT);
				minfilter = reader->getNodeData();
			}
			else if(tags::magfilter == reader->getNodeName())
			{
				reader->read();
				assert(reader->getNodeType() == io::EXN_TEXT);
				magfilter = reader->getNodeData();
			}
			else if(tags::mipfilter == reader->getNodeName())
			{
				reader->read();
				assert(reader->getNodeType() == io::EXN_TEXT);
				mipfilter = reader->getNodeData();
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::sampler2D == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}


void 
CSurface::ResolveURL(CCollada &data)
{
	iImage = -1;
	std::list<CImage>::iterator imageIt = data.libraryImages.images.begin();
	for(int i = 0, sz = data.libraryImages.images.size(); i < sz; i++)
	{
		if(imageIt->id == initFrom)
		{
			iImage = i;
			break;
		}
		imageIt++;
	}
}

void
CSurface::Write(Chunk &chk)
{
	chk.Write4s(iImage);
}

void
CSurface::Read(io::IXMLReaderUTF8 *reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::initFrom == reader->getNodeName())
			{
				reader->read();
				assert(reader->getNodeType() == io::EXN_TEXT);
				initFrom = reader->getNodeData();
			} 
			break;
		case io::EXN_ELEMENT_END:
			if(tags::surface == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void
CBindMaterial::ResolveURL(CCollada &data)
{
	std::list<CInstanceMaterial *>::iterator instIt = instanceMaterials.begin();
	for(int i = 0, sz = instanceMaterials.size(); i < sz; i++)
	{
		(*instIt)->ResolveURL(data);
		instIt++;
	}
}

void
CBindMaterial::Write(Chunk &chk)
{
	chk.Write(instanceMaterials);
}

void
CBindMaterial::Read(io::IXMLReaderUTF8 *reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::instanceMaterial == reader->getNodeName()) 
			{
				CInstanceMaterial *pInst = new CInstanceMaterial();
				pInst->Read(reader);
				instanceMaterials.push_back(pInst);
			} 
			break;
		case io::EXN_ELEMENT_END:
			if(tags::bindMaterial == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}

void
CNewParam::Read(io::IXMLReaderUTF8 *reader)
{
	sid = reader->getAttributeValueSafe("sid");

	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::sampler2D == reader->getNodeName())
			{
				eType = ESAMPLER2D;
				sampler.Read(reader);
			} 
			else if(tags::surface == reader->getNodeName())
			{
				eType = ESURFACE;
				surface.Read(reader);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::newparam == reader->getNodeName())
			{
				return;
			}
			break;
		}
	};
}
