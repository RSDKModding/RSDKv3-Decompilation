#!/bin/sh
sudo docker run \
    --rm \
    -v $PWD/..:/work \
    -w /work/RSDKv3.vita \
    vitasdk/vitasdk \
    /bin/sh -C "./build-internal.sh"
