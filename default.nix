{ nixpkgs ? import <nixpkgs> {}}:

with nixpkgs;

let
  deps = [
    cmake 
    xorg.libX11 
    xorg.libXrandr 
    xorg.libXrender 
    xorg.xcbutilimage
    flac
    libvorbis
    freetype 
    libjpeg 
    openal 
    libsndfile 
    glew 
    udev 
    boost ]; 

in:

{
  deps =
    stdenv.mkDerivation {
      buildInputs = deps;
      name = "deps";
      phases = "installPhase";
      installPhase = "touch $out";
    };

  default = 
    stdenv.mkDerivation {
      buildInputs = deps;
      name = "solemnsky";
      src = ./.;
    };
}
