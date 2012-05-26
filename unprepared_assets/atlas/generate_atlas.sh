cd $1
../../../sac/bin/texture_packer *png | ../../../sac/tools/texture_packer/texture_packer.sh $1
cd -
cp -v /tmp/$1.png /tmp/$1.desc ../../assets/
