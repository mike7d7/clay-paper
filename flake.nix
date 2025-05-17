{
  description = "A Nix-flake-based C/C++ development environment";

  # inputs.nixpkgs.url = "https://flakehub.com/f/NixOS/nixpkgs/0.1.*.tar.gz";
  inputs.nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";

  outputs =
    { self, nixpkgs }:
    let
      supportedSystems = [
        "x86_64-linux"
        "aarch64-linux"
        "x86_64-darwin"
        "aarch64-darwin"
      ];
      forEachSupportedSystem =
        f:
        nixpkgs.lib.genAttrs supportedSystems (
          system:
          f {
            pkgs = import nixpkgs { inherit system; };
          }
        );

    in
    {
      packages = forEachSupportedSystem (
        { pkgs }:
        {
          default = pkgs.clangStdenv.mkDerivation {
            pname = "clay-paper";
            version = "0.0.1";
            src = self;
            nativeBuildInputs = with pkgs; [
              pkg-config
            ];
            buildInputs = with pkgs; [
              sdl3
              sdl3-ttf
              sdl3-image
            ];
            buildPhase = ''
              clang clay-paper.c widget_functions.c -o clay-paper `pkg-config --libs --cflags sdl3 sdl3-ttf`
            '';
            installPhase = ''
              mkdir -p $out/bin
              cp clay-paper $out/bin/
            '';
          };
          debug = pkgs.clangStdenv.mkDerivation {
            pname = "clay-paper-debug";
            version = "0.0.1";
            src = self;
            nativeBuildInputs = with pkgs; [
              pkg-config
            ];
            buildInputs = with pkgs; [
              sdl3
              sdl3-ttf
              sdl3-image
            ];
            dontStrip = true;
            # dont use address sanitizer since with it SDL fails with 'No available video device'
            buildPhase = ''
              clang clay-paper.c widget_functions.c -o clay-paper-debug `pkg-config --libs --cflags sdl3 sdl3-ttf` -glldb -O0
            '';
            installPhase = ''
              mkdir -p $out/bin
              cp clay-paper-debug $out/bin/
            '';
          };
        }
      );
      devShells = forEachSupportedSystem (
        { pkgs }:
        {
          default =
            pkgs.mkShell.override
              {
                # Override stdenv in order to change compiler:
                stdenv = pkgs.clangStdenv;
              }
              {
                packages =
                  with pkgs;
                  [
                    clang-tools
                    cmake
                    cppcheck
                    pkg-config
                    sdl3
                    sdl3-ttf
                    sdl3-image
                  ]
                  ++ (if system == "aarch64-darwin" then [ ] else [ lldb ]);
              };
        }
      );
    };
}
