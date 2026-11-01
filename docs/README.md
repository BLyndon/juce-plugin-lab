# Docs

Companion documents with theory, guides, and toolchain documents. Where each type lives and what it must meet: [CLAUDE.md](CLAUDE.md).

Documents in _Not yet migrated_ still lie flat in `docs/` and move to their place after review.

## Theory (learning path)

Companion documents, in plugin order. Each explains one transferable DSP concept with a plugin as the example.

1. [decibels-and-parameter-smoothing.md](theory/decibels-and-parameter-smoothing.md): decibel scale, parameter scaling, smoothing, constant factor (plugin `gain`)

## Guides

_None yet._

## Toolchain

_None yet._

## Not yet migrated

| Document                                     | Content                                                                | Target       |
| -------------------------------------------- | ---------------------------------------------------------------------- | ------------ |
| [stereo-panning.md](stereo-panning.md)       | Pan laws, mono/balance/true stereo pan, width and position             | `theory/`    |
| [audiopluginhost.md](audiopluginhost.md)     | Build, start, and load plugins into JUCE's AudioPluginHost             | `guides/`    |
| [test-audio.md](test-audio.md)               | Test audio files and how to run a listening test in the host           | `guides/`    |
| [build-chain.md](build-chain.md)             | The path from source to sound: configure, build, test, CI, host        | `toolchain/` |
| [cmake-and-ninja.md](cmake-and-ninja.md)     | What CMake and Ninja do, compared with Maven                           | `toolchain/` |
| [plugin-filesystem.md](plugin-filesystem.md) | Where plugins live on macOS and Linux, bundle layout, inspect commands | `toolchain/` |

## Agent references and ideas

| Document                                                         | Content                                                            |
| ---------------------------------------------------------------- | ------------------------------------------------------------------ |
| [toolstack-reference.md](context/toolstack-reference.md)         | Toolchain decisions, build chain, standard commands, failure map   |
| [workflow-reference.md](context/workflow-reference.md)           | Session routine, spec template, review checklist                   |
| [concept-plugin-map.md](context/concept-plugin-map.md)           | Plugin sequence, concepts, reference plugins                       |
| [pirkle-theory-reference.md](context/pirkle-theory-reference.md) | DSP theory map for Pirkle, "Designing Audio Effect Plugins in C++" |
| [zettelkasten/](zettelkasten/README.md)                          | Ideas not implemented yet, linked and pruned                       |
