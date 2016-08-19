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
    gdb
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

  default-gcc =
    stdenv.mkDerivation {
      buildInputs = deps ++ [gcc6];
      name = "solemnsky";
      src = ./.;
    };
}
