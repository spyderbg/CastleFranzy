#include "stdafx.h"
#include "Core/irrlicht.h"
#include "IO/TouchScreen/TouchPad.h"
#include "PlatformSpecifics/Win32/CImageLoaderPVR.h"
#include <cstdlib>
#include <cstring>
#include <string>

bool g_visualizeWaves = false;
extern "C" void startGLLive() {}

class DesktopInput : public IEventReceiver
{
    bool down[2] = {false, false};
public:
    bool OnEvent(const SEvent& event)
    {
        if (event.EventType == EET_KEY_INPUT_EVENT && !event.KeyInput.PressedDown)
        {
            if (event.KeyInput.Key == KEY_ESCAPE) __IRR_DEVICE()->closeDevice();
            if (event.KeyInput.Key == KEY_F2) os::Timer::setSpeed(os::Timer::getSpeed() * 0.5f);
            if (event.KeyInput.Key == KEY_F3) os::Timer::setSpeed(os::Timer::getSpeed() * 2.0f);
        }
        if (event.EventType != EET_MOUSE_INPUT_EVENT) return false;
        const int x = event.MouseInput.X, y = event.MouseInput.Y;
        switch (event.MouseInput.Event)
        {
        case EMIE_LMOUSE_PRESSED_DOWN: down[0] = true; CTouchPad::EventTouchDown(0,x,y); break;
        case EMIE_RMOUSE_PRESSED_DOWN: down[1] = true; CTouchPad::EventTouchDown(1,x,y); break;
        case EMIE_LMOUSE_LEFT_UP: down[0] = false; CTouchPad::EventTouchUp(0,x,y); break;
        case EMIE_RMOUSE_LEFT_UP: down[1] = false; CTouchPad::EventTouchUp(1,x,y); break;
        case EMIE_MOUSE_MOVED:
            for (int i=0; i<2; ++i) if (down[i]) CTouchPad::EventTouchMove(i,x,y);
            break;
        default: break;
        }
        return false;
    }
};

int main(int argc, char** argv)
{
#ifdef CASTLE_LOCAL_DRI_DIR
    // Mesa/LLVM use RUNPATH for optional libraries. The dynamic loader must see
    // the local dependency directory before it starts, including under GDB.
    const char* previous = getenv("LD_LIBRARY_PATH");
    if (!previous || std::string(previous).find(CASTLE_LOCAL_LIBRARY_DIR) != 0)
    {
        std::string libraries = CASTLE_LOCAL_LIBRARY_DIR;
        if (previous && *previous) libraries += std::string(":") + previous;
        setenv("LD_LIBRARY_PATH", libraries.c_str(), 1);
        execv("/proc/self/exe", argv);
        perror("Restart with local libraries");
        return 1;
    }
    setenv("LIBGL_DRIVERS_PATH", CASTLE_LOCAL_DRI_DIR, 0);
    setenv("LIBGL_ALWAYS_SOFTWARE", "1", 0);
    setenv("__GLX_VENDOR_LIBRARY_NAME", "mesa", 0);
#endif
    const char* assets = CASTLE_ASSET_DIR;
    int frames = 0;
    for (int i=1; i<argc; ++i)
    {
        if (!strcmp(argv[i], "--assets") && i+1<argc) assets = argv[++i];
        else if (!strcmp(argv[i], "--frames") && i+1<argc) frames = atoi(argv[++i]);
        else { fprintf(stderr, "Usage: %s [--assets directory] [--frames count]\n", argv[0]); return 2; }
    }
    if (chdir(assets) != 0) { perror(assets); return 1; }
    const char* dataHome = getenv("XDG_DATA_HOME");
    const char* home = getenv("HOME");
    std::string save = dataHome && *dataHome ? dataHome : std::string(home ? home : ".") + "/.local/share";
    // Create parents without invoking a shell; XDG_DATA_HOME may contain spaces.
    save += "/castle-frenzy/";
    for (size_t i=1; i<save.size(); ++i) if (save[i]=='/') mkdir(save.substr(0,i).c_str(),0700);

    IrrlichtDevice* device = createDevice(video::EDT_OPENGL, dimension2d<s32>(480,320),32,false,false,false,0);
    if (!device) { fprintf(stderr,"Could not create an OpenGL window. Check DISPLAY and graphics drivers.\n"); return 1; }
    device->setWindowCaption(L"Castle Frenzy - Ubuntu");
    device->setResizeAble(false);
    CImageLoaderPVR* loader = new CImageLoaderPVR();
    device->getVideoDriver()->addExternalImageLoader(loader);
    loader->drop();
    Application* app = __APP();
    app->setFilePaths("", "./", save.c_str());
    app->InitLinux(device);
    DesktopInput input;
    app->RegisterForIrrlichtEvents(&input);
    int count = 0;
    while (device->run())
    {
        if (!app->isPaused()) app->Update();
        device->sleep(10);
        if (frames > 0 && ++count >= frames) device->closeDevice();
    }
    app->Quit();
    return 0;
}
