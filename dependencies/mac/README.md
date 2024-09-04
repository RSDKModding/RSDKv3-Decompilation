# Mac

Most of the Xcode project work has been done for you. Once you've set up the following sources in this dependencies folder, you can build RSDKv3 and the frameworks will compile beforehand.

## Install dependencies

* SDL2: [Download](https://github.com/libsdl-org/SDL/releases) as a DMG, open the file and replace the SDL2.framework in `dependencies/mac` with the newer version located inside.

* libogg: [Download](https://xiph.org/downloads/) and unzip it as `./libogg/`.
    * For libogg specifically, go into `./libogg/include/ogg` and rename `config_types.h.in` to `config_types.h` if necessary. I don't know why that specific header file comes like this.

* libvorbis: [Download](https://xiph.org/downloads/) and unzip it as `./libvorbis/`.

* libtheora: [Download](https://xiph.org/downloads/) and unzip it as `./libtheora/`.
