Prerequesites:
git
- Linux: (at least) cmake, g++, GLFW, UnitTest++
- Android: android-sdk, android-ndk, eclipse Android (adt) plugin

Grab the sources:
git clone --depth 1 --recursive git://git.damsy.net/sac/heriswap

Compile for linux:
cd heriswap
cmake .
make

Run (must be executed from heriswap folder):
./source/tilematch

Compile for Android:
cd sac/libs/
./convert_tremor_asm.sh
cd -
ndk-build NDK_DEBUG=0

Open Eclipse:
File -> Import -> General -> Existing Project into workspace
Select root directory: select 'heriswap' folder
Then select Heriswap Project

Fix compilation errors the way you want:
- HeriswapSecret.java file is missing: it contains OpenFeint and ad providers app id
- 2 jar files are missing: SDK from GreyStripes and Chartboost (ad provider)
