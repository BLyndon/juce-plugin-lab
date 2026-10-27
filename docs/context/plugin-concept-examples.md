# Plugin Examples per Concept

Defines the numbered plugin sequence (single source of truth) plus optional additions and the reference plugins for the Explore phase. Pirkle references point into `pirkle-theory-reference.md`.

Each plugin has one main new concept. Concepts reused from earlier plugins and small side concepts come along; the sequence does not force exactly one. The order can change. Detailed planning covers the next two or three plugins; the rest stays rough.

## Sequence

| #   | Plugin                                               | Main concept                                                              | Why it teaches it well                                                                                                                                            | Pirkle               |
| --- | ---------------------------------------------------- | ------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------- | -------------------- |
| 0   | Empty plugin (Session 0b; Session 0a is repo and CI) | Toolchain, build chain                                                    | Whole chain once, no DSP noise                                                                                                                                    | Ch. 2, 3             |
| 1   | Gain / Pan                                           | Parameters, smoothing, lifecycle                                          | Minimal DSP; zipper noise and smoothing become isolated and audible                                                                                               | Ch. 2                |
| 2   | Tremolo / Auto-Pan                                   | LFO as modulation source                                                  | Modulation without delay complexity; the LFO is introduced before it is combined with fractional delay                                                            | 13.1                 |
| 3   | Biquad (LP/HP/peak)                                  | H(z) to coefficients, per-channel state                                   | Known theory meets implementation; shared state across channels is measurable                                                                                     | 10.23, 11            |
| 4   | Delay (feedback, wet/dry)                            | Circular buffer, state across blocks                                      | The ring buffer is the key idea; tail time and feedback stability come with it                                                                                    | 14                   |
| 5   | Chorus / Flanger                                     | Modulated delay, fractional delay (LFO reused from #2)                    | Shows why integer delay is not enough                                                                                                                             | 15                   |
| 6   | Auto-Wah (envelope filter)                           | Envelope detector driving a modulated filter (state-variable filter, SVF) | The detector is introduced before the compressor builds on it. The SVF shows why a filter whose frequency moves is built differently from the static biquad of #3 | 12.4-12.5, 13.2-13.5 |
| 7   | Distortion (tanh waveshaper)                         | Nonlinearity, aliasing, oversampling                                      | A/B with and without oversampling: hear and measure the aliasing                                                                                                  | 19, 22               |
| 8   | Compressor                                           | Gain curve, knee, attack/release (detector reused from #6)                | Time constants and knee in one place                                                                                                                              | 18                   |
| 9   | Schroeder reverb                                     | Comb and allpass filters                                                  | Density emerges from simple building blocks                                                                                                                       | 17.1-17.5            |

## Optional additions (not numbered)

| Plugin          | Insert after  | Adds                                                     | Pirkle |
| --------------- | ------------- | -------------------------------------------------------- | ------ |
| Phaser          | #3 Biquad     | Allpass chain + LFO; links filter and modulation strands | 13.6   |
| Ping-Pong Delay | #4 Delay      | Stereo routing, cross-feedback                           | 14.4   |
| Bitcrusher      | #7 Distortion | Aliasing used deliberately; easy to hear                 | 19.4   |

## Explore phase: references and listening notes

Used in Phase 0 (`workflow-reference.md` §1). Play the reference in AudioPluginHost on the fixed test audio, with the question in "Listen for". Candidates are starting points; check current availability and macOS/Apple Silicon support before installing, and download only from the vendor's site.

Reference sources:

- **Apple AUs** (built into macOS, listening reference only): AUDelay, AUDistortion, AUDynamicsProcessor, AUParametricEQ, AUMatrixReverb.
- **MeldaProduction MFreeFXBundle** (free VST3s): chorus, flanger, phaser, tremolo and more.
- **Airwindows** (free VST3 collection, readable source): many simple effects, incl. distortion and bitcrushing variants.
- **Dragonfly Reverb** (free VST3): reverb.
- **A free spectrum analyzer** (for example Voxengo SPAN) after the plugin under test, to see filter curves and aliasing.

| Plugin              | Reference                             | Listen for                                                                                      |
| ------------------- | ------------------------------------- | ----------------------------------------------------------------------------------------------- |
| Gain / Pan          | Any free utility plugin               | Pan law: level in the center vs. hard left/right                                                |
| Tremolo / Auto-Pan  | MFreeFXBundle, Airwindows             | How rate, depth, and waveform shape the character                                               |
| Biquad (LP/HP/peak) | AUParametricEQ + analyzer             | Change frequency and Q, watch the magnitude curve; compare LP, HP, peak                         |
| Phaser              | MFreeFXBundle                         | Number of allpass stages vs. number of notches; feedback effect                                 |
| Delay               | AUDelay                               | Feedback toward 100 %: decay, stability; wet/dry balance                                        |
| Ping-Pong Delay     | MFreeFXBundle or similar              | How cross-feedback moves repeats between channels                                               |
| Chorus / Flanger    | MFreeFXBundle                         | Short vs. longer delay time, feedback on/off: where chorus turns into flanger                   |
| Auto-Wah            | Airwindows, MFreeFXBundle, or similar | How filter frequency follows playing dynamics; how smooth the cutoff sweep is while audio plays |
| Distortion          | AUDistortion + analyzer               | Sine in, drive up: see harmonics; high sine: see aliasing                                       |
| Bitcrusher          | Airwindows                            | Reduced bit depth vs. reduced sample rate; audible aliasing                                     |
| Compressor          | AUDynamicsProcessor                   | Attack/release on a drum loop, knee, gain reduction                                             |
| Reverb              | AUMatrixReverb, Dragonfly Reverb      | Decay, pre-delay, density: when it sounds metallic (comb) vs. smooth                            |
