# Decibels, parameter scaling, and smoothing

The way from a knob to the signal: the host's normalised value is scaled to a parameter value, converted to the coefficient the DSP needs, and smoothed per sample. The `gain` plugin serves as the example.

Builds on: nothing

## What you will learn

- How the decibel scale expresses ratios of amplitudes and powers
- How a normalised host value is mapped to a parameter value
- Why coefficients are smoothed, and how a linear ramp does it across blocks
- What a constant factor does to a signal

## Key concepts

- **Decibel:** logarithmic unit for a ratio. Transferable: every level, gain, and threshold parameter.
- **Parameter scaling:** mapping from the normalised host value to the parameter value. Transferable: every parameter.
- **Parameter smoothing:** turns a jump of a coefficient into a ramp. Transferable: every parameter that changes the signal audibly.
- **Constant factor:** the simplest linear, time-invariant system.

## Symbols

| Symbol                         | Meaning                                                    | Unit                 |
| ------------------------------ | ---------------------------------------------------------- | -------------------- |
| $a$, $a_\text{ref}$            | Amplitude and reference amplitude                          | sample value         |
| $P$, $P_\text{ref}$            | Power and reference power                                  | sample value²        |
| $L$                            | Level of a ratio                                           | dB                   |
| $p$                            | Normalised parameter value as the host stores it, $0 … 1$  | none                 |
| $v$, $v_\text{min}$, $v_\text{max}$ | Parameter value in its unit, and its range           | unit of the parameter |
| $L_\text{min}$, $L_\text{max}$ | Range of the gain parameter                                | dB                   |
| $c_\text{target}$, $c[n]$      | Coefficient the DSP needs, target and smoothed value       | depends on the DSP   |
| $g_\text{target}$, $g[n]$      | Gain factor, target and smoothed value                     | none (linear factor) |
| $g_0$                          | Gain factor just before a ramp starts, $g_0 = g[n_0 - 1]$  | none (linear factor) |
| $x[n]$, $y[n]$                 | Input and output sample                                    | sample value         |
| $n$                            | Sample index of the signal (time)                          | none                 |
| $n_0$                          | Sample index at which a ramp starts                        | none                 |
| $k$                            | Step within a ramp, $k = n - n_0$                          | none                 |
| $f_s$                          | Sample rate                                                | Hz                   |
| $f$, $f_x$                     | Frequency, and frequency of a sine input                   | Hz                   |
| $T$                            | Ramp time                                                  | s                    |
| $N$                            | Ramp length                                                | samples              |
| $B$                            | Block size, samples per channel in one processing call    | samples              |
| $H(z)$                         | Transfer function, $z$ the complex frequency variable      | none                 |

The chain from the knob to the signal:

```mermaid
flowchart LR
    p["p (host)"] -- "scaling" --> v["v"]
    v -- "conversion" --> ct["c_target"]
    ct -- "smoothing" --> cn["c[n]"]
    cn --> dsp["DSP"]
```

For the gain plugin, $v = L$, $c_\text{target} = g_\text{target}$, and $c[n] = g[n]$.

## Decibel scale

A level is the logarithm of a ratio:

$$L = 20 \log_{10}\frac{a}{a_\text{ref}} = 10 \log_{10}\frac{P}{P_\text{ref}}$$

Both forms give the same $L$ because power is proportional to amplitude squared, $P \propto a^2$. Without a fixed reference, $L$ is relative: $L = -6\ \text{dB}$ means $a \approx a_\text{ref}/2$. With $a_\text{ref} = 1$, the largest sample value of a floating-point signal, the level is absolute and is written dBFS. Equal ratios become equal steps in $L$, and hearing judges level roughly by ratio, which is why level parameters are stated in dB.

**gain:** the plugin converts the level $L$ to the gain factor

$$g_\text{target} = 10^{L/20}$$

so $-6\ \text{dB} \mapsto 0.501$, $0\ \text{dB} \mapsto 1$, and $+6\ \text{dB} \mapsto 1.995$.

## Parameter scaling

Hosts store and automate every parameter as $p \in [0, 1]$; the plugin defines the mapping $p \mapsto v$. The rule is that equal knob travel gives an equal perceived change. A value that is already perceptual, such as a level in dB, is mapped linearly:

$$v = v_\text{min} + p\,(v_\text{max} - v_\text{min})$$

Frequency and time are perceived by ratio and get a skewed mapping instead; choices get a stepped one.

**gain:** linear in dB, $L = L_\text{min} + p\,(L_\text{max} - L_\text{min})$ with $L_\text{min} = -60\ \text{dB}$ and $L_\text{max} = +12\ \text{dB}$. The range and the parameter ID are part of the contract with the host: saved projects store $p$, so changing the range changes what a saved project sounds like.

## Parameter smoothing

The output $y[n] = g[n]\,x[n]$ is a product in time, so its spectrum is that of $x$ convolved with that of $g$: for a sine at $f_x$, the spectrum of $g$ appears on either side of $f_x$. A constant $g$ is a single line at 0 Hz, and the sine stays a sine. A jump in $g$ is a step, whose spectrum covers all frequencies $f$ and falls only as $1/f$, so the sine gains sidebands at $f_x \pm f$ across the whole band: a click, or zipper noise when the knob moves in small jumps. This is distortion, since the output contains frequencies the input does not; unlike clipping, it comes from a gain that varies in time (amplitude modulation), not from a nonlinearity.

