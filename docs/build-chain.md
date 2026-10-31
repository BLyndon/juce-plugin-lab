# Build chain: from source to sound

The path one plugin takes from `plugins/<name>/` to a sound in the host, using `template` as the example. Each stage only consumes what the previous one produced. Commands are in `docs/context/toolstack-reference.md` §3; CMake and Ninja are explained in `docs/cmake-and-ninja.md`; the host in `docs/audiopluginhost.md`.

```text
plugins/template/{src,test} JUCE (submodule)   Catch2, pluginval (CPM downloads)
            \                    |                     /
             └──────── 1. CMake configure ────────────┘
                                 │  Builds/build.ninja
                                 ▼
                        2. Ninja compile + link
                     ┌───────────┴────────────┐
                     ▼                        ▼
              Template.vst3 bundle      template_Tests binary
          (copied to ~/Library/...)            │
                     │                         │
                     ├──────── 3. CTest ───────┤
                     │   pluginval      Catch2 tests
                     │
                     ├──── 4. CI: stages 1–3 again on macOS, Windows, Linux
                     │
                     ▼
         5. AudioPluginHost: scan → load → listen
```

## 1. Configure (CMake)

CMake reads the root `CMakeLists.txt` and, through one `add_subdirectory` line per plugin, each `plugins/<name>/CMakeLists.txt`. It does not compile anything. It:

- pulls in JUCE from the `JUCE/` submodule and the template's helper functions from `cmake/`
- downloads Catch2 and pluginval into the build folder through CPM (nothing is installed system-wide)
- turns `juce_add_plugin(...)` into concrete targets named after the plugin directory (`template`), using the identity set there: product name "Template", plugin code, bundle id, format VST3
- collects the plugin's sources into a shared-code target, so the plugin and its tests compile from the same code
- registers the tests and pluginval with CTest

Output: `Builds/build.ninja`, a list of exact compiler and linker commands. Re-run only when a `CMakeLists.txt` changes.

## 2. Build (Ninja + Clang)

Ninja runs the commands from stage 1 in parallel and only redoes what changed. Two results matter:

- **`template_VST3`**: the plugin as a VST3 bundle, `Template.vst3`. A bundle is a folder that looks like a file: the compiled binary plus metadata the host reads before loading it. The build signs it ad hoc (enough for local hosts) and copies it to `~/Library/Audio/Plug-Ins/VST3`. Layout: `docs/plugin-filesystem.md`.
- **`template_Tests`**: a normal command-line program. It contains the plugin's processor directly, without a host, plus the Catch2 tests from `plugins/template/test/`.

Compiler warnings appear here and must be fixed before a change counts as done.

## 3. Test and validate (CTest)

CTest runs every registered test and reports pass or fail:

- **Catch2 tests** call the processor in-process: create it, prepare it, push a buffer through `processBlock`, compare the result with the expected values from `spec.md`. For `template`: an impulse must come out unchanged, bit for bit.
- **pluginval** loads the finished bundle like a host would and stresses it at strictness level 10: different sample rates and block sizes, state save and restore, editor open and close, parameter automation, calls from several threads.

Catch2 checks what the plugin computes. pluginval checks that it behaves as a plugin. Neither listens.

## 4. CI (GitHub Actions)

On every push, `.github/workflows/build_and_test.yml` repeats stages 1–3 on fresh macOS, Windows, and Linux machines. This catches code that only builds or passes on this Mac. The host stage does not run in CI.

## 5. Host (AudioPluginHost)

The host finds the bundle by scanning the VST3 folder, loads it into a graph (Audio Input → Template → Audio Output), and runs `processBlock` on live audio from the sound card. This is the only stage where a human listens, and the only one that compares against reference plugins. A rebuilt plugin is picked up when the host restarts; a new plugin needs a new scan.

## Where each kind of failure shows up

| Stage     | Typical failure                                                   |
| --------- | ----------------------------------------------------------------- |
| Configure | Typo in a `CMakeLists.txt`, missing submodule, no network for CPM |
| Build     | Compile error, warning, link error                                |
| Catch2    | Output differs from the spec's expected values                    |
| pluginval | Crash or wrong behavior under host-like conditions                |
| CI        | Platform-specific code or compiler difference                     |
| Host      | Not found by the scan, sounds wrong, clicks                       |

Details and fixes: `docs/context/toolstack-reference.md` §4.
