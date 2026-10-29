# CMake and Ninja: what they do

Two tools build this repository. CMake decides _what_ to build. Ninja _runs_ the build. All commands run from the repository root.

## CMake: the configurator

CMake does not compile anything itself. It reads the `CMakeLists.txt` files (the root one, plus each `plugins/<name>/CMakeLists.txt` added via `add_subdirectory`) and works out:

- which targets exist (`<name>_VST3`, `<name>_Tests`, ...)
- which source files belong to each target
- which JUCE modules, compiler flags, include paths, and dependencies each target needs (JUCE via `juce_add_plugin`, Catch2 via CPM)

From this it generates build files for a lower-level tool. This is the configure step:

```bash
cmake -B Builds -G Ninja -DCMAKE_BUILD_TYPE=Debug
```

CMake also ships CTest, which runs the Catch2 tests and pluginval:

```bash
ctest --test-dir Builds/plugins/<name> --output-on-failure
```

## Ninja: the executor

Ninja reads the generated `Builds/build.ninja` and runs the actual compiler and linker commands.

- Fast: it starts quickly and runs jobs in parallel on all cores by default.
- Incremental: it rebuilds only what changed. Edit one `.cpp` file and only that file recompiles, then relinks.

Ninja is one of several backends CMake can generate for (others: Make, Xcode, Visual Studio). It is the usual choice for command-line and CI builds.

## How they fit together

```text
CMakeLists.txt ──(cmake configure)──► Builds/build.ninja ──(ninja)──► .vst3 bundle + test binaries
```

Day to day, build through CMake. It hands the work to Ninja:

```bash
cmake --build Builds --target <name>_VST3 <name>_Tests
```

Configure again only after changing a `CMakeLists.txt` or adding a plugin. CMake usually notices this and re-runs the configure step on its own.

## Compared with Maven

Maven bundles several jobs into one tool. In C++ they are split across tools.

| Job                     | Maven (Java)                                       | This repository (C++)                                                        |
| ----------------------- | -------------------------------------------------- | ---------------------------------------------------------------------------- |
| Describe the project    | `pom.xml`                                          | `CMakeLists.txt`                                                             |
| Fetch dependencies      | Built in (Maven Central → `~/.m2`)                 | Not built in: JUCE is a git submodule, Catch2 comes via CPM (a CMake script) |
| Plan the build          | Built in                                           | CMake (configure step)                                                       |
| Run compiler and linker | Built in (calls `javac` through plugins)           | Ninja (runs `clang++` and the linker)                                        |
| Run tests               | `mvn test` (Surefire)                              | CTest + Catch2                                                               |
| Lifecycle               | Fixed phases: `compile → test → package → install` | No fixed phases, only targets built by name                                  |
| Multi-module            | Parent POM + `<modules>`                           | Root `CMakeLists.txt` + `add_subdirectory` per plugin                        |

What is different underneath:

- **Explicit, not convention.** Maven assumes `src/main/java` and knows what a JAR is. CMake assumes almost nothing: sources, flags, and outputs are listed explicitly. `juce_add_plugin` brings back some conventions for plugins.
- **No standard package ecosystem.** C++ has no central repository that everyone uses. Hence CPM here; other projects use vcpkg or Conan. These are add-ons, not part of CMake.
- **Native output.** A JAR runs wherever a JVM runs. C++ compiles to machine code per compiler, OS, and architecture. That is why CI builds separately on macOS, Windows, and Linux.
- **Separate configure step.** Maven plans and executes in one run. CMake generates build files once, then Ninja executes them. The split keeps incremental rebuilds fast, which matters because C++ compiles slowly.

In short: CMake is roughly the `pom.xml`-reading part of Maven, Ninja is the part that executes, and CPM stands in for Maven's dependency resolution.