A smoother lets $c[n]$ follow $c_\text{target}$ over a ramp of 10 to 50 ms instead. A linear ramp of duration $T$ has a spectrum falling as $1/f^2$ above roughly $1/T$, which keeps the sidebands within a few tens of hertz of $f_x$, masked by the sine itself.

The ramp starts at sample $n_0$ from the value $g_0 = g[n_0 - 1]$ and reaches the target after

$$N = \operatorname{round}(T f_s)$$

samples. With the step $k = n - n_0$ within the ramp,

$$g[n_0 + k] = g_0 + (g_\text{target} - g_0)\,\frac{\min(k + 1, N)}{N}, \qquad k \ge 0$$

The index $k$ only counts through the ramp; $n$ remains the time index of the signal.

Three consequences follow. First, $N$ depends on $f_s$ and is recomputed in the preparation stage. Second, the host passes audio in blocks of $B$ samples, chooses $B$, and may change it between calls; at 48 kHz a 20 ms ramp has $N = 960$ and runs over many blocks, so the smoother keeps its state (current value, target, remaining steps) between calls. Third, a new target during a running ramp starts a new ramp from the current $g[n]$, not from the old target, or the coefficient jumps. When several coefficients depend on one parameter (for example a filter), the parameter value $v$ is smoothed and the coefficients are recomputed from it.

**gain:** a linear ramp in $g$ with $T = 20\ \text{ms}$. The target is read once per block, so a ramp starts at a block boundary. One smoother serves all channels. Prepare and reset set $g[n]$ to $g_\text{target}$ without a ramp, and so does loading a saved state.

## Constant factor

The plugin's system is

$$y[n] = g\,x[n], \qquad H(z) = g$$

It is linear, time-invariant, and has no memory. The magnitude response is $|g|$ at all frequencies; the phase is $0$ for $g > 0$ and $\pi$ for $g < 0$, which inverts the polarity. During a ramp, $g$ becomes $g[n]$ and the system is no longer time-invariant, which is why measurements use a settled parameter.

**gain:** $g > 0$ always, so the level shifts by $L$ at all frequencies and the waveform keeps its shape.

## Measuring

The tests check the implementation against values written out from the spec, not recomputed from the code's constants; a wrong constant in the code would otherwise go unnoticed.

- **Impulse:** with a settled parameter, the input $x[n] = \delta[n]$ gives $y[n] = g\,\delta[n]$: the value $g = 10^{L/20}$ at $n = 0$ and $0$ elsewhere, on every channel. Checked at $L = -60$, $0$, and $+12\ \text{dB}$ with a relative tolerance of $10^{-6}$.
- **Ramp:** an input of ones gives $y[n] = g[n]$ directly. The change from $0$ to $-60\ \text{dB}$ at $f_s = 48\ \text{kHz}$ is set before the first block, so the ramp starts at $n_0 = 0$ and $k = n$. The output is $1 + (0.001 - 1)(n + 1)/960$ for $n < 960$ and $0.001$ after, with an absolute tolerance of $10^{-5}$. Both channels are identical.
- **State:** after loading a state with $L = +12\ \text{dB}$, the output is $10^{12/20}$ from the first sample on.

## Summary

- Hearing judges level changes roughly by ratio, so levels are logarithmic: $L = 20 \log_{10}(a/a_\text{ref}) = 10 \log_{10}(P/P_\text{ref})$, the factors 20 and 10 differing because $P \propto a^2$. The gain factor is $g = 10^{L/20}$.
- In a digital signal the natural reference is full scale, $a_\text{ref} = 1$, which gives dBFS.
- The host stores $p \in [0, 1]$; the plugin maps it to $v$ so that equal knob travel gives an equal perceived change. Levels in dB are already perceptual and are mapped linearly.
- A jump in a coefficient multiplies the signal by a step, whose broad spectrum adds new frequencies around every component of the signal: a distortion from time-varying gain, heard as a click. A ramp over $N = \operatorname{round}(T f_s)$ samples confines the spreading to low frequencies. The ramp state survives block boundaries, and a new target ramps from the current value.
- With a settled parameter, $y[n] = g\,x[n]$ has $H(z) = g$: every frequency is scaled by $|g|$ with no phase shift for $g > 0$, so the level changes and the waveform keeps its shape.

## References

- Pirkle, "Designing Audio Effect Plugins in C++", Ch. 2 (parameters, smoothing)
- JUCE: `juce::AudioParameterFloat`, `juce::NormalisableRange`, `juce::AudioProcessorValueTreeState`
- [plugins/gain/spec.md](../../plugins/gain/spec.md)
- [plugins/gain/src/Parameters.h](../../plugins/gain/src/Parameters.h), [plugins/gain/src/PluginProcessor.cpp](../../plugins/gain/src/PluginProcessor.cpp)
- [plugins/gain/test/ImpulseResponse.cpp](../../plugins/gain/test/ImpulseResponse.cpp)
