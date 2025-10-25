# Contribute

Thanks for wanting to contribute!
PopCorn Bucket is a collection of **simple, focused MS-DOS game programming examples** made with **DJGPP**, meant for learning and sharing.
Keep it clean, small, and authentic to the retro PC dev experience.

## General Guidelines

- Each example should focus on a **single topic or concept**.
  (e.g. “VGA pixel plotting”, “sprite movement”, “Sound Blaster playback”, etc.)
- Code should compile cleanly with **DJGPP** and run on **real hardware** or **DOSBox**.
- Stick to **C or C++** only. No weird build tools or dependencies.
- Keep things **educational** — clarity beats cleverness every time.

## Repository Structure

The project follows a **flat structure** where each example lives in its **own folder**.

Each example folder should contain:

- `main.c` (or similar)
- `Makefile`
- Any local assets (sprites, sounds, etc.)
- A short `README.md` describing what it does and how to build/run it
- Add it to the root Makefile list as follows `MAP_folder-name := filename.exe`

> [!WARNING]
> **Keep file names DOS-friendly** — short, simple, and alphanumeric. (8 characters long max.)

## Example Dependencies

Some examples may depend on code from other examples — that’s fine.
Just keep it **simple and well documented**.

If Example B depends on Example A:

- Mention it clearly in Example B’s `README.md`
- Use relative includes (e.g. `#include "../example_a/somecode.h"`)
- Avoid circular or complex dependency chains

## Assets & Third-Party Code

If you include **assets** (sprites, sounds, fonts, etc.) or **third-party code**:

- Make sure it’s **credited properly** (see credit format below).
- Check that its license is **compatible** with this project’s license.
  Ideally use:
  - **Creative Commons (BY or BY-SA)**
  - **Public Domain / CC0**
  - **Permissive licenses** (MIT, BSD, etc.)
- If in doubt, don’t include it directly — link to the source instead.

## Code Style

- Keep code **clean and readable**.
- Use consistent indentation (tabs or 4 spaces — just be consistent).
- Use simple function and variable names.
- Comment briefly where it helps understanding.
- No need for over-engineered abstractions — keep it retro and direct.


## Pull Requests

When submitting a PR:

1. Make sure your example **builds with `make`** and **runs cleanly** in DOSBox.
2. Include a short description of what your example demonstrates.
3. Keep commits clean — no build outputs, `.exe`s, or junk files.
4. Include proper **credit** if you used external material.

## Credits & Attribution

Always give credit where it’s due.
If your contribution includes any third-party material, clearly state:

- Who created it
- Where it came from (URL if possible)
- What license it uses

This keeps everything transparent and respectful.
