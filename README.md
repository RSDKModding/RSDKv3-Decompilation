# Sonic-CD-11-Decompilation
A Full Decompilation of Sonic CD 2011, Based on the PC port with improvements & tweaks from the mobile ports


# **SUPPORT THE OFFICIAL RELEASE OF SONIC CD**
+ Without assets from the official release this decompilation will not run.

+ You can get the official release of sonic cd from:
* [Windows (Via Steam)](https://store.steampowered.com/app/200940/Sonic_CD/)
* [IOS (Via the App Store)](https://apps.apple.com/us/app/sonic-cd-classic/id454316134)
* [Android (Via Google Play)](https://play.google.com/store/apps/details?id=com.sega.soniccd.classic&hl=en&gl=US)
* [Android (Via Amazon)](https://www.amazon.com/Sega-of-America-Sonic-CD/dp/B008K9UZY4/ref=sr_1_2?dchild=1&keywords=Sonic+CD&qid=1607930514&sr=8-2)

even if your platform isn't supported by the official releases, buy it for the assets (you dont need to run the official release, you just need the game assets)

# Advantages over the PC version of Sonic CD
* sharp, pixel-perfect display
* controls are completely remappable via the settings.ini file
* the window allows windows shortcuts to be used
* complete support for using mobile/updated scripts, allowing for features the official PC version never got to be played on PC
* native windows x64 version, as well as an x86 version

# Advantages over the Mobile versions of Sonic CD
* the rendering backend is based off the PC version, so palettes are fully supported (tidal tempest water in particular)

# Additional Tweaks
* there is now a settings.ini file that the game uses to load all settings, similar to Sonic Mania
* dev menu can now be accessed from anywhere by pressing the escape key if enabled in the config
* the f12 pause, f11 step over & fast forward debug features from sonic mania have all be ported and are enabled if devMenu is enabled in the config
* if devMenu is enabled in the config, pressing f10 will activate a palette overlay that shows the game's 8 internal palettes in real time
* if useSteamDir is set in the config, and the user is on windows, the game will try to load savedata from steams userdata directory (where the steam ver saves to)

# How to build:
## Windows:
* Clone the repo, then follow the instructions in the [depencencies readme for windows](./dependencies/windows/dependencies.txt) to setup dependencies, then build via the visual studio solution
* or grab a prebuilt executable from the releases section

## Mac:
* Clone the repo, then follow the instructions in the [depencencies readme for mac](./dependencies/mac/dependencies.txt) to setup dependencies, then build via the xcode solution
* or grab a prebuilt executable from the releases section

## Other platforms:
Currently only windows & mac are supported by this decompilation, however the backend uses libogg, libvorbis, libtheora & SDL2 to power it, so the codebase is very multiplatform.
if you've cloned this repo and ported it to a platform not on the list or made some changes you'd like to see added to this repo, submit a pull request and it'll most likely be added

# FAQ
### Q: sound doesn't play when videos are playing!
### A: currently the video playback system doesn't support streaming audio from .ogv files, the workaround is to open the ogv file in any kind of video/audio software that supports it and save just the audio as an ogg vorbis file called [videoName].ogg in the same directory as the .ogv

### Q: Why dont some buttons in the menu work?
### A: buttons like leaderboards & achievements require code to be added to support online functionality & menus (though they are saved anyways), and other buttons like the controls button on PC or privacy button on mobile have no game code and are instead hardcoded through callbacks, and I just didnt feel like going through the effort to decompile all that, since its not really worth it

### Q: is it just me or is the titlecard text slightly offset when using a PC datafile?
### A: its not just you, but unfortunately its an error with the scripts, so if you wanna go into TitleCards/R[X]\_TitleCard.txt and fix it be my guest

### Q: the screen is tearing, how do I fix it
### A: try turning on vsync, that worked for me (on mac)

### Q: I found a bug/I have a feature request!
### A: submit an issue in the issues tab and I'll fix/add (if possible) it as soon as I can

### Q: will you do a decompilation for sonic 1/sonic 2?
### A: maybe. it took about 2-3 years of on/off work to get this to the state it is, so doing one for sonic 1/sonic 2 would likely take a lot of time to do, considering so much less is known about S1&S2/RSDKv4 compared to CD/RSDKv3

### Q: will you do a decompilation for sonic Mania?
### A: absolutely not. mania is tons bigger and requires that I'd decompile not only how the (far more complex) RSDKv5 works, but also all _600_+ objects work

# Special Thanks
* [Xeeynamo](https://github.com/Xeeynamo): For creating the RSDK Animation editor & an early version of the script unpacker, both of which got me into RSDK modding
* [Sappharad](https://github.com/Sappharad): For making a decompilation of the windows phone 7 version of sonic cd (found [here](https://github.com/Sappharad/rvm_soniccd)) which gave me the idea & motivation to decompile the PC/IOS/Android versions
* [SuperSonic16](https://github.com/TheSuperSonic16): for creating & adding some stuff to the sonic cd mod loader that I asked for
* Everyone in the [Retro Engine Modding Server](https://dc.railgun.works/retroengine): for being supportive of me and for giving me a place to show off these things that I've found
