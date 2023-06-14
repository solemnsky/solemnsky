{
  description = "solemnsky, the open-source competitive multiplayer 2d plane game";
  inputs = {
    nixpkgs.url = "nixpkgs";
  };

  outputs = {
    self,
    nixpkgs,
  }:
    with nixpkgs.lib; let
      supportedSystems = [
        "aarch64-darwin"
        # "x86_64-linux" # buils fails on linux at the moment
      ];

      solemnsky = {system}:
        with nixpkgs.legacyPackages.${system}; let
          appResources = runCommand "solemnsky-media" {} ''
            mkdir -p $out/{media,tests,environments}
            cp -r ${./media}/* $out/media
            rm $out/media/memo_for_everybody.png
            cp ${./tests}/test.zip $out/tests
            cp -r ${./tests}/test $out/tests/
            cp -r ${./environments/export}/* $out/environments
          '';
        in
          makeOverridable stdenv.mkDerivation rec {
            pname = "solemnsky";
            version = "0.1";

            # so we only rebuild if these directories change
            src = runCommand "solemnsky-source" {} ''
              mkdir -p $out/{src,thirdparty,tests}
              cp -r ${./src}/* $out/src
              cp -r ${./thirdparty}/* $out/thirdparty
              cp -r ${./tests}/* $out/tests
              cp ${./CMakeLists.txt} $out/CMakeLists.txt
            '';
            # asdf

            env = {
              SOLEMNSKY_RESOURCES = appResources;
            };

            nativeBuildInputs = [
              cmake
              makeWrapper
            ];

            buildInputs =
              [
                flac
                libvorbis
                freetype
                libjpeg
                openal
                libsndfile
                glew
                p7zip
                boost
                sfml
              ]
              ++ lib.optionals (!stdenv.isDarwin) (with xorg; [
                udev
                libX11
                libXrandr
                libXrender
                xcbutilimage
              ])
              ++ lib.optionals stdenv.isDarwin (with pkgs.darwin.apple_sdk.frameworks; [
                IOKit
                Foundation
                Carbon
                AppKit
              ]);

            postInstall = let
              wrapBinary = binaryName: let
                unwrappedBinary = "${binaryName}_unwrapped";
              in ''
                mv $out/bin/${binaryName} $out/bin/${unwrappedBinary}
                makeWrapper $out/bin/${unwrappedBinary} $out/bin/${binaryName} \
                  --set SOLEMNSKY_RESOURCES "${appResources}" \
                  --set PATH "${p7zip}/bin"
              '';
            in
              concatStringsSep "\n" (map wrapBinary
                ["solemnsky_client" "solemnsky_server" "solemnsky_tests"]);
          };
    in {
      packages = genAttrs supportedSystems (system: with nixpkgs.legacyPackages.${system}; rec {
        default = solemnsky {inherit system;};

        tests = runCommand "solemnsky-tests" {} ''
          ${default}/bin/solemnsky_tests
          echo PASS > $out
        '';
      });
    };
}
