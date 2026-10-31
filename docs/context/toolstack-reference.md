# Toolstack Reference: VST3 Plugins with JUCE

Stable reference. Describes the toolchain, the build chain, and where things typically break. Contains no tool versions, except the language and CMake minimums the template requires; check the actual versions in the repo.

**Scope:** VST3 only, C++ with JUCE, macOS (development machine), VSCode. Learning project: many basic plugins, no showpiece. CI builds on macOS, Windows, and Linux as the template does.

---

## 1. Decisions and rationale

| Decision                    | Choice                                                                    | Why                                                                                                                                                                                                                                                                                                       |
| --------------------------- | ------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Template                    | Pamplejuce (GitHub template repo for JUCE plugins), adapted to a monorepo | A maintained, proven setup for JUCE + CMake + CI + pluginval + Catch2. Learn from it instead of inventing a stack. Removed on purpose: other plugin formats, code signing and notarization, Benchmarks target, Melatonin Inspector, Intel IPP setup. Kept: `VERSION` file, `.clang-format`, `.gitignore`. |
| Plugin format               | VST3 only                                                                 | One format is enough to learn the craft. JUCE wraps the format, so the plugin logic is format-independent. AU and AAX can be added later without rewriting DSP.                                                                                                                                           |
| Framework                   | JUCE, as git submodule tracking `develop`                                 | Handles the host interface, parameters, state, and GUI. Pirkle's DSP objects and concepts transfer 1:1; his framework-specific parts (ASPiK) do not. A submodule records one exact commit, so `develop` only matters when the submodule is updated deliberately.                                          |
| Build system                | CMake (template minimum: 3.25) with C++23                                 | JUCE's supported non-IDE route. Works from the terminal and from VSCode. Other dependencies come through the template's CPM setup.                                                                                                                                                                        |
| Generator / build directory | As in the template                                                        | Follow the template instead of deviating. Record the local commands in §3 in Session 0a.                                                                                                                                                                                                                  |
| Editor                      | VSCode                                                                    | Preferred. No full Xcode.                                                                                                                                                                                                                                                                                 |
| Compiler                    | Apple Clang via Xcode Command Line Tools                                  | Must support C++23 as the template requires. To be confirmed in Session 0a.                                                                                                                                                                                                                               |
| Host                        | JUCE AudioPluginHost                                                      | Free, ships with JUCE, no DAW needed. One host for both exploring finished reference plugins (Phase 0) and testing own builds (Phase 4). Loads plugins, routes audio, shows the plugin GUI. Learning its operation is part of the goal.                                                                   |
| Reference plugins           | Free third-party VST3s plus Apple's built-in Audio Units                  | Finished effects to explore before writing a spec, and to A/B against own builds. Apple AUs load in AudioPluginHost on macOS and serve as listening references only, not as build targets (scope stays VST3 only). Candidates per plugin in `plugin-concept-examples.md`.                                 |
| Validation                  | pluginval                                                                 | Automated plugin test suite. Catches realtime, state, and parameter bugs that listening does not. Runs in CI as in the template.                                                                                                                                                                          |
| Tests / measurement         | Catch2 (from the template), run through CTest                             | The plugin processor is driven in-process (impulse, sweep) and compared with the expected values stated in the spec. No separate language or file hand-off.                                                                                                                                               |
| CI                          | GitHub Actions from the template, on macOS, Windows, Linux                | Free on public repositories. Code signing steps are removed. A platform job that causes problems is dropped at once, not debugged (known risk: pluginval on Linux). See §5 for the minutes question.                                                                                                      |
| Code author                 | Claude Code                                                               | The human focuses on design, concepts, and review.                                                                                                                                                                                                                                                        |

---

## 2. The build chain

```text
Spec ──► Source (C++/JUCE) ──► CMake configure ──► Build ──► VST3 bundle
  ▲                                                           │
  └── expected values ◄── Measurement (Catch2) ◄── Host / test run / CI
```

Each arrow is a place where an error can enter. Knowing which step failed is the main diagnostic skill.

| Step         | Input → Output                 | What it proves                                    |
| ------------ | ------------------------------ | ------------------------------------------------- |
| Configure    | `CMakeLists.txt` → build files | Toolchain and JUCE are found                      |
| Build        | Source → VST3 bundle           | Code compiles and links                           |
| Install/scan | Bundle → visible in host       | Host finds and loads the binary                   |
| Validate     | Bundle → pluginval report      | Plugin behaves as a well-formed plugin            |
| Listen       | Host + audio → ears            | Gross errors are absent                           |
| Measure      | Test input → numbers           | Plugin matches the expected values in the spec    |
| CI           | Push → report per platform     | The same chain holds on macOS, Windows, and Linux |

---

## 3. Standard commands

Confirmed locally in Session 0a (macOS, Command Line Tools only). Generator and build directory match the CI workflow.

```bash
# configure (once; re-run only when CMakeLists.txt changes)
cmake -B Builds -G Ninja -DCMAKE_BUILD_TYPE=Debug

# build one plugin (VST3 bundle + test executable) / everything
cmake --build Builds --target <name>_VST3 <name>_Tests
cmake --build Builds

# tests and pluginval for one plugin / everything
ctest --test-dir Builds/plugins/<name> --output-on-failure
ctest --test-dir Builds --output-on-failure
```

