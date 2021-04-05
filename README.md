# Retro Engine (RSDK) v3 Decompilation
A Full Decompilation of RSDKv3 & Sonic CD 2011, Based on the PC remake with improvements & tweaks from the mobile remakes

# **SUPPORT THE OFFICIAL RELEASE OF SONIC CD**
+ Without assets from the official release this decompilation will not run.

+ You can get the official release of Sonic CD from:
  * [Windows (Via Steam)](https://store.steampowered.com/app/200940/Sonic_CD/)
  * [IOS (Via the App Store)](https://apps.apple.com/us/app/sonic-cd-classic/id454316134)
  * [Android (Via Google Play)](https://play.google.com/store/apps/details?id=com.sega.soniccd.classic&hl=en&gl=US)
  * [Android (Via Amazon)](https://www.amazon.com/Sega-of-America-Sonic-CD/dp/B008K9UZY4/ref=sr_1_2?dchild=1&keywords=Sonic+CD&qid=1607930514&sr=8-2)

Even if your platform isn't supported by the official releases, you **must** buy it for the assets (you dont need to run the official release, you just need the game assets)

# Advantages over the PC version of Sonic CD
* Sharp, pixel-perfect display
* Controls are completely remappable via the settings.ini file
* The window allows windows shortcuts to be used
* Complete support for using mobile/updated scripts, allowing for features the official PC version never got to be played on PC
* Native Windows x64 version, as well as an x86 version

# Advantages over the Mobile versions of Sonic CD
* The rendering backend is based off the PC version, so palettes are fully supported (tidal tempest water in particular)

# Additional Tweaks
* There is now a settings.ini file that the game uses to load all settings, similar to Sonic Mania
* Dev menu can now be accessed from anywhere by pressing `ESC` if enabled in the config
* The `F12` pause, `F11` step over & fast forward debug features from Sonic Mania have all be ported and are enabled if `devMenu` is enabled in the config
* If `devMenu` is enabled in the config, pressing `F10` will activate a palette overlay that shows the game's 8 internal palettes in real time
* If `useSteamDir` is set in the config, and the user is on Windows, the game will try to load savedata from Steam's `userdata` directory (where the Steam version saves to)

# How to build:
## Windows:
* Clone the repo, then follow the instructions in the [depencencies readme for Windows](./dependencies/windows/dependencies.txt) to setup dependencies, then build via the Visual Studio solution
* or grab a prebuilt executable from the releases section

## Windows UWP (Phone, Xbox, etc.):
* Clone the repo, then follow the instructions in the [depencencies readme for Windows](./dependencies/windows/dependencies.txt) and [depencencies readme for UWP](./dependencies/win-uwp/dependencies.txt) to setup dependencies, then build and deploy via the UWP Visual Studio solution.
* After install copy your `Data.rsdk` and `videos` folder into the apps localstate folder

## Windows via MSYS2 (64-bit Only):

* Download the newest version of the MSYS2 installer from [here](https://www.msys2.org/) and install it.
* Run the MINGW64 prompt (from the windows Start Menu/MSYS2 64-bit/MSYS2 MinGW 64-bit), when the program starts enter `pacman -Syuu` in the prompt and hit Enter. Press `Y` when it asks if you want to update packages. If it asks you to close the prompt, do so, then restart it and run the same command again. This updates the packages to their latest versions.
* Now install the dependencies with the following command: `pacman -S pkg-config make git mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2 mingw-w64-x86_64-libogg mingw-w64-x86_64-libvorbis mingw-w64-x86_64-libtheora`
* Clone the repo with the following command: `git clone https://github.com/Rubberduckycooly/Sonic-CD-11-Decompilation.git`
* Go into the repo you just cloned with `cd Sonic-CD-11-Decompilation`
* Then run `make CXXFLAGS=-O2 CXX=x86_64-w64-mingw32-g++ STATIC=1 -j5` (-j switch is optional but will make building faster, it's based on the number of cores you have +1 so 8 cores wold be -j9)


## Mac:
* Clone the repo, then follow the instructions in the [depencencies readme for Mac](./dependencies/mac/dependencies.txt) to setup dependencies, then build via the Xcode project
* or grab a prebuilt executable from the releases section

## Linux:
* To setup your build enviroment and library dependecies run the following commands:
* Ubuntu (Mint, Pop!_OS, etc...): `sudo apt install build-essential git libsdl2-dev libvorbis-dev libogg-dev libtheora-dev`
* Arch Linux: `sudo pacman -S base-devel git sdl2 libvorbis libogg libtheora`
* Clone the repo with the following command: `git clone https://github.com/Rubberduckycooly/Sonic-CD-11-Decompilation.git`
* Go into the repo you just cloned with `cd Sonic-CD-11-Decompilation`
* Then run `make CXXFLAGS=-O2 -j5` (-j switch is optional but will make building faster, it's based on the number of cores you have +1 so 8 cores wold be -j9)

## iOS:
* Clone the repo, then follow the instructions in the [depencencies readme for iOS](./dependencies/ios/dependencies.txt) to setup dependencies, then build via the Xcode project

## PS Vita:
* Ensure you have Docker installed and run the script `build.sh` from `SonicCD.Vita`. If you are on Windows, WSL2 is recommended.
NOTE: You would need to copy Sonic CD game data into `ux0:data/SonicCD` to boot the game.

## Other platforms:
Currently the only "officially" supported platforms are the ones listed above, however the backend uses libogg, libvorbis, libtheora & SDL2 to power it, so the codebase is very multiplatform.
If you've cloned this repo and ported it to a platform not on the list or made some changes you'd like to see added to this repo, submit a pull request and it'll most likely be added

# FAQ
### Q: Why dont some buttons in the menu work?
A: Buttons like leaderboards & achievements require code to be added to support online functionality & menus (though they are saved anyways), and other buttons like the controls button on PC or privacy button on mobile have no game code and are instead hardcoded through callbacks, and I just didnt feel like going through the effort to decompile all that, since its not really worth it

### Q: Is the titlecard text slightly offset when using a PC datafile?
A: Its not just you, but unfortunately its an error with the scripts, so if you wanna go into `TitleCards/R[X]\_TitleCard.txt` and fix it, be my guest, alternatively set `screenWidth` to 400, instead of 424 in the settings.ini file to match the PC version's resolution

### Q: Theres a weird spot on the title screen how do I fix it?
A: Set `screenWidth` to 400, instead of 424 in the settings.ini file to match the PC version's resolution

### Q: The screen is tearing, how do I fix it
A: Try turning on vsync, that worked for me (on Mac)

### Q: I found a bug/I have a feature request!
A: Submit an issue in the issues tab and I'll fix/add (if possible) it as soon as I can

### Q: Will you do a decompilation for Sonic 1/Sonic 2?
A: Yes! I've done a decompilation of RSDKv4 (the engine ver that S1/S2 run on) and it can be found [here](https://github.com/Rubberduckycooly/Sonic-1-2-2013-Decompilation)!

### Q: Will you do a decompilation for Sonic Mania?
A: No. Mania is tons bigger and requires that I'd decompile not only how the (far more complex) RSDKv5 works, but also make all _600_+ objects work

# Special Thanks
* [Xeeynamo](https://github.com/Xeeynamo): For creating the RSDK Animation editor & an early version of the script unpacker, both of which got me into RSDK modding
* [Sappharad](https://github.com/Sappharad): For making a decompilation of the Windows Phone 7 version of Sonic CD (found [here](https://github.com/Sappharad/rvm_soniccd)) which gave me the idea & motivation to decompile the PC/IOS/Android versions
* [SuperSonic16](https://github.com/TheSuperSonic16): for creating & adding some stuff to the Sonic CD mod loader that I asked for
* Everyone in the [Retro Engine Modding Server](https://dc.railgun.works/retroengine): for being supportive of me and for giving me a place to show off these things that I've found

# Background:
In 2018 I started researching Christan Whitehead's 'Retro Engine' as a side project since I was bored, I started with Sonic CD (RSDKv3) since it was the most well known version that hadn't had much support, since at that time Sonic Mania's (RSDKv5) modding scene was already thriving, and eventually I expanded my range to Retro-Sonic (Retro-Sonic Engine), Sonic Nexus (RSDKv1) & Sonic 1/2 (RSDKv4), since then I have worked during spare moments to document and reverse all that I can of all versions of RSDK as it was just interesting to see how things worked under the hood or how features evolved and changed over time. Fast forward to 2020 and [Sappharad](https://github.com/Sappharad) shows me his decompilation of Sonic CD based on the Windows Phone 7 port since they'd seen my other github repositories relating to RSDK reversing. After seeing their decompilation I had the idea to start my own Sonic CD decompilation based on the PC port, with improvements and tweaks Android port, though I didn't have much time to get around to it, so the project was shelved until I had more time to work on it. in mid-December 2020, I remembered the Sonic CD decompilation that I started and finally had the time to work on it more, so after around 2 weeks of on/off working the decompilation was finally in a solid working state, though I continued tweaking it for another few weeks just to iron out all the glitches and bugs that I found. 

# Contact:
you can join the [Retro Engine Modding Discord Server](https://dc.railgun.works/retroengine) for any extra questions you may need to know about the decompilation or modding it
