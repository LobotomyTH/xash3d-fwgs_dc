# Xash3D FWGS Engine Dreamcast port <img align="right" width="128" height="128" src="https://github.com/FWGS/xash3d-fwgs/raw/master/game_launch/icon-xash-material.png" alt="Xash3D FWGS icon" />

Xash3D (pronounced `[ksɑʂ]`) FWGS is a game engine, aimed to provide compatibility with Half-Life Engine and extend it. This is a Dreamcast port of the engine.

## Prerequisites

1. KallistiOS (KOS) development environment
2. GLDC
3. HLSDK-portable 

## Repository Setup

1. Create a development directory (e.g., `Xash3D_DC`)
2. Clone the following repositories in the same directory:
   ```bash
   git clone https://github.com/maximqaxd/hlsdk-portable_dc.git
   git clone https://github.com/maximqaxd/xash3d-fwgs_dc
   ```

## Required Modifications

1. Modify KOS `include/kos/opts.h`:
   ```c
   // Change this line
   #define FS_CD_MAX_FILES 48
   ```

## Building Dependencies

1. Build GLDC:
   ```bash
   cd GLdc/GL
   # Modify texture.c line 1700:
   # Replace FASTCPY(targetData, conversionBuffer, destBytes);
   # With memcpy(targetData, conversionBuffer, destBytes);
   ```

## Building Xash3D

This will build:
- Filesystem
- GL renderer
- Main engine (1ST_READ.BIN)
- HLSDK

## Installation & Running

1. Copy engine binaries to a directory
2. Copy Half-Life game data (valve folder) to the same directory
3. Copy `build/valve` folder to the valve directory, overwrite when prompted
4. Build the engine and all components (make all)
5. Run on your Dreamcast

## Hardware Requirements

Currently we support multiplayer. It requires BBA. Modem tested, but seems it won't work for now.

## Support Development

You can support the port development for acquiring dev tools at https://boosty.to/maximqad

## Notes

- This is a work in progress
- Some features may be unstable or missing
- Performance optimizations are ongoing

## Credits

- Original Xash3D Engine by Uncle Mike
- FWGS team for Xash3D FWGS fork
- Kazade for GLdc
- KallistiOS team
