{
  outputs =
    {
      nixpkgs,
      self,
      ...
    }:
    let
      forAllSystems =
        function:
        nixpkgs.lib.genAttrs nixpkgs.lib.systems.flakeExposed (
          system: function nixpkgs.legacyPackages.${system}
        );
    in
    {
      devShells = forAllSystems (pkgs: {
        default = pkgs.callPackage ./shell.nix { };
      });
    };

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };
}
