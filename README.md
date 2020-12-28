# <p align="center"><img src=Resources/Icon512Dark.png /></p>

VREngine is a Unreal Engine plugin that lets you easilly create VR projects. It provides all the necessary ground work so you can focus more on the game rather than creating basic VR things like movement and item pickup.

## Motivation

VREngine was developed as a side project for one of my games I made at college. The main idea was to create a plugin containing all the essential tools for me to create more VR games later on without needing to code everything again.

## Features

- Item Pickup and Drop <p align="center"><img src=Resources/Pickup.gif /></p>
- **Full body rig (IK) with Valve Index Support** <p align="center"><img src=Resources/BodyTracking.gif /></p>
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

### Using precompiled dlls

Once you download the zip file extract it and either place the extracted folder in the Plguins folder of your game or in the engine plugin folder for global use.

### Building from Source

If you build VREngine from souce you must place this repository in a folder called Plugins in the base directory of your game where the .uproject lives e.g. mygame/Plugins/VREngine

## API Reference

You can use the Wiki to find information about all the functions.

## Example Project

An example project for Unreal Engine 4.24 can be found [here](https://drive.google.com/file/d/1Q7Wmv_tRNbsVIsQVHaun2AzgNQj1ar8L/view?usp=sharing).

## Contribute

## VREngine Plugins

VREngine is setup in a way that allows users to make plugins that can be used the extend the functionality of VREngine.

- [VR Guns](https://github.com/Jordonbc/VREngineGuns)

## Disclaimer

This plugin contains content used from the 3rd person character template Epic Games provides e.g. Mannequin. This is to be used as a placeholder and testing purposes and should not be used in production as stated in the [marketplace guidelines under 2.1.e](https://www.unrealengine.com/en-US/marketplace-guidelines#21e).

## License

This project is licensed under the [Apache License 2.0](https://github.com/Jordonbc/VREngine/blob/master/license)

Apache License 2.0 © Jordon Brooks
