# Plugin Filesystem (macOS and Linux)

To run the commands as they are, set two shell variables first.

```bash
# macOS
NAME="<name>"; P="$HOME/Library/Audio/Plug-Ins/VST3/$NAME.vst3"
# Linux
NAME="<name>"; P="$HOME/.vst3/$NAME.vst3"
```

## 1. Locations

### macOS

| Format        | User                                   | System (`sudo`)                       |
| ------------- | -------------------------------------- | ------------------------------------- |
| VST3          | `~/Library/Audio/Plug-Ins/VST3/`       | `/Library/Audio/Plug-Ins/VST3/`       |
| AU            | `~/Library/Audio/Plug-Ins/Components/` | `/Library/Audio/Plug-Ins/Components/` |
| VST2 (legacy) | `~/Library/Audio/Plug-Ins/VST/`        | `/Library/Audio/Plug-Ins/VST/`        |
| CLAP          | `~/Library/Audio/Plug-Ins/CLAP/`       | `/Library/Audio/Plug-Ins/CLAP/`       |
| Standalone    | `.app`, anywhere                       |                                       |

### Linux

| Format        | User                 | System (`sudo`)                          |
| ------------- | -------------------- | ---------------------------------------- |
| VST3          | `~/.vst3/`           | `/usr/lib/vst3/`, `/usr/local/lib/vst3/` |
| LV2           | `~/.lv2/`            | `/usr/lib/lv2/`, `/usr/local/lib/lv2/`   |
| VST2 (legacy) | `~/.vst/`            | `/usr/lib/vst/`, `/usr/local/lib/vst/`   |
| CLAP          | `~/.clap/`           | `/usr/lib/clap/`                         |
| Standalone    | executable, anywhere |                                          |

Hosts scan these folders. There is no registry and no install step.

## 2. Build to install

With `COPY_PLUGIN_AFTER_BUILD TRUE`, JUCE copies after every build:

```text
<build dir>/…/<Name>_artefacts/<Config>/VST3/<Name>.vst3
        │
        ▼
~/Library/Audio/Plug-Ins/VST3/<Name>.vst3   (macOS)
~/.vst3/<Name>.vst3                         (Linux)
```

The last build wins. Debug and Release overwrite each other.

## 3. VST3 bundle layout

### macOS

```text
<Name>.vst3/Contents/
├── Info.plist                 bundle ID, version, name
├── MacOS/<Name>               binary
├── Resources/moduleinfo.json  VST3 metadata
└── _CodeSignature/            signature (ad-hoc is enough locally)
```

### Linux

```text
<Name>.vst3/Contents/
├── x86_64-linux/<Name>.so     binary (aarch64-linux/ on ARM)
└── Resources/moduleinfo.json  VST3 metadata
```

## 4. Inspect

### Both

```bash
find "$P"                                                       # structure
python3 -m json.tool "$P/Contents/Resources/moduleinfo.json"    # class IDs, categories, vendor
file "$P"/Contents/*/*                                          # binary type and architecture
```

### macOS

```bash
ls -la ~/Library/Audio/Plug-Ins/VST3 /Library/Audio/Plug-Ins/VST3
plutil -p "$P/Contents/Info.plist"          # name, bundle ID, version
lipo -archs "$P/Contents/MacOS/$NAME"       # arm64 / x86_64
codesign -dv "$P"                           # must be signed to load on Apple Silicon
stat -f "%Sm  %N" "$P"                      # install time
open -R "$P"                                # reveal in Finder
```

### Linux

```bash
ls -la ~/.vst3 /usr/lib/vst3 /usr/local/lib/vst3
stat -c "%y  %n" "$P"                                        # install time
ldd "$P"/Contents/*-linux/*.so | grep "not found"            # missing shared libraries
nm -D "$P"/Contents/*-linux/*.so | grep -i GetPluginFactory  # VST3 entry point exported
```

## 5. Add and remove

- Own plugin: build it.
- Third-party plugin: copy the bundle into the matching folder.
  - macOS: `xattr -l "$P"` shows quarantine. Clear it only for trusted plugins.
  - Linux: check `ldd` if it does not load.
- Remove (recoverable):

```bash
mv "$P" ~/.Trash/    # macOS
gio trash "$P"       # Linux
```

## 6. Host does not see the change

1. Check the install time. Did the copy step run?
2. Rescan in the host. Hosts cache scan results.
3. Check the host's blacklist. A plugin that crashed during a scan stays blacklisted after it is fixed.
4. Run pluginval on the installed bundle. That is what the host loads.
