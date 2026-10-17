#include "CIrrLichtCanvas.h"
#include <Irrlicht.h>
#include <CColladaDatabase.h>
#include <CColladaConverter.h>
#include "CMainFrame.h"
#include "CMessageManager.h"
#include "CColladaSceneNodeAnimatorSet.h"
#include "CColladaSceneNodeAnimatorBlender.h"
#include <iostream>
#include "CIrrLichtEditorCamera.h"
#include "CIrrLichtEditorDriver.h"
#include "CIrrLichtEditorDevice.h"

using namespace irr;
using namespace video;

#ifdef _IRR_COMPILE_WITH_EDITOR_
bool g_bEditionMode = false;
#else
bool g_bEditionMode = true;
#endif


wxMutex CIrrLichtCanvas::s_SceneModiticationLock;

CIrrLichtCanvas *CIrrLichtCanvas::s_pInstance = 0;
CIrrLichtGameThread *CIrrLichtGameThread::s_pInstance = 0;

BEGIN_EVENT_TABLE(CIrrLichtCanvas, wxWindow)
      //EVT_PAINT(CIrrLichtCanvas::OnPaint)
	  EVT_MOUSE_EVENTS(CIrrLichtCanvas::OnMouseEvent)
      EVT_SIZE(CIrrLichtCanvas::OnResize)
	  EVT_DROP_FILES(CIrrLichtCanvas::OnDropFile)
	  EVT_KEY_DOWN(CIrrLichtCanvas::OnKeyDown)
	  EVT_KEY_UP(CIrrLichtCanvas::OnKeyUp)
	  EVT_CLOSE(CIrrLichtCanvas::OnClose)
END_EVENT_TABLE()

void
CIrrLichtCanvas::OnKeyDown(wxKeyEvent &event)
{
	irr::SEvent irrEvent;
	irrEvent.EventType = EET_KEY_INPUT_EVENT;
	irrEvent.KeyInput.PressedDown = true;
	

	switch(event.GetKeyCode())
	{
		case WXK_UP:
		case 'W':
		case 'w':
			irrEvent.KeyInput.Key = KEY_UP; 
			break;
		case WXK_DOWN:
		case 'S':
		case 's':
			irrEvent.KeyInput.Key = KEY_DOWN; 
			break;
		case WXK_LEFT:
		case 'A':
		case 'a':
			irrEvent.KeyInput.Key = KEY_LEFT; 
			break;
		case WXK_RIGHT:
		case 'D':
		case 'd':
			irrEvent.KeyInput.Key = KEY_RIGHT; 
			break;
		case WXK_ESCAPE:
			irrEvent.KeyInput.Key = KEY_ESCAPE; 
			break;
		default:
			event.Skip();
			return;
	}

	if(g_bEditionMode && m_pCamera)
	{
		m_pCamera->OnEvent(irrEvent);
	}
}

void
CIrrLichtCanvas::OnKeyUp(wxKeyEvent &event)
{
	irr::SEvent irrEvent;
	irrEvent.EventType = EET_KEY_INPUT_EVENT;
	irrEvent.KeyInput.PressedDown = false;

	switch(event.GetKeyCode())
	{
		case WXK_UP:
		case 'W':
		case 'w':
			irrEvent.KeyInput.Key = KEY_UP; 
			break;
		case WXK_DOWN:
		case 'S':
		case 's':
			irrEvent.KeyInput.Key = KEY_DOWN; 
			break;
		case WXK_LEFT:
		case 'A':
		case 'a':
			irrEvent.KeyInput.Key = KEY_LEFT; 
			break;
		case WXK_RIGHT:
		case 'D':
		case 'd':
			irrEvent.KeyInput.Key = KEY_RIGHT; 
			break;
		case WXK_ESCAPE:
			irrEvent.KeyInput.Key = KEY_ESCAPE; 
			break;
		default:
			event.Skip();
			return;
	}

	if(g_bEditionMode && m_pCamera)
	{
		m_pCamera->OnEvent(irrEvent);
	}
}

wxThread::ExitCode 
CIrrLichtLoadingThread::Entry()
{

	while(!TestDestroy())
	{
		if(m_pDedicatedCanvas)
		{
			m_pDedicatedCanvas->m_ThreadWakeMutex.Lock();
			if(WAIT_TIMEOUT == m_pDedicatedCanvas->m_ThreadWakeCondition.Wait(10))
			{
				m_pDedicatedCanvas->m_ThreadWakeMutex.Unlock();
			}
			else
			{
				m_pDedicatedCanvas->m_ThreadWakeMutex.Unlock();
				Loading loading;
				if(m_pDedicatedCanvas->popLoading(false, loading))
				{ // Found something that need conversion
					for(int i = 0; i < loading.Filenames.size(); i++)
					{
						wxString newName = loading.Filenames[i];
						if(newName.find(".dae") != wxString::npos)
						{
							newName.Replace(".dae", ".bdae");
							CMessageManager::LogMessage("Converting %s to %s", loading.Filenames[i], newName);
							irr::collada::ConvertDAE2BDAE(loading.Filenames[i], newName);
							loading.Filenames[i] = newName;
						}
						else if(newName.find(".max") != wxString::npos)
						{
							newName.Replace(".max", ".bdae");
							CMessageManager::LogMessage("Converting %s to %s", loading.Filenames[i], newName);
							irr::collada::ConvertMAX2BDAE(loading.Filenames[i], newName);
							loading.Filenames[i] = newName;
						}
					}
					m_pDedicatedCanvas->LoadAnimation(loading.Filenames, loading.pSceneNode);
				}
			}
		}

		
		// Convert invalid loading
		//Pause();
	}
	return 0;
}

void
CIrrLichtCanvas::pushLoading(Loading& loading)
{
	SLockSceneForModifications lock(this);
	loading.Filenames[0].MakeLower();
	m_Loadings.push_back(loading);
}

bool
CIrrLichtCanvas::popLoading(bool validity, Loading& loading)
{
	bool bFound = false;
	SLockSceneForModifications lock(this);
	if(validity == false)
	{
		for(std::vector<Loading>::iterator it = m_Loadings.begin();
			it != m_Loadings.end(); 
			it++)
		{
			if((it->Filenames[0].find(".dae") != wxString::npos)
				|| (it->Filenames[0].find(".max") != wxString::npos))
			{
				loading = *it;
				m_Loadings.erase(it);
				bFound = true;
				break;
			}
		}
	}
	else
	{
		for(std::vector<Loading>::iterator it = m_Loadings.begin(); 
			it != m_Loadings.end(); 
			it++)
		{
			if(it->Filenames[0].find(".bdae") != wxString::npos)
			{
				loading = *it;
				m_Loadings.erase(it);
				bFound = true;
				break;
			}
		}
	}
	return bFound;
}

CIrrLichtCanvas::CIrrLichtCanvas(wxWindow *parent, wxWindowID id ,
        const wxPoint& pos,
        const wxSize& size, long style,
        const wxString& name)
: wxWindow(parent, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE, name)
, m_pIrrlichtDriver(0)
, m_pCamera(0)
, m_pSceneManager(0)
, m_iLoadingThread(0)
, m_ThreadWakeCondition(m_ThreadWakeMutex)
, m_pDevice(0)
, m_pLoadingThread(0)
, m_pGameThread(0)
, m_bActiveCameraIsSet(true)
{
	s_pInstance = this;
	DragAcceptFiles(true);

	m_iLoadingThread = (wxThread::GetCPUCount() - 1) * 2;
	m_pLoadingThread = new CIrrLichtLoadingThread[m_iLoadingThread];
	m_ActiveCameraUID = "Camera_Preview-node";
	

	for(int i = 0; i < m_iLoadingThread; i++)
	{
		m_pLoadingThread[i].setDedicatedCanvas(this);
		m_pLoadingThread[i].Create();
		m_pLoadingThread[i].Run();
	}

	
	m_pGameThread = new CIrrLichtGameThread();
	m_pGameThread->setDedicatedCanvas(this);
	m_pGameThread->Create();
	m_pGameThread->Run();
}

void
CIrrLichtCanvas::OnClose(wxCloseEvent &evt)
{
	m_ThreadWakeMutex.Lock();
	m_ThreadWakeCondition.Broadcast();
	m_ThreadWakeMutex.Unlock();

	if(m_pLoadingThread)
	{
		for(int i = 0; i < m_iLoadingThread; i++)
		{
			m_pLoadingThread[i].Delete();
			m_pLoadingThread[i].Wait();
		}
		delete [] m_pLoadingThread;
	}
	
	if(m_pGameThread)
	{
		// TODO: Fix crash on close.
		exit(0);
		m_pGameThread->Delete();
	}
}

CIrrLichtCanvas::~CIrrLichtCanvas()
{
	
}

void
CIrrLichtCanvas::OnDropFile(wxDropFilesEvent& event)
{
	SLockSceneForModifications lock(this);
	for(int i = 0; i < event.GetNumberOfFiles(); i++)
	{
		if((event.GetFiles()[i].find(".bdae") != wxString::npos)
			|| (event.GetFiles()[i].find(".dae") != wxString::npos)
			|| (event.GetFiles()[i].find(".max") != wxString::npos))
		{
			LoadDatabase(event.GetFiles()[i]);
		}
	}
}

void
CIrrLichtCanvas::OnMouseEvent(wxMouseEvent& event)
{
	irr::SEvent irrEvent;
	irrEvent.EventType = EET_MOUSE_INPUT_EVENT;

	if(event.GetEventType() == wxEVT_LEFT_DCLICK)
	{
		irr::core::position2di pos(event.GetX(), event.GetY());
		irr::scene::ISceneNode *pNode = 0;
#ifndef _IRR_COMPILE_WITH_EDITOR_
		if(m_pCamera)
		{
			core::line3d<f32> ln = m_pDevice->getSceneManager()->getSceneCollisionManager()->getRayFromScreenCoordinates(pos, m_pCamera);

			if ( ln.start != ln.end )
			{
				pNode = m_pDevice->getSceneManager()->getSceneCollisionManager()->getSceneNodeFromRayBB(ln);
			}
		}
		else
		{
			pNode = 
				m_pDevice->getSceneManager()->getSceneCollisionManager()->getSceneNodeFromScreenCoordinatesBB(pos);
		}
#endif
		if(pNode)
		{
			wxCommandEvent MyEvent(SelectionChangedEvent);
			MyEvent.SetClientData(pNode);
			wxPostEvent(this, MyEvent);
		}
	}
	else if(event.GetEventType() == wxEVT_LEFT_DOWN)
	{
		SetFocus();
		irrEvent.MouseInput.Event = EMOUSE_INPUT_EVENT::EMIE_LMOUSE_PRESSED_DOWN;
	}
	else if(event.GetEventType() == wxEVT_LEFT_UP)
	{
		irrEvent.MouseInput.Event = EMOUSE_INPUT_EVENT::EMIE_LMOUSE_LEFT_UP;
	}
	else if(event.GetEventType() == wxEVT_MIDDLE_DOWN)
	{
		SetFocus();
		
//#ifdef _IRR_COMPILE_WITH_EDITOR_
		SLockSceneForModifications lock(this);
		g_bEditionMode = !g_bEditionMode;

		if(g_bEditionMode)
		{
			scene::ICameraSceneNode *activeCam = m_pDevice->getSceneManager()->getActiveCamera();
			if(activeCam)
			{
				core::vector3df upVector = activeCam->getUpVector();
				upVector.normalize();
				m_pCamera->setUpVector(upVector);
				m_pCamera->setPosition(activeCam->getAbsolutePosition());
				m_pCamera->setTarget(activeCam->getTarget());
				m_pCamera->setFOV(activeCam->getFOV());
			}
			#ifdef _IRR_COMPILE_WITH_EDITOR_
			((irr::video::CEditorDriver*)m_pDevice->getVideoDriver())->setEditingCamera(m_pCamera);
			#else
			m_pDevice->getSceneManager()->setActiveCamera(m_pCamera);
			#endif
		}
		else
		{
			((irr::video::CEditorDriver*)m_pDevice->getVideoDriver())->setEditingCamera(0);
		}
//#endif
		irrEvent.MouseInput.Event = EMOUSE_INPUT_EVENT::EMIE_MMOUSE_PRESSED_DOWN;
	}
	else if(event.GetEventType() == wxEVT_MIDDLE_UP)
	{
		irrEvent.MouseInput.Event = EMOUSE_INPUT_EVENT::EMIE_MMOUSE_LEFT_UP;
	}
	else if(event.GetEventType() == wxEVT_RIGHT_DOWN)
	{
		SetFocus();
		irrEvent.MouseInput.Event = EMOUSE_INPUT_EVENT::EMIE_RMOUSE_PRESSED_DOWN;
	}
	else if(event.GetEventType() == wxEVT_RIGHT_UP)
	{
		irrEvent.MouseInput.Event = EMOUSE_INPUT_EVENT::EMIE_RMOUSE_LEFT_UP;
	}
	else if(event.GetEventType() == wxEVT_MOTION)
	{
		irrEvent.MouseInput.Event = EMOUSE_INPUT_EVENT::EMIE_MOUSE_MOVED;
	}
	else if(event.GetEventType() == wxEVT_MOUSEWHEEL)
	{
		irrEvent.MouseInput.Event = EMOUSE_INPUT_EVENT::EMIE_MOUSE_WHEEL;
		irrEvent.MouseInput.Wheel = event.GetWheelRotation() / 120.0f;
	}
	else if((event.GetEventType() == wxEVT_ENTER_WINDOW) ||
			(event.GetEventType() == wxEVT_LEAVE_WINDOW))
	{
		event.Skip();
		return;
	}
	irrEvent.MouseInput.X = event.m_x;
	irrEvent.MouseInput.Y = event.m_y;
	
	if(g_bEditionMode && m_pCamera)
	{
		m_pCamera->OnEvent(irrEvent);
	}
}

void
CIrrLichtCanvas::OnResize(wxSizeEvent& event)
{	
	core::dimension2d<s32> size;
	m_iWidth = event.GetSize().GetWidth();
	m_iHeight = event.GetSize().GetHeight();
	m_bSizeHasChanged = true;
}

void
CIrrLichtCanvas::OnPaint(wxPaintEvent& event)
{

}

void
CIrrLichtCanvas::ProcessLoading()
{
	if(m_Loadings.size() > 0)
	{
		int iSize = m_Loadings.size();
		//for(int i = 0; i < iSize; i++)
		{
			m_ThreadWakeMutex.Lock();
			m_ThreadWakeCondition.Signal();
			m_ThreadWakeMutex.Unlock();
		}
		Loading loading;
		if(popLoading(true, loading))
		{
			
			if(loading.pSceneNode)
			{
				CMessageManager::LogMessage("Loading Animation : %s", loading.Filenames[0]);
				//std::cout << "Loading Animation : " << loading.pFilename << std::ends;
				LoadAnimation(loading.Filenames, loading.pSceneNode);
			}
			else
			{
				CMessageManager::LogMessage("Loading Scene : %s", loading.Filenames[0]);
				//std::cout << "Loading Scene : " << loading.pFilename << std::ends;
				LoadDatabase(loading.Filenames[0]);
			}
		}
	}
}

void 
CIrrLichtCanvas::LoadDatabase(const char *pFilename)
{
	if(m_pGameThread->GetId() == wxThread::GetCurrentId())
	{
		scene::ISceneNode *pNode =  collada::CColladaDatabase::constructScene(pFilename);
		m_pSceneManager->getRootSceneNode()->addChild(pNode);
		CMessageManager::OnSceneChanged();
	}
	else
	{
		Loading loading;
		loading.Filenames.push_back(pFilename);
		loading.pSceneNode = 0;
		pushLoading(loading);
	}
}

void
CIrrLichtCanvas::SetActiveCamera(const char *pUID)
{
	m_bActiveCameraIsSet = false;
	m_ActiveCameraUID = pUID;
}

void
CIrrLichtCanvas::LoadAnimation(std::vector<wxString> Filenames, scene::ISceneNode *pNode)
{
	if(m_pGameThread->GetId() == wxThread::GetCurrentId())
	{
		collada::CAnimationSet *pAnimationSet = new collada::CAnimationSet();
		pAnimationSet->setMismatchBehavior(collada::CAnimationSet::EMB_KEEP_ANIMATION);
		for(int i = 0; i < Filenames.size(); i++)
		{
			collada::CColladaDatabase database(Filenames[i]);
			pAnimationSet->addAnimationLibrary(database);
		}

		pAnimationSet->compile();
		collada::CSceneNodeAnimatorSet *pAnimatorSet = new collada::CSceneNodeAnimatorSet(pAnimationSet);
		/*collada::CSceneNodeAnimatorBlender *pBlender = new collada::CSceneNodeAnimatorBlender();
		pBlender->addAnimator(pAnimatorSet);
		pBlender->compile();
		pBlender->setWeight(0,1);
		pAnimatorSet->getTimelineCtrl()->setLoop(true);
		pAnimationSet->drop();
		pNode->addAnimator((irr::scene::ISceneNodeAnimator *)pBlender);*/
		pNode->addAnimator((irr::scene::ISceneNodeAnimator *)pAnimatorSet);
		pAnimatorSet->drop();
		
		CMessageManager::OnAddAnimation();
	}
	else
	{
		Loading loading;
		loading.Filenames = Filenames;
		loading.pSceneNode = pNode;
		pushLoading(loading);
	}
}
/*
void
CIrrLichtCanvas::OnEndScene()
{
	m_pEndOfSceneCallback();
}*/

