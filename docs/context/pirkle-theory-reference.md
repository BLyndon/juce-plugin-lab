# Theory Reference: Pirkle, "Designing Audio Effect Plugins in C++"

Stable reference. Uses the book as a map: chapter classification, glossary, reading paths. Contains no book content.

**Context:** The book is built on ASPiK (own framework). Code is ported to JUCE (VST3 only, macOS). DSP objects and concepts transfer 1:1, framework-specific parts do not.

**Assumed known (skim or skip):** Ch. 1, Ch. 9, Ch. 10.1-10.22 (difference equations, transfer function, poles/zeros, z-transform, FIR/IIR theory).

---

## 1. Chapters in practical perspective

Priority: green = read, yellow = selective (see classification), red = skip or already known.

| Ch.  | Topic                                   | Priority     | Classification                                                                                                                                                                                    |
| ---- | --------------------------------------- | ------------ | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 1    | Introduction, difference equations      | 🔴 Skip      | Skim, known                                                                                                                                                                                       |
| 2    | Anatomy of an audio plugin              | 🟢 Read      | **Essential.** Lifecycle, parameters vs. variables, smoothing, threading. Maps to JUCE concepts                                                                                                   |
| 3    | VST3 programming guide                  | 🟢 Read      | **Essential, read as concepts.** Process cycle, state save/load, latency, tail, bypass. SDK code is wrapped by JUCE                                                                               |
| 4, 5 | AU, AAX                                 | 🔴 Skip      | Skip (VST3 only)                                                                                                                                                                                  |
| 6, 7 | ASPiK, RackAFX                          | 🔴 Skip      | Skip (tools not used)                                                                                                                                                                             |
| 8    | C++ conventions                         | 🟡 Selective | Only 8.1 (object model, interfaces) and 8.2.2 (JUCE notes)                                                                                                                                        |
| 9    | How filters work without complex math   | 🔴 Skip      | Skip, known                                                                                                                                                                                       |
| 10   | Basic DSP theory, biquad                | 🟡 Selective | 10.1-10.22 reference only. **Read 10.23 (Biquad object)**, foundation for Ch. 11                                                                                                                  |
| 11   | IIR filter design                       | 🟢 Read      | **Core.** Coefficient recipes for LPF/HPF/BPF/shelf/EQ, Vicanek, impulse invariant                                                                                                                |
| 12   | Wave digital and virtual analog filters | 🟡 Selective | **12.4-12.5 (Zavalishin SVF and the ZVAFilter object) are part of the project sequence (Auto-Wah).** WDF (12.1-12.3, library 12.6, chapter plugin RLCFilters 12.7) is an advanced specialty topic |
| 13   | LFO, envelope detector, phaser          | 🟢 Read      | **Core.** Modulation underlies most effects                                                                                                                                                       |
| 14   | Delay, circular buffer                  | 🟢 Read      | **Core.** The circular buffer is a key concept                                                                                                                                                    |
| 15   | Modulated delay (flanger/chorus)        | 🟢 Read      | **Core.** Direct continuation of Ch. 14                                                                                                                                                           |
| 16   | FIR filters, convolution                | 🟡 Selective | Medium. Practical for cabinet/IR plugins                                                                                                                                                          |
| 17   | Reverb                                  | 🟡 Selective | Advanced. Comb/allpass basics are medium, FDN/plate/tank are hard                                                                                                                                 |
| 18   | Dynamics processing                     | 🟢 Read      | **Core.** Compressor/gate/sidechain, builds on the detector from Ch. 13                                                                                                                           |
| 19   | Nonlinear processing                    | 🟢 Read      | **Core.** Waveshaping, tube, bitcrusher. Aliasing becomes a topic here                                                                                                                            |
| 20   | FFT, phase vocoder                      | 🟡 Selective | Advanced, self-contained                                                                                                                                                                          |
| 21   | Custom views (waveform/FFT)             | 🔴 Skip      | Skip (ASPiK GUI). Only the lock-free ring buffer concept (21.1.2) transfers                                                                                                                       |
| 22   | Sample rate conversion                  | 🟡 Selective | Medium. Connects to oversampling against aliasing (Ch. 19)                                                                                                                                        |

**Book plugins as exercise series:** IIRFilters, ModFilter, StereoDelay, ModDelay, Phaser, Dynamics, TubePreamp, AnalogFIR, Reverb, PitchShifter.

---

## 2. Glossary

### Plugin basics

- **Buffer / Frame:** Block of samples per callback / one sample across all channels.
- **Parameter vs. variable:** Parameters are visible to GUI/host, variables are internal.
- **Parameter smoothing:** Smoothing of parameter changes to avoid clicks (zipper noise).
- **Sample-accurate update:** Parameter change applied at the exact sample position within a block.
- **Latency:** Delay the plugin reports to the host (e.g. look-ahead, FIR).
- **Tail time:** Ring-out time after the input ends (delay, reverb).
- **Soft bypass:** Bypass without clicks, using a crossfade.
- **Side chain / external keying:** Extra input that drives the detector, not the signal path.

