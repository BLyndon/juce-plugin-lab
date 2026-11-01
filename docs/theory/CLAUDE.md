# CLAUDE.md (theory)

Rules for companion documents in `docs/theory/`. They apply in addition to `docs/CLAUDE.md` and the repository's root `CLAUDE.md`.

## Purpose

A companion document explains the theory behind a plugin. It usually grows out of one plugin and is tailored to it at first, but its subject is the concept, not the plugin. The plugin is the worked example.

## Scope and length

- One concept, or a small group of concepts that only make sense together. The file name names the concept (`stereo-panning.md`, not `stereo-pan.md`).
- 1–2 printed A4 pages. If a document needs more, split it into two concepts.
- Concise means short overall, not telegraphic. Explanations are full sentences; lists are for enumerations, the header, and the summary.

## Layout

```markdown
# <Concept>

<One or two sentences: what the concept is, and which plugin serves as the example.>

Builds on: <links to companion documents whose concepts are used here, or "nothing">

## What you will learn

- <one bullet per learning goal; each goal is one section below>

## Key concepts

- **<Concept>:** <one line>. <Marked as transferable where it applies beyond this plugin.>

## Symbols

| Symbol | Meaning | Unit |
| ------ | ------- | ---- |

## <One section per learning goal>

<The general principle, then a paragraph that applies it to the plugin:>

**<plugin>:** <how the plugin uses it>

## Measuring (optional)

<How the expected values in the plugin's tests follow from the theory.>

## Summary

- <one bullet per core statement: its central equation and the physical or perceptual reason behind it; nothing that the sections above do not explain>

## References

- <Pirkle chapter and section, JUCE classes, the plugin's spec.md and test>
```

## Writing rules

- **Math first.** Whatever can be stated as an equation is stated as one, in LaTeX math (`$…$` inline, `$$…$$` as a block). The reader prefers equations to prose. Text says what the equation does not: assumptions, meaning, consequences.
- **Every quantity is defined before it is used:** symbol, meaning, and unit in the symbols table. Name each quantity explicitly; no shorthand like "dB → linear".
- **Nothing appears without introduction.** A concept from another companion document is introduced briefly in place, as in a paper: definition, central equation, symbols. The derivation and the full motivation stay in one document, and the others link to it.
- **Transferable concepts are named as such.** Say why the plugin needs the concept, and briefly where else it is used (other plugins, other parts of audio DSP). Plugin-specific details go into the `**<plugin>:**` paragraphs.
- **References are exact:** chapter and section, class name, file path. List only sources the document actually uses.
- Diagrams are Mermaid blocks.

## Relation to the spec

`plugins/<name>/spec.md` states what the plugin does and which values the tests check. The companion document explains why. When theory accumulates in a spec, it belongs here. The spec belongs to the human: propose the move, do not edit the spec unasked.
