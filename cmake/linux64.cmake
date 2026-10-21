# Optional native Ubuntu dependencies prepared without sudo by bootstrap.sh.
get_filename_component(CASTLE_DEPS "${CMAKE_CURRENT_LIST_DIR}/../.deps/linux64/root" ABSOLUTE)
if(EXISTS "${CASTLE_DEPS}/usr/lib/x86_64-linux-gnu/libopenal.so")
    list(PREPEND CMAKE_PREFIX_PATH "${CASTLE_DEPS}/usr")
    list(PREPEND CMAKE_LIBRARY_PATH "${CASTLE_DEPS}/usr/lib/x86_64-linux-gnu")
    set(CMAKE_BUILD_RPATH "${CASTLE_DEPS}/usr/lib/x86_64-linux-gnu;${CASTLE_DEPS}/usr/lib/x86_64-linux-gnu/pulseaudio")
endif()