### Filters

- **Biquad:** 2nd-order IIR filter (2 poles, 2 zeros), the standard building block for EQ.
- **Direct form / transposed:** Different structures of the same filter, differing in noise and numerical behavior.
- **Q:** Quality factor, sharpness of the resonance.
- **Shelving / parametric EQ:** Boosts/cuts everything above or below a frequency / a bell-shaped boost or cut.
- **Butterworth / Linkwitz-Riley:** Maximally flat magnitude response / crossover whose bands sum to a flat response.
- **APF (allpass):** Changes phase only, not magnitude.
- **Bilinear transform:** Analog-to-digital mapping, with frequency warping.
- **Impulse invariant / matched-z:** Alternative mappings that can match behavior near Nyquist better.
- **State variable filter (SVF):** Provides LP/BP/HP simultaneously, well suited to modulation.
- **Virtual analog (ZVA/TPT):** Discrete-time model of analog circuits with clean modulation behavior.
- **Wave digital filter (WDF):** Circuit simulation via wave variables and adaptors.
- **FIR / convolution:** Filter without feedback / applying an impulse response.
- **Frequency sampling, Parks-McClellan:** FIR design methods.

### Modulation and time effects

- **LFO:** Low-frequency oscillator as modulation source.
- **Envelope detector:** Tracks the signal envelope (attack/release).
- **Delay line / circular buffer:** Delay implemented with a ring buffer.
- **Fractional delay / interpolation:** Delay between sample positions.
- **Wet/dry, feedback:** Mix ratio / routing output back to input.
- **Flanger, chorus, phaser:** Short modulated delay with feedback / longer modulated delay / modulated allpass chain.

### Reverb

- **RT60:** Time for the reverb to decay by 60 dB.
- **Comb filter:** Delay with feedback, produces comb response and echoes.
- **Delaying APF:** Allpass with delay, densifies reverb without coloration.
- **FDN (feedback delay network):** Multiple delays coupled through a matrix.

### Dynamics

- **Threshold / ratio / knee:** Level above which processing starts / strength of reduction / hard or soft transition.
- **Compressor, limiter, expander, gate:** Reduce dynamics / expand downward.
- **Look-ahead:** Detector sees the signal early, introduces latency.
- **Gain reduction (GR):** Applied level reduction.

### Nonlinearity

- **Waveshaping:** Transfer curve distorts amplitude, generates harmonics.
- **Aliasing:** Harmonics above Nyquist fold back. Remedy: oversampling.
- **Bitcrusher / ring modulation:** Reduce quantization resolution / multiply two signals.
- **Exciter:** Adds high-frequency harmonics.

### Spectral and sample rate

- **FFT / window / overlap-add:** Frequency representation / windowing / overlapping blocks for reconstruction.
- **Phase vocoder:** Spectral processing with phase tracking for time-stretch and pitch-shift.
- **Fast convolution:** FFT-based convolution, needed for long IRs.
- **Interpolation / decimation:** Raise / lower the sample rate.
- **Polyphase:** Efficient filter structure for sample rate conversion.

---

## 3. Learning paths

### By depth

**Beginner (the project sequence in `plugin-concept-examples.md`)**
Ch. 2, 3 (concepts), 13.1 (LFO), 10.23, 11 (biquad, standard filters), 14, 15, 12.4-12.5 (SVF, ZVAFilter), 13.2-13.5 (envelope detector, ModFilter), 19 (waveshaping, tube), 22 (oversampling), 18 (compressor basics), 17.1-17.5 (comb/Schroeder)
Plugins: IIRFilters, StereoDelay, ModDelay, ModFilter, TubePreamp, Dynamics

**Intermediate**
Beginner path plus 13.6 (phaser), 16 (FIR, convolution)
Plugins: Phaser, AnalogFIR

**Advanced**
Intermediate path plus 11.4-11.5 (Vicanek, impulse invariant), 12.1-12.3 and 12.6-12.7 (WDF, RLCFilters), 17.6-17.12 (Dattorro, FDN), 18.5-18.10 (spectral, parallel, sidechain), 20, 21.1.2
Plugins: RLCFilters, Reverb (FDN), PitchShifter

### By focus

| Focus                   | Order                                          |
| ----------------------- | ---------------------------------------------- |
| Filter / EQ             | 10.23, 11, 12.4-12.5, 12.1-12.3, 12.6-12.7, 16 |
| Time / modulation       | 13, 14, 15, 17                                 |
| Dynamics                | 13.2 (detector), 18                            |
| Saturation / distortion | 19, 22 (oversampling)                          |
| Space / reverb          | 14, 17, 16.9 (convolution reverb)              |
| Spectral                | 16, 20, 22                                     |

**Entry point:** Ch. 2/3 is enough for the first plugin (Gain / Pan). Read 10.23 and Ch. 11 before the Biquad. No path needs to be completed before starting.
