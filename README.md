# NoLifeStory

A group of C++ projects that have no life.

## Supported Platforms

At the moment NoLifeStory only supports Windows and *nix on x86 hardware.
While it will technically run on 32bit hardware, it won't be able to work with large files due to address space limitations, so it is highly recommended that you use 64bit.

## Compilers

Supported compilers are MSVC 14, Clang 3.4, GCC 4.8.2, and GCC 4.9

Caveats:
GCC <4.9's libstdc++ has incomplete regex support.
Due to this, NoLifeClient won't be able to read/write NoLifeClient.cfg

libstdc++ does not support `<codecvt>` at all yet so NoLifeWzToNx relies on terrible C code to do utf-16 -> utf-8 when `<codecvt>` is not available.
If you are using Clang, you have the option to build with libc++, which does support it.

## Dependencies

NoLifeStory has a bunch of dependencies.
If you're using Visual Studio, just go in the sdk folder and read the readme there.
If you are using any other compiler, good luck getting the dependencies somehow. Usually you'll have some sort of package manager that can get the stuff for you.

## Build System

If you are using Visual Studio, just open up the solution.
Make sure you switch to release mode and use 64bit mode.
If you are using any other compiler, there are cmake files.
Good luck.

## NX Files

Everything in NoLifeStory depends on NX files, except NoLifeWzToNx which creates those NX files.
Before you attempt to run anything, you'll need to use NoLifeWzToNx to convert WZ files to NX files.
If you run NoLifeWzToNx it will provide information on how to use it.
Don't forget to use the -client flag if you want the NX files to be usable for NoLifeClient.
If you're using Visual Studio, keep in mind that the working directory for any given project is the project directory, not the output directory, and not the solution directory.

## Troubleshooting

* If NoLifeClient is crashing for whatever reason, open up the file NoLifeClient.log and at the very bottom should be information on what went wrong.
* When running NoLifeClient, if no music is playing and no sprites are being displayed, then you forgot to pass the -client flag to NoLifeWzToNx
* On macs in particular, sometimes the max texture size reported by the OpenGL drivers is not actually a valid texture size. This results in a black screen, although the music is still playing. To fix this, just manually lower the atlas size in the config (unless you're using GCC <4.9 in which case you have to modify the config code to just override the default with something smaller and recompile).
* If you can't find the log or config, remember that NoLifeClient stores those files in the working directory, not necessarily the directory the program is in.
