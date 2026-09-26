# NintendoWare4Ctr

This is a decompilation of the Nw4c, a Graphics/Sound engine.

The objective is to recreate Nw4c as accurately as possible.

Note that some names (especially for inlined, templated functions) are just plain guesses.

Also note, not *every* function from this library is in this repo.

## Folder Structure

* **/LIBRARY_ROOT/Nw4cEngine/**

*    |____ **include/nw** - Headers used for the *Nw4c Engine*.

*    |____ **shaders** - Nw4c ShaderBinary files used in games.

*    |____ **sources/libraries** - Module source code.

## Libraries

* **anim** - Animation (Anim curves, controllers, etc.)
* **ctrl** - Layout 2D Controller (Used in `module2`)
* **eft** - Effect Emitter (Effects used in `module2` version of Nw4c)
* **font** - Font (Font writing, rendering, handling, etc.)
* **h3d** - Hitrus3D (`module2` version of the Graphics engine, improved rendering, models, etc.)
* **gfx** - GraphicsFX (Graphics engine, cameras, particles, models, materials, etc.)
* **io** - Input/Output
* **lyt** - Layout (2D UI)
* **math** - Math Utilities
* **os** - Operating System (SDK Utilities)
* **snd** - Sound Engine (Sound 3D, Banks, Voice, etc.)
* **ut** - Utility (Misc)

### Version specific source

Different features of the Nw4c Engine can be implemented/left out in conjunction to which game the library is being used for:

Set `NW_VERSION` to:
- `NW_VERSION_CUSTOM`     (0): Starters for making a new configuration. 
- `NW_VERSION_MILLI4C`    (1): Mario & Luigi Dream Team
- `NW_VERSION_REDPEPPER`  (2): Super Mario 3D Land
- `NW_VERSION_CTRDASH`    (3): Mario Kart 7
- `NW_VERSION_GARDEN`     (4): Animal Crossing New Leaf: Welcome Amiibo!
- `NW_VERSION_STICKSTR`   (5): Paper Mario: Sticker Star

Presets and features for more games can be added if desired.

## Building

Building this project requires:

- ARM C++ Complier (ARMCC) Version 4.0/4.1/5.0 [which can be found here.](https://github.com/RE-Pepper/data/releases/tag/dasdasdsa)
- The Nintendo 3DS Software Development Kit hooked to your project [which can be found here.](https://github.com/ctrdecomp/CTR_SDK)

### Configuration

Nw4c can be configured with several compile-time defines:

* `NW_BUILD_DEBUG`: Enables assertions. (Note: Debug builds use ARM flags `-O0` `-Otime`)
* `NW_BUILD_DEVELOPMENT`: Enables assertions but builds optimized. (Note: Development builds use ARM flags `-O3` `-Otime`)
* `NW_BUILD_RELEASE`: Disables assertions. (Note: Release builds use ARM flags `-O3` `-Otime`)

## Contributing

### Non-inlined functions
When **implementing non-inlined functions**, please compare the assembly output against the original function and make it match the original code. At this scale, that is pretty much the only reliable way to ensure accuracy and functional equivalency.

However, given the large number of functions, certain kinds of small differences can be ignored when a function would otherwise be equivalent:

* Regalloc differences.

* Instruction reorderings when it is obvious the function is still semantically equivalent (e.g. two add/mov instructions that operate on entirely different registers being reordered)

When ignoring minor differences, add a `// NOT_MATCHING: explanation` comment and explain what does not match.

### Header utilities or inlined functions
For **header-only utilities** (like container classes), use pilot/debug builds, assertion messages and common sense to try to undo function inlining. For example, if you see the same assertion appear in many functions and the file name is a header file, or if you see identical snippets of code in many different places, chances are that you are dealing with an inlined function. In that case, you should refactor the inlined code into its own function.

Also note that introducing inlined functions is sometimes necessary to get the desired codegen.

If a function is inlined, you should try as hard as possible to make it match perfectly. For inlined functions, it is better to use weird code or small hacks to force a match as differences would otherwise appear in every single function that inlines the non-matching code, which drastically complicates matching other functions. If a hack is used, wrap it inside a `#ifdef MATCHING_HACK_CTR` (see above for a list of defines).

### Tentative PR Contributing rules
The `ctrdecomp` organization follows a set of standards to maintain consistency and quality across our projects. To help contributors meet these standards, our team has established the following guidelines:

* **All code must be submitted through the GitHub Pull Request process.**

* **Code must not be obtained from illegal or unauthorized material.** If such material is detected, the contribution **will not** be accepted.

* **Use of AI must be disclosed.** Contributors must disclose when and where they use AI.

* **All code must be reviewed by a human before submission.** Contributors are responible for reviewing to match styling, errors, etc.

* **Decompiled code should be reasonably representative of how the original source code may have been written. Avoid excessive or unnecessary pointer arithmetic when the underlying data is clearly identifiable as a struct or class.** In general, a raw Ghidra decompilation that merely compiles is not sufficient for PR acceptance; the code should be properly cleaned up, structured, and made readable.

* **Most functions should have a corresponding Doxygen documentation comment above its top-most declaration.** Most one-line functions are exempt here, but generally over 2-3 lines is a decent rule of thumb.

* **All code must be C++03-compliant.**