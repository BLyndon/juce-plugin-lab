# Plugins

## New plugin from the template

From the repository root (`<name>` = the new directory name: lowercase letters, digits, hyphens; it becomes the target name and the end of the bundle id):

```bash
cp -R plugins/template plugins/<name>
```

### Metadata to change

| File                       | What                                                                                                                             |
| -------------------------- | -------------------------------------------------------------------------------------------------------------------------------- |
| `CMakeLists.txt`           | `PLUGIN_CODE`: four characters, at least one upper-case, unique in this repository (`grep PLUGIN_CODE plugins/*/CMakeLists.txt`) |
| `CMakeLists.txt`           | `PRODUCT_NAME`: the name shown in the host                                                                                       |
| `test/PluginBasics.cpp`    | Expected name: same string as `PRODUCT_NAME`                                                                                     |
| `spec.md`                  | Rewrite for the new plugin                                                                                                       |
| `test/ImpulseResponse.cpp` | Expected values from the new `spec.md`                                                                                           |

Derived from the directory name, nothing to change: the targets (`<name>_VST3`, `<name>_Tests`, `<name>_pluginval`) and `BUNDLE_ID` (`com.blyndon.<name>`). Shared by all plugins and set in the root `CMakeLists.txt`: `COMPANY_NAME`, `PLUGIN_MANUFACTURER_CODE`.

### Build integration

Add one line at the end of the root `CMakeLists.txt`:

```cmake
add_subdirectory(plugins/<name>)
```

### Host

A new plugin needs one new scan in AudioPluginHost (see `docs/audiopluginhost.md`).
