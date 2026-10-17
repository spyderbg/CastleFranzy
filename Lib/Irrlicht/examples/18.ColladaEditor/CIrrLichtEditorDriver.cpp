#include "CIrrLichtEditorCamera.h"
#include "CIrrLichtEditorDriver.h"
#include "CMessageManager.h"
#include "CIrrLichtCanvas.h"
#include "CIrrLichtEditorDevice.h"
#include "ISceneManager.h"

using namespace irr;
using namespace video;

CEditorDrawFilter CEditorDrawFilter::s_Instance;

bool EditorFlipValue = false;

CEditorDriver *CEditorDriver::s_Instance = 0;

void 
CEditorDriver::setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat)
{
	core::matrix4 transMat = mat;
	
	switch(state)
	{
	case video::ETS_TEXTURE_0:
	case video::ETS_TEXTURE_1:
	case video::ETS_TEXTURE_2:
	case video::ETS_TEXTURE_3:
		if(EditorFlipValue)
		{
			core::matrix4 mat;
			mat(2, 0) = 1;
			mat(1, 1) = -1;
			transMat = mat * transMat;
		}
		break;
	}

	if(Mode == E_CAPTURE_FRAME)
	{
		switch(state)
		{
		case video::ETS_PROJECTION:
			Projection = mat;
			break;
		case video::ETS_VIEW:
			View = mat;
			break;
		default:
			COpenGLDriver::setTransform(state, transMat);
			break;
		}
		_IRR_DEBUG_BREAK_IF(!COpenGLDriver::getTransform(video::ETS_VIEW).isIdentity());
	}
	else if((pEditingCamera == 0) || (Mode == E_DISPLAY_CAPTURE))
	{
		COpenGLDriver::setTransform(state, transMat);
	}
	else
	{
		switch(state)
		{
		case video::ETS_PROJECTION:
			Projection = mat;
			break;
		case video::ETS_VIEW:
			View = mat;
			break;
		default:
			COpenGLDriver::setTransform(state, transMat);
			break;
		}
	}
}


