{ nixpkgs ? import <nixpkgs> {}}:

with nixpkgs;

stdenv.mkDerivation {
  # phases = "patchPhase installPhase";

  patchPhase = ''
  rm -rf thirdparty/Box2D/Box2D/{glew,glfw,HelloWorld,Testbed}/*
  '';

  buildInputs = [cmake mesa xorg.libX11 xorg.xcbutilimage eudev libjpeg openal flac libogg freetype libvorbis glew glfw2];
  name = "solemnsky";
  src = ./.;
}
