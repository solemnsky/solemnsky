{ nixpkgs ? import <nixpkgs> {}}:

with nixpkgs;

stdenv.mkDerivation {
  buildInputs = [cmake mesa xorg.libX11 xorg.xcbutilimage eudev libjpeg openal flac libogg freetype libvorbis glew glfw2 boost];

  installPhase = ''
    cmake .;
    make solemnsky_client solemnsky_server;
    mkdir -p $out;
    cp solemnsky_{client,server} $out/
  '';

  name = "solemnsky";
  src = ./.;
}
