# juce-plugin-lab

[![Build and Test](https://github.com/BLyndon/juce-plugin-lab/actions/workflows/build_and_test.yml/badge.svg)](https://github.com/BLyndon/juce-plugin-lab/actions/workflows/build_and_test.yml)

Learning project: basic audio effect plugins, one concept per plugin. VST3 only, C++23 with JUCE, built with CMake. Developed on macOS; CI builds and tests on macOS, Windows, and Linux.

Derived from the [Pamplejuce](https://github.com/sudara/pamplejuce) template and restructured into a multi-plugin monorepo.

## Layout

```text
plugins/<name>/
├── spec.md          what the plugin does and the values the tests check
├── src/             processor and editor
└── test/            Catch2 tests (impulse, sweep) and pluginval
common/              shared code, extracted once a second plugin needs it
docs/                theory, guides, toolchain docs (index: docs/README.md)
JUCE/                JUCE submodule (develop)
cmake/               Pamplejuce CMake includes (submodule)
CMakeLists.txt       one root, one add_subdirectory line per plugin
```

Plugins do not depend on each other.

## Plugins

| Plugin     | Concept                                   |
| ---------- | ----------------------------------------- |
| `template` | Copy source for new plugins (passthrough) |

The planned sequence is in [plugin-concept-examples.md](docs/context/plugin-concept-examples.md).

## Setup

Requirements: CMake 3.25+, Ninja, a C++23 compiler (on macOS, the Xcode Command Line Tools are enough).

```bash
git clone --recurse-submodules https://github.com/BLyndon/juce-plugin-lab.git
```

In an existing clone:

```bash
git submodule update --init --recursive
```

## Build and test

```bash
# configure (once; again when CMakeLists.txt changes)
cmake -B Builds -G Ninja -DCMAKE_BUILD_TYPE=Debug

# build one plugin / everything
cmake --build Builds --target <name>_VST3 <name>_Tests
cmake --build Builds

# Catch2 tests and pluginval for one plugin / everything
ctest --test-dir Builds/plugins/<name> --output-on-failure
ctest --test-dir Builds --output-on-failure
```

pluginval runs as a CTest test (`<name>_pluginval`) and is downloaded into the build folder. Each build copies the VST3 bundle into the user plugin folder.

## Adding a plugin

Copy `plugins/template/`, change its metadata, and add one `add_subdirectory` line. Steps in [plugins/README.md](plugins/README.md).

## Docs

Companion documents with theory, guides, and toolchain documents. Index and structure: [docs/README.md](docs/README.md).

`CLAUDE.md` holds the rules for the coding agent (scope, realtime rules, verification).

## CI

GitHub Actions ([build_and_test.yml](.github/workflows/build_and_test.yml)) builds, tests, and validates every plugin on macOS, Windows, and Linux. Changes that cannot affect the build skip CI, and so do commits with `[ci skip]` in the message.

## License

[AGPLv3](LICENSE), because the plugins link [JUCE](https://juce.com), which is licensed under AGPLv3 or the commercial JUCE licence.

Parts derived from the Pamplejuce template remain under the MIT license, see [LICENSE-PAMPLEJUCE](LICENSE-PAMPLEJUCE). Submodules keep their own licenses.