void
CEditorDriver::displayCapture()
{	
	CIrrLichtCanvas::s_pInstance->EndSceneModifications();
	// Allow scene modification outside this thread
	CIrrLichtCanvas::s_pInstance->BeginSceneModifications();

	SColor oldClearColor = getClearColor();

	if(pEditingCamera)
	{
		if(pEditingCamera->getParent() == 0)
		{
			CEditorDevice::s_Instance->getSceneManager()->getRootSceneNode()->addChild(pEditingCamera);
		}
		pEditingCamera->OnAnimate(os::Timer::getTime());
		pEditingCamera->OnRegisterSceneNode();
		COpenGLDriver::setTransform(video::ETS_PROJECTION, pEditingCamera->getViewFrustum()->Matrices [ video::ETS_PROJECTION ] );
		COpenGLDriver::setTransform(video::ETS_VIEW, pEditingCamera->getViewFrustum()->Matrices [ video::ETS_VIEW ] );
	}
	else
	{
		//COpenGLDriver::setTransform(video::ETS_PROJECTION, pEditingCamera->getViewFrustum()->Matrices [ video::ETS_PROJECTION ] );
		//COpenGLDriver::setTransform(video::ETS_VIEW, pEditingCamera->getViewFrustum()->Matrices [ video::ETS_VIEW ] );
	}

	//COpenGLDriver::beginScene(true, true, SColor(0), windowId, sourceRect);

	if(DisplayCaptureMode == EDCM_OVERDRAW)
	{
		setClearColor(SColor(0));
		COpenGLDriver::beginScene();
		clearBuffers(EFB_COLOR | EFB_DEPTH);
		video::SMaterial mat;
		mat.setFlag(EMF_LIGHTING, false);
		mat.setFlag(EMF_PER_VERTEX_MATERIAL_COLOR, false);
		mat.setFlag(EMF_ZWRITE_ENABLE, true);
		mat.setFlag(EMF_ZBUFFER, true);
		setOption(EVDO_ALLOW_ZWRITE_ON_TRANSPARENT, true);
		mat.setMaterialType(EMT_TRANSPARENT_ADD_COLOR);
		mat.setDiffuseColor(video::SColor(255, 32, 32, 32));
		mat.setAmbientColor(video::SColor(255, 32, 32, 32));
		mat.setEmissiveColor(video::SColor(0,0,0,0));
		

		COpenGLDriver::setMaterial(mat);
		setDepthMask(true);

		glColorMask(false, true, false, false);
		for(int i = 0; i < DrawCalls.size(); i++)
		{
			
			//if(!DrawCalls[i]->Show)
			{
				//COpenGLDriver::setMaterial(DrawCalls[i]->Material);
				mat.setFlag(EMF_ZWRITE_ENABLE, DrawCalls[i]->Material.getFlag(EMF_ZWRITE_ENABLE));
				mat.setFlag(EMF_ZBUFFER, DrawCalls[i]->Material.getFlag(EMF_ZBUFFER));
				mat.setFlag(EMF_BACK_FACE_CULLING, DrawCalls[i]->Material.getFlag(EMF_BACK_FACE_CULLING));
				mat.setFlag(EMF_FRONT_FACE_CULLING, DrawCalls[i]->Material.getFlag(EMF_FRONT_FACE_CULLING));
				COpenGLDriver::setMaterial(mat);
				setDepthMask(true);

				glColorMask(false, true, false, false);
				IMaterialRenderer* mr = getMaterialRenderer(!DrawCalls[i]->Material.getMaterialType());
				setOption(EVDO_ALLOW_ZWRITE_ON_TRANSPARENT, !(mr && mr->isTransparent()));

				COpenGLDriver::setTransform(video::ETS_WORLD, DrawCalls[i]->WorldTransform);
				
				if(!pEditingCamera)
				{
					COpenGLDriver::setTransform(video::ETS_PROJECTION, 
						DrawCalls[i]->ProjectionTransform );
					COpenGLDriver::setTransform(video::ETS_VIEW, 
						DrawCalls[i]->ViewTransform );
				}
				
				COpenGLDriver::setDepthRange(DrawCalls[i]->NearRange, DrawCalls[i]->FarRange);

				irr::video::S3DVertexComponentArrays vertices = DrawCalls[i]->Vertices;
				vertices.Color0 = 0;
				vertices.TexCoord[0].Coord = 0;
				vertices.TexCoord[1].Coord = 0;
				COpenGLDriver::drawVertexPrimitiveList((void *)&vertices, DrawCalls[i]->IndexListU16,
					DrawCalls[i]->StartIndex, DrawCalls[i]->EndIndex, DrawCalls[i]->PrimitiveCount,
					DrawCalls[i]->vType, DrawCalls[i]->pType, DrawCalls[i]->iType, DrawCalls[i]->fVBOFlag);
			}
		}

		glColorMask(true, true, true, true);
		setDepthMask(GL_TRUE);
		glClear(GL_DEPTH_BUFFER_BIT);
		
		ITexture *pUsedTexture = 0;
		if(DrawCalls.size() > 0)
		{
			
			for(int i = 0; i < DrawCalls.size(); i++)
			{
				if(DrawCalls[i]->Show)
				{
					if(pUsedTexture == 0)
					{
						pUsedTexture = DrawCalls[i]->Material.getTexture(0);
					}
					COpenGLDriver::setMaterial(DrawCalls[i]->Material);
					COpenGLDriver::setTransform(video::ETS_WORLD, DrawCalls[i]->WorldTransform);
					
					if(!pEditingCamera)
					{
						COpenGLDriver::setTransform(video::ETS_PROJECTION, 
							DrawCalls[i]->ProjectionTransform );
						COpenGLDriver::setTransform(video::ETS_VIEW, 
							DrawCalls[i]->ViewTransform );
					}

					COpenGLDriver::setDepthRange(DrawCalls[i]->NearRange, DrawCalls[i]->FarRange);

					COpenGLDriver::drawVertexPrimitiveList((void *)&DrawCalls[i]->Vertices, DrawCalls[i]->IndexListU16,
						DrawCalls[i]->StartIndex, DrawCalls[i]->EndIndex, DrawCalls[i]->PrimitiveCount,
						DrawCalls[i]->vType, DrawCalls[i]->pType, DrawCalls[i]->iType, DrawCalls[i]->fVBOFlag);
				}
			}
		}
	}

	if(DisplayCaptureMode == EDCM_SELECTED_DRAW_CALL)
	{
		setClearColor(SColor(0xffffffff));
		COpenGLDriver::beginScene();
		clearBuffers(EFB_COLOR | EFB_DEPTH);
		glColorMask(false, true, false, false);
		if(DrawCalls.size() > 0)
		{
			
			for(int i = 0; i < DrawCalls.size(); i++)
			{
				if(!DrawCalls[i]->Show)
				{
					COpenGLDriver::setMaterial(DrawCalls[i]->Material);
					COpenGLDriver::setTransform(video::ETS_WORLD, DrawCalls[i]->WorldTransform);
					
					if(!pEditingCamera)
					{
						COpenGLDriver::setTransform(video::ETS_PROJECTION, 
							DrawCalls[i]->ProjectionTransform );
						COpenGLDriver::setTransform(video::ETS_VIEW, 
							DrawCalls[i]->ViewTransform );
					}

					COpenGLDriver::setDepthRange(DrawCalls[i]->NearRange, DrawCalls[i]->FarRange);

					COpenGLDriver::drawVertexPrimitiveList((void *)&DrawCalls[i]->Vertices, DrawCalls[i]->IndexListU16,
						DrawCalls[i]->StartIndex, DrawCalls[i]->EndIndex, DrawCalls[i]->PrimitiveCount,
						DrawCalls[i]->vType, DrawCalls[i]->pType, DrawCalls[i]->iType, DrawCalls[i]->fVBOFlag);
				}
			}
		}

		glColorMask(true, true, true, true);
		setDepthMask(GL_TRUE);
		glClear(GL_DEPTH_BUFFER_BIT);
		
		ITexture *pUsedTexture = 0;
		if(DrawCalls.size() > 0)
		{
			
			for(int i = 0; i < DrawCalls.size(); i++)
			{
				if(DrawCalls[i]->Show)
				{
					if(pUsedTexture == 0)
					{
						pUsedTexture = DrawCalls[i]->Material.getTexture(0);
					}
					COpenGLDriver::setMaterial(DrawCalls[i]->Material);
					COpenGLDriver::setTransform(video::ETS_WORLD, DrawCalls[i]->WorldTransform);
					
					if(!pEditingCamera)
					{
						COpenGLDriver::setTransform(video::ETS_PROJECTION, 
							DrawCalls[i]->ProjectionTransform );
						COpenGLDriver::setTransform(video::ETS_VIEW, 
							DrawCalls[i]->ViewTransform );
					}

					COpenGLDriver::setDepthRange(DrawCalls[i]->NearRange, DrawCalls[i]->FarRange);

					COpenGLDriver::drawVertexPrimitiveList((void *)&DrawCalls[i]->Vertices, DrawCalls[i]->IndexListU16,
						DrawCalls[i]->StartIndex, DrawCalls[i]->EndIndex, DrawCalls[i]->PrimitiveCount,
						DrawCalls[i]->vType, DrawCalls[i]->pType, DrawCalls[i]->iType, DrawCalls[i]->fVBOFlag);
				}
			}
		}

		setDepthMask(GL_TRUE);
		glClear(GL_DEPTH_BUFFER_BIT);

		if(pUsedTexture)
		{
			COpenGLDriver::beginScene2D();

			core::rect<s32> sourceRect;
			core::rect<s32> destRect;
			sourceRect.LowerRightCorner.X = pUsedTexture->getSize().Width;
			sourceRect.LowerRightCorner.Y = pUsedTexture->getSize().Height;

			int iMaxDimension = pUsedTexture->getSize().Width > pUsedTexture->getSize().Height 
				? pUsedTexture->getSize().Width : pUsedTexture->getSize().Height;
			
			destRect.LowerRightCorner.X = 128.0f * pUsedTexture->getSize().Width / iMaxDimension;
			destRect.LowerRightCorner.Y = 128.0f * pUsedTexture->getSize().Height / iMaxDimension;
		
			draw2DImage(pUsedTexture, destRect, sourceRect);

			COpenGLDriver::endScene2D();
		}	
	}

	//if(pEditingCamera)	// Show frustum box
	//{
	//	video::SMaterial mat;
	//	/*mat.setFlag(EMF_LIGHTING, false);
	//	mat.setFlag(EMF_PER_VERTEX_MATERIAL_COLOR, false);
	//	mat.setFlag(EMF_ZWRITE_ENABLE, false);
	//	mat.setFlag(EMF_ZBUFFER, false);
	//	setOption(EVDO_ALLOW_ZWRITE_ON_TRANSPARENT, false);
	//	mat.setMaterialType(EMT_SOLID);
	//	mat.setDiffuseColor(video::SColor(255, 255, 255,255));
	//	mat.setAmbientColor(video::SColor(255, 255, 255, 255));
	//	mat.setEmissiveColor(video::SColor(255,255,255,255));*/
	//	setMaterial(mat);
	//	COpenGLDriver::setTransform(video::ETS_WORLD, core::matrix4());

	//	for(int i = 0; i < DrawCalls.size(); i++)
	//	{
	//		core::matrix4 frustum, frustum2; // = DrawCalls[i]->ProjectionTransform * DrawCalls[i]->ViewTransform;
	//		frustum2.setbyproduct(DrawCalls[i]->ProjectionTransform, DrawCalls[i]->ViewTransform);
	//		if(frustum2.makeInverse())
	//		{
	//			frustum2.getTransposed(frustum);


	//			core::vector3df origin;
	//			
	//			core::vector3df upLeft;
	//			core::vector3df upRight;
	//			core::vector3df bottomLeft;
	//			core::vector3df bottomRight;

	//			frustum2.transformVect(origin, core::vector3df(0, 0, 0));
	//			frustum2.transformVect(upLeft, core::vector3df(-1, 1, 1));
	//			frustum2.transformVect(upRight, core::vector3df(1, 1, 1));
	//			frustum2.transformVect(bottomLeft, core::vector3df(-1, 1, -1));
	//			frustum2.transformVect(bottomRight, core::vector3df(1, 1, -1));

	//			core::vector3df box[16];
	//			draw3DLine(origin, upLeft);
	//			draw3DLine(origin, upRight);
	//			draw3DLine(origin, bottomLeft);
	//			draw3DLine(origin, bottomRight);
	//			draw3DLine(upLeft, upRight);
	//			draw3DLine(upRight, bottomRight);
	//			draw3DLine(bottomRight, bottomLeft);
	//			draw3DLine(bottomLeft, upLeft);
	//		}

	//		/*
	//		unsigned short indices[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

	//		irr::video::S3DVertexComponentArrays vertices;
	//		vertices.Position = box;
	//		
	//		COpenGLDriver::drawVertexPrimitiveList((void *)&vertices, indices,
	//						0, 15, 8,
	//						EVT_COMPONENT_ARRAYS, scene::EPT_LINES, EIT_16BIT, false);*/
	//	}

	//	
	//}

	setClearColor(oldClearColor);
	COpenGLDriver::endScene();
	swapBuffers(0);
}

