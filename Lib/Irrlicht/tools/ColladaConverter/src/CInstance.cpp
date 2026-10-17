#include "CDatabaseCollada.h"
#include "CColladaTags.h"
#include "util.h"

using namespace irr;
using namespace collada;
using namespace converter;

void 
CInstanceGeometry::ResolveURL(CCollada &data)
{
	bindMaterial.ResolveURL(data);
}

void 
CInstanceGeometry::Write(Chunk &chk)
{
	chk.Write4u(type);
	Chunk &inst = chk.AddAndWriteChunkOffset();
	WriteURL(inst, url);
	inst.Write4u(0);
	bindMaterial.Write(inst);
	inst.Write4s(drawLayer);
}

void
CInstanceGeometry::Read(io::IXMLReaderUTF8 *reader)
{
	type = CInstance::ITGeometry;
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
			} else if(tags::drawLayer == reader->getNodeName())
			{
				reader->read();
				drawLayer = atoi(reader->getNodeData());
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::instanceGeometry == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
	assert(false);
}


void 
CCategory::ResolveURL(CCollada &data)
{
	std::list<CInstanceController *>::iterator it = modules.begin();

	for(;it != modules.end(); it++)
	{
		(*it)->ResolveURL(data);
	}
}

void 
CCategory::Write(Chunk &chk)
{
	chk.WriteStr(name.c_str());
	chk.WriteStr(selectedSkin.c_str());
	chk.Write(modules);
}
/*
	std::string						selectedSkin;
	std::string						sid;
	std::list<CInstanceController>	modules;
*/
void
CCategory::Read(io::IXMLReaderUTF8 *reader)
{
	selectedSkin = reader->getAttributeValueSafe("selected_skin");
	name = reader->getAttributeValueSafe("name");
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::instanceController == reader->getNodeName())
			{
				CInstanceController *pInst = new CInstanceController();
				pInst->Read(reader);
				modules.push_back(pInst);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::category == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
	assert(false);
}

void 
CInstanceModularSkin::ResolveURL(CCollada &data)
{
	std::list<CCategory *>::iterator it = categories.begin();
	for(; it != categories.end(); it++)
	{
		(*it)->ResolveURL(data);
	}
}

void 
CInstanceModularSkin::Write(Chunk &chk)
{
	chk.Write4u(type);
	Chunk &inst = chk.AddAndWriteChunkOffset();
	inst.Write(categories);
	inst.Write(orphanModules);
}

void
CInstanceModularSkin::Read(io::IXMLReaderUTF8 *reader)
{
	type = ITModularSkin;

	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::category == reader->getNodeName())
			{
				CCategory *pCategory = new CCategory();
				pCategory->Read(reader);
				categories.push_back(pCategory);
			}
			else if(tags::instanceController == reader->getNodeName())
			{
				CInstanceController *pInst = new CInstanceController();
				pInst->Read(reader);
				orphanModules.push_back(pInst);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::instanceModularSkinMaster == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
	assert(false);
}


void 
CInstanceController::ResolveURL(CCollada &data)
{
	bindMaterial.ResolveURL(data);
}

void 
CInstanceController::Write(Chunk &chk)
{
	chk.Write4u(type);
	Chunk &inst = chk.AddAndWriteChunkOffset();
	WriteURL(inst, url);
	inst.Write4u(0);
	bindMaterial.Write(inst);
	inst.Write4s(drawLayer);
	inst.Write(skeletons);
}

void
CInstanceController::Read(io::IXMLReaderUTF8 *reader)
{
	type = ITController;
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
			} else if(tags::drawLayer == reader->getNodeName())
			{
				reader->read();
				drawLayer = atoi(reader->getNodeData());
			} else if(tags::skeleton == reader->getNodeName())
			{
				reader->read();
				std::string skeleton = reader->getNodeData();
				skeletons.push_back(skeleton);
			} 
			break;
		case io::EXN_ELEMENT_END:
			if(tags::instanceController == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
	assert(false);
}


void 
CInstanceAnimation::Write(Chunk &chk)
{
	chk.Write4u(type);
	WriteURL(chk, url);
}

