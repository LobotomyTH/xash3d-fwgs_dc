# Xash3D FWGS Engine Dreamcast port <img align="right" width="128" height="128" src="https://github.com/FWGS/xash3d-fwgs/raw/master/game_launch/icon-xash-material.png" alt="Xash3D FWGS icon" />

Xash3D (pronounced `[ksɑʂ]`) FWGS is a game engine, aimed to provide compatibility with Half-Life Engine and extend it, as well as to give game developers well known workflow.

Xash3D FWGS is a heavily modified fork of an original [Xash3D Engine](https://www.moddb.com/engines/xash3d-engine) by Uncle Mike.

This port was originally authored by maximqaxd and megavolt85. This fork is for archival, but who knows what may happen later.

## Installation & Running (Dreamcast)
0) Build the binaries
1) Copy engine binaries to some directory
2) Copy Half-Life game data (valve) to same directory with engine binaries
3) Copy build/valve folder to valve folder which you copied to directory with engine binaries, overwrite all if asked
4) Scramble binary, make cdi or gdi.
5) Run

## Building
#### Dreamcast
Currently it runs only on 32MB modded Dreamcast, 16 MB RAM support will come later.

0) Build GPFTroy/SDL1.2 and GLDC by Kazade, replace at GLDC's texture.c:1700  FASTCPY(targetData, conversionBuffer, destBytes); to memcpy;
1) Build filesystem `cd filesystem && make` and ref_gldc `cd ref/gldc && make`
2) Compile engine at root dir: `make 1ST_READ.BIN`


You can support port for acquering dev tools at https://boosty.to/maximqad
