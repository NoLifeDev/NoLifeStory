# NoLifeStory

A group of C++ projects that have no life.

## Supported Platforms

At the moment NoLifeStory only supports Windows and *nix on x86 hardware.
While it will technically run on 32bit hardware, it won't be able to work with large files due to address space limitations, so it is highly recommended that you use 64bit.

## Compilers

Supported compilers are MSVC 2013, latest Clang, and latest GCC.

Caveats:
GCC <4.9's libstdc++ has incomplete regex support.
Due to this, NoLifeClient won't be able to read/write NoLifeClient.cfg

libstdc++ does not support `<codecvt>` at all yet so NoLifeWzToNx relies on terrible C code to do utf-16 -> utf-8 when `<codecvt>` is not available.
If you are using Clang, you have the option to build with libc++, which does support it.

## Dependencies

NoLifeStory has a bunch of dependencies.
If you're using Visual Studio, just go in the sdk folder and read the readme there.
If you are using any other compiler, good luck getting the dependencies somehow.

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

If NoLifeClient is crashing for whatever reason, open up the file NoLifeClient.log and at the very bottom should be information on what went wrong.
