#!/bin/bash

# This script build the native part of Heriswap without including 
# non-opensource plugins (Google Play Game Services, etc.)


cd $(dirname $0)

# Remove Google Play plugin from APK
sed -i '/SacGooglePlay/d' project.properties


cancel_changes() {
	git checkout project.properties res/values/plugins.xml AndroidManifest.xml
}

options='--target android -release n -c --c -DUSE_RESTRICTIVE_PLUGINS=OFF'
# Generate arm & x86 APK
if ! bash sac/tools/build/build-all.sh -x86 $options; then
    echo "Error when building x86 version"
    cancel_changes
    exit 1
elif ! bash sac/tools/build/build-all.sh -arm $options; then
    echo "Error when building ARM version"
    cancel_changes
    exit 2
fi        

# Then cancel changes
cancel_changes
