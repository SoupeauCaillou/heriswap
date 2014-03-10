#!/bin/bash

cd $(dirname $0)

# Remove Google Play plugin from APK
sed -i '/SacGooglePlay/d' project.properties

# Generate arm & x86 APK
if ! bash sac/tools/build/android_generate_signed_apk.sh --args "--c -DSAC_RESTRICTIVE_PLUGINS=0"; then
	git checkout AndroidManifest.xml
fi

# Then cancel changes
git checkout project.properties res/values/plugins.xml
