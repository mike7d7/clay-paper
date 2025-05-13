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
              raylib
            ];
            buildPhase = ''
              clang clay-paper.c widget_functions.c -o clay-paper `pkg-config --libs --cflags raylib` -lm
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
              raylib
            ];
            dontStrip = true;
            buildPhase = ''
              clang clay-paper.c widget_functions.c -o clay-paper-debug `pkg-config --libs --cflags raylib` -lm -glldb -O0 -fsanitize=address
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
                    raylib
                  ]
                  ++ (if system == "aarch64-darwin" then [ ] else [ lldb ]);
              };
        }
      );
    };
}
