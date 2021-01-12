# Titanium Decompiler
Writing my own decompiler for fun. 


## How to build 
Clone the repository with `git clone --recursive https://github.com/autun12/TitaniumDecompiler`.

If the repository was cloned non-recursively previously, use `git submodule update --init` to clone the necessary submodules.

## Installing and setup

Start by cloning the repository with `git clone --recursive https://github.com/autun12/TitaniumDecompiler`.

If the repository was cloned non-recursively previously, use `git submodule update --init` to clone the necessary submodules.
TitaniumDecompiler uses _Premake 5_ as a build generation tool. Visit the [Premake website](https://premake.github.io/download.html) to download and install it.

Next: Follow the steps relevant to your operating system.

### Linux Platforms

- `libxcursor`
- `libxrandr`
- `libxinerama`
- `libxi`
- `zenity`

To compile TitaniumDecompiler:
```bash
premake5 gmake2
make
```

#### Arch

On Arch and Arch derivative distributions

`sudo pacman -S libxcursor libxrandr libxinerama libxi zenity`

#### Debian

On Debian and Debian derivative distributions

`sudo apt install libxcursor-dev libxrandr-dev libxinerama-dev libxi-dev libglu1-mesa-dev zenity`

### Windows

Currently does not support the Windows operating system.