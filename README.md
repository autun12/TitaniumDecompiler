# Decompiler-Kotlin

## Architecture

- [Architecture](Docs/Design/Architecture.md)

## Installing and setup
Make sure you have an ssh key for github on your computer. You can find out how to make one if you don't know how read and follow the following link. [adding new SSH key to your Github account](https://docs.github.com/en/authentication/connecting-to-github-with-ssh/adding-a-new-ssh-key-to-your-github-account?tool=webui)

After you create the ssh key start by cloning the repository with `git clone --recursive git@github.com:uwp-se/Decompiler-Kotlin.git`.

If the repository was cloned non-recursively previously, use `git submodule update --init` to clone the necessary submodules.
TitaniumDecompiler uses _Bazel_ as a build tool. Visit the [Bazels website](https://github.com/bazelbuild/bazelisk/releases) to download and install it. Place that bazelisk binary into your specific os folder in vendor/bazel/bin.

Next: Follow the steps relevant to your operating system.

### Linux Platforms

- `libxcursor`
- `libxrandr`
- `libxinerama`
- `libxi`
- `zenity`

To compile TitaniumDecompiler on linux:
```bash
./vendor/bazel/bin/linux/bazelisk-linux-amd64 run //TitaniumApplication/src:TitaniumApplication
```

To compile TitaniumDecompiler on windows open the command prompt in the root directory of the project and type:

```
vendor/bazel/bin/windows/bazelisk-windows-amd64.exe run //TitaniumApplication/src:TitaniumApplication
```

To run test for the decompiler run: 
```
vendor/bazel/bin/<os>/bazelisk-<os>-amd64.exe test //TitaniumTests/TitaniumDecompilerTests/DecompilerTests:disassembly_test
```

To add test coverage for the tests run:
```
vendor/bazel/bin/<os>/bazelisk-<os>-amd64.exe coverage --config=gcov --subcommands //TitaniumTests/TitaniumDecompilerTests/DecompilerTests:*
```

To addd tests go into the _BUILD.bazel_ file under the TitaniumTests directory you are adding a test for and copy the example cc_test and change it to your test name. 

#### Arch

On Arch and Arch derivative distributions

`sudo pacman -S libxcursor libxrandr libxinerama libxi zenity`

#### Debian

On Debian and Debian derivative distributions

`sudo apt install libxcursor-dev libxrandr-dev libxinerama-dev libxi-dev libglu1-mesa-dev zenity`
