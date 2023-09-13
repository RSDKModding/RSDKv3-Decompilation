![](header.png?raw=true)

A complete decompilation of Retro Engine v3.

# **SUPPORT THE OFFICIAL RELEASE OF SONIC CD**
+ Without assets from the official release, this decompilation will not run.

+ You can get an official release of Sonic CD from:
  * Windows
    * Via Steam, whether it's the original release or from [Sonic Origins](https://store.steampowered.com/app/1794960)
    * Via the Epic Games Store, from [Sonic Origins](https://store.epicgames.com/en-US/p/sonic-origins)
  * [iOS (Via the App Store)](https://apps.apple.com/us/app/sonic-cd-classic/id454316134)
    * A tutorial for finding the game assets from the iOS version can be found [here](https://gamebanana.com/tuts/14491).
  * Android
    * [Via Google Play](https://play.google.com/store/apps/details?id=com.sega.soniccd.classic)
    * [Via Amazon](https://www.amazon.com/Sega-of-America-Sonic-CD/dp/B008K9UZY4/)
    * A tutorial for finding the game assets from the Android version can be found [here](https://gamebanana.com/tuts/14942).

Even if your platform isn't supported by the official releases, you **must** buy or officially download it for the assets (you don't need to run the official release, you just need the game assets). Note that only FMV files from the original Steam release of the game are supported; mobile and Origins video files do not work.

# Advantages over the PC version of Sonic CD
* Sharp, pixel-perfect display.
* Controls are completely remappable via the settings.ini file.
* The window allows windows shortcuts to be used.
* Complete support for using mobile/updated scripts, allowing for features the official PC version never got to be played on PC.
* Native Windows x64 version, as well as an x86 version.

# Advantages over the Mobile versions of Sonic CD
* The rendering backend is based off the PC version by default, so palettes are fully supported (Tidal Tempest water in particular).

# Additional Tweaks
* Added a built in mod loader and API, allowing to easily create and play mods with features such as save file redirection and XML GameConfig data.
* There is now a settings.ini file that the game uses to load all settings, similar to Sonic Mania.
* The dev menu can now be accessed from anywhere by pressing the `ESC` key if enabled in the config.
* The `F12` pause, `F11` step over & fast forward debug features from Sonic Mania have all been ported and are enabled if `devMenu` is enabled in the config.
* A number of additional dev menu debug features have been added:
  * `F1` will load the first scene in the Presentation stage list (usually the title screen).
  * `F2` and `F3` will load the previous and next scene in the current stage list.
  * `F5` will reload the current scene, as well as all assets and scripts.
  * `F8` and `F9` will visualize touch screen and object hitboxes.
  * `F10` will activate a palette overlay that shows the game's 8 internal palettes in real time.
* If `useSteamDir` is set in the config (Windows only), the game will try to load savedata from Steam's `userdata` directory (where the original Steam version saves to).
* Added the idle screen dimming feature from Sonic Mania Plus, as well as allowing the user to disable it or set how long it takes for the screen to dim.

# How to Build

## Get the source code

Clone the repo **recursively**, using:
`git clone --recursive https://github.com/Rubberduckycooly/Sonic-CD-11-Decompilation`

If you've already cloned the repo, run this command inside of the repository:
```git submodule update --init```

## Follow the build steps

### Windows
[Install vcpkg](https://github.com/microsoft/vcpkg#quick-start-windows), then run the following:
- `[vcpkg root]\vcpkg.exe install glew sdl2 libogg libtheora libvorbis --triplet=x64-windows-static` (the triplet can be whatever preferred)

Finally, follow the [compilation steps below](#compiling) using `-DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake -DVCKPG_TARGET_TRIPLET=[chosen triplet] -DCMAKE_PREFIX_PATH=[vcpkg root]/installed/[chosen triplet]/` as arguments for `cmake -Bbuild`.

### Linux
Install the following dependencies: then follow the [compilation steps below](#compiling):
- **pacman (Arch):** `sudo pacman -S base-devel cmake glew sdl2 libogg libtheora libvorbis`
- **apt (Debian/Ubuntu):** `sudo apt install build-essential cmake libglew-dev libglfw3-dev sdl2-dev libogg-dev libtheora-dev libvorbis-dev`
- **rpm (Fedora):** `sudo dnf install make gcc cmake glew-devel glfw-devel sdl2-devel libogg-devel libtheora-devel libvorbis-devel zlib-devel`
- Your favorite package manager here, [make a pull request](https://github.com/Rubberduckycooly/Sonic-CD-11-Decompilation/fork)

## Android
Follow the android build instructions [here.](./dependencies/android/README.md)

### Compiling

Compiling is as simple as typing the following in the root repository directory:
```
cmake -Bbuild # add additional flags here
cmake --build build
```

The resulting build will be located somewhere in `build/` depending on your system.

The following cmake arguments are available when compiling:
- Use these on the first `cmake -Bbuild` step like so: `cmake -Bbuild -DRETRO_DISABLE_PLUS=on`

### RSDKv3 flags
- `RETRO_DISABLE_PLUS`: Whether or not to disable the Plus DLC. Takes a boolean (on/off): build with `on` when compiling for distribution. Defaults to `off`.
- `RETRO_FORCE_CASE_INSENSITIVE`: Forces case insensivity when loading files. Takes a boolean, defaults to `off`.
- `RETRO_MOD_LOADER`: Enables or disables the mod loader. Takes a boolean, defaults to `on`.
- `RETRO_USE_HW_RENDER`: Enables the Hardware Renderer as an option. Takes a boolean, defaults to `on`.
- `RETRO_ORIGINAL_CODE`: Removes any custom code. *A playable game will not be built with this enabled.* Takes a boolean, defaults to `off`.
- `RETRO_SDL_VERSION`: *Only change this if you know what you're doing.* Switches between using SDL1 or SDL2. Takes an integer of either `1` or `2`, defaults to `2`.

## Unofficial Branches
Follow the installation instructions in the readme of each branch.
* For the **Nintendo Switch**, go to [heyjoeway's fork](https://github.com/heyjoeway/Sonic-CD-11-Decompilation).
* For the **Nintendo 3DS**, go to [SaturnSH2x2's fork](https://github.com/SaturnSH2x2/Sonic-CD-11-3DS).
  * A New Nintendo 3DS is required for the game to run smoothly.
  
Because these branches are unofficial, we can't provide support for them and they may not be up-to-date.

## Other Platforms
Currently the only supported platforms are the ones listed above, however the backend uses libogg, libvorbis, libtheora & SDL2 to power it (as well as tinyxml2 for the mod API), so the codebase is very multiplatform.
If you're able to, you can clone this repo and port it to a platform not on the list.

# FAQ
You can find the FAQ [here](./FAQ.md).

# Special Thanks
* [Xeeynamo](https://github.com/Xeeynamo): for creating the RSDK Animation editor & an early version of the script unpacker, both of which got me into RSDK modding.
* [Sappharad](https://github.com/Sappharad): for making a decompilation of the Windows Phone 7 version of Sonic CD (found [here](https://github.com/Sappharad/rvm_soniccd)) which gave me the idea & motivation to decompile the PC/iOS/Android versions.
* [SuperSonic16](https://github.com/TheSuperSonic16): for creating & adding some stuff to the Sonic CD mod loader that I asked for.
* [The Weigman](https://github.com/TheWeigman) for creating the header you see up here along with similar assets.
* Everyone in the [Retro Engine Modding Server](https://dc.railgun.works/retroengine) for being supportive of me and for giving me a place to show off these things that I've found.

# Contact:
Join the [Retro Engine Modding Discord Server](https://dc.railgun.works/retroengine) for any extra questions you may need to know about the decompilation or modding it.