bool 
CEditorDriver::beginScene()
{
	iDrawCount = 0;

	if(Mode == E_BEGIN_CAPTURE_FRAME)
	{
		for(int i = 0; i < DrawCalls.size(); i++)
		{
			delete DrawCalls[i];
		}
		DrawCalls.clear();

		COpenGLDriver::setTransform(ETS_PROJECTION, core::matrix4());
		COpenGLDriver::setTransform(ETS_VIEW, core::matrix4());
		Mode = E_CAPTURE_FRAME;
	}
	else if(Mode == E_DISPLAY_CAPTURE)
	{
		CMessageManager::OnDisplayCapture(&DrawCalls);
		while(Mode == E_DISPLAY_CAPTURE)
		{
			displayCapture();
		}
		CMessageManager::OnDisplayCapture(0);
		//return 0;
	}

	if(pEditingCamera && Mode != E_CAPTURE_FRAME)
	{
		if(pEditingCamera->getParent() == 0)
		{
			CEditorDevice::s_Instance->getSceneManager()->getRootSceneNode()->addChild(pEditingCamera);
		}
		pEditingCamera->OnAnimate(os::Timer::getTime());
		pEditingCamera->OnRegisterSceneNode();
		COpenGLDriver::setTransform(video::ETS_PROJECTION, pEditingCamera->getViewFrustum()->Matrices [ video::ETS_PROJECTION ] );
		COpenGLDriver::setTransform(video::ETS_VIEW, pEditingCamera->getViewFrustum()->Matrices [ video::ETS_VIEW ] );
	}
	return COpenGLDriver::beginScene();
}

bool
CEditorDriver::endScene()
{
	if(Mode == E_CAPTURE_FRAME)
	{
		Mode = E_DISPLAY_CAPTURE;
	}

	if(pEditingCamera)
	{
	/*	core::matrix4 prjView(View * Projection, core::matrix4::EM4CONST_INVERSE);
		core::vector3df farLeftDown(-1, -1, 1), farRightDown(1, -1, 1);
		core::vector3df farLeftUp(-1, -1, 1), farRightUp(1, -1, 1);
		core::vector3df cameraPosition(0, 0, 0);

		prjView.transformVect(farLeftUp);
		prjView.transformVect(farLeftDown);
		prjView.transformVect(farRightDown);
		prjView.transformVect(farRightUp);
		prjView.transformVect(cameraPosition);

		#ifdef _IRR_COMPILE_WITH_EDITOR_
		draw3DLine(farLeftDown,		farRightDown);
		draw3DLine(farRightUp,		farRightDown);
		draw3DLine(farLeftUp,		farRightUp);
		draw3DLine(farLeftUp,		farLeftDown);
		draw3DLine(cameraPosition,	farRightDown);
		draw3DLine(cameraPosition,	farRightUp);
		draw3DLine(cameraPosition,	farLeftDown);
		draw3DLine(cameraPosition,	farLeftUp);
		#endif*/
	}

	return COpenGLDriver::endScene();
}

void 
CEditorDriver::setEditingCamera(irr::scene::ICameraSceneNode *pCamera)
{
	CIrrLichtCanvas::SLockSceneForModifications lock(CIrrLichtCanvas::s_pInstance);
	pEditingCamera = dynamic_cast<irr::CEditorCameraSceneNode *>(pCamera);
}



void 
CEditorDriver::drawVertexPrimitiveList(const void* vertices,
								 const void* indexList,
								 u32 startIndex,
								 u32 endIndex,
								 u32 primitiveCount,
								 E_VERTEX_TYPE vType,
								 scene::E_PRIMITIVE_TYPE pType,
								 E_INDEX_TYPE iType)
{
	/*if(DisplayCaptureMode == EDCM_OVERDRAW 
		&& vType == video::EVT_COMPONENT_ARRAYS
		&& Mode != E_CAPTURE_FRAME)
	{
		//bool oldBatchingOption = getOption(video::EVDO_BATCHING);
		//CNullDriver::setOption(video::EVDO_BATCHING, false);
		bool oldAllowWriteOnTransparent = getOption(EVDO_ALLOW_ZWRITE_ON_TRANSPARENT);
		setOption(EVDO_ALLOW_ZWRITE_ON_TRANSPARENT, !getCurrentMaterial().isTransparent());

		video::SMaterial mat;
		mat.setFlag(EMF_LIGHTING, false);
		mat.setFlag(EMF_PER_VERTEX_MATERIAL_COLOR, false);
		mat.setFlag(EMF_ZWRITE_ENABLE, !getCurrentMaterial().isTransparent());
		mat.setFlag(EMF_ZBUFFER, true);
		mat.setMaterialType(EMT_TRANSPARENT_ADD_COLOR);
		mat.setDiffuseColor(video::SColor(255, 32, 32, 32));
		mat.setAmbientColor(video::SColor(255, 32, 32, 32));
		mat.setEmissiveColor(video::SColor(0,0,0,0));
		
		COpenGLDriver::setMaterial(mat);

		glColorMask(false, true, false, false);
		
		irr::video::S3DVertexComponentArrays new_vertices = *(irr::video::S3DVertexComponentArrays*)vertices;
		new_vertices.Color0 = 0;
		COpenGLDriver::drawVertexPrimitiveList((void *)&new_vertices, indexList,
								  startIndex, endIndex,
								  primitiveCount,
								  vType, pType, iType, false);
			

		glColorMask(true, true, true, false);
		//CNullDriver::setOption(video::EVDO_BATCHING, oldBatchingOption);
		CNullDriver::setOption(EVDO_ALLOW_ZWRITE_ON_TRANSPARENT, oldAllowWriteOnTransparent);
	}
	else*/
	{
		COpenGLDriver::drawVertexPrimitiveList( vertices, indexList,
										  startIndex, endIndex,
										  primitiveCount,
										  vType, pType, iType);
	}
}

void 
CEditorDriver::drawVertexPrimitiveList(const void* vertices,
								 const void* indexList,
								 u32 startIndex,
								 u32 endIndex,
								 u32 primitiveCount,
								 E_VERTEX_TYPE vType,
								 scene::E_PRIMITIVE_TYPE pType,
								 E_INDEX_TYPE iType,
								 bool fVBOFlag)
{
	if (!primitiveCount || startIndex == endIndex)
		return;

	
	iDrawCount++;

	if(iDrawCount < CEditorDrawFilter::s_Instance.iMinDrawCall 
		|| iDrawCount > CEditorDrawFilter::s_Instance.iMaxDrawCall + 1)
	{
		return;
	}

	if(getCurrentMaterial().getMaterialType() < EMT_COUNT)
	{ 
		if(CEditorDrawFilter::s_Instance.bMaterial[getCurrentMaterial().getMaterialType()] == 0)
			return;
	}
	
	IMaterialRenderer* mr = getMaterialRenderer(getCurrentMaterial().getMaterialType());
	if(mr && mr->isTransparent())
	{
		if(CEditorDrawFilter::s_Instance.bTranslucent == 0)
		{
			return;
		}
	}
	else
	{
		if(CEditorDrawFilter::s_Instance.bSolid == 0)
		{
			return;
		}
	}

	if(primitiveCount < CEditorDrawFilter::s_Instance.iMinPrimitiveCount 
		|| primitiveCount > CEditorDrawFilter::s_Instance.iMaxPrimitiveCount)
	{
		return;
	}

	
	COpenGLDriver::drawVertexPrimitiveList( vertices, indexList,
									  startIndex, endIndex,
									  primitiveCount,
									  vType, pType, iType, fVBOFlag);
	
	
	if(Mode == E_CAPTURE_FRAME)
	{
		if (!fVBOFlag
			&& pType != scene::EPT_POINTS
			&& pType != scene::EPT_POINT_SPRITES
			&& pType != scene::EPT_LINES
			&& pType != scene::EPT_LINE_STRIP
			&& iType == EIT_16BIT)
		{
			// convert to components everything
			SScopedProcessArray<SColor> colorBuffer;
			if(vType != EVT_COMPONENT_ARRAYS)
			{
				convertVertexType(const_cast<void*>(vertices),
								  startIndex,
								  endIndex,
								  vType,
								  colorBuffer);
				vertices = &VertexComponents;
			}
			const S3DVertexComponentArrays* components
				= reinterpret_cast<const S3DVertexComponentArrays*>(vertices);

			core::matrix4 oglMatrix;
			glGetFloatv(GL_MODELVIEW_MATRIX, &oglMatrix[0]);
 
			CBatch::SInitData initData;
			initData.Material = getCurrentMaterial();
			initData.WorldTransform = oglMatrix;
			initData.ProjectionTransform = Projection;
			initData.ViewTransform = View;
			initData.Vertices = components;
			initData.IndexList = indexList;
			initData.StartIndex = startIndex;
			initData.EndIndex = endIndex;
			initData.PrimitiveCount = primitiveCount;
			initData.vType = EVT_COMPONENT_ARRAYS;
			initData.pType = pType;
			initData.iType = iType;
			initData.fVBOFlag = fVBOFlag;
			float nearFar[2];
			glGetFloatv(GL_DEPTH_RANGE, nearFar);
			initData.NearRange = nearFar[0];
			initData.FarRange = nearFar[1];
			CBatch *batch = new CBatch(initData);

			DrawCalls.push_back(batch);
		}
	}
}


//#ifdef _IRR_COMPILE_WITH_EDITOR_

namespace irr
{
namespace video
{

IVideoDriver* createEditorDriver(const core::dimension2d<s32>& screenSize,
	HWND window, u32 bits, bool stencilBuffer, io::IFileSystem* io, bool vsync, bool antiAlias)
{
	COpenGLDriver* ogl =  irrnew CEditorDriver(screenSize, window, stencilBuffer, io, antiAlias);
	if (!ogl->initDriver(screenSize, window, bits, vsync, stencilBuffer))
	{
		ogl->drop();
		ogl = 0;
	}
	return ogl;
}



}
}

//#endif


