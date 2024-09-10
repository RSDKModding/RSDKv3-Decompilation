# Mac

Before anything, you'll need to install XCode from either the App Store or the [Apple developer website](https://developer.apple.com/download/). After which to use Command Line Tools, you'll need to run `xcode-select --install` to begin the setup.

Most of the Xcode project work has been done for you. Once you've set up the following sources in the dependencies folder, you can build RSDKv3 and the frameworks will compile along with it.

## Install dependencies

* SDL2: [Download](https://github.com/libsdl-org/SDL/releases) as a DMG, open the file and replace the SDL2.framework in `dependencies/mac` with the newer version located inside.

* libogg: [Download](https://xiph.org/downloads/) and unzip it as `./libogg/`. After which, set the current directory to said folder and run `./configure`, or run it externally as the full or partial path.

* libvorbis: [Download](https://xiph.org/downloads/) and unzip it as `./libvorbis/`.

* libtheora: [Download](https://xiph.org/downloads/) and unzip it as `./libtheora/`.

## Customization

If you're debugging the engine, best to just open the XCode project directly and run it through the debugging process.However, if you have the Command Line Tools and want to play games using it, run `xcodebuild -project RSDKv3.xcodeproj -scheme RSDKv3 DSTROOT="./build" -configuration Release DEPLOYMENT_LOCATION=YES` to create the app.

* You can still apply flags for compiling by appending the line `OTHER_CFLAGS=` and then the list of flags. All of the flags except for the last two represent bools where `0` is false and `1` is true. The last two flags take string arguments.

  * `RETRO_USE_MOD_LOADER` is used for `RETRO_MOD_LOADER`. Default is `-DRETRO_USE_MOD_LOADER=1`.

  * `RETRO_USING_OPENGL` is used for `RETRO_USE_HW_RENDER`. Default is `-DRETRO_USING_OPENGL=1`.

  * `RETRO_USE_SDL1` and `RETRO_USE_SDL2` are used for `RETRO_SDL_VERSION`. You should only have one on at a time or neither on. By default, `RETRO_USE_SDL2` is on. You don't really need this unless you want to switch out SDL2 with SDL1. Default is `-DRETRO_USE_SDL1=0 -DRETRO_USE_SDL2=1`.

  * `FORCE_CASE_INSENSITIVE` is used for `RETRO_FORCE_CASE_INSENSITIVE`. Default is `-DFORCE_CASE_INSENSITIVE=1`.

  * `RETRO_USE_ORIGINAL_CODE` is used for `RETRO_ORIGINAL_CODE`. Default is `-RETRO_USE_ORIGINAL_CODE=0`.

  * `RSDK_AUTOBUILD` is used for `RETRO_DISABLE_PLUS`. Default is `-DRSDK_AUTOBUILD=0`.
    * You **MUST** have this set to `1` if you plan on distributing RSDKv3 builds. And for MacOS builds specifically, you should inform users to use the command `xattr -cr /path/to/RSDKv3.app` in order to be able to open the app, or `xattr -cr /path/to/RSDKv3.zip` if said zip contains the app. This also applies to the autobuilds.

  * `RETRO_DEV_EXTRA` is an argument used in autobuilds to state the `CMAKE_SYSTEM_NAME` and the `CMAKE_CXX_COMPILER_ID` in the Dev Menu, such as `Darwin - AppleClang`.
    * You need to have a backslash escape for every special character since this is a string.
    * It may be better to edit [Other C++ Flags](https://developer.apple.com/documentation/xcode/build-settings-reference#Other-C++-Flags) using the XCode app so you can have a better sense of what goes wrong.

  * `DECOMP_VERSION` is an argument used in autobuilds to display the decomp's version number in the Dev Menu. In this case, it would be `1.3.2` already, and may not need changing.
    * You need to have a backslash escape for every special character since this is a string.
    * It may be better to edit [Other C++ Flags](https://developer.apple.com/documentation/xcode/build-settings-reference#Other-C++-Flags) using the XCode app so you can have a better sense of what goes wrong.

* The default for `OTHER_CPLUSPLUSFLAGS` contains the same flags in `OTHER_CFLAGS`. You could use `OTHER_CPLUSPLUSFLAGS` to be more specific, especially if you have a major difference between the two.

* The resulting command could look something like `xcodebuild -project RSDKv3.xcodeproj -scheme RSDKv3 DSTROOT="./build" -configuration Release DEPLOYMENT_LOCATION=YES OTHER_CFLAGS="-DRSDK_AUTOBUILD=1 -DRETRO_DEV_EXTRA=\\\"Darwin\\ -\\ AppleClang\\\""`
