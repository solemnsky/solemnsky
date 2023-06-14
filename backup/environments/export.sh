#! /bin/sh

mkdir -p export
rm -rf export/*

root=$PWD

cd src/
for map in *
do
  echo "** exporting \"$map.sky\" **"

  cd $root/src/$map
  7z a $root/export/$map.zip *

  cd $root/export
  mv $map.zip $map.sky

done