- `--target <name>` alone builds only the plugin's static library, not the VST3 bundle or the tests. JUCE names the bundle target `<name>_VST3`.
- pluginval is not run separately: each plugin registers it as a CTest test (`<name>_pluginval`, strictness level 10). It is downloaded into the build folder; nothing is installed system-wide.
- `Builds/` is git-ignored. A rebuild without source changes finishes in about a second; CTest always reruns every test, and pluginval takes most of that time (about 16 s per plugin).
- VST3 bundles are installed per user under `~/Library/Audio/Plug-Ins/VST3`. JUCE can copy the bundle there after each build (CMake option on the plugin target).
- AudioPluginHost is part of the JUCE repository (extras). It is built once, then reused for all plugins and for reference plugins.
- Use a Debug build while developing, Release for performance checks.
- Commit message containing `[ci skip]` skips CI (template FAQ). Use it for documentation changes and work in progress that is known to fail.

---

## 4. Where it typically breaks

| Symptom                                   | Likely step  | Typical causes                                                                                                                                             |
| ----------------------------------------- | ------------ | ---------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Configure fails                           | Configure    | JUCE submodule not initialized, CMake below the template minimum, missing Command Line Tools, compiler without C++23                                       |
| Compile error in JUCE headers             | Build        | Compiler/standard mismatch, JUCE module not linked, `develop` branch moved on after a submodule update                                                     |
| Plugin not listed in host                 | Install/scan | Wrong install folder, host scan path, architecture mismatch, code signature                                                                                |
| Reference plugin not listed in host       | Install/scan | Scan paths for VST3 and AU not set in AudioPluginHost, plugin built for the wrong architecture (Intel vs. Apple Silicon), macOS quarantine on the download |
| Host loads but crashes                    | Validate     | Uninitialized state, wrong channel count, work done before `prepareToPlay`                                                                                 |
| pluginval fails on state/parameters       | Validate     | Parameters not registered with the host, state not restored completely                                                                                     |
| Clicks when moving a control              | Listen       | No parameter smoothing                                                                                                                                     |
| Sounds right but measures wrong           | Measure      | Sample-rate dependence, coefficient bug, state shared across channels, wrong expected value in the spec                                                    |
| Works at one block size, fails at another | Measure      | State not carried across blocks                                                                                                                            |
| CI red right after creating the repo      | CI           | Signing steps of the template still present (remove them), secrets missing                                                                                 |
| CI red on one platform only               | CI           | Platform-specific code, pluginval step on that platform, path or case-sensitivity differences. Drop that platform's job instead of debugging it (§1)       |

---

## 5. Settled and open decisions

**Settled**

1. JUCE as git submodule tracking `develop`; other dependencies via the template's CPM setup.
2. Measurement: Catch2 tests in-process, compared with expected values from the spec. Python is not part of the stack.
3. Template cleanup: formats other than VST3, code signing and notarization, Benchmarks, Melatonin Inspector, Intel IPP removed; `VERSION` kept.
4. CI as in the template (three platforms), hosted on GitHub. A platform job that causes problems is dropped immediately.
5. Generator, build directory, and prerequisites follow the template (C++23, CMake minimum).
6. Session 0 is split: 0a (repo and CI), 0b (empty plugin through the chain).
7. Monorepo layout `plugins/<name>/` with one CMake root is in place (first plugin: `plugins/empty`, since renamed to `plugins/template`, the copy source for new plugins); extras and signing steps listed in §1 are removed; CI is green. A second plugin has not been added yet.
8. Command Line Tools alone are sufficient on this machine: a clean configure, build (C++23, no warnings), Catch2 tests, and pluginval pass without Xcode. CMake and Ninja are installed via Homebrew.
9. Exact local commands: see §3.
10. Repository is public, so CI runs free of charge.
11. JUCE is used under AGPLv3 (learning project, not sold; source is public). A closed-source or commercial release would need the commercial JUCE licence; check its terms then.
12. The template's `CLAUDE.md` / `AGENTS.md` are removed. Its relevant rules are covered by this project's `CLAUDE.md`.
13. Local signature: locally built bundles are ad-hoc signed by the build and load in AudioPluginHost without any signing setup (checked with `Empty.vst3`, now `Template.vst3`: scanned, loaded, editor opens).

**Open, Session 0a**

None; Session 0a is complete.

**Open, Session 0b**

8. One-time host setup: AudioPluginHost is built and the VST3 scan path is set (see `docs/audiopluginhost.md`). Still open: scan for AU plug-ins (Apple's built-in effects, used as listening references; AUs have no scan path, macOS registers them centrally), install the reference plugins and a free spectrum analyzer listed in `plugin-concept-examples.md`, and keep a fixed set of test audio (drum loop, guitar DI, noise, sweep) so reference and own builds are compared on the same sources. AudioPluginHost cannot play audio files, so the test audio needs a way into the host (for example a loopback audio device or a DAW as second host).
