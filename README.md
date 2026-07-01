Improved Camera SF — Beta

**A first-person camera overhaul for Starfield that lets you see your full character body in first-person view.**

> ⚠ **Beta version** — The mod works but is still in development. Some features are incomplete and issues remain. See *Known Issues* below.

## Beta Version 2
- Added F4 key to toggle the pseudo-camera on and off in-game.
- Added an .ini file allowing everyone to customize the camera position and FOV.
- Fixed a few bugs.

## Beta Version 3
-Better furniture integration (the camera won't spin out of control anymore),
-Numerous bug fixes,
-Perspective shift (camera is now at head height, right where the nose is),
-Full integration with SAF,
-Compatibility with SnuSnuField (enjoy animations in first-person with a visible body)
-Three functions for controlling the pseudo-camera position have been added to the 
ImprovedCameraSF.ini file:
fUpOffset – camera height/vertical position
fForwardOffset – moves the camera forward/backward
fSideOffset – centers the camera if it drifts away from the middle
The provided values are my personal preferences and may or may not suit your taste, but you can always adjust them to your liking.
I haven't found an effective way to hide the head yet.

## Overview

Improved Camera SF brings back the classic third-person-to-first-person hybrid camera — also known as **pseudo-FPP** or **first-person with visible body**. Instead of the game's standard first-person mode (which hides your character), this mod places the camera at your character's head while keeping the full body rendered, animated, and visible. The result is an immersive first-person view where you can see your own body, gear, and shadows.

Toggle between normal third-person and the enhanced first-person view with a hotkey.

## Features

- **Full body in first person** — See your character's body, equipment, and armor while in first-person view
- **Head tracking** — Camera follows the head bone during standard animations (walking, running, sprinting, jumping, aiming)
- **Player rotation on look** — Your character model rotates naturally when you look left or right in pseudo-FPP mode
- **Toggle on/off** — F4
- **Compatible with third-person animations** — Body animations play normally while in pseudo-FPP mode
- **INI files** — for manual tuning of the pseudo-camera.

## Requirements

- **Starfield** version 1.16.244 (or compatible)
- **SFSE** (Starfield Script Extender)
- **Address Library** (for SFSE)

## Installation
Mod Manager or manually:
1. Install SFSE and Address Library if you haven't already
2. Extract the archive
3. Copy `ImprovedCameraSF.dll`and `ImprovedCameraSF.ini` to `YourGameFolder\Data\SFSE\Plugins\`
4. Launch the game through SFSE

## Usage

The toggle hotkey is configurable. By default:
- **Press the toggle key** while in third-person to activate pseudo-FPP mode
- **Press again** to return to normal third-person

## Known Issues

> ⚠ This is a **beta release**. The mod is functional but not yet polished. Expect imperfections.

- **Head is visible in pseudo-FPP** — The camera sits at the head position, which means you can see the inside of the head mesh (mouth, nose, eyes). We have not yet found an effective method to hide the head geometry in Starfield. None of the approaches tried so far (setting `kHidden` flag bit, scaling to near-zero, modifying bounding sphere, moving geometry out of view) have worked reliably
- **Third-party animation mods** may cause the camera to temporarily detach from the head position during custom animations. The camera reverts to following the player's root position as a fallback when the head bone cannot be tracked
- **Weapon draw/holster** transitions may cause a brief camera reposition while the skeleton reloads

## To Do

- Proper head/body geometry visibility toggle (researching the correct NiAVObject flag in Starfield)
- Improved tracking during third-party animation mods
- Forward position offset fine-tuning
- Forward-looking camera rotation for pseudo-FPP (currently uses third-person camera orientation)
- Forward offset for improved head geometry culling
- INI-based configuration for offsets and toggle keys

## Technical Notes

Built with CommonLibSF and MinHook. The mod hooks into `ThirdPersonState::Update` and `TESCamera::Update` to override the camera position without modifying game files.

## Credits

- CommonLibSF by the Starfield modding community
- MinHook by Tsuda Kageyu
- Inspired by Improved Camera for Skyrim SE