#ifndef _IRR_COMPILE_EDITOR_STANDALONE_
	extern void main(int argc, char* argv[]);

	LRESULT WXDLLIMPEXP_CORE APIENTRY  wxWndProc(HWND hWnd, UINT message,
                                   WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	LRESULT  CanvasWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		wxWndProc(hWnd, message, wParam, lParam);

		switch(message)
		{
		case WM_SIZE:
			CEditorDevice::s_Instance->pushMessage(hWnd, message, wParam, lParam);
			break;
		default:
			if(!g_bEditionMode)
			{
				CEditorDevice::s_Instance->pushMessage(hWnd, message, wParam, lParam);
			}
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
	}
#endif



wxThread::ExitCode 
CIrrLichtGameThread::Entry()
{
	
#ifndef _IRR_COMPILE_EDITOR_STANDALONE_

	irr::SIrrlichtCreationParameters param;
	param.Vsync = true;
	param.Bits = 16;
	param.WindowSize = core::dimension2d<s32>(m_pDedicatedCanvas->GetVirtualSize().GetWidth(),m_pDedicatedCanvas->GetVirtualSize().GetHeight());
	param.DriverType = EDT_OPENGL;

        // Use GetHandle() of any wxWindow to get the WindowId where you 
        // want to draw your scene in
	param.WindowId = (void *)reinterpret_cast<s32>( m_pDedicatedCanvas->GetHandle() );

        //Create IrrlichtDevice
	m_pDedicatedCanvas->m_pDevice = createEditorDeviceEx(param);
	m_pDedicatedCanvas->m_pDevice->setResizeAble(true);
	
	//Show(true);

	char* argv[] = {(char *)m_pDedicatedCanvas->m_pDevice };

	::SetWindowLong((HWND)reinterpret_cast<s32>(m_pDedicatedCanvas->GetHandle()), GWL_WNDPROC, (LONG_PTR)CanvasWndProc);

	scene::ICameraSceneNode *pCamera = new CEditorCameraSceneNode();
	pCamera->setFarValue(20000.f);
	pCamera->setPosition(core::vector3df(0, 50, -200));
	m_pDedicatedCanvas->m_pCamera = pCamera;

	//((irr::video::CEditorDriver*)m_pDevice->getVideoDriver())->setEditingCamera(pCamera);
	m_pDedicatedCanvas->m_pDevice->getSceneManager()->getRootSceneNode()->addChild(pCamera);

	main(1, argv);
	glFinish();
	
#else
	m_pDedicatedCanvas->BeginSceneModifications();

	irr::collada::CColladaDatabase::setVersionCheckBehavior(irr::collada::CColladaDatabase::EVCB_DONT_LOAD);
	irr::collada::CColladaDatabase::optimizeSceneNodes(false);

	irr::SIrrlichtCreationParameters param;
	param.Vsync = true;
	param.Bits = 16;
	param.WindowSize = core::dimension2d<s32>(m_pDedicatedCanvas->GetVirtualSize().GetWidth(),m_pDedicatedCanvas->GetVirtualSize().GetHeight());
	param.DriverType = EDT_OPENGL;

        // Use GetHandle() of any wxWindow to get the WindowId where you 
        // want to draw your scene in
	param.WindowId = (void *)reinterpret_cast<s32>( m_pDedicatedCanvas->GetHandle() );

        //Create IrrlichtDevice
	m_pDedicatedCanvas->m_pDevice = createDeviceEx(param);
	m_pDedicatedCanvas->m_pDevice->setResizeAble(true);

	m_pDedicatedCanvas->Show(true);


	scene::ISceneManager *smgr = m_pDedicatedCanvas->m_pDevice->getSceneManager();
	m_pDedicatedCanvas->m_pIrrlichtDriver = m_pDedicatedCanvas->m_pDevice->getVideoDriver();

	m_pDedicatedCanvas->m_pSceneManager = smgr;
	//m_pSceneGraph->setSceneManager(smgr);

	m_pDedicatedCanvas->m_pSceneManager->addLightSceneNode();
	m_pDedicatedCanvas->m_pSceneManager->addLightSceneNode(0, core::vector3df(0,200,0), video::SColorf(1.0f,1.0f,1.0f),100);
	

	// add a camera scene node
	scene::ICameraSceneNode *pCamera = new CEditorCameraSceneNode();
	pCamera->setFarValue(20000.f);
	pCamera->setPosition(core::vector3df(0, 50, -200));
	m_pDedicatedCanvas->m_pSceneManager->getRootSceneNode()->addChild(pCamera);
	
	m_pDedicatedCanvas->m_pCamera = pCamera;
	m_pDedicatedCanvas->m_pSceneManager->setActiveCamera(pCamera);

	//m_pSceneGraph->Update();
    // Do a test rendering
	
	while(!TestDestroy() && m_pDedicatedCanvas->m_pDevice->run())
	{
		IVideoDriver *videoDriver = m_pDedicatedCanvas->m_pDevice->getVideoDriver();
		scene::ISceneManager *sceneManager = m_pDedicatedCanvas->m_pSceneManager;

		m_pDedicatedCanvas->EndSceneModifications();
		wxThread::This()->Yield(); // Allow other thread to modify the scene
		m_pDedicatedCanvas->BeginSceneModifications();

		m_pDedicatedCanvas->ProcessLoading();

		if(m_pDedicatedCanvas->m_bSizeHasChanged)
		{
			core::dimension2d<s32> size;
			size.Width = m_pDedicatedCanvas->m_iWidth;
			size.Height = m_pDedicatedCanvas->m_iHeight;
			m_pDedicatedCanvas->m_pIrrlichtDriver->OnResize(size);
			m_pDedicatedCanvas->m_bSizeHasChanged = false;
		}

		if(!m_pDedicatedCanvas->m_bActiveCameraIsSet)
		{
			irr::scene::ISceneNode *node = sceneManager->getSceneNodeFromUID(m_pDedicatedCanvas->m_ActiveCameraUID.c_str());
			if(node)
			{
				irr::scene::ISceneNode *firstChild = *node->getChildren().begin();
				if(firstChild->getType() == irr::scene::ESNT_COLLADA_CAMERA)
				{
					sceneManager->setActiveCamera((scene::ICameraSceneNode *)firstChild);
					m_pDedicatedCanvas->m_bActiveCameraIsSet = true;
				}
			}
		}

		scene::ICameraSceneNode* cam = sceneManager->getActiveCamera();
		if(cam)
		{
			cam->setAspectRatio((float)m_pDedicatedCanvas->m_iWidth / (float)m_pDedicatedCanvas->m_iHeight);
		}
		
		videoDriver->setClearColor(irr::video::SColor(255, 50, 50, 50)); 
		videoDriver->beginScene();
		videoDriver->clearBuffers(irr::video::EFB_COLOR | irr::video::EFB_DEPTH); 
		sceneManager->update();
		sceneManager->drawAll();
		videoDriver->endScene();
		videoDriver->swapBuffers(0);

		sceneManager->getParameters()->setAttribute("FPS", m_pDedicatedCanvas->m_pDevice->getVideoDriver()->getFPS());
		sceneManager->getParameters()->setAttribute("Primitive Count", (int)m_pDedicatedCanvas->m_pDevice->getVideoDriver()->getPrimitiveCountDrawn());
	}

	m_pDedicatedCanvas->EndSceneModifications();

#endif
	return 0;
}

#ifdef _IRR_COMPILE_EDITOR_STANDALONE_
void* IrrAlloc(size_t size, irr::MemHint hint) 
{
	return malloc(size);
}

void* IrrAlloc(size_t size, irr::MemHint hint, int block, const char* filename, int line)
{
	return IrrAlloc(size, hint);
}


void IrrFree(void* p)
{
	free(p);
}
#endif


