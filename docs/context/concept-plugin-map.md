# Concept-Plugin Map

One main concept per plugin; the order may change. `*` = optional.
Pirkle sections refer to `pirkle-theory-reference.md`.

| #   | Plugin              | Concept                                 | Pirkle               | Reference (Phase 0)                 | Listen for                                    |
| --- | ------------------- | --------------------------------------- | -------------------- | ----------------------------------- | --------------------------------------------- |
| 0   | Template            | Toolchain, build chain, copy source     | Ch. 2, 3             | —                                   | —                                             |
| 1   | Gain / Pan          | Parameters, smoothing, lifecycle        | Ch. 2                | Any free utility plugin             | Pan law: center vs. hard left/right           |
| 2   | Tremolo / Auto-Pan  | LFO as modulation source                | 13.1                 | MFreeFXBundle, Airwindows           | How rate, depth, waveform shape the character |
| 3   | Biquad (LP/HP/peak) | H(z) to coefficients, per-channel state | 10.23, 11            | AUParametricEQ, AULowpass, AUHipass | Frequency and Q vs. the magnitude curve       |
| 3\* | Phaser              | Allpass chain + LFO                     | 13.6                 | MFreeFXBundle                       | Allpass stages vs. notches; feedback          |
| 4   | Delay               | Circular buffer, feedback               | 14                   | AUDelay                             | Feedback toward 100 %: decay, stability       |
| 4\* | Ping-Pong Delay     | Stereo routing, cross-feedback          | 14.4                 | MFreeFXBundle                       | Repeats moving between channels               |
| 5   | Chorus / Flanger    | Modulated, fractional delay             | 15                   | MFreeFXBundle                       | Where chorus turns into flanger               |
| 6   | Auto-Wah            | Envelope detector + SVF                 | 12.4-12.5, 13.2-13.5 | Airwindows, MFreeFXBundle           | Cutoff following dynamics; sweep smoothness   |
| 7   | Distortion (tanh)   | Nonlinearity, aliasing, oversampling    | 19, 22               | AUDistortion                        | Harmonics on a sine; aliasing on a high sine  |
| 7\* | Bitcrusher          | Aliasing used deliberately              | 19.4                 | Airwindows                          | Bit depth vs. sample rate reduction           |
| 8   | Compressor          | Gain curve, knee, attack/release        | 18                   | AUDynamicsProcessor                 | Attack/release on a drum loop; knee           |
| 9   | Schroeder reverb    | Comb and allpass filters                | 17.1-17.5            | AUMatrixReverb, Dragonfly Reverb    | Metallic (comb) vs. smooth; decay, density    |

Use a spectrum analyzer (e.g. Voxengo SPAN) after the plugin for filters and distortion.
Apple AUs are listening references only. Get third-party plugins from the vendor's site and check Apple Silicon support first.

Apple AUs come with macOS, so there is nothing to install. In AudioPluginHost they are listed under the manufacturer "Apple" and their names start with "AU". To list them in Terminal, run `auval -a`; Apple's entries have the manufacturer code `appl` (for example `aufx dely appl - Apple: AUDelay`).
