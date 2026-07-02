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

## )
- Forward offset for improved head geometry culling
- INI-based configuration for offsets and toggle key

---

## Building (Linux → Windows cross-compile)

This fork adds an [xmake](https://xmake.io) build that cross-compiles the SFSE
plugin **from Linux** to a Windows x64 DLL, using `clang-cl` + `lld-link` +
`llvm-rc` against an [xwin](https://github.com/Jake-Shadle/xwin)/MSVC Windows SDK
tree (referenced here as `~/.vsxwin`).

```sh
# Provide the dependencies under extern/ first (see below), then:
xmake f -p windows -a x64 -m releasedbg --toolchain=clang-cl --sdk=$HOME/.vsxwin -y
xmake build
```

Output: `build/windows/x64/releasedbg/ImprovedCameraSF.dll`.

### Dependencies

- `extern/commonlibsf/` (not committed — `extern/` is git-ignored): CommonLibSF
  (libxse fork, runtime 1.16.244.0) + `commonlib-shared`, built from source.
- Pulled automatically from xrepo: `glm`, `imgui` (core), `minhook`, `mini` (mINI),
  plus `spdlog` transitively via `commonlib-shared`.

`modules/detect/tools/find_rc.lua` (included) lets xmake locate an `rc` compiler on Linux.

## Credits

Original mod by **mielu91m** / the IC Team — <https://github.com/mielu91m/ImprovedCameraSF>.
This repository is a modified fork that adds the Linux cross-compile build system
above; all mod functionality and design are the original author's work.

## License

Distributed under the Mozilla Public License 2.0 — see [LICENSE](LICENSE).
