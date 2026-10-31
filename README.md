XanMusicPlayer

Custom music player plugin for GTA San Andreas (Android) using AndroidModLoader (AML). Plays user-provided audio files from a folder and integrates with the SAUtils in-game settings menu.

· Author: XanCold
· Version: 3.11
· Platform: GTA SA Android (32-bit and 64-bit)
· Framework: AndroidModLoader (AML) + SAUtils
· Website: https://xancold.blogspot.com
· LibertyCity: https://libertycity.net/user/Bdbbdbsbsnbd/
· GitHub: https://github.com/xancold/XanMusicPlayer

---

Description

XanMusicPlayer allows playing custom music during gameplay. By default the music plays only while the player is inside a vehicle (configurable). The plugin uses the Android MediaPlayer API through JNI, so it does not interfere with game audio streams. All controls are exposed through a dedicated tab in the SAUtils settings menu.

Supported audio formats: .mp3, .ogg, .wav, .flac, .m4a, .aac.

---

Features

· Play custom music tracks from a user-provided folder
· Playback option: vehicle-only mode or play-always mode
· Playback modes: Loop Directory, Loop One, Shuffle
· Auto-next when a track finishes
· Automatic stop when exiting a vehicle (in vehicle-only mode)
· Config file with hot-reload (changes applied within 3 seconds)
· In-game menu via SAUtils: toggles, mode selector, current track display, next/prev/reload buttons
· Supports both legacy and modern data directory layouts

---

Requirements

· GTA San Andreas Android (com.rockstargames.gtasa)
· AndroidModLoader (AML) installed
· SAUtils (RusJJ) installed as a mod
· Audio files placed in the correct folder (see below)

---

Installation

1. Copy libXanMusicPlayer.so (for 32-bit) and/or libXanMusicPlayer64.so (for 64-bit) into the game's mods folder:
   · /storage/emulated/0/Android_unprotected/data/com.rockstargames.gtasa/mods/
   · or /storage/emulated/0/Android/data/com.rockstargames.gtasa/mods/
2. Launch the game. The plugin will create the required config and log folders on first run.
3. Add audio files to the music folder (see Music Folder section below).
4. Open the in-game pause menu, go to Settings, and select the XanMusicPlayer tab to configure the plugin.

---

Music Folder

The plugin searches for music in two locations, in this order:

1. Legacy path (preferred, matches older versions):
   ```
   Android_unprotected/data/com.rockstargames.gtasa/XanMusicPlayer/
   ```
2. Files path:
   ```
   Android_unprotected/data/com.rockstargames.gtasa/files/XanMusicPlayer/
   ```

If a folder contains at least one supported audio file, it is used. The plugin does not move or copy files between folders. Place your audio files in whichever path already exists.

If neither folder exists, the plugin creates the legacy folder and logs a warning.

---

Configuration

Config file location:

```
Android_unprotected/data/com.rockstargames.gtasa/files/XanCold/XanMusicPlayer/config.ini
```

Default contents:

```ini
; XanMusicPlayer v3.11
; Author: XanCold
; Website: https://xancold.blogspot.com
; LibertyCity: https://libertycity.net/user/Bdbbdbsbsnbd/
;
; Enabled=1               -> mod active
; PlayOnVehicleOnly=1     -> music only plays while in a vehicle
; PlayOnVehicleOnly=0     -> music plays anytime (on foot also plays)
;
[XanMusicPlayer]
Enabled=1
PlayOnVehicleOnly=1
```

Options:

Key Values Description
Enabled 0 or 1 Enables or disables the entire plugin
PlayOnVehicleOnly 0 or 1 When 1, music plays only inside vehicles. When 0, music plays at all times

Config is hot-reloaded: edits are applied automatically within 3 seconds without restarting the game.

---

In-Game Menu

The plugin registers a new tab called XanMusicPlayer in the SAUtils settings menu. It contains the following controls:

· Enable Mod (OFF / ON)
· Play On Vehicle Only (OFF / ON)
· Playback Mode (Loop Directory / Loop One / Shuffle)
· Now Playing (displays the current track name, or (no tracks) if the folder is empty)
· Next Track button
· Prev Track button
· Reload Folder button (re-scans the music folder without restarting)

---

Behavior

Enabled PlayOnVehicleOnly Behavior
1 1 Music only plays while inside a vehicle. Stops on exit. Resumes on next entry.
1 0 Music plays whenever the game is running, regardless of vehicle state.
0 any Plugin is inactive. Music does not play.

When a track finishes and the playback mode is:

· Loop Directory: plays the next track (wrapping to the first at the end)
· Loop One: replays the same track
· Shuffle: plays a random different track

Note: in vehicle-only mode, exiting a vehicle stops playback. Re-entering a vehicle restarts the current track from the beginning.

---

File Locations

File Path
Config files/XanCold/XanMusicPlayer/config.ini
Log files/XanCold/XanMusicPlayer/log.txt
Music (legacy) com.rockstargames.gtasa/XanMusicPlayer/
Music (files) com.rockstargames.gtasa/files/XanMusicPlayer/

Both Android_unprotected/data and Android/data layouts are supported for path resolution.

---

Building from Source

Requirements

· Android NDK (tested with r27d)
· AML PSDK headers (included in aml-psdk/)
· SAUtils headers (included in SAUtils/ and isautils.h)

Steps

1. Clone the repository:
   ```
   git clone https://github.com/xancold/XanMusicPlayer.git
   cd XanMusicPlayer
   ```
2. Set the NDK path in ndkpath.txt if needed, or set the NDK environment variable.
3. Build using ndk-build:
   ```
   cd jni
   ndk-build
   ```
   Or use the provided script:
   ```
   ./build.sh
   ```
   On Windows:
   ```
   .\build.ps1
   ```
4. The compiled binaries will be placed in libs/armeabi-v7a/ and libs/arm64-v8a/.

---

Dependencies

· AndroidModLoader (AML) by RusJJ — plugin loader framework
· SAUtils by RusJJ — shared interface for in-game menus, events, and utilities
· AML PSDK — headers for GTA SA internals used by AML plugins

---

Known Limitations

· Volume control via SAUtils slider was removed in v3.3 because MediaPlayer.setVolume did not work reliably on some devices (likely due to audio focus held by the game's BASS engine). Volume is controlled by the device's media volume instead.
· HUD overlay text was removed in v2.5 after consistent crashes in CFont::PrintString when called from third-party plugins on some devices. Current track name is displayed only in the SAUtils menu.
· Track playback restarts from the beginning when re-entering a vehicle in vehicle-only mode. Resume-from-position is not implemented.

---

Log

The plugin writes a log file at:

```
files/XanCold/XanMusicPlayer/log.txt
```

The log contains startup info, config values, track scan results, playback events, and debug state lines (printed every 3 seconds). Include this log when reporting issues.

---

Credits

· XanCold — author and maintainer
· RusJJ — AndroidModLoader and SAUtils
· GTA modding community — reverse-engineering documentation used by the AML PSDK

---

License

See the LICENSE file in the repository root.

---

Links

· Website: https://xancold.blogspot.com
· LibertyCity profile: https://libertycity.net/user/Bdbbdbsbsnbd/
· GitHub repository: https://github.com/xancold/XanMusicPlayer
· AndroidModLoader Discord: https://discord.gg/2MY7W39kBg
