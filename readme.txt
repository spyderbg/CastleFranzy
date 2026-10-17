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
   Save data goes to $XDG_DATA_HOME/castle-frenzy/ or, by default,
   ~/.local/share/castle-frenzy/.

3. VS Code:

   Open this project root and install the recommended Microsoft C/C++ and
   CMake Tools extensions. Select "Castle Frenzy: Ubuntu Debug" in Run and
   Debug, then press F5. Configuration and compilation run automatically.
   Ctrl+Shift+B builds; the "Run Castle Frenzy (Ubuntu)" task runs without GDB.

Controls: left mouse button/drag emulates touch; right mouse button emulates
another touch. Escape exits. F2 halves game speed; F3 doubles game speed.
For a bounded smoke run: ./build/linux-debug/bin/FortressDefender --frames 600

The Windows and iPhone projects remain under Projects/Win32 and Projects/IPhone.



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

