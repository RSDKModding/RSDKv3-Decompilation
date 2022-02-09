# Flatpak

You will need the FreeDesktop.org 21.08 SDK installed, if you don't have it,
install [Flathub](https://flathub.org/) which provides it.

# Sonic CD (Steam Version) Flatpak

First, you first need to install the game using Steam.
To get it, visit https://store.steampowered.com/app/200940/Sonic_CD/.

Once you have the game, right click it in your Steam library and click `Manage` > `Browse local files`. 
Copy the `Data.rsdk` file and the `videos` folder into this directory.

To build and install the flatpak, run:
```
$ sudo flatpak-builder --install --force-clean soniccd com.sega.SonicCDSteam.json
```

# Sonic CD (Android Version) Flatpak

First, you need to install the game on an Android device.
To get it, visit https://www.sega.com/games/sonic-cd.

Once you have the game, use a file manager on your device to navigate
to the directory `Android/obb/com.sega.soniccd.classic` and copy the
`patch.[number].com.sega.soniccd.classic.obb` file into this directory.
Rename the patch obb file to `Data.rsdk`.

To build and install the flatpak, run:
```
$ sudo flatpak-builder --install --force-clean soniccd com.sega.SonicCDAndroid.json
```

Note that using this method will not install the video files, as the decompilation
only supports video files from the Steam version.
