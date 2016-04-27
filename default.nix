{ nixpkgs ? import <nixpkgs> {}}:

with nixpkgs;

stdenv.mkDerivation {
  buildInputs = [cmake mesa xorg.libX11 sfml xorg.xcbutilimage eudev libjpeg openal flac libogg freetype libvorbis glew glfw2 boost];

  installPhase = ''
    mkdir dist;
    cd dist;
    cmake ..;
    mkdir -p $out;
    cp -r ./* $out;
  '';

  name = "solemnsky";
  src = ./.;
}
