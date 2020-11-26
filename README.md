# <p align="center"><img src=Resources/Icon512Dark.png /></p>

VREngine is a Unreal Engine plugin that lets you easilly create VR projects. It provides all the necessary ground work so you can focus more on the game rather than creating basic VR things like movement and item pickup.

## Motivation

VREngine was developed as a side project for one of my games I made at college. The main idea was to create a plugin containing all the essential tools for me to create more VR games later on without needing to code everything again.

## Features

- Item Pickup and Drop <p align="center"><img src=Resources/Pickup.gif /></p>
- **Full body rig (IK)** <p align="center"><img src=Resources/BodyTracking.gif /></p>
- **Fully Customisable** <p align="center"><img src=Resources/Options.jpg /></p>
- **Full Item Physics** <p align="center"><img src=Resources/Collision.gif /></p>

## Code

Everything in the plugin is written in C++, no blueprints here.

You can use blueprints if you dont like coding in C++ the functions will be listed on the wiki.

## Tech/Frameworks used

Built with:

- Unreal Engine
- SteamVR

## Installation

>**VREngine currently only has support for Unreal Engine 4.24** but once I have fully migrated the files from my game into the plugin it will be kept up to date with the latest unreal version.

### Building from Source

If you build VREngine from souce you must place this repository in a folder called Plugins in the base directory of your game where the .uproject lives e.g. mygame/Plugins/VREngine

### Using precompiled binaries

Once you download the zip file extract it and either place the extracted folder in the Plguins folder of your game or in the engine plugin folder for global use.

## API Reference

You can use the Wiki to find information about all the functions.

## Example Project

Example projects can be found in the example folder. Just make sure you copy the Plugin in the the Plugins folder before opening.

## Contribute

## VREngine Plugins

- [VR Guns](https://github.com/Jordonbc/VREngineGuns)

## License
