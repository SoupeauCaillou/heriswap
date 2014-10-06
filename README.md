Heriswap is a match-3 game, published on Android ([Google Play](https://play.google.com/store/apps/details?id=net.damsy.soupeaucaillou.heriswap), [F-droid](https://f-droid.org/repository/browse/?fdfilter=heriswap&fdid=net.damsy.soupeaucaillou.heriswap) and [direct download](http://soupeaucaillou.com/games/heriswap.apk)). It can also run on Linux, Windows, MacOSX and iOS devices.

[It uses sac (Soupe au Caillou) 2D engine](https://github.com/soupeaucaillou/sac).

#Prerequesites (at least)
* All: `git`
* Linux: `cmake g++`
* Android: android-sdk and android-ndk

#Get the sources
`git clone --recursive git://git.damsy.net/sac/heriswap`

#How to build
To build the game, you can use script 'sac/tools/build/build-all.sh'.
Use -h to get available options.

##For Unix/Linux
* Build and launch:
`./sac/tools/build/build-all.sh --target linux n`

##For Android
* Build APK, install it on any plugged device and launch it:
`./sac/tools/build/build-all.sh --target android n -p -i r`

* Generate a signed APK:
`./sac/tools/build/android_generate_signed_apk.sh`

* Generate a free build (excluding Google Play services):
`./android_fdroid_APK.sh`

#License
See [License file](LICENSE).

#Contact
If you encounter any problem, please email us at soupeaucaillou@damsy.net.
