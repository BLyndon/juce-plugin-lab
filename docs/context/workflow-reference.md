# Workflow Reference: One Plugin (Module)

## 1. Session routine (about 4 h, once per weekend)

| Phase             | Time                                                 | Content                                                                                                                                                                                                                                                                                              |
| ----------------- | ---------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0. Explore        | ~20-30 min, before the session (not part of the 4 h) | Human plays a finished reference plugin for the concept in AudioPluginHost, guided by the "Listen for" column in `plugin-concept-examples.md`. Closing question: what was surprising, and what should my version do the same or differently? The answer feeds "Purpose" and "Reference" of the spec. |
| 1. Spec           | ~45 min                                              | Human writes the spec (template below), including the expected values the measurement will check. Chat challenges it with counter-questions.                                                                                                                                                         |
| 2. Implementation | ~90 min                                              | Claude Code builds from the spec.                                                                                                                                                                                                                                                                    |
| 3. Review         | ~60 min                                              | Code is explained to the human; the human explains it back. Where this fails is the gap.                                                                                                                                                                                                             |
| 4. Test           | ~45 min                                              | Build, tests (Catch2) and pluginval, CI result, listening test in the host (A/B against the reference plugin from Phase 0).                                                                                                                                                                          |

**Session 0a: repo and CI** (about 2 h, no DSP): the Pamplejuce template is adapted into the monorepo (first step: check that its CMake setup can host several plugins), extras and signing are removed, CI is green, prerequisites and local commands are settled. Open decisions marked 0a in `toolstack-reference.md` §5 are settled here.

**Session 0b: empty plugin** (now `plugins/template`; about 2 h, no DSP): an empty plugin goes through the whole chain once. AudioPluginHost and the test audio are set up. The human then sketches the chain in their own words. Open decisions marked 0b in `toolstack-reference.md` §5 are settled here.

Phase 0 does not apply to Sessions 0a and 0b.

Repeat key concepts by short quiz before the next session, not by rereading.

---

## 2. Spec template (one per plugin)

```markdown
# <Plugin name>

## Purpose

One sentence: what it does to the sound.

## Reference

Which finished plugin was explored in Phase 0, and what was observed (surprises, behavior to copy or avoid).

## Signal flow

Block diagram in text: input → stages → output, including wet/dry and feedback paths.

## Parameters

| Name | Range | Default | Unit | Scaling (linear/log) | Smoothed? |

## DSP

- Transfer function H(z) or difference equation
- Mapping from parameters to coefficients
- Stability condition
- Expected values for the measurement, derived by hand from H(z) or the difference equation (for example magnitude at DC, cutoff, and Nyquist; the first samples of the impulse response), each with a tolerance
- State carried across blocks (per channel)

## Edge cases

Extreme parameter values, sample-rate changes, mono vs. stereo, silence in, very small/large blocks.

## Acceptance

- Build, tests, and pluginval pass; CI is green
- Measured response matches the expected values in this spec within the stated tolerance
- No clicks when parameters move
```

---

## 3. Verification: three paths independent of reading the code

1. **Build and validate:** the build succeeds and pluginval passes.
2. **Measure:** impulse and/or sweep through the processor in a Catch2 test, compared with the expected values stated in the spec.
3. **Listen:** hearing test in the host, including moving parameters while audio plays.

CI repeats paths 1 and 2 on macOS, Windows, and Linux.

---

## 4. Review checklist

- Which parameters exist, and are they the ones in the spec?
- Where does DSP state live, and is it per channel?
- What happens when sample rate or block size changes?
- Is anything in the audio path that must not be (allocation, locks, file or UI access)?
- Do parameter changes cause clicks?
- Does the plugin report anything it must (latency, tail)?
- Does it do anything the spec did not ask for? (Remove it.)

---

## 5. Related documents

| Document                     | Role                                                                                            |
| ---------------------------- | ----------------------------------------------------------------------------------------------- |
| `pirkle-theory-reference.md` | Chapter map, glossary, reading paths for the DSP theory                                         |
| `toolstack-reference.md`     | Toolchain, build chain, failure map                                                             |
| `plugin-concept-examples.md` | Numbered plugin sequence, optional additions, reference plugins and listening notes for Phase 0 |
| `CLAUDE.md`                  | Standing rules for Claude Code                                                                  |
