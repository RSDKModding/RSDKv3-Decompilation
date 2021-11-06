#!/bin/sh
apk add --no-cache make cmake

function getSdl2() {
    wget https://github.com/rsn8887/SDL-Vita/releases/download/2.0.8dev_vita_rsn8887_git_20180523/sdl2_vita_20180523.tar.xz && \
    mkdir -p ../dependencies/vita/sdl2 || true && \
    tar -xf sdl2_vita_20180523.tar.xz -C ../dependencies/vita/sdl2
    rm sdl2_vita_20180523.tar.xz
}

function getLibTheora() {
    wget https://downloads.xiph.org/releases/theora/libtheora-1.1.1.tar.bz2 && \
    tar -xf libtheora-1.1.1.tar.bz2 -C ../dependencies/vita && \
    (cd ../dependencies/vita/libtheora || \
        (mv ../dependencies/vita/libtheora-1.1.1 ../dependencies/vita/libtheora && cd ../dependencies/vita/libtheora)) && \
    cd ../dependencies/vita/libtheora && \
    (./configure --host=arm-vita-eabi --prefix=${VITASDK}/arm-vita-eabi --disable-shared --enable-static --disable-spec --disable-examples) ||
    cd -
    rm libtheora-1.1.1.tar.bz2
}

function installLibTheora() {
    cd ../dependencies/vita/libtheora && \
    make install && \
    cd -
}

# Download dependencies
[ ! -d ../dependencies/vita/sdl2 ] && getSdl2
[ ! -d ../dependencies/vita/libtheora ] && getLibTheora
installLibTheora

cmake . && \
make
rm RSDKv3 # this is not ignored by .gitignore
