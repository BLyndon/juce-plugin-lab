# CLAUDE.md (zettelkasten)

Rules for working in `docs/zettelkasten/`. They apply in addition to the repository's root `CLAUDE.md`.

## Purpose

The zettelkasten holds ideas the human has no time to implement yet. They are captured in a structured way, linked, and pruned until something valuable remains. It is not a plain idea backlog and not learning material.

Results that grow out of it (for example a structure document for `docs/`) live outside the zettelkasten. The zettel that led to a result links to it.

## Roles and workflow

1. The human drops a raw draft, in chat or as a file in `inbox/`.
2. Claude finalizes it:
   - One idea per zettel. A draft with several ideas is split.
   - The idea stays the human's. Sharpen the wording; do not change the meaning or add ideas. If the meaning is unclear, ask.
   - Name the file, add links to related zettels, and update the structure notes and `structure-notes.md`.
   - If the draft overlaps an existing zettel, propose a merge instead of adding a near-duplicate.
   - Delete the draft from `inbox/` once it is finalized.
3. The human reviews Claude's part.

## Zettel format

```markdown
# <The idea as a statement>

Status: open

<A few sentences in plain English that state the idea so it stands on its own.>

## Links

- [<target heading>](<target file>.md): <why these two are linked>
```

- The heading is a statement, not a topic ("Document purpose determines its place in docs/", not "Documentation").
- The status is a plain text line: `open`, `done`, or `rejected`. A new zettel starts as `open`.
- Every link carries a short reason.
- Links use standard relative Markdown links, so they work in VS Code, on GitHub, and in Notion. No wiki syntax (`[[...]]`), no tags, no front matter.
- Language: English.

## File names

`YYYYMMDDHHMM-short-title.md`, for example `202611011312-docs-keeps-growing.md`.

- The timestamp is the creation time and makes the name unique.
- The short title is lowercase, words separated by hyphens.
- The file name is fixed at creation and never changed, so links never break. The heading inside may evolve.

## Structure notes

A structure note is a normal zettel that serves as a hub for one topic. It lists the zettels of that topic in a meaningful order, each with one line on its role in the chain. Every zettel is reachable from at least one structure note.

`structure-notes.md` lists all structure notes, each with one line on its topic. It does not list single zettels.

`README.md` is the entry point for humans: what the zettelkasten is, how to use it, and a link to `structure-notes.md`. It does not list structure notes or zettels.

## Pruning

- Review happens at the start and at the end of a weekend session, when there is time.
- Claude proposes merges, rewrites, and deletions; the human decides.
- When a zettel is merged or deleted, every link to it is updated or removed in the same change.
- A zettel whose idea has become a result outside the zettelkasten stays, gets status `done`, and links to the result.
- A rejected zettel stays, gets status `rejected`, and states the reason in one line.

## Out of scope

- Do not change documents outside `docs/zettelkasten/` because of a zettel. Implementing an idea is a separate decision by the human.
- No tools, scripts, or generated indexes.
