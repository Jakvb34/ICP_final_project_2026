# Interactive Multimedia 3D Environment
## ICP Final OpenGL Project

### Authors
- Jakub Pepłowski

Lodz University of Technology  
International Faculty of Engineering  
Electronics and Telecommunications  

---

# Project Description

This project is a real-time interactive 3D OpenGL application developed as a final assignment for the ICP course.

The application presents a multimedia 3D environment containing multiple animated models, dynamic lighting, first-person navigation, collision detection, real-time camera processing, positional audio and graphical user interface controls.

The project was implemented using modern OpenGL Core Profile, GLSL shaders, OpenCV, ImGui and Miniaudio.

---

# Implemented Features

## Graphics

- OpenGL Core Profile
- GLSL shaders
- OpenGL debug output
- Ambient lighting
- Directional lighting
- Three animated point lights
- Spotlight attached to the camera
- Multiple independently animated 3D models
- Texture mapping
- Antialiasing (MSAA)
- Fullscreen and windowed mode switching
- Screenshot generation

---

## Models

The scene contains four different objects:

- Rotating textured box
- Animated bunny with jumping movement
- Floating and rotating teapot
- Textured ground plane

Models are loaded from external OBJ files.

---

## Textures

The application uses multiple textures:

- Grass texture
- Box texture
- Metal texture
- Lava texture

---

## Camera System

- First-person camera
- Mouse look
- WASD movement
- Collision detection
- Limited playfield boundaries

---

## Audio

The application uses 3D positional audio.

Implemented audio sources:

- Ambient background music
- Box sound source
- Bunny sound source
- Teapot sound source

Features:

- Distance-based attenuation
- Listener attached to camera position
- Real-time microphone monitoring

---

## Real-Time 2D Raster Processing

The project uses OpenCV to capture video from a webcam.

The camera stream is processed in real time and displayed inside the application GUI as a live preview.

---

## Multithreading and Synchronization

The application uses multiple threads and synchronization mechanisms.

Implemented techniques:

- Dedicated audio management thread
- Microphone callback processing
- Mutex synchronization
- Condition variables
- Thread-safe sound management

---

## GUI

The project includes an ImGui interface displaying:

- FPS counter
- OpenGL version
- OpenGL profile
- Camera position
- Audio information
- Application state
- Available controls

---

# Controls

| Key | Function |
|------|----------|
| W | Move forward |
| S | Move backward |
| A | Move left |
| D | Move right |
| Mouse | Look around |
| Mouse Wheel | Change background brightness |
| Left Mouse Button | Capture cursor |
| Right Mouse Button | Release cursor |
| TAB | Release cursor |
| ESC | Release cursor / Exit application |
| V | Toggle VSync |
| M | Toggle Antialiasing |
| L | Toggle Fullscreen |
| P | Save Screenshot |
| SPACE | Pause / Resume animations |
| R | Red background |
| G | Green background |
| B | Blue background |
| C | Default background |

---

# Build Instructions

## Requirements

- CMake 3.20+
- C++17 compiler
- vcpkg
- OpenGL 4.1+
- GLFW
- GLEW
- GLM
- ImGui
- OpenCV
- nlohmann-json
- Miniaudio

---

## Configure

bash cmake --preset vcpkg 

## Build

bash cmake --build --preset vcpkg-debug 

## Run

bash ./build/icp_final_opengl 

---

# Hardware Limitations

The project was developed and tested on macOS running on Apple Silicon hardware.

Apple currently provides OpenGL support up to version 4.1 through a Metal compatibility layer. Therefore the application uses OpenGL 4.1 Core Profile and GLSL 410 shaders.

According to the project specification, hardware limitations such as macOS OpenGL restrictions may be ignored when newer OpenGL functionality is unavailable on the target platform.

All required project functionality remains fully operational despite these platform-specific limitations.

---

# Implemented Requirements Checklist

✔ Realtime 2D raster processing

✔ Multiple threads and synchronization

✔ OpenGL Core Profile

✔ GLSL shaders

✔ OpenGL debug output

✔ High performance (> 60 FPS)

✔ VSync control

✔ Fullscreen / windowed switching

✔ Screenshot functionality

✔ Antialiasing toggle

✔ GUI displaying OpenGL information and FPS

✔ Mouse, keyboard and window event processing

✔ Multiple independently animated 3D models

✔ First-person camera

✔ Multiple textures

✔ 3D positional audio

✔ Ambient, directional, point and spotlight lighting

✔ Collision detection and limited playfield

✔ Bounding volume collisions

---

# Tested Platform

- macOS
- Apple Silicon (ARM64)
- OpenGL 4.1 Core Profile
- C++17
- CMake + vcpkg