{ nixpkgs ? import <nixpkgs> {}}:

with nixpkgs;

stdenv.mkDerivation {
  buildInputs = [cmake eudev xorg.libX11 xorg.xcbutilimage eudev libjpeg openal flac libogg freetype libvorbis glew glfw2 boost];

  name = "solemnsky";
  src = ./.;
}
