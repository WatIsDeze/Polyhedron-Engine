# Nail & Crescent



**Nail & Crescent**  is a project that aims to;

1) Replace all of the copywritten assets of Quake 2, enabling a freely downloadable
   package that makes it easy for mappers and mod makers to create and share their
   own content.

2) Update and add new features to the engine/game code that allow for maximum
   creativity and control by content creators/game devs.

3) Develop and provide tools, tutorials, and documentation.


**This is not a source-port for running pre-Q2RTX mods/expansions or 
  playing Quake 2 as too many significant changes have been made,
  most notably game tick rate. This fork is dedicated to future
  content/mod development**
  

This list will be constantly changing and evolving, but for now here is
a summary of the most significant features and udpates added so far:


## Game Code (DLL)

  Our game DLL is forked from Knightmare Quake 2: http://www.markshan.com/knightmare/
  This gives us most of the features of the Lazarus mod, which greatly enhances
  what mappers can do with the engine, as well as some features from Knightmare,
  such as Entity Alias scripting.
  
  **Highlights**
   - Improved monster AI
   - Improved physics
   - Greatly expanded mapper control of monsters (friendly, neutral, teams, scripting)
   - Model/sprite spawning entities that easily allow the mapper to add custom content
   - "movewith" which allows for greater control over moving entities
   - real-time camera/cinematics
 
## Engine/Rendering

 **Fog**
 - Based on god rays, can be dynamically controlled by map triggers.
   4 modes:
           - Mode0 Exp based fog t = T * T * T
           - Mode1 Exp2 based fog t = T * T
           - Mode2 Exp2 based fog t = T * T * T * RNG
           - Mode3 Exp2 Mode2 + no sun phase cherry

            Mode 0 & 1 have no godrays and have been coded to use Spherical Fibonacci Blue Noise
            Mode 2 & 3 use the inscatter as outscatter input
   
  **Sprites**
  - 2 auto-rotation types (360 & locked z-axis)
  - 3 "fixed" orientations (cards) x, y, z-axis
  - 30 or 20 FPS playback (mapper selectable)
  
  **Light Entities**
  - Basic map light entity support merged from **Savvy: https://github.com/savvykms/Q2RTX
  - 2 new light types added; spotlight and directional
  - Custom lightstyles with full map trigger control (play different animations on one light)
  

  **Merged from SkacikPL - https://github.com/SkacikPL/Q2RTX**
  - Open AL audio w/ map triggered effects; Occulusion, Doppler, Reverb
  - Demo pre-rendering; Playing a demo with cl_renderdemo 1 will
    essentially "render" a sequence of accumulated frames
   (according to pt_accumulation_rendering_framenum cvar)
   - Monster footsteps
   
   
   ## Mapping Triggers
    -  Sun 
    -  God Rays
    -  Fog
    -  OpenAL Reverb (presets and custom)
    -  Lightstyles (custom brightness animations)
    
   ## Other updates/improvements
     - Fixed animated texture playback, added 20fps and 30fps playback speeds
     - Custom dynamic lights (effects) for simulating flame/candle light etc.
     - Re-enabled god rays in physical_sky_space mode (allows for "standard" 6 image skyboxes)
     - Custom folder for planetary rendering textures (based on map name)
     - Cvar to turn planetary rendering off
     - MD3 multiple skin support
     - Permenently enabled quakeworld style movement (qwmod)
     - Changed game tick to 20hz
     - Expanded dyanmic/map light maximums to 256 from 32
   
     
   ## Tools/Resources
    - "drag and drop" sprite creation from text file 
    - Modified Blender export addon (added MD3 skin support)
    - Custom FDG (entity definitions) and placeholder models/entity presets for map editors
   
   
   ## Team
  - Project Lead (and a little bit of everything else): Adam "PalmliX" Palmer
  - Graphics/Engine programmer: Matthew "omegaminus1" Motsinger - https://github.com/OmegaMinus1
  - Modelling/Texturing: Gabriel "Zedekiel" Von Gertten - http://gabrielvongertten.com/
  - Game DLL programmer: Mark "Knightmare" Shan - http://www.markshan.com/knightmare/
  - Game DLL programmer & Tools: Jonathan "Paril" Barkley - https://github.com/Paril
  - Music: John S. "Primeval" Weekly - https://johnsweekley.bandcamp.com/
  - Foley Sound: Ryan Haynes - https://soundcloud.com/minayr/tracks
  - Level Design: Aleksandr "alex_mmc" Lavrinovitš  - www.alexmmc.net
  - Graphic design: Isaac Silver
   
   
 _______________________________________________________________________


# Quake II RTX

**Quake II RTX** is NVIDIA's attempt at implementing a fully functional 
version of Id Software's 1997 hit game **Quake II** with RTX path-traced 
global illumination.

**Quake II RTX** builds upon the [Q2VKPT](http://brechpunkt.de/q2vkpt) 
branch of the Quake II open source engine. Q2VKPT was created by former 
NVIDIA intern Christoph Schied, a Ph.D. student at the Karlsruhe Institute 
of Technology in Germany.

Q2VKPT, in turn, builds upon [Q2PRO](https://skuller.net/q2pro/), which is a 
modernized version of the Quake II engine. Consequently, many of the settings 
and console variables that work for Q2PRO also work for Quake II RTX.

## License

**Quake II RTX** is licensed under the terms of the **GPL v.2** (GNU General Public License).
You can find the entire license in the [license.txt](license.txt) file.

The **Quake II** game data files remain copyrighted and licensed under the
original id Software terms, so you cannot redistribute the pak files from the
original game.

## Features

**Quake II RTX** introduces the following features:
  - Caustics approximation and coloring of light that passes through tinted glass
  - Cutting-edge denoising technology
  - Cylindrical projection mode
  - Dynamic lighting for items such as blinking lights, signs, switches, elevators and moving objects
  - Dynamic real-time "time of day" lighting
  - Flare gun and other high-detail weapons
  - High-quality screenshot mode
  - Multi-GPU (SLI) support
  - Multiplayer modes (deathmatch and cooperative)
  - Optional two-bounce indirect illumination
  - Particles, laser beams, and new explosion sprites
  - Physically based materials, including roughness, metallic, emissive, and normal maps
  - Player avatar (casting shadows, visible in reflections)
  - Recursive reflections and refractions on water and glass, mirror, and screen surfaces
  - Procedural environments (sky, mountains, clouds that react to lighting; also space)
  - "Shader balls" as a way to experiment with materials and see how they look in different environments
  - Sunlight with direct and indirect illumination
  - Volumetric lighting (god-rays)

You can download functional builds of the game from [NVIDIA](https://www.geforce.com/quakeiirtx/)
or [Steam](https://store.steampowered.com/app/1089130/Quake_II_RTX/).

## Additional Information

  * [Announcement Article](https://www.nvidia.com/en-us/geforce/news/quake-ii-rtx-ray-tracing-vulkan-vkray-geforce-rtx/)
  * [Ray-Tracing Deep Dive](https://www.nvidia.com/en-us/geforce/news/geforce-gtx-dxr-ray-tracing-available-now/)
  * [Launch Trailer Video](https://www.youtube.com/watch?v=unGtBbhaPeU)
  * [Path Tracer Overview Video](https://www.youtube.com/watch?v=BOltWXdV2XY)
  * [GDC 2019 Presentation](https://www.gdcvault.com/play/1026185/)
  * [Client Manual](doc/client.md)
  * [Server Manual](doc/server.md)

Also, some source files have comments that explain various parts of the renderer:

  * [asvgf.glsl](src/refresh/vkpt/shader/asvgf.glsl) explains the denoiser filters
  * [checkerboard_interleave.comp](src/refresh/vkpt/shader/checkerboard_interleave.comp) shows how checkerboarded rendering facilitates path tracing on multiple GPUs and helps with water and glass surfaces
  * [path_tracer.h](src/refresh/vkpt/shader/path_tracer.h) gives an overview of the path tracer
  * [tone_mapping_histogram.comp](src/refresh/vkpt/shader/tone_mapping_histogram.comp) explains the tone mapping solution 


## Support and Feedback

  * [GeForce.com Forums](https://forums.geforce.com/default/topic/1119082/geforce-rtx-20-series/quake-ii-rtx-installation-guide/)
  * [Steam Community Hub](https://steamcommunity.com/app/1089130)
  * [GitHub Issue Tracker](https://github.com/NVIDIA/Q2RTX/issues)

## System Requirements

In order to build **Quake II RTX** you will need the following software
installed on your computer (with at least the specified versions or more 
recent ones).

### Operating System

|             | Windows    | Linux        |
|-------------|------------|--------------|
| Min Version | Win 7 x64  | Ubuntu 16.04 |

Note: only the Windows 10 version has been extensively tested.

Note: distributions that are binary compatible with Ubuntu 16.04 should work as well.

### Software

|                                                     | Min Version |
|-----------------------------------------------------|-------------|
| NVIDIA driver <br> https://www.geforce.com/drivers  | 430         |
| git <br> https://git-scm.com/downloads              | 2.15        |
| CMake <br> https://cmake.org/download/              | 3.8         |
| Vulkan SDK <br> https://www.lunarg.com/vulkan-sdk/  | 1.1.92      |

## Submodules

* [zlib](https://github.com/madler/zlib)
* [curl](https://github.com/curl/curl)
* [SDL2](https://github.com/spurious/SDL-mirror)
* [stb](https://github.com/nothings/stb)
* [tinyobjloader-c](https://github.com/syoyo/tinyobjloader-c)
* [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers)

## Build Instructions

  1. Clone the repository and its submodules from git :

     `git clone --recursive https://github.com/NVIDIA/Q2RTX.git `

  2. Create a build folder named `build` under the repository root (`Q2RTX/build`)     

     Note: this is required by the shader build rules.

  3. Copy (or create a symbolic link) to the game assets folder (`Q2RTX/baseq2`) 

     Note: the asset packages are required for the engine to run.
     Specifically, the `blue_noise.pkz` and `q2rtx_media.pkz` files or their extracted contents.
     The package files can be found in the [GitHub releases](https://github.com/NVIDIA/Q2RTX/releases) or in the published builds of Quake II RTX.

  4. Configure CMake with either the GUI or the command line and point the build at the `build` folder
     created in step 2.

     `cd build`  
     `cmake ..`

     **Note**: only 64-bit builds are supported, so make sure to select a 64-bit generator during the initial configuration of CMake.
     
     Note 2: when CMake is configuring `curl`, it will print warnings like `Found no *nroff program`. These can be ignored.

  5. Build with Visual Studio on Windows, make on Linux, or the CMake command
     line:

     `cmake --build . `

## Music Playback Support

Quake II RTX supports music playback from OGG files, if they can be located. To enable music playback, copy the CD tracks into a `music` folder either next to the executable, or inside the game directory, such as `baseq2/music`. The files should use one of these two naming schemes:
  - `music/02.ogg` for music copied directly from a game CD;
  - `music/Track02.ogg` for music from the version of Quake II downloaded from [GOG](https://www.gog.com/game/quake_ii_quad_damage).

In the game, music playback is enabled when console variable `ogg_enable` is set to 1. Music volume is controlled by console varaible `ogg_volume`. Playback controls, such as selecting the track or putting it on pause, are available through the `ogg` command.

Music playback support is using code adapted from the [Yamagi Quake 2](https://www.yamagi.org/quake2/) engine.

## Photo Mode

When a single player game or demo playback is paused, normally with the `pause` key, the photo mode activates. 
In this mode, denoisers and some other real-time rendering approximations are disabled, and the image is produced
using accumulation rendering instead. This means that the engine renders the same frame hundreds or thousands of times,
with different noise patterns, and averages the results. Once the image is stable enough, you can save a screenshot.

In addition to rendering higher quality images, the photo mode has some unique features. One of them is the
**Depth of Field** (DoF) effect, which simulates camera aperture and defocus blur, or bokeh. In contrast with DoF effects
used in real-time renderers found in other games, this implementation computes "true" DoF, which works correctly through reflections and refractions, and has no edge artifacts. Unfortunately, it produces a lot of noise instead, so thousands
of frames of accumulation are often needed to get a clean picture. To control DoF in the game, use the mouse wheel and 
`Shift/Ctrl` modifier keys: wheel alone adjusts the focal distance, `Shift+Wheel` adjusts the aperture size, and `Ctrl` makes
the adjustments finer.

Another feature of the photo mode is free camera controls. Once the game is paused, you can move the camera and 
detach it from the character. To move the camera, use the regular `W/A/S/D` keys, plus `Q/E` to move up and down. `Shift` makes
movement faster, and `Ctrl` makes it slower. To change orientation of the camera, move the mouse while holding the left 
mouse button. To zoom, move the mouse up or down while holding the right mouse button. Finally, to adjust camera roll,
move the mouse left or right while holding both mouse buttons.

Settings for all these features can be found in the game menu. To adjust the settings from the console, see the
`pt_accumulation_rendering`, `pt_dof`, `pt_aperture`, `pt_freecam` and some other similar console variables in the 
[Client Manual](doc/client.md).

## MIDI Controller Support

The Quake II console can be remote operated through a UDP connection, which
allows users to control in-game effects from input peripherals such as MIDI controllers. This is 
useful for tuning various graphics parameters such as position of the sun, intensities of lights, 
material parameters, filter settings, etc.

You can find a compatible MIDI controller driver [here](https://github.com/NVIDIA/korgi)

To enable remote access to your Quake II RTX client, you will need to set the following 
console variables _before_ starting the game, i.e. in the config file or through the command line:
```
 rcon_password "<password>"
 backdoor "1"
```

Note: the password set here should match the password specified in the korgi configuration file.

Note 2: enabling the rcon backdoor allows other people to issue console commands to your game from 
other computers, so choose a good password.

## Shader Balls Feature

The engine includes support for placing a set of material sampling balls in any location. Follow these steps to use this feature:

  - Download the `shader_balls.pkz` package from the [Releases](https://github.com/NVIDIA/Q2RTX/releases) page.
  - Place or extract that package into your `baseq2` folder.
  - Run the game with `cl_shaderballs` set to 1, either from command line or from console before loading a map.
  - Use the `drop_balls` command to place the balls at the current player location.
