# Motion Matching Qt Viewer

## Overview

A Qt-based 3D viewer for motion matching visualization, migrated from an original MFC/OpenGL application. The viewer renders skeletal animation, trajectory data, and supports interactive camera control and playback. The core motion matching algorithm is unchanged; only the application layer has been ported.

## Features

- **Skeleton rendering** — joint hierarchy with shadow projection
- **Trajectory visualization** — path, current goal, and future goal markers
- **Playback control** — timer-driven animation at ~30 fps, with pause and frame stepping
- **Camera interaction** — orbit, pan, and zoom via mouse
- **Menu-driven UI** — play toggle, motion visibility toggles, trajectory cycling, reset
- **Experimental perspective mode** — optional, toggled from the View menu; orthographic is the default

## Tech Stack

| Component | Version |
|---|---|
| Qt | 6.3+ |
| CMake | 3.20+ |
| C++ | C++17 |
| OpenGL | Legacy immediate-mode (gl\*, glu\*) via system `opengl32` / `glu32` |
| GLUT headers | freeglut (headers only, no binary linked) |
| Platform | Windows (MSVC) |

## Build Instructions

**Prerequisites**

- Qt 6.3 or later (Widgets + OpenGLWidgets components)
- CMake 3.20+
- MSVC (Visual Studio 2019 or 2022 recommended)
- freeglut headers placed under `opengl/include/GL/` in the project root

**Steps**

```bash
# Configure
cmake -S . -B build -G "Visual Studio 17 2022" -A x64

# Build
cmake --build build --config Debug
```

The executable must be run from the project root directory so that BVH motion files (e.g., `LocomotionFlat01_000.bvh`) are resolved correctly. This is preconfigured in the CMake target as `VS_DEBUGGER_WORKING_DIRECTORY`.

## Controls

### Mouse

| Input | Action |
|---|---|
| Left drag | Orbit (rotate camera) |
| Middle drag | Pan |
| Scroll wheel | Zoom in / out |

### Keyboard

| Key | Action |
|---|---|
| `Space` | Toggle play / pause; on pause, advances one frame and resets the frame offset |
| `←` Left arrow | Step one frame backward (while paused) |
| `→` Right arrow | Step one frame forward, up to the current state position |

### Menu

| Menu | Action | Notes |
|---|---|---|
| Control → Play | Toggle playback | Checkable |
| View → Show Src Motion | Toggle source motion display | Checkable |
| View → Show Dst Motion | Toggle destination motion display | Checkable |
| View → Perspective (Experimental) | Toggle perspective projection | Checkable; orthographic is default |
| MotionMatching → Change Trajectory | Cycle trajectory mode (0 → 1 → 2 → 0) | |
| MotionMatching → Reset | Reset motion state and frame counter | |

## Project Structure

```
.
├── CMakeLists.txt
├── main.cpp                  # Qt application entry point
├── MainWindow.h/.cpp         # QMainWindow: menu bar, action wiring
├── MotionGLWidget.h/.cpp     # QOpenGLWidget: rendering, camera, input, playback
├── MotionState.h/.cpp        # Owns CMotionMatching + Trajectory; no Qt dependency
├── CMotionMatching.h/.cpp    # Core motion matching algorithm (unmodified)
├── Motion.h/.cpp             # Motion clip data
├── MotionFeatured.h/.cpp     # Feature vectors for motion matching
├── MySkeleton.h/.cpp         # Skeleton hierarchy
├── MyJoint.h/.cpp            # Joint node
├── MyPosture.h/.cpp          # Single-frame pose
├── Trajectory.h/.cpp         # Trajectory path and goal management
├── quat.h/.cpp               # Quaternion math
├── opengl/include/GL/        # freeglut headers (not linked, declarations only)
└── [MFC source files]        # Original MFC app layer — excluded from CMake build
```

## Current Status

The Qt viewer is functional for interactive visualization. All core rendering paths from the original MFC viewer have been ported. The following are confirmed working:

- Scene rendering: floor board, axis gizmo, lighting
- Skeleton and trajectory rendering via `DrawObjects`
- Timer-based playback and keyboard frame stepping
- Mouse camera (orbit, pan, zoom)
- Menu actions for all major view and playback flags

Several MFC-specific UI details (root-only display modes, 1000-frame src preview, experiment/export dialogs) are not yet wired to menu actions.
