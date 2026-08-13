---
knowledge-id: styr.manual.localization
status: canonical
locale: en-US
source-locale: en-US
planned-locales: [de-DE, fr-FR, it-IT]
---

# Localization model

## One technical truth

The shared knowledge base remains canonical en-US regardless of how many localized manuals exist. Future German, French, Italian, or other language agents must consume the same product facts and terminology IDs rather than maintain separate technical brains.

This prevents a timing fix, default change, or renamed workflow from diverging between languages.

## Source locale

US English (`en-US`) is the source documentation locale.

Prospective locales explicitly anticipated by the project owner are:

- German (`de-DE`)
- French (`fr-FR`)
- Italian (`it-IT`)

No localized manual or localization agent is created by this file; it defines the contract for adding them later.

## Localization agent rules

A future locale-specific agent should:

1. read this shared knowledge base and the source-locale manual;
2. preserve technical meaning, numbers, units, UI labels, commands, paths, filenames, MIDI/CV terminology, and screenshot identity;
3. translate explanatory prose naturally for the target locale rather than mechanically mirroring English syntax;
4. preserve stable term IDs internally when a terminology mapping is introduced;
5. flag terminology that has no established target-language equivalent instead of inventing one inconsistently;
6. never change product facts solely to make a translation read more naturally;
7. update the complete affected locale bundle when the source manual changes.

## UI-label policy

Until the firmware itself provides localized UI strings, visible Styr labels in prose should normally remain exactly as displayed by the device, with target-language explanation around them. A localization agent must not invent translated on-device labels.

## File-layout direction

The exact multilingual file layout is intentionally deferred. When localization is implemented, choose a structure that keeps language variants mechanically pairable and permits completeness checks between `en-US` and each target locale.
