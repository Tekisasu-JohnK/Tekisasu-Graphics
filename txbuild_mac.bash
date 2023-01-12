#!/bin/bash
if [[ ! -d "build" ]]; then
	echo "txbuild: creating build directory."
	mkdir build
fi

echo "txbuild: entering build directory."
cd build

echo "txbuild: compiling with cmake..."
cmake \       
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 \
  -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk \
  -DLAF_BACKEND=skia \
  -DSKIA_DIR=$HOME/Tekisasu/deps/skia \
  -DSKIA_LIBRARY_DIR=$HOME/Tekisasu/deps/skia/out/Release-arm64 \
  -DSKIA_LIBRARY=$HOME/Tekisasu/deps/skia/out/Release-arm64/libskia.a \
  -DPNG_ARM_NEON:STRING=on \
  -G Ninja \
..

echo "txbuild: cmake compile job attempt has finished."

echo "txbuild: running ninja to complete the build."
ninja aseprite


