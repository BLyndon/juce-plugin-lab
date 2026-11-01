# CLAUDE.md (docs)

Rules for working in `docs/`. They apply in addition to the repository's root `CLAUDE.md`.

## Document types and places

The purpose of a document decides where it lives, not its topic or the plugin it came from.

| Place           | Type               | Purpose                                                            | Rules                      |
| --------------- | ------------------ | ------------------------------------------------------------------ | -------------------------- |
| `theory/`       | Companion document | Explains one transferable DSP concept, using a plugin as example   | `theory/CLAUDE.md`         |
| `guides/`       | Guide              | Steps to get one thing done (build the host, run a listening test) | this file                  |
| `toolchain/`    | Toolchain document | Explains or lists how the build and the tools work                 | this file                  |
| `context/`      | Agent reference    | Reference material for the coding agent                            | own form, not covered here |
| `zettelkasten/` | Zettel             | The human's ideas not implemented yet                              | `zettelkasten/CLAUDE.md`   |

- A new document gets its type first. If no type fits, ask the human instead of adding a place.
- `README.md` is the index of all documents (see below).

## Requirements for guides and toolchain documents

These documents have no fixed layout, because steps, explanations, and lookup tables need different forms. Every one of them meets these requirements:

- The first sentence says what the document is for and when it is needed.
- Prerequisites are stated, if there are any (for example "the host is built").
- Commands can be copied as they are and run from the repository root. Placeholders are marked as `<name>`.
- Steps that change something end with a check that shows it worked.
- No term is used without an introduction. DSP terms link to the companion document that introduces them.
- Diagrams are Mermaid blocks.

## Index

`README.md` lists every document with one line on its content, grouped by place. Companion documents are listed as a learning path, in the plugin order of `context/concept-plugin-map.md`. Update the index in the same change that adds, moves, or renames a document.

## Migration

The documents that still lie flat in `docs/` predate this structure. The human reviews each one before it moves. Do not move or rewrite them unless asked.

| Document               | Target       |
| ---------------------- | ------------ |
| `audiopluginhost.md`   | `guides/`    |
| `test-audio.md`        | `guides/`    |
| `build-chain.md`       | `toolchain/` |
| `cmake-and-ninja.md`   | `toolchain/` |
| `plugin-filesystem.md` | `toolchain/` |

When a document moves, update every link to it in the same change: `README.md` at the root, `docs/README.md`, other documents, `CLAUDE.md`, and `context/`. Images move with the document that uses them. Remove its row here once it is done.

## Later

Publishing `docs/` as GitHub Pages is a possible later stage, not planned yet: [zettel](zettelkasten/202611011448-docs-as-github-pages.md).
