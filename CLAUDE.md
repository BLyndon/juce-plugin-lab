# CLAUDE.md

Learning project: basic audio effect plugins. VST3 only, C++ with JUCE, macOS (development machine), built with CMake. The repository is derived from the Pamplejuce template.

## Roles

- The human designs and reviews. You implement from the spec.
- Implement exactly what the spec says. Do not add features, options, or abstractions beyond it. If the spec is unclear or seems wrong, ask instead of guessing.
- Do not edit `plugins/<name>/spec.md` unless asked. It belongs to the human.
- `docs/` is maintained by you: keep it consistent with the code, the repository structure, and the workflow whenever they change.

## Language

- Code, comments, and repository documentation are always in English.

## Scope

- VST3 only. Do not add AU, AAX, or other plugin formats.
- Parts of the template were removed on purpose and must not be re-added: other plugin formats, code signing and notarization, the Benchmarks target, Melatonin Inspector, Intel IPP setup.
- CI (GitHub Actions, from the template) runs on macOS, Windows, and Linux. Keep code platform-independent.

## Repository structure

- One repository, one CMake root, many plugins. JUCE is a git submodule at the root (tracking `develop`, as in the template). Other dependencies (for example Catch2) come through the template's CPM setup.
- Each plugin lives in its own directory `plugins/<name>/` with its own spec, source, and Catch2 test target. Adding a plugin means adding a directory and one `add_subdirectory` line. Existing plugins are not touched. This replaces the template's single-plugin layout.
- Plugins do not depend on each other. Shared code goes in `common/` and is only extracted once a second plugin needs it. Extraction is a decision for the human, not done preemptively.
- Layout:

      plugins/<name>/{spec.md, src/, test/}
      common/
      docs/
      CMakeLists.txt

- Build, validate, and test must work per plugin (`--target <name>_VST3 <name>_Tests`, then `ctest --test-dir Builds/plugins/<name>`) as well as for all plugins at once.

## Realtime rules (audio thread)

- No memory allocation, no locks, no file or network access, no logging, and no GUI calls in the audio callback.
- Allocate and initialize in the preparation stage, not in the processing function.
- No container growth or string building in the audio callback. Prefer fixed-size storage and buffers sized in the preparation stage.
- DSP state is kept per channel and must be reset cleanly on start and reset.
- Parameters changed from the UI or host must be smoothed where they affect the signal audibly.

## Threads

- Audio thread: runs the processing callback. Message thread: runs the UI, parameter listeners, and timers.
- UI/host → audio: through the plugin's parameters (atomic) or `std::atomic` for simple values.
- Audio → UI: the UI polls state from the message thread (for example with a `juce::Timer`). The audio thread never calls UI code.

## Verification

- After each change: build, then run the tests (Catch2 via CTest) and pluginval on the result.
- Each plugin must be measurable in-process: a Catch2 test in `plugins/<name>/test/` feeds an impulse (and a sweep where the spec asks for one) through the processor and compares the output with the expected values stated in `spec.md`, within the stated tolerance.
- Do not claim a plugin works until the build, the tests, and the validation have actually run. Report failures as they are.

## Code quality

- Fix all compiler warnings before considering a task complete.
- clangd often reports false errors in JUCE code (for example "undeclared identifier", "file not found") because it does not see the JUCE module setup. Ignore them unless the actual build fails.
- Format code according to the repository's `.clang-format`.

## Code explanations

- When asked to explain code, stay at concept level: signal flow, state, parameters, threads. Class details only if asked.

## Documents

- `docs/context/toolstack-reference.md`: toolchain, build chain, failure map
- `docs/context/workflow-reference.md`: session routine, spec template, review checklist
- `docs/context/pirkle-theory-reference.md`: DSP theory map (the book uses its own framework; DSP concepts transfer, framework parts do not)
