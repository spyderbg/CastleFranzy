Ubuntu / Linux build (tested on Ubuntu 24.04 x86_64):

The game and its bundled Irrlicht fork must be compiled as 32-bit programs.
BRES model resources contain 32-bit pointers and cannot be loaded by a 64-bit
build without changing the asset format. The existing Build/bin assets are
ready to use; the old Windows/Java asset tools are not needed.

1. Install CMake, GCC/G++, make, Python 3 and GDB, then prepare dependencies:

   bash tools/linux/bootstrap.sh

   This downloads Ubuntu development/runtime packages into .deps/root without
   sudo. It includes Mesa software rendering for machines without a 32-bit GPU
   driver. The bootstrap uses the Ubuntu repositories and the installed GCC
   major version. Re-run it after moving the project, then configure in a new
   build directory (the local loader/library paths are absolute).

   Alternatively, install dependencies system-wide and omit the bootstrap:

   sudo dpkg --add-architecture i386
   sudo apt update
   sudo apt install build-essential cmake gdb g++-multilib libgl-dev:i386 \
       libglu1-mesa-dev:i386 libopenal-dev:i386 libx11-dev:i386 \
       libxxf86vm-dev:i386 libgl1-mesa-dri:i386 libpulse0:i386

2. From the project root:

   cmake --preset linux-debug
   cmake --build --preset linux-debug
   ./build/linux-debug/bin/FortressDefender

   For an optimized build, use the linux-release configure/build presets.
   Launch from a graphical desktop with DISPLAY available (X11 or XWayland).
   The executable locates Build/bin automatically, independent of the current
   working directory. Override with --assets /path/to/assets if needed.
   The window starts at 960x640 (2x the original 480x320 layout). Menus,
   fonts, gameplay and mouse input scale together; no asset rebuild is needed.
   Use --scale 1 for 480x320, --scale 3 for 1440x960, or --scale 4 for 1920x1280.
   Save data goes to $XDG_DATA_HOME/castle-frenzy/ or, by default,
   ~/.local/share/castle-frenzy/.

3. Full Linux rebuild and resource format:

   ./rebuild_Linux.sh

   RESOURCE_TEXTURE_FORMAT at the beginning of rebuild_Linux.sh selects "png"
   (default) or "pvr". It can also be overridden for one invocation:

   RESOURCE_TEXTURE_FORMAT=pvr ./rebuild_Linux.sh

   PNG mode requires Pillow (sudo apt install python3-pil). It prefers matching
   PNGs in Data/Sprites, Data/Fonts, Data/Scenes and Data/Effects; otherwise it
   decodes exported PVR textures or converts TGA/BMP images. PNG atlases must
   retain the dimensions used by their exported .bsprite files. The PNG output
   contains no PVR/TGA/BMP textures. Existing model/effect texture references
   resolve to the corresponding PNGs at runtime. PVR mode retains the original
   PVR textures and TGA fallbacks, as in the Windows build.

   The script reads the version from config.bat, prepares resources, performs a
   clean Release build, and creates build/packages/CastleFrenzy_Linux_v*_*.zip.
   Use --debug for Debug, --incremental to skip cleaning, --no-package to skip
   the ZIP, or --resources-only to update assets without compiling the game.
   Each preset's generated assets live in build/linux-{release,debug}/assets;
   CMake configures that build's executable to find them automatically.
   Build/bin and the source images are preserved. Logs are in the preset's
   build directory as rebuild.log. Run the ZIP's run.sh after extracting it.

   Node.js 22.12+ and npm are required for the AuroraMX sprite exporter.
   The resource step runs Data/Sprites/Sprites.sprcmd through AuroraMX to
   regenerate all 29 .bsprite files and frame/animation ID headers. Its npm
   dependencies are installed automatically on the first rebuild. Generated
   sprites are staged in the preset's auroramx/bin directory, then copied into
   its assets directory; original Data/Sprites/bin exports are preserved.
   The step reuses checked-in .bdae models, string tables and configuration
   XML, and rebuilds textures and WAV sound packs natively. Changes to .dae or
   spreadsheet layouts still require their legacy exporters and headers.
   Packages built with the rootless .deps setup retain absolute dependency
   paths to this project. Use system-installed dependencies when building
   packages for other machines.

4. VS Code:

   Open this project root and install the recommended Microsoft C/C++ and
   CMake Tools extensions. Select "Castle Frenzy: Ubuntu Debug" in Run and
   Debug, then press F5. Configuration and compilation run automatically.
   Ctrl+Shift+B builds; the "Run Castle Frenzy (Ubuntu)" task runs without GDB.

Controls: left mouse button/drag emulates touch; right mouse button emulates
another touch. Escape exits. F2 halves game speed; F3 doubles game speed.
For a bounded smoke run: ./build/linux-debug/bin/FortressDefender --frames 600

The Windows and iPhone projects remain under Projects/Win32 and Projects/IPhone.

AuroraMX sprite editor:

   ./Tools/AuroraMX/auroramx.sh

   This opens the TypeScript/Vue 3 editor in a browser at localhost:5180.
   It follows AuroraGT's module/frame/animation layout and supports source
   editing, undo/redo, PNG/TGA/BMP atlas previews, collision rectangles,
   animation playback, .bsprite export, and batch .sprcmd execution.
   Tools > Build game data runs the Linux resource build with AuroraMX.
   See Tools/AuroraMX/README.md for launch, development and format details.



Win32 build:

1. Set 'JAVA_PATH' and 'VSBuilder_EXE' paths in 'setenv.bat' file.
2. Set version number in 'config.bat' file.
3. Call 'rebuild_Win32.bat'
4. If no errors occure the game shoul be in 'Build' directory.


iPhone build:

1. Set 'JAVA_PATH' path in 'setenv.bat' file.
2. Set version number in 'config.bat' file.
3. In command line call 'make data_rebuild'.
4. From folder 'Projects/IPhone' open Mac project and rebuild it.

