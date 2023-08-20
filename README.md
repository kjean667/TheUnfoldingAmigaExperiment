# The Unfolding Amiga Experiment

## Development environment

This project is developed in Visual Studio Code on Windows.

* Clone this project.
* Open the cloned root folder in Visual Studio Code.
* In Visual Studio Code, install the extension "Amiga C/C++ Compile, Debug & Profile" by Bartman^Abyss.
* Configure the extension settings with a Kickstart ROM obtained from a source of own choice.
* Compile and run the project with Ctrl + F5, or debug with just F5.

## Notes
* Must build twice on a clean build due to dependency issues.

## Debugging
In order to set breakpoints and debug the code, the `-Ofast` flag must be removed from `CCFLAGS` in the [Makefile](Makefile).