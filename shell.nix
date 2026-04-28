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
	  export TMPDIR=/tmp
	  export TEMP=/tmp
	  export TMP=/tmp
  '';
}
