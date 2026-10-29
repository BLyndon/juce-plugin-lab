# Empty

## Purpose

Passes audio through unchanged; it exists only to take one plugin through the whole chain (build, tests, pluginval, CI, host) without any DSP.

## Reference

None. Phase 0 does not apply to Session 0b.

## Signal flow

```
input (mono or stereo) → output (same layout), unchanged
```

No stages, no wet/dry, no feedback.

## Parameters

| Name | Range | Default | Unit | Scaling (linear/log) | Smoothed? |
| ---- | ----- | ------- | ---- | -------------------- | --------- |
| —    | —     | —       | —    | —                    | —         |

The plugin has no parameters.

## DSP

- Difference equation: y[n] = x[n] (identity), so H(z) = 1
- Mapping from parameters to coefficients: none
- Stability condition: always stable (no feedback, no state)
- Expected values for the measurement:
  - Impulse in (1.0 at sample 0, 0.0 elsewhere): output is 1.0 at sample 0 and 0.0 at every other sample, on every channel. Tolerance: 0 (bit-exact), since no arithmetic touches the samples.
  - Magnitude: 0 dB at DC, at any frequency in between, and at Nyquist. Phase: 0. Follows from the impulse response; no separate sweep test.
- State carried across blocks (per channel): none
- Reported latency: 0 samples. Reported tail: 0 s.

## Edge cases

- Mono and stereo are supported; input and output layout must match. Other layouts are rejected.
- Sample-rate changes: no effect on output (no rate-dependent state).
- Silence in: silence out.
- Very small (1 sample) and large blocks: output equals input regardless of block size.

## Acceptance

- Build, tests, and pluginval pass; CI is green
- Measured impulse response matches the expected values in this spec (bit-exact)
- Plugin loads in AudioPluginHost, the editor opens, and audio passes through audibly unchanged
- No clicks when parameters move: not applicable (no parameters)
