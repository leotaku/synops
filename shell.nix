{ pkgs ? import <nixpkgs> { } }:

with pkgs;
let stdenv = pkgs.gccStdenv;
in stdenv.mkDerivation {
  name = "dev-shell";
  nativeBuildInputs = [ meson ninja pkgconfig sqlite clang-tools ]
    ++ [ valgrind kcachegrind graphviz ];
  buildInputs = [ sqlite libgit2 xxHash ];
  strictDeps = true;
}
