# Test audio for listening in the host

A fixed set of files, so reference plugins and own builds are always compared on the same sources. The files live outside the repository in `~/Music/plugin-lab-test-audio/` (no binaries or licences in git).

## Files

All 48 kHz, 24-bit, stereo (both channels identical), 10 s.

| File                   | Content                | Level           | Listen for                 |
| ---------------------- | ---------------------- | --------------- | -------------------------- |
| `sine-1k-18dBFS.wav`   | 1 kHz sine             | RMS −18 dBFS    | Gain, distortion, panning  |
| `pink-noise.wav`       | Pink noise             | Peak −13.5 dBFS | Filter shape, modulation   |
| `white-noise.wav`      | White noise            | Peak −18 dBFS   | Filter shape, top end      |
| `sweep-20-20k-10s.wav` | Log sweep 20 Hz–20 kHz | Peak −12 dBFS   | Resonances, aliasing       |
| `clicks-1s.wav`        | One click per second   | Peak −6 dBFS    | Delay repeats, decay tail  |
| _drum loop_            | To be added (download) |                 | Transients, timing         |
| _guitar DI_            | To be added (download) |                 | Realistic program material |

The synthetic files are generated with ffmpeg:

```bash
mkdir -p ~/Music/plugin-lab-test-audio && cd ~/Music/plugin-lab-test-audio
F=(-af "pan=stereo|c0=c0|c1=c0" -ar 48000 -c:a pcm_s24le -y)
ffmpeg -f lavfi -i "sine=frequency=1000:sample_rate=48000:duration=10,volume=3dB" $F sine-1k-18dBFS.wav
ffmpeg -f lavfi -i "anoisesrc=color=pink:amplitude=0.25:duration=10:sample_rate=48000" $F pink-noise.wav
ffmpeg -f lavfi -i "anoisesrc=color=white:amplitude=0.125:duration=10:sample_rate=48000" $F white-noise.wav
ffmpeg -f lavfi -i "aevalsrc=0.25*sin(2*PI*20*10/log(1000)*(exp(t/10*log(1000))-1)):s=48000:d=10" $F sweep-20-20k-10s.wav
ffmpeg -f lavfi -i "aevalsrc=if(lt(mod(n\,48000)\,1)\,0.5\,0):s=48000:d=10" $F clicks-1s.wav
```

(zsh syntax: `F` is an array. `pan` copies mono to both channels without the −3 dB that `-ac 2` applies.)

## Running a listening test

AudioPluginHost cannot play files; it only takes audio from a sound-card input. A loopback device (BlackHole 2ch, `brew install --cask blackhole-2ch`) acts as a virtual sound card: what is played into it comes out of its input.

1. **Options → Change the Audio Device Settings**: Input = **BlackHole 2ch** (channels 1+2 active), Output = headphones directly (not routed through a system EQ or other audio tool), 48 kHz.
2. Graph: **Audio Input → plugin → Audio Output**, with no direct **Audio Input → Audio Output** connection (it would mask every change).
3. Turn the headphone volume down. Play a file in a loop into BlackHole; `<N>` is the index listed as **BlackHole 2ch** (first command). Stop with Ctrl+C.

```bash
ffmpeg -f lavfi -i "anullsrc=d=0.1" -f audiotoolbox -list_devices true - 2>&1 | grep '\['
ffmpeg -re -stream_loop -1 -i ~/Music/plugin-lab-test-audio/pink-noise.wav -f audiotoolbox -audio_device_index <N> -
```

4. Check the routing: quitting the host or removing **plugin → Audio Output** must give silence. If not, ffmpeg plays to the wrong device or a second path bypasses the plugin.
5. Sanity check with a known gain plugin on `sine-1k-18dBFS.wav`: −20 dB clearly quieter, +6 dB clearly louder (clips from about +15 dB; expected).
6. Listen: file per "Listen for" column, one parameter at a time, A/B with **Toggle Bypass**; exact values via right-click → **Show all parameters**.
