#!/bin/sh
sudo docker run \
    --rm \
    -v $PWD/..:/work \
    -w /work/SonicCD.Vita \
    vitasdk/vitasdk \
    /bin/sh -C "./build-internal.sh"
