# Sonic CD (2011) Decompilation
A full decompilation of Sonic CD 2011, based on the PC remake with improvements & tweaks from the mobile remakes. Ported to the Switch.

# **SUPPORT THE OFFICIAL RELEASE OF SONIC CD**
+ Without assets from the official release this decompilation will not run.

+ You can get the official release of sonic cd from:
  * [Windows (Via Steam)](https://store.steampowered.com/app/200940/Sonic_CD/)
  * [IOS (Via the App Store)](https://apps.apple.com/us/app/sonic-cd-classic/id454316134)
  * [Android (Via Google Play)](https://play.google.com/store/apps/details?id=com.sega.soniccd.classic&hl=en&gl=US)
  * [Android (Via Amazon)](https://www.amazon.com/Sega-of-America-Sonic-CD/dp/B008K9UZY4/ref=sr_1_2?dchild=1&keywords=Sonic+CD&qid=1607930514&sr=8-2)

Even if your platform isn't supported by the official releases, buy it for the assets (you don't need to run the official release, you just need the game assets.)

## Installation Instructions
You can find downloads in [releases](https://github.com/heyjoeway/Sonic-CD-11-Decompilation/releases).

1. Make sure your Switch can run homebrew.
2. Extract the contents of the zip to the root of your SD card.
3. Copy the assets to `/switch/scd2011` on your Switch's SD card. You will need the following files/folders:
    - help (folder, optional as How to Play is broken)
    - videos (folder)
    - Data.rsdk
    - fonts.arc
    - images.arc
    - sounds.arc
    - Strings.str
3. Start Sonic CD via hbmenu (or whatever method you prefer).

NOTE: It is recommended full RAM access. This means you shouldn't launch hbmenu from the album applet when running this. With the latest Atmosphere build and its default config, you can hold R while starting any game to open hbmenu with full RAM access. If you have any issues make sure the game has full RAM access before reporting them; launching as an applet will not be supported.

## Additional Tweaks
* There is now a settings.ini file that the game uses to load all settings, similar to Sonic Mania
* Dev menu can now be accessed from anywhere by pressing the escape key if enabled in the config
* The f12 pause, f11 step over & fast forward debug features from sonic mania have all be ported and are enabled if devMenu is enabled in the config
* If devMenu is enabled in the config, pressing f10 will activate a palette overlay that shows the game's 8 internal palettes in real time

----

Sections below are lifted directly from the source repository.

-----

## FAQ
### Q: Sound doesn't play when videos are playing!
### A: Currently the video playback system doesn't support streaming audio from .ogv files, the workaround is to open the ogv file in any kind of video/audio software that supports it and save just the audio as an ogg vorbis file called [videoName]\[JP/US].ogg in the same directory as the .ogv

### Q: Why don't some buttons in the menu work?
### A: Buttons like leaderboards & achievements require code to be added to support online functionality & menus (though they are saved anyways), and other buttons like the controls button on PC or privacy button on mobile have no game code and are instead hardcoded through callbacks, and I just didnt feel like going through the effort to decompile all that, since it's not really worth it

### Q: Is the titlecard text slightly offset when using a PC datafile?
### A: Its not just you, but unfortunately its an error with the scripts, so if you wanna go into TitleCards/R[X]\_TitleCard.txt and fix it, be my guest

### Q: The screen is tearing, how do I fix it
### A: Try turning on vsync, that worked for me (on mac)

### Q: I found a bug/I have a feature request!
### A: Submit an issue in the issues tab and I'll fix/add (if possible) it as soon as I can

### Q: Will you do a decompilation for sonic 1/sonic 2?
### A: Maybe. it took about 2-3 years of on/off work to get this to the state it is, so doing one for sonic 1/sonic 2 would likely take a lot of time to do, considering so much less is known about S1&S2/RSDKv4 compared to CD/RSDKv3

### Q: Will you do a decompilation for sonic Mania?
### A: No. mania is tons bigger and requires that I'd decompile not only how the (far more complex) RSDKv5 works, but also all _600_+ objects work

## Special Thanks
* [Xeeynamo](https://github.com/Xeeynamo): For creating the RSDK Animation editor & an early version of the script unpacker, both of which got me into RSDK modding
* [Sappharad](https://github.com/Sappharad): For making a decompilation of the windows phone 7 version of sonic cd (found [here](https://github.com/Sappharad/rvm_soniccd)) which gave me the idea & motivation to decompile the PC/IOS/Android versions
* [SuperSonic16](https://github.com/TheSuperSonic16): for creating & adding some stuff to the sonic cd mod loader that I asked for
* Everyone in the [Retro Engine Modding Server](https://dc.railgun.works/retroengine): for being supportive of me and for giving me a place to show off these things that I've found

## Background:
in 2018 I started researching Christan Whitehead's 'Retro Engine' as a side project since I was bored, I started with Sonic CD (RSDKv3) since it was the most well known version that hadn't had much support, since at that time Sonic Mania's (RSDKv5) modding scene was already thriving, and eventually I expanded my range to Retro-Sonic (Retro-Sonic Engine), Sonic Nexus (RSDKv1) & Sonic 1/2 (RSDKv4), since then I have worked during spare moments to document and reverse all that I can of all versions of RSDK as it was just interesting to see how things worked under the hood or how features evolved and changed over time. Fast forward to 2020 and [Sappharad](https://github.com/Sappharad) shows me his decompilation of Sonic CD based on the windows phone 7 port since they'd seen my other github repositories relating to RSDK reversing. After seeing their decompilation I had the idea to start my own Sonic CD decompilation based on the PC port, with improvements and tweaks android port, though I didn't have much time to get around to it, so the project was shelved until I had more time to work on it. in mid-december 2020, I remembered the sonic CD decompilation that I started and finally had the time to work on it more, so after around 2 weeks of on/off working the decompilation was finally in a solid working state, though I continued tweaking it for another few weeks just to iron out all the glitches and bugs that I found. 