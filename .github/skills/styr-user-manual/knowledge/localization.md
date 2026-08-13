---
knowledge-id: styr.manual.localization
status: canonical
locale: en-US
source-locale: en-US
active-locales: [en-US]
locale-expansion: open
---

# Localization model

## One technical truth

The shared knowledge base has one canonical technical meaning. US English (`en-US`) is the current source and only active documentation locale.

The localization architecture must remain open to additional locales without predicting which languages will be added. A future locale may use Latin, Cyrillic, Greek, CJK, RTL, or other writing systems and may have grammatical or typographic requirements that differ substantially from English.

Do not encode an assumed finite language list into the product knowledge, manual IDs, screenshot IDs, or publication model.

## Source locale

Current state:

```text
source locale: en-US
active locales: en-US
future locale set: open
```

No localized manual or localization agent is created by this contract.

## Future localization contract

When a locale is introduced, its agent/process must:

1. consume the same canonical product knowledge and the approved source-locale manual;
2. preserve stable documentation IDs so source and localized sections remain mechanically pairable;
3. preserve technical meaning, numbers, units, UI labels, commands, paths, filenames, MIDI/CV terminology, and screenshot identity unless a future localized UI explicitly changes visible labels;
4. produce natural target-language prose rather than mirror English syntax mechanically;
5. maintain a locale-specific terminology map keyed by canonical term IDs rather than fork the technical knowledge base;
6. flag unresolved terminology instead of inventing inconsistent translations;
7. update only source sections that changed in incremental mode, while allowing a complete locale bootstrap/regeneration when a language is first introduced;
8. respect target-language typography, punctuation, segmentation, pluralization, reading direction, and line-breaking requirements rather than assume English layout rules.

## UI-label policy

Until Styr itself provides localized UI strings, visible on-device labels in localized prose should normally remain exactly as displayed by the device, with target-language explanation around them. A localization agent must not invent translated on-device labels.

## File-layout direction

The exact multilingual file layout is intentionally deferred until a second locale is implemented. The chosen structure must support:

- stable pairing by `doc_id`;
- independent locale completeness checks;
- incremental propagation from the source locale;
- full bootstrap/regeneration for a newly introduced locale;
- locale-neutral screenshots where the device UI itself is not localized;
- locale-specific publication artifacts.
