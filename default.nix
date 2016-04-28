{ nixpkgs ? import <nixpkgs> {}}:

with nixpkgs;

stdenv.mkDerivation {
  buildInputs = [ 
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

  name = "solemnsky";
  src = ./.;
}
