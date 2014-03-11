#!/bin/bash

# This script build the native part of Heriswap without including 
# non-opensource plugins (Google Play Game Services, etc.)


cd $(dirname $0)

# Remove Google Play plugin from APK
sed -i '/SacGooglePlay/d' project.properties

# Generate arm & x86 APK

if ! bash sac/tools/build/build-all.sh --target android -x86 -release n -c --c "-DSAC_RESTRICTIVE_PLUGINS=0"; then
    echo "Error when building x86 version"
elif ! bash sac/tools/build/build-all.sh --target android -arm -release n -c --c "-DSAC_RESTRICTIVE_PLUGINS=0"; then
    echo "Error when building ARM version"
fi        

# Then cancel changes
git checkout project.properties res/values/plugins.xml
git checkout AndroidManifest.xml
