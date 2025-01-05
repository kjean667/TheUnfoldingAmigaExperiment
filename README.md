# The Unfolding Amiga Experiment

## TODOs / future modifications
- LSPlayer sound library is compiled on an Amiga. Consider compiling using the host toolchain instead.
  Either inline __asm in or an .S-file. Much nicer than having the lightspeedplayer-mod.bin file from the Amiga.

## Development environment

This project is developed in Visual Studio Code on Windows.

* Clone this project.
* Open the cloned root folder in Visual Studio Code.
* In Visual Studio Code, install the extension "Amiga C/C++ Compile, Debug & Profile" by Bartman^Abyss.
* Configure the extension settings with a Kickstart ROM obtained from a source of own choice.
* Compile and run the project with Ctrl + F5, or debug with just F5.

## Notes
* Must build twice on a clean build due to dependency issues.

## Sound
Playback library: https://github.com/arnaud-carre/LSPlayer
Use LSPConvert.exe to convert .mod to .lsbank and .lsmusic
LightSpeedPlayer-mod.asm was assembled/compiled on an Amiga using AsmOne148.
Added jump table at the beginning of the source that is used by the C-code.
	JT_LSP_MusicInit:		bra.w LSP_MusicInit
	JT_LSP_MusicPlayTick:	bra.w LSP_MucisPlayTick
	JT_LSP_MusicSetPos:		bra.w LSP_MusicSetPos
	JT_LSP_MusicGetPos:		bra.w LSP_MusicGetPos

MOD file can be converted using ```.\LSPConvert.exe .\coconut.mod -shrink``` to get the .lsbank and .lsmusic files.

The coconut.mod actually contains two incorrect samples (off tune) as compared what's intended, but it sounds a little funny so
it's left as-is and can be changed.

## Debugging
In order to set breakpoints and debug the code, the `-Ofast` flag must be removed from `CCFLAGS` in the [Makefile](Makefile).