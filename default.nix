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
    libarchive
    freetype 
    libjpeg 
    llvmPackages.clang
    openal 
    libsndfile 
    glew 
    udev 
    p7zip
    boost ]; 

in

{
  deps =
    stdenv.mkDerivation {
      buildInputs = deps;
      name = "deps";
      phases = "installPhase";
      installPhase = "touch $out";
    };

  default = 
    llvmPackages.stdenv.mkDerivation {
      buildInputs = deps;
      name = "solemnsky";
      src = ./.;
    };
}
