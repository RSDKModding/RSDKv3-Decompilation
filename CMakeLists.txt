cmake_minimum_required(VERSION 3.7)

project(RetroEngine)

set(DECOMP_VERSION 1.3.2)

option(RETRO_FORCE_CASE_INSENSITIVE "Forces case insensivity." OFF)
option(RETRO_MOD_LOADER "Enables or disables the mod loader." ON)
option(RETRO_USE_HW_RENDER "Enables usage of Hardware Render as an option." ON)
option(RETRO_DISABLE_PLUS "Disables Plus. Should be set on for any public releases." OFF)
option(RETRO_ORIGINAL_CODE "Removes any change that differs from the original code, a playable game can't be built this way." OFF)

set(RETRO_SDL_VERSION 2 CACHE STRING "select between SDL2 and SDL1, defaults to SDL2")

if(RETRO_ORIGINAL_CODE)
    set(RETRO_MOD_LOADER OFF)
endif()

set(RETRO_NAME "RSDKv3")

set(RETRO_OUTPUT_NAME ${RETRO_NAME} CACHE STRING "The exported name of the executable.")

set(RETRO_FILES
    dependencies/all/tinyxml2/tinyxml2.cpp
    dependencies/all/theoraplay/theoraplay.c
    RSDKv3/Animation.cpp
    RSDKv3/Audio.cpp
    RSDKv3/Collision.cpp
    RSDKv3/Debug.cpp
    RSDKv3/Drawing.cpp
    RSDKv3/Ini.cpp
    RSDKv3/Input.cpp
    RSDKv3/fcaseopen.c
    RSDKv3/main.cpp
    RSDKv3/Math.cpp
    RSDKv3/ModAPI.cpp
    RSDKv3/Object.cpp
    RSDKv3/Palette.cpp
    RSDKv3/Player.cpp
    RSDKv3/Reader.cpp
    RSDKv3/RetroEngine.cpp
    RSDKv3/Scene.cpp
    RSDKv3/Scene3D.cpp
    RSDKv3/Script.cpp
    RSDKv3/Sprite.cpp
    RSDKv3/String.cpp
    RSDKv3/Text.cpp
    RSDKv3/Userdata.cpp
    RSDKv3/Video.cpp
)

if(NOT PLATFORM)
    if(WIN32) # THIS ASSUMES VCPKG OR SOURCES !!!!!!!
        set(PLATFORM "Windows" CACHE STRING "The platform to compile for.")
    elseif(ANDROID)
        set(PLATFORM "Android" CACHE STRING "The platform to compile for.")
    else()
        set(PLATFORM ${CMAKE_SYSTEM_NAME} CACHE STRING "The platform to compile for.")
    endif()
endif()

include(platforms/${PLATFORM}.cmake)

set_target_properties(RetroEngine PROPERTIES OUTPUT_NAME ${RETRO_OUTPUT_NAME})

if(COMPILE_OGG)
    set(OGG_DIR dependencies/${DEP_PATH}/libogg)
    add_library(
        libogg
        STATIC
        ${OGG_DIR}/src/bitwise.c
        ${OGG_DIR}/src/framing.c
    )

    target_compile_options(libogg PRIVATE ${OGG_FLAGS})

    target_include_directories(libogg PRIVATE ${OGG_DIR}/include)
    target_include_directories(RetroEngine PRIVATE ${OGG_DIR}/include)
    target_link_libraries(RetroEngine libogg)
endif()

if(COMPILE_VORBIS)
    set(VORBIS_DIR dependencies/${DEP_PATH}/libvorbis)
    set(OGG_DIR dependencies/${DEP_PATH}/libogg)
    add_library(libvorbis STATIC
        ${VORBIS_DIR}/lib/analysis.c
        ${VORBIS_DIR}/lib/barkmel.c
        ${VORBIS_DIR}/lib/bitrate.c
        ${VORBIS_DIR}/lib/block.c
        ${VORBIS_DIR}/lib/codebook.c
        ${VORBIS_DIR}/lib/envelope.c
        ${VORBIS_DIR}/lib/floor0.c
        ${VORBIS_DIR}/lib/floor1.c
        ${VORBIS_DIR}/lib/info.c
        ${VORBIS_DIR}/lib/lookup.c
        ${VORBIS_DIR}/lib/lpc.c
        ${VORBIS_DIR}/lib/lsp.c
        ${VORBIS_DIR}/lib/mapping0.c
        ${VORBIS_DIR}/lib/mdct.c
        ${VORBIS_DIR}/lib/psy.c
        ${VORBIS_DIR}/lib/registry.c
        ${VORBIS_DIR}/lib/res0.c
        ${VORBIS_DIR}/lib/sharedbook.c
        ${VORBIS_DIR}/lib/smallft.c
        ${VORBIS_DIR}/lib/synthesis.c
        ${VORBIS_DIR}/lib/tone.c
        ${VORBIS_DIR}/lib/vorbisenc.c
        ${VORBIS_DIR}/lib/vorbisfile.c
        ${VORBIS_DIR}/lib/window.c
    )

    target_compile_options(libvorbis PRIVATE ${VORBIS_FLAGS})

    target_include_directories(libvorbis
    PRIVATE
    ${VORBIS_DIR}/include
    ${VORBIS_DIR}/lib
    ${OGG_DIR}/include
    )
    target_include_directories(RetroEngine PRIVATE ${VORBIS_DIR}/include)
    target_link_libraries(RetroEngine libvorbis libogg)
endif()

if(COMPILE_THEORA)
    set(THEORA_DIR dependencies/${DEP_PATH}/libtheora)

    add_library(libtheora STATIC
        ${THEORA_DIR}/lib/analyze.c
        ${THEORA_DIR}/lib/apiwrapper.c
        ${THEORA_DIR}/lib/bitpack.c
        ${THEORA_DIR}/lib/cpu.c
        ${THEORA_DIR}/lib/decapiwrapper.c
        ${THEORA_DIR}/lib/decinfo.c
        ${THEORA_DIR}/lib/decode.c
        ${THEORA_DIR}/lib/dequant.c
        ${THEORA_DIR}/lib/encapiwrapper.c
        ${THEORA_DIR}/lib/encfrag.c
        ${THEORA_DIR}/lib/encinfo.c
        ${THEORA_DIR}/lib/encode.c
        ${THEORA_DIR}/lib/encoder_disabled.c
        ${THEORA_DIR}/lib/enquant.c
        ${THEORA_DIR}/lib/fdct.c
        ${THEORA_DIR}/lib/fragment.c
        ${THEORA_DIR}/lib/huffdec.c
        ${THEORA_DIR}/lib/huffenc.c
        ${THEORA_DIR}/lib/idct.c
        ${THEORA_DIR}/lib/info.c
        ${THEORA_DIR}/lib/internal.c
        ${THEORA_DIR}/lib/mathops.c
        ${THEORA_DIR}/lib/mcenc.c
        ${THEORA_DIR}/lib/quant.c
        ${THEORA_DIR}/lib/rate.c
        ${THEORA_DIR}/lib/state.c
        ${THEORA_DIR}/lib/tokenize.c
    )

    target_compile_options(libtheora PRIVATE ${THEORA_FLAGS})

    target_include_directories(libtheora PRIVATE ${THEORA_DIR}/include ${OGG_DIR}/include)
    target_include_directories(RetroEngine PRIVATE ${THEORA_DIR}/include)
    target_link_libraries(RetroEngine libtheora)
endif()

target_include_directories(RetroEngine PRIVATE
    RSDKv3/
    dependencies/all/theoraplay/
    dependencies/all/tinyxml2/
)

if(DEFINED DEP_PATH)
    target_include_directories(RetroEngine PRIVATE
        dependencies/${DEP_PATH}/
    )
endif()


target_compile_definitions(RetroEngine PRIVATE
    RETRO_USE_MOD_LOADER=$<BOOL:${RETRO_MOD_LOADER}>
    RETRO_USING_OPENGL=$<BOOL:${RETRO_USE_HW_RENDER}>
    RETRO_USE_SDL${RETRO_SDL_VERSION}=1
    FORCE_CASE_INSENSITIVE=$<BOOL:${RETRO_FORCE_CASE_INSENSITIVE}>
    RETRO_USE_ORIGINAL_CODE=$<BOOL:${RETRO_ORIGINAL_CODE}>
    
    RSDK_AUTOBUILD=$<BOOL:${RETRO_DISABLE_PLUS}>
    
    RETRO_DEV_EXTRA="${PLATFORM} - ${CMAKE_CXX_COMPILER_ID}"
    DECOMP_VERSION="${DECOMP_VERSION}"
)