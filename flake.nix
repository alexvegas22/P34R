{
  description = "Go development environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        devShell = pkgs.mkShell {
          buildInputs = [
            pkgs.go
            pkgs.gopls
            pkgs.delve
            pkgs.gotools
            pkgs.golint
            pkgs.zsh
          ];

          shellHook = ''
            export SHELL=${pkgs.zsh}/bin/zsh
            echo "P34R Go Shell!"
          '';
        };
      }
    );
}
