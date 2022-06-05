call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64

cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DLAF_BACKEND=skia -DSKIA_DIR=D:\Dev\deps\bin\skia-m102 -DSKIA_LIBRARY_DIR=D:\Dev\deps\bin\skia-m102\out\Release-x64 -DSKIA_LIBRARY=D:\Dev\deps\bin\skia-m102\out\Release-x64\skia.lib -G Ninja ..

ninja aseprite