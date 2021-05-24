![](header.png?raw=true)
# **SUPPORT THE OFFICIAL RELEASE OF SONIC CD**
+ Without files from the official release this decompilation will not run.

+ You can get the official release of Sonic CD from:
  * [Windows (Via Steam)](https://store.steampowered.com/app/200940/Sonic_CD/)
  * [IOS (Via the App Store)](https://apps.apple.com/us/app/sonic-cd-classic/id454316134)
  * [Android (Via Google Play)](https://play.google.com/store/apps/details?id=com.sega.soniccd.classic&hl=en&gl=US)
  * [Android (Via Amazon)](https://www.amazon.com/Sega-of-America-Sonic-CD/dp/B008K9UZY4/ref=sr_1_2?dchild=1&keywords=Sonic+CD&qid=1607930514&sr=8-2)

# Authors
* Rubberduckycooly and RMGRich as original authors of the project ( https://github.com/Rubberduckycooly/Sonic-1-2-2013-Decompilation )
* Jack0b0 for modifications of the forked source code

# Additional Tweaks
* There is now a settings.ini file that the game uses to load all settings, similar to Sonic Mania
* Dev menu can now be accessed from anywhere by pressing `ESC` if enabled in the config
* The `F12` pause, `F11` step over & fast forward debug features from Sonic Mania have all be ported and are enabled if `devMenu` is enabled in the config
* If `devMenu` is enabled in the config, pressing `F10` will activate a palette overlay that shows the game's 8 internal palettes in real time
* If `useSteamDir` is set in the config, and the user is on Windows, the game will try to load savedata from Steam's `userdata` directory (where the Steam version saves to)

# How to build:

## Windows via MSYS2 (64-bit Only):

* Download the newest version of the MSYS2 installer from [here](https://www.msys2.org/) and install it.
* Run the MINGW64 prompt (from the windows Start Menu/MSYS2 64-bit/MSYS2 MinGW 64-bit), when the program starts enter `pacman -Syuu` in the prompt and hit Enter. Press `Y` when it asks if you want to update packages. If it asks you to close the prompt, do so, then restart it and run the same command again. This updates the packages to their latest versions.
* Now install the dependencies with the following command: `pacman -S pkg-config make git mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2 mingw-w64-x86_64-libogg mingw-w64-x86_64-libvorbis mingw-w64-x86_64-libtheora`
* Clone the repo with the following command: `git clone https://github.com/Jackobo-Chocobo/Sonic-CD-11-Decompilation.git`
* Go into the repo you just cloned with `cd Sonic-CD-11-Decompilation`
* Then run `make CXXFLAGS=-O2 CXX=x86_64-w64-mingw32-g++ STATIC=1 -j5` (-j switch is optional but will make building faster, it's based on the number of cores you have +1 so 8 cores wold be -j9)

## Linux:
* To setup your build enviroment and library dependecies run the following commands:
* Ubuntu (Mint, Pop!_OS, etc...): `sudo apt install build-essential git libsdl2-dev libvorbis-dev libogg-dev libtheora-dev`
* Arch Linux: `sudo pacman -S base-devel git sdl2 libvorbis libogg libtheora`
* Clone the repo with the following command: `git clone https://github.com/Jackobo-Chocobo/Sonic-CD-11-Decompilation.git`
* Go into the repo you just cloned with `cd Sonic-CD-11-Decompilation`
* Then run `make CXXFLAGS=-O2 -j5` (-j switch is optional but will make building faster, it's based on the number of cores you have +1 so 8 cores wold be -j9)

# Special Thanks of Rubberduckycooly
* [Xeeynamo](https://github.com/Xeeynamo): For creating the RSDK Animation editor & an early version of the script unpacker, both of which got me into RSDK modding
* [Sappharad](https://github.com/Sappharad): For making a decompilation of the Windows Phone 7 version of Sonic CD (found [here](https://github.com/Sappharad/rvm_soniccd)) which gave me the idea & motivation to decompile the PC/IOS/Android versions
* [SuperSonic16](https://github.com/TheSuperSonic16): for creating & adding some stuff to the Sonic CD mod loader that I asked for
* The Weigman for creating the header you see up here along with similar assets
* Everyone in the Retro Engine Modding Server: for being supportive of me and for giving me a place to show off these things that I've found
