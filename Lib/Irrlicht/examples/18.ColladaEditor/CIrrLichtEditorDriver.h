#ifndef __CIRRLICHTEDITORDRIVER_H__
#define __CIRRLICHTEDITORDRIVER_H__

#include "COpenGLDriver.h"
#include <vector>
#include "CIrrLichtBatch.h"
#include "Matrix4.h"

namespace irr
{
namespace scene
{
class ICameraSceneNode;
};
class CEditorCameraSceneNode;
};



namespace irr
{
namespace video
{
class IVideoDriver;

class CEditorDrawFilter
	: public virtual io::IAttributeExchangingObject
{
public:
	CEditorDrawFilter()
		: iMinDrawCall(0), iMaxDrawCall(65535)
		, iMinPrimitiveCount(0), iMaxPrimitiveCount(65535)
		, bTranslucent(true), bSolid(true)
	{
		for(int i = 0; i < kMaterialCount; i++)
		{
			bMaterial[i] = true;
		}
	}
	unsigned int iMinDrawCall;
	unsigned int iMaxDrawCall;
	unsigned int iMinPrimitiveCount;
	unsigned int iMaxPrimitiveCount;
	bool bTranslucent;
	bool bSolid;
	static const int kMaterialCount = sizeof(sBuiltInMaterialTypeNames) / sizeof(sBuiltInMaterialTypeNames[0]);
	bool bMaterial[kMaterialCount];

	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
	{
		out->addInt("Draw Calls Min", iMinDrawCall);
		out->addInt("Draw Calls Max", iMaxDrawCall);
		out->addInt("Primitive Count Min", iMinPrimitiveCount);
		out->addInt("Primitive Count Max", iMaxPrimitiveCount);
		out->addBool("Solid", bSolid);
		out->addBool("Translucent", bTranslucent);

		/*for(int i = 0; i < kMaterialCount; i++)
		{
			char temp[256];
			sprintf(temp, "Material.%s", sBuiltInMaterialTypeNames[i]);
			out->addBool(temp, bMaterial[i]);
		}*/
	}

	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
	{
		iMinDrawCall = in->getAttributeAsInt("Draw Calls Min");
		iMaxDrawCall = in->getAttributeAsInt("Draw Calls Max");
		iMinPrimitiveCount = in->getAttributeAsInt("Primitive Count Min");
		iMaxPrimitiveCount = in->getAttributeAsInt("Primitive Count Max");
		bSolid = in->getAttributeAsBool("Solid");
		bTranslucent = in->getAttributeAsBool("Translucent");
	}

	static CEditorDrawFilter s_Instance;
};

class CEditorDriver
	: public COpenGLDriver
{
public:
	static CEditorDriver *s_Instance;

	CEditorDriver(const core::dimension2d<s32>& screenSize, HWND window,
			bool stencilBuffer, io::IFileSystem* io, bool antiAlias)
			: COpenGLDriver(screenSize, window, stencilBuffer, io, antiAlias)
			, pEditingCamera(0)
			, Mode(E_DEFAULT)
			, DisplayCaptureMode(EDCM_SELECTED_DRAW_CALL)
	{
		s_Instance = this;
	}

	virtual ~CEditorDriver() {}

	virtual void setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat);

	virtual bool beginScene();

	virtual bool endScene();

	void setEditingCamera(irr::scene::ICameraSceneNode *pCamera);

	virtual void drawVertexPrimitiveList(const void* vertices,
								 const void* indexList,
								 u32 startIndex,
								 u32 endIndex,
								 u32 primitiveCount,
								 E_VERTEX_TYPE vType,
								 scene::E_PRIMITIVE_TYPE pType,
								 E_INDEX_TYPE iType,
								 bool fVBOFlag);

	virtual void  drawVertexPrimitiveList(const void* vertices,
								 const void* indexList,
								 u32 startIndex,
								 u32 endIndex,
								 u32 primitiveCount,
								 E_VERTEX_TYPE vType,
								 scene::E_PRIMITIVE_TYPE pType,
								 E_INDEX_TYPE iType);

	void captureDrawCall(const void* vertices,
								 const void* indexList,
								 u32 startIndex,
								 u32 endIndex,
								 u32 primitiveCount,
								 E_VERTEX_TYPE vType,
								 scene::E_PRIMITIVE_TYPE pType,
								 E_INDEX_TYPE iType,
								 bool fVBOFlag);


	enum E_Mode
	{
		E_BEGIN_CAPTURE_FRAME,
		E_CAPTURE_FRAME,
		E_DISPLAY_CAPTURE,
		E_DEFAULT,
	};

	E_Mode Mode;

	enum E_DISPLAY_CAPTURE_MODE
	{
		EDCM_SELECTED_DRAW_CALL,
		EDCM_OVERDRAW,
	};

	E_DISPLAY_CAPTURE_MODE DisplayCaptureMode;

protected:
	void displayCapture();

	core::matrix4 View, Projection;
	//core::matrix4 LastUsedView, LastUsedProjection;
	int iDrawCount;
	irr::CEditorCameraSceneNode *pEditingCamera;
	std::vector<CBatch *> DrawCalls;
};

};
};



#endif
