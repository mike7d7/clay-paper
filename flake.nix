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
              clang main.c -o clay-paper `pkg-config --libs --cflags sdl3 sdl3-ttf sdl3-image`
            '';
            installPhase = ''
              mkdir -p $out/bin
              cp clay-paper $out/bin/
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
                  ++ (if system == "aarch64-darwin" then [ ] else [ gdb ]);
              };
        }
      );
    };
}
