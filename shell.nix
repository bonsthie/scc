# this nix shell don't work by itself for me i suggest using the dev flake insted
{
  pkgs ? import <nixpkgs> { },
}:

pkgs.mkShell {
  packages = with pkgs; [
		bazelisk
		clang
		gcc
		jdk21
  ];
  shellHook = ''
	  alias bazel=bazelisk
  '';
}