void
CInstanceAnimation::Read(io::IXMLReaderUTF8 *reader)
{
	type = ITAnimation;
	url = reader->getAttributeValueSafe("url");
	ChangeExtension(url);

	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT_END:
			if(tags::instanceAnimation == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
	assert(false);
}

void 
CInstanceMaterial::ResolveURL(CCollada &data)
{
	std::list<CMaterial>::iterator materialIt = data.libraryMaterials.materials.begin();
	iMaterial = -1;
	for(int i = 0, sz = data.libraryMaterials.materials.size(); i < sz; i++)
	{
		if(materialIt->id == url.c_str() + 1)
		{
			iMaterial = i;
		}
		materialIt++;
	}
}

void 
CInstanceMaterial::Write(Chunk &chk)
{
	WriteURL(chk, url);
	chk.Write4u(iMaterial);
	chk.Write4u(iEffectComponent);
	chk.Write4u(iGeometryComponent);
}

void
CInstanceMaterial::Read(io::IXMLReaderUTF8 *reader)
{
	type = ITMaterial;
	symbol = reader->getAttributeValueSafe("symbol");
	url = reader->getAttributeValueSafe("target");
	ChangeExtension(url);
	if(url[0] != '#')
	{
		url = url.substr(0, url.find('#') + 1);
		url += symbol;
	}

	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT_END:
			if(tags::instanceMaterial == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
	assert(false);
}

void 
CInstanceEffect::ResolveURL(CCollada &data)
{
	std::list<CEffect>::iterator effectIt = data.libraryEffects.effects.begin();
	for(int i = 0, sz = data.libraryEffects.effects.size(); i < sz; i++)
	{
		if(effectIt->id == url.c_str() + 1)
		{
			iEffect = i;
			break;
		}
		effectIt++;
	}
}

void 
CInstanceEffect::Read(io::IXMLReaderUTF8 *reader)
{
	url = reader->getAttributeValueSafe("url");
	ChangeExtension(url);

	/*while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT_END:
			if(tags::instanceEffect == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}*/
	//assert(false);
}

void 
CInstanceEffect::Write(Chunk &chk)
{
	//chk.Write4u(type);
	WriteURL(chk, url);
	chk.Write(setParams);
	chk.Write4s(iEffect);
}

void 
CInstanceVisualScene::Write(Chunk &chk)
{
	chk.Write4u(type);
	Chunk &inst = chk.AddAndWriteChunkOffset();
	WriteURL(inst, url);
}

void
CInstanceVisualScene::Read(io::IXMLReaderUTF8 *reader)
{
	type = ITVisualScene;
	url = reader->getAttributeValueSafe("url");
	ChangeExtension(url);

	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT_END:
			if(tags::instanceVisualScene == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
	assert(false);
}


void 
CInstanceCamera::ResolveURL(CCollada &data)
{
	iCamera = -1;
	
	/*std::list<CCamera>::iterator cameraIt = data.libraryCameras.cameras.begin();
	for(int i = 0; i < data.libraryCameras.cameras.size(); i++)
	{
		if(cameraIt->id == url.c_str() + 1)
		{
			iCamera = i;
			break;
		}
		cameraIt++;
	}*/
}

// [4B+4B] - camera count + ptr
void 
CInstanceCamera::Write(Chunk &chk)
{
	chk.Write4u(type);
	Chunk &inst = chk.AddAndWriteChunkOffset();
	WriteURL(inst, url);
	inst.Write4u(iCamera);
}

void
CInstanceCamera::Read(io::IXMLReaderUTF8 *reader)
{	
	type = ITCamera;
	url = reader->getAttributeValueSafe("url");
	ChangeExtension(url);

	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT_END:
			if(tags::instanceCamera == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
	assert(false);
}

void 
CInstanceLight::ResolveURL(CCollada &data)
{
	iLight = -1;
	
	/*std::list<CCamera>::iterator cameraIt = data.libraryCameras.cameras.begin();
	for(int i = 0; i < data.libraryCameras.cameras.size(); i++)
	{
		if(cameraIt->id == url.c_str() + 1)
		{
			iCamera = i;
			break;
		}
		cameraIt++;
	}*/
}

// [4B+4B] - camera count + ptr
void 
CInstanceLight::Write(Chunk &chk)
{
	chk.Write4u(type);
	Chunk &inst = chk.AddAndWriteChunkOffset();
	WriteURL(inst, url);
	inst.Write4u(iLight);
}

void
CInstanceLight::Read(io::IXMLReaderUTF8 *reader)
{	
	type = ITLight;
	url = reader->getAttributeValueSafe("url");
	ChangeExtension(url);

	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT_END:
			if(tags::instanceLight == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
	assert(false);
}