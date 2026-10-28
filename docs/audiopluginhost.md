# AudioPluginHost: build, start, load plugins

AudioPluginHost is JUCE's test host (`JUCE/extras/AudioPluginHost`). It is built once and reused for all plugins in this repository and for the reference plugins. All commands run from the repository root.

## Build (once, or after a JUCE submodule update)

```bash
cmake -S JUCE -B Builds/AudioPluginHost -G Ninja -DCMAKE_BUILD_TYPE=Release -DJUCE_BUILD_EXTRAS=ON
cmake --build Builds/AudioPluginHost --target AudioPluginHost
```

- Separate build tree from the plugins; `Builds/` is git-ignored. Deleting `Builds/` also deletes the host.
- The warning "Couldn't find actool" is expected with Command Line Tools only: the app has no icon, nothing else is affected.

## Start

```bash
open Builds/AudioPluginHost/extras/AudioPluginHost/AudioPluginHost_artefacts/Release/AudioPluginHost.app
```

On first start, macOS may ask for microphone access (needed for the Audio Input node).

## Make plugins known to the host (once, and after adding a plugin)

1. **Options → Edit the List of Available Plug-ins...**
2. In that window: **Options... → Scan for new or updated VST3 plug-ins**.
3. Check that the scan paths include `~/Library/Audio/Plug-Ins/VST3`, then start the scan.
4. Own plugins appear with the product name from their `CMakeLists.txt` (for example **Empty**). Failed plugins are listed at the end of the scan.
5. For Apple's built-in effects (AUDelay, AUParametricEQ, ...; listening references only): **Options... → Scan for new or updated AudioUnit plug-ins**. AUs have no scan path; macOS registers them centrally.

Our plugins are copied to `~/Library/Audio/Plug-Ins/VST3` after each build, so a rebuilt plugin needs no new scan; a new plugin does.

## Load a plugin into the graph

1. Right-click an empty area of the graph and choose the plugin.
2. Connect by dragging from an output pin (bottom of a node) to an input pin (top of the next node): **Audio Input → plugin → Audio Output**. If the input or output node is missing, add it from the same right-click menu.
3. Double-click the plugin node to open its editor. Right-click the node for more: **Show all parameters**, **Toggle Bypass**, **Test state save/load**, **Delete this filter**.
4. **Options → Change the Audio Device Settings** selects the audio interface, sample rate, and buffer size.
5. **File → Save** stores the graph as a file, so the same setup can be reopened for the next session.

Use headphones when Audio Input is routed to the speakers: a microphone input feeds back.

## Rebuilt a plugin while the host is open

A loaded plugin binary is not reloaded while the host runs. Close the host, rebuild, start the host again, and reopen the saved graph.
