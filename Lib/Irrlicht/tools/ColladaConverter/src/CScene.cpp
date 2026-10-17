#include "CDatabaseCollada.h"
#include "CColladaTags.h"

using namespace irr;
using namespace collada;
using namespace converter;

//! CScene::Write
// [4B] array size
// [4B] array pointer
void 
CScene::Write(Chunk &chk)
{
	chk.Write(intances, tags::scene.c_str());
}

void
CScene::Read(io::IXMLReaderUTF8 *reader)
{
	while(reader->read())
	{
		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT:
			if(tags::instanceVisualScene == reader->getNodeName())
			{
				CInstanceVisualScene *pInstance = new CInstanceVisualScene();
				pInstance->Read(reader);
				intances.push_back(pInstance);
			}
			break;
		case io::EXN_ELEMENT_END:
			if(tags::scene == reader->getNodeName())
			{
				return;
			}
			break;
		}
	}
}