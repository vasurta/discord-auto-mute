# Discord Auto Mute

A Geometry Dash mod that toggles Discord's Push to Mute keybind after you reach a chosen percentage in a level.

## How to use

1. In Discord, set a **Push to Mute** keybind.
2. In Geometry Dash, open a level's info page or the in-game pause menu and press the mod's Discord button.
3. Choose the percentage at which the mod should mute. Set it to **Off** to disable auto-mute for that level.
4. Set the matching Windows virtual-key code in the mod settings. The default is `123` (`F12`).

The selected percentage is saved per level. The global **Default percent** setting is used for levels without a saved value. The mod unmutes when you die, restart, complete the level, quit, or open the pause menu.

## License

The original source code and project files are licensed under the MIT License; see [LICENSE](LICENSE). The `resources/discord.png` image contains Discord branding and is excluded from this license. Rights to that asset remain with their respective owner.

## Compatibility

- Geometry Dash 2.2081
- Geode 5.11.2 or newer in the 5.x series
- Windows x64
- Discord must be running with the matching Push to Mute keybind configured

## Install

Download `vasurta.discord-auto-mute.geode` from the [latest GitHub release](https://github.com/vasurta/discord-auto-mute/releases/latest), then open the file with Geode or place it in the Geometry Dash `geode/mods` folder.

## Build from source

Install the Geode SDK and set the `GEODE_SDK` environment variable, then configure and build with CMake:

```powershell
cmake -B build
cmake --build build --config RelWithDebInfo
```

The packaged `.geode` file is written to the build directory.

