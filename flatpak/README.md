# Flatpak

You will need the FreeDesktop.org 21.08 SDK installed, if you don't have it,
install [Flathub](https://flathub.org/) which provides it.

# Sonic CD (Original Steam Version) Flatpak

First, go to your Steam library, right click on Sonic CD and click `Manage` > `Browse local files`.
Copy the `Data.rsdk` file and the `videos` folder into this directory.

To build and install the flatpak, run:

**System-wide:**
```
$ sudo flatpak-builder --install --force-clean soniccd com.sega.SonicCDSteam.json
```
**User:**
```
$ flatpak-builder --user --install --force-clean soniccd com.sega.SonicCDSteam.json
```

# Sonic CD (Origins Version) Flatpak

**This method will not install the video files, as the decompilation
only supports video files from the Steam version. This method will
also not include music or sound effects, as the RSDK file from Origins
does not contain them.**

First, you need to install Sonic Origins.
To get it, visit https://store.steampowered.com/app/1794960 for the Steam version or https://store.epicgames.com/en-US/p/sonic-origins for the Epic Games version.

Once you have the game, you need to navigate to the game's files.
If you have the Steam version, go to your Steam library, right click on Sonic Origins and click `Manage` > `Browse local files`.

Once you're in the game's files, navigate to `image/x64/raw/retro`. Copy the `SonicCDu.rsdk` file into this directory.

To build and install the flatpak, run:

**System-wide:**
```
$ sudo flatpak-builder --install --force-clean soniccd com.sega.SonicCDOrigins.json
```
**User:**
```
$ flatpak-builder --user --install --force-clean soniccd com.sega.SonicCDOrigins.json
```

# Sonic CD (Android Version) Flatpak

**This method will not install the video files, as the decompilation
only supports video files from the Steam version.**

First, you need to install the game on an Android device.
To get it, visit https://www.sega.com/games/sonic-cd.

Once you have the game, use a file manager on your device to navigate
to the directory `Android/obb/com.sega.soniccd.classic` and copy the
`patch.[number].com.sega.soniccd.classic.obb` file into this directory as `Data.rsdk`.

To build and install the flatpak, run:

**System-wide:**
```
$ sudo flatpak-builder --install --force-clean soniccd com.sega.SonicCDAndroid.json
```
**User:**
```
$ flatpak-builder --user --install --force-clean soniccd com.sega.SonicCDAndroid.json
```

