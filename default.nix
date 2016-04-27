{ nixpkgs ? import <nixpkgs> {}}:

with nixpkgs;

stdenv.mkDerivation {
  buildInputs = [gdb cmake mesa xorg.libX11 xorg.xcbutilimage eudev libjpeg openal flac libogg freetype libvorbis boost];

  name = "solemnsky";
  src = ./.;
}
