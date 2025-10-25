<h1>
<p align="center">
    <img src="./logo.png" alt="Logo" width="128" />
    <br>PopCorn Bucket
</p>
</h1>

  <p align="center">
    A collection of bite-sized examples for programming MS-DOS video games using DJGPP.
    <br />
    <a href="#about">🍿About</a>
    ·
    <a href="#features">👾Features</a>
    ·
    <a href="#requirements">💻Requirements</a>
    ·
    <a href="#how-to-use">🖱️How to use</a>
    ·
    <a href="#example-topics">💾Example topics</a>
    ·
    <a href="#license">📜License</a>
    ·
    <a href="#license">🤝Contribute</a>
  </p>

[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

## About

PopCorn Bucket is a curated set of minimal, well-commented examples showing how to build real MS-DOS games with DJGPP, targeting 386/486-era PCs and emulators like DOSBox.

Each example focuses on a specific concept — from basic graphics and keyboard input to sound playback and game loops — designed to be small enough to understand at a glance, yet practical enough to reuse in your own projects.

Whether you’re reliving the golden age of PC gaming or just curious about how things worked before SDL and Unity, this repo is your snack-sized entry into retro game dev.

## Features

- Runs on real hardware (386/486) and DOSBox
- Uses DJGPP (C/C++)
- Focus on clarity and authenticity
- Covers graphics, sound, input, timing, and more
- Open for contributions and new examples!

## Requirements

Before running any example you will need the following tools to be installed properly:

- A copy of [DJGPP](https://www.delorie.com/djgpp/) installed in your system. Pre-built versions also exists [here](https://github.com/andrewwutw/build-djgpp).
- An environment variable `DJGPP_PATH` pointing to the installation root folder (alternatively you can run the `setenv` script inside DJGPP installation folder)
- [Make](https://www.gnu.org/software/make/) application installed
- On Windows, a POSIX compatibility system (e.g. [MSYS2](https://www.msys2.org/), [Cygwin](https://cygwin.com/), etc)
- [DOSBOX](https://www.dosbox.com/) or any fork that allows to run programs directlythrough the CLI
- `DOSBOX_PATH` environment variable pointing to DOSBox executable file

## How to use

All examples are isolated in subfolders inside `examples` folder. Each one has its own source code and README.md with information about the example. Each folder also contains its own `Makefile` but you don't need to call them directly. All examples can be built and run from the root folder.

These are the basic commands:

- Type `make list`to see a list of available examples.
- Type `make build <example_name>`to build the examples
- Type `make run <example_name>` to build and run the example with DOSBox

> [!TIP]
> Most if not all examples are prefixed by categories like basic, gfx (graphics), etc. so you can do something like `make list | grep gfx` to filter the list of available examples

## Example Topics

- Hello World
- VGA Mode 13h pixel plotting
- Keyboard and mouse input
- PC speaker & Sound Blaster sound
- Timer interrupts and frame limiting
- Sprites and animation basics
- Loading and parsing assets
- Rendering techniques

## License

This project is licensed under the **MIT License** — feel free to use, modify, and share!
Just keep the attribution: © 2025 onepopcorn — [PopCorn Bucket Project](https://github.com/onepopcorn/popcorn-bucket)

## Contribute

Got a cool DOS trick, a better way to do something, or just want to share your own example?
Contributions are welcome!

**Basic rules:**

- Each example lives in its **own folder** with its **own Makefile** and `README.md`.
- Keep examples **small, clear, and focused** on a single concept.
- If your example reuses code from another, that’s fine — just keep it simple and mention it.
- If you include **assets or third-party code**, make sure it’s credited and licensed properly.
  (Creative Commons, Public Domain, MIT, etc.)

Check out the full [CONTRIBUTE.md](CONTRIBUTE.md) for details before sending a PR.

---

<p align="center">💾 ¡Obsoletos pero orgullosos! 🕹️</p>
