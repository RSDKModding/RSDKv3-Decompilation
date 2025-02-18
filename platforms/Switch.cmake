# -----------------------------------------------------------------------------
# RSDKv3 NINTENDO SWITCH PLATFORM CONFIGURATION

add_executable(RetroEngine ${RETRO_FILES})

# Do not compile libraries, we will use the provided devkitPro ones instead
set(COMPILE_OGG OFF)
set(COMPILE_VORBIS OFF)
set(COMPILE_THEORA OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(NX_SDL2 REQUIRED sdl2)
pkg_check_modules(NX_GLESv1_CM REQUIRED glesv1_cm)
pkg_check_modules(NX_GLAPI REQUIRED glapi)
pkg_check_modules(NX_GLAD REQUIRED libglad)
pkg_check_modules(NX_OGG REQUIRED ogg)
pkg_check_modules(NX_VORBIS REQUIRED vorbis)
pkg_check_modules(NX_VORBISFILE REQUIRED vorbisfile)
pkg_check_modules(NX_THEORA REQUIRED theora)

# Set up our library includes to the switch portlibs
target_include_directories(RetroEngine PRIVATE
    ${NX_STANDARD_INCLUDE_DIRECTORIES}
    ${NX_SDL2_INCLUDE_DIRS}
    ${NX_GLESv1_CM_INCLUDE_DIRS}
    ${NX_GLAPI_INCLUDE_DIRS}
    ${NX_GLAD_INCLUDE_DIRS}
    ${NX_OGG_INCLUDE_DIRS}
    ${NX_VORBIS_INCLUDE_DIRS}
    ${NX_VORBISFILE_INCLUDE_DIRS}
    ${NX_THEORA_INCLUDE_DIRS}
)

# Set up our library links to the switch portlibs
target_link_libraries(RetroEngine PRIVATE
    ${NX_STANDARD_LIBRARIES}
    ${NX_SDL2_LIBRARIES}
    ${NX_GLESv1_CM_LIBRARIES}
    ${NX_GLAPI_LIBRARIES}
    -ldrm_nouveau
    ${NX_GLAD_LIBRARIES}
    ${NX_THEORA_LIBRARIES}
    ${NX_VORBIS_LIBRARIES}
    ${NX_VORBISFILE_LIBRARIES}
    ${NX_OGG_LIBRARIES}
    -lpthread
)

# Metadata Configuration
set(SWITCH_METADATA_DIR ${CMAKE_SOURCE_DIR}/RSDKv3.switch)
set(SWITCH_ICON ${SWITCH_METADATA_DIR}/icon.jpg)

if(NOT EXISTS ${SWITCH_ICON})
    message(WARNING "Icon not found at ${SWITCH_ICON}")
endif()

# NACP Configuration
nx_generate_nacp(RetroEngine.nacp
    NAME "Sonic CD (RSDKv3)"
    AUTHOR "RSDKModding"
    VERSION "${DECOMP_VERSION}"
)

# NRO Configuration
nx_create_nro(RetroEngine
    NACP RetroEngine.nacp
    ICON ${SWITCH_ICON}
    OUTPUT ${CMAKE_BINARY_DIR}/bin/switch/RSDKv3/RSDKv3.nro
)

# Custom Command to create our output directory
add_custom_command(TARGET RetroEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/switch/RSDKv3
)