# Decibels, parameter scaling, and smoothing

General principles on the way from a knob to the signal, with a gain plugin as the example.

## What you will learn

- **Decibel scale:** logarithmic unit for ratios
- **Parameter scaling:** mapping the host's normalised value to the parameter value
- **Parameter smoothing:** turning parameter jumps into ramps
- **Constant factor:** the simplest signal processing system

```text
p ──(scaling)──► v ──(conversion)──► c_target ──(smoothing)──► c[n] ──► DSP
```

| Symbol     | Meaning                              | Gain       |
| ---------- | ------------------------------------ | ---------- |
| `p`        | Normalised value, 0 … 1 (host)       | knob       |
| `v`        | Parameter value in its unit          | `L` in dB  |
| `c_target` | Coefficient the DSP needs            | `g_target` |
| `c[n]`     | Smoothed coefficient, per sample     | `g[n]`     |

## Decibel scale

- dB is a unit for a ratio: `L = 20 · log10(a / a_ref)` for amplitudes, `10 · log10` for power (same dB, since power ∝ amplitude²).
- Without a reference it is relative ("−6 dB" = half the amplitude); dBFS is absolute (0 dBFS = sample value 1.0).
- Equal ratios become equal steps, and hearing judges level roughly by ratio.

**Gain:** `g = 10^(L/20)`. −6 dB → 0.501, 0 dB → 1, +6 dB → 1.995. JUCE: `Decibels::decibelsToGain`.

## Parameter scaling

- Hosts store every parameter as `p` from 0 to 1; the plugin defines `p → v`.
- Rule: equal knob travel should give an equal perceived change. Linear for values already perceptual (dB), skewed for frequency or time, stepped for choices.

**Gain:** linear in dB, `L = L_min + p · (L_max − L_min)`. JUCE: `NormalisableRange`.

## Parameter smoothing

- A coefficient applied at once makes a step in the waveform: clicks, zipper noise.
- A smoother ramps `c[n]` to `c_target` over 10–50 ms. It holds state, needs the sample rate, and is reset on start.
- With several linked coefficients (filters), smooth `v` and recompute them instead.
- Measurements with expected values use a settled parameter.

**Gain:** `g[n]` ramps to `g_target`; one smoother serves all channels. JUCE: `SmoothedValue`.

## Constant factor

- `y[n] = c · x[n]`: linear, time-invariant, no memory, `H(z) = c` (flat, no phase shift).
- `c` is real; `c < 0` inverts polarity.

**Gain:** an impulse comes out as `g` at sample 0 and 0 elsewhere: the expected value for the impulse test.
