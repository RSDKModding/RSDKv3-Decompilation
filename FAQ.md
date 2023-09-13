# General
### Q: How do I set this up?
A: You can find a tutorial for setting up both the game and mods [here](https://gamebanana.com/tuts/14111). Alternatively, opening the decompilation without any game assets present will generate a TXT file containing a brief guide to setting them up.

### Q: Why is the DLC disabled in release builds and autobuilds?
A: Long story short, it's to minimize piracy and ensure an extra layer of legal protection for Sonic Mania Plus and Sonic Origins Plus. Giving players paid content for free is not the goal of this project.

### Q: Why don't some buttons in the menu work?
A: Buttons like Leaderboards & Achievements require code to be added to support online functionality & menus (though they are still saved in the Udata.bin file), and other buttons like the Controls button in the Blit versions or the Privacy button in the mobile version have no scripts and are instead hardcoded. I just didn't feel like going through the effort to decompile all that, since it's not really worth it.

### Q: The screen is tearing, how do I fix it?
A: Try turning on VSync in settings.ini.

### Q: I'm on Windows and experiencing issues such as mods not appearing in the mod menu, what's wrong?
A: A likely reason for this is that you put the decomp in a directory that's managed by OneDrive (Desktop, Downloads, etc). These directories are known to cause issues, so move your decomp installation elsewhere, such as the root of the C drive or another drive.

### Q: I found a bug!
A: Submit an issue in the issues tab and we _might_ fix it in the master branch. Don't expect any major future releases, however.

# Using Blit/Steam RSDK Files
### Q: Why is the titlecard text slightly offset when playing in widescreen?
A: This is a known issue that is caused by the scripts from this version of the game. You can either use a mod that modifies `TitleCards/R[X]_TitleCard.txt` to fix it, or simply set the `screenWidth` option in settings.ini to 400 to match the intended value used in the original release.

### Q: Why is there a weird spot on the title screen when playing in widescreen?
A: Similar to above, this is a script issue. You can fix it via a mod or by changing the screen width.

# Using Mobile RSDK Files
### Q: Why does pressing B pause the game during gameplay?
A: This is a known script issue with most Sega Forever versions of the game. Using the [decompiled scripts](https://github.com/Rubberduckycooly/Sonic-CD-2011-Script-Decompilation) should fix it.

### Q: Why are the SEGA and Christian Whitehead logos in the title screen so low resolution? 
A: This happens when using the Software Renderer, you can fix this by setting `HardwareRenderer` in settings.ini to true.

### Q: Why don't the video files from the mobile version of the game work?
A: The decompilation only supports the OGV video files from the Blit/Steam versions of the game.

# Using Origins RSDK Files
### Q: Why doesn't using the datafile work?
A: The RSDK file from Sonic Origins is encrypted in the RSDKv5 datapack format, not the RSDKv3 format. Repacking the files in the correct format or using Data Folder Mode will fix the issue.

### Q: Why is there no audio?
A: Sonic Origins doesn't have any music or sound effects contained in the game's data file, instead storing and handling all in-game audio itself through Hedgehog Engine 2. You can fix this by simply inserting the audio files from another version of the game. Sound effects added in Origins will have to be inserted manually.

### Q: Why is the Drop Dash disabled by default? How do I turn it on?
A: By default, the game mode is set to Classic Mode, which disables the Drop Dash. The only way to change this is through a mod, either by changing the default value of the `game.playMode` global variable in `GameConfig.bin` or by setting the variable to another value via scripts.

### Q: How do I play as Knuckles or Amy?
A: Sonic Team implemented Knuckles and Amy in a way where they aren't playable on the decomp out of the box. This can be fixed via mods. **Do not ask about this in an issue, as we will not be able to help you.**
There are also checks implemented in the engine to prevent playing as these characters on release builds and autobuilds.

# Miscellaneous
### Q: Will you do a decompilation for Sonic 1/Sonic 2?
A: I already have! You can find it [here](https://github.com/Rubberduckycooly/Sonic-1-2-2013-Decompilation).

### Q: Will you do a decompilation for Sonic Mania?
A: I already have! You can find the source code for Sonic Mania [here](https://github.com/Rubberduckycooly/Sonic-Mania-Decompilation) and RSDKv5 which is used to run it [here](https://github.com/Rubberduckycooly/RSDKv5-Decompilation).
