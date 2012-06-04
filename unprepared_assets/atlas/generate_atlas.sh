cd $1
../../../sac/bin/texture_packer *png | ../../../sac/tools/texture_packer/texture_packer.sh $1
cd -
convert /tmp/$1.png -alpha extract -depth 8 ../../assets/$1_alpha.png
convert ../../assets/$1_alpha.png -background white -flatten +matte -depth 8 ../../assets/$1_alpha.png
convert /tmp/$1.png -background red -alpha off -type TrueColor PNG24:../../assets/$1.png
cp -v /tmp/$1.desc ../../assets/
~/code/android-sdk-linux/tools/etc1tool --encode ../../assets/$1.png -o ../../assets/$1.pkm

