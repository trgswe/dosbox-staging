# Packaging Guidelines

This document is intended for **package maintainers**.
If you are a normal user, follow instructions in [README.md](README.md).
If you are a contributor, follow instructions in [BUILD.md](BUILD.md).


## Build dependencies

Aside of [Meson] and C++14 compiler, the build requires several dependencies to
provide fully-featured emulator (the majority of DOSBox users will expect these
features to be enabled).

Look into [`meson_options.txt`] to learn how to disable these features.

| Dependency             | Type      | Feature
|-                       |-          |-
| ALSA<sup>＊</sup>      | optional  | ALSA MIDI support
| FluidSynth<sup>†</sup> | optional  | Built-in MIDI synthesizer
| GoogleTest<sup>†</sup> | optional  | Unit tests
| libpng                 | optional  | Raw screenshots using Ctrl+F5
| mt32emu<sup>†</sup>    | optional  | Built-in Roland MT-32 emulator
| OpenGL                 | optional  | Sharp-bilinear scaling and shader support
| opusfile               | mandatory | Opus codec for CD-DA emulation
| SDL2                   | mandatory | (multiple features)
| SDL2\_net              | optional  | IPX emulator, Serial modem emulator

<sub>
＊&ensp;– ALSA is enabled automatically only on Linux.
          You don't need to disable it manually on other OSes.
<br>
†&emsp; – You can use this library via meson subproject and make it a 
          <a href="vendoring-dependencies">vendored dependency</a>
          if it's missing from the repositories. Or simply disable it
          until the library will be packaged.
</sub>

## Runtime dependencies

In addition to all required libraries, make **FluidR3 (GM)** soundfont
a dependency. It a FLOSS, very high quality soundfont, working very well in
wide range of DOS games.

This font package includes `default.sf2` symlink - it will be picked up
automatically as default soundfont when user turns on built-in `fluidsynth`
synthesizer, making it easier for end-users to turn on MIDI.

*It's not a hard dependency, DOSBox Staging will work fine if this soundfont
is missing, except end users will need to configure soundfont manually.*

[Repology: fluid-soundfont package][fluid-soundfont]


## Normal build and install instructions

Your Linux distribution probably already has guidelines regarding packaging
projects that use meson - refer to your OS documentation.

Meson supports two build types appropriate for packaging:

- `--buildtype=plain` - Use this to pick the default build flags for your OS.
- `--buildtype=release` - Use this for generic release build.

```sh
meson setup --buildtype=release \
            --wrap-mode=nodownload \
            --prefix=/usr \
            builddir
ninja -C builddir
ninja -C builddir test  # this step is optional
DESTDIR=$(pwd)/dest ninja -C builddir install
```
*`DESTDIR` environment variable is a standard way of installing files to
specific directory during packaging.*

Instructions above will install all the files, desktop entries, icons, man page,
licenses, documentation, etc.:

```
$ tree dest
dest/
└── usr
    ├── bin
    │   └── dosbox
    └── share
        ├── applications
        │   └── dosbox-staging.desktop
        ├── doc
        │   └── dosbox-staging
        │       ├── AUTHORS
        │       ├── README
        │       └── THANKS
        ├── icons
        │   └── hicolor
        │       ├── 16x16
        │       │   └── apps
        │       │       └── dosbox-staging.png
        │       ├── 22x22
        │       │   └── apps
        │       │       └── dosbox-staging.png
        │       ├── 24x24
        │       │   └── apps
        │       │       └── dosbox-staging.png
        │       ├── 32x32
        │       │   └── apps
        │       │       └── dosbox-staging.png
        │       └── scalable
        │           └── apps
        │               └── dosbox-staging.svg
        ├── licenses
        │   └── dosbox-staging
        │       └── COPYING
        ├── man
        │   └── man1
        │       └── dosbox.1
        └── metainfo
            └── dosbox-staging.metainfo.xml
```

If this is incorrect for your OS, adjust `meson setup` step to your
requirements, or simply remove the files you don't need.

[Meson Universal Options](https://mesonbuild.com/Builtin-options.html#universal-options)


## Vendoring dependencies

Some optional dependencies are provided as Meson subprojects - this way
you can optionally vendor them, if you need to.

Before build:

```sh
meson subprojects download
```

Source code of dependencies is downloaded to `subprojects/packagecache/` -
Meson will look in there for source code before attempting to download
these dependencies.

The exact URLs are in `subprojects/*.wrap` files.


## Static linking

We recommend using dynamic linking and libraries provided by your OS whenever
possible.

However, static linking of certain dependencies might be desired or necessary in
specific situations.  There is *no guarantee* that static linking will work with
libraries available on your OS, but here's how to do it:

- Vendored dependencies.

  Add following configure flags:
  `--default-library=static -Db_lto=true -Db_asneeded=true`

- Normal dependencies.

  Use `-Dtry_static_libs=<list>` flag.  For example, if you have `libpng.a` and
  `libSDL2_net.a` installed through your package manager, you can use flags:
  `-Dtry_static_libs=png,sdl2_net -Db_lto=true -Db_asneeded=true`.


## macOS-specific instructions

macOS icon in .icns format is not bundled in the repository, you need to
build it yourself:

```sh
brew install librsvg
make -C contrib/icons dosbox-staging.icns
```
You'll find some other files that might be relevant to macOS packaging
in `contrib/macos/`.


## FAQ

### Why binary is named `dosbox` and not `dosbox-staging`?

DOSBox Staging is a drop-in replacement for older versions of DOSBox.
We are very strict about backwards-compatibility - including compatibility
with other software and with user scripts.

Notably, Wine VDM runs `dosbox` binary with special configuration when
attempting to run 16-bit DOS software. Changing our binary name would break
this usecase.

By preserving the binary name we're also integrating nicely with GUI frontends,
such as GameHub or Lutris.


### What's with the license?

This project uses code covered by various Free/Libre licenses (see specific
files to learn what licenses do they use). As an aggregate, this project is
using **GNU General Public License 2.0 or later** (SPDX identifier:
`GPL-2.0-or-later`) - please use the correct license name when filling in
information in your package. Also, be vigilant about including all license
files.

[Meson]: https://mesonbuild.com/
[`meson_options.txt`]: meson_options.txt
[fluid-soundfont]: https://repology.org/project/fluid-soundfont/versions
