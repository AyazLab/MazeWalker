# MazeWalker

MazeWalker is a Windows 3D maze rendering and navigation engine, part of [MazeSuite](https://github.com/AyazLab). It presents virtual maze environments for neuroscience and psychology research, including spatial navigation studies and neuroimaging experiments (fNIRS, EEG).

Mazes are designed in [MazeMaker](https://github.com/AyazLab/MazeMaker) and run in MazeWalker.

## Features

- First-person 3D navigation with mouse, keyboard, and joystick input
- Top-down and 3/4 perspective view modes
- Bullet Physics collision detection and gravity
- OBJ model loading with dynamic object behaviors (triggers, highlights, animations)
- FMOD audio playback and recording
- Maze lists for sequencing experiments (mazes, text screens, audio, commands)
- MazePoints scoring system with configurable end conditions
- Detailed position/event logging for behavioral analysis
- TCP/IP API for remote control from external software
- LPT parallel port and serial port sync for neuroimaging equipment
- Skybox, particle effects, and GLSL per-pixel lighting

## Build Requirements

- Visual Studio 2022
- Platform Toolset v143
- Windows 8.1 SDK
- MFC for v143, ATL for v143
- C++17

## Maze File Formats

| Format | Description |
|--------|-------------|
| `.maz` | Legacy text-based maze |
| `.mazx` | XML maze package (zip with assets) |
| `.mel` | Text maze list (experiment protocol) |
| `.melx` | XML maze list |

## License

MIT - Copyright (c) 2026 Ayaz Lab
