{
  pkgs ? import <nixpkgs> { },
}:

with pkgs;

mkShell rec {
  nativeBuildInputs = [
    pkg-config
  ];

  buildInputs = with pkgs; [
    pkg-config

    expat
    fontconfig
    libGL
    curl

    webkitgtk_4_1

    gtk3

    glib
    sysprof
    libsysprof-capture
    pcre2
    util-linux

    stdenv.cc.cc.lib

    llvmPackages_latest.llvm
    libselinux
    libsepol
    libthai
    libdatrie
    libxdmcp
    libdeflate
    libwebp
    lerc
    xz

    sqlite

    libxkbcommon
    libepoxy
    libxtst

    xorg.libX11
    xorg.libXcursor
    xorg.libXext
    xorg.libXinerama
    xorg.libXrandr
    xorg.libXrender
    xorg.libXfixes
    xorg.libXi

    alsa-lib
    freetype
  ];

  PKG_CONFIG_PATH = "${pkgs.sysprof}/lib/pkgconfig";
  # shellHook = ''
  #   export LD_LIBRARY_PATH=${pkgs.stdenv.cc.cc.lib}/lib:$LD_LIBRARY_PATH
  # '';

  LD_LIBRARY_PATH = lib.makeLibraryPath buildInputs;
}
