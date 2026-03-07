# Configuration Guide

DIII uses JSON rule files to define which icons appear on which inventory items. Place your config files in:

```
Data/SKSE/Plugins/DIII/*.json
```

All `.json` files in this folder are loaded automatically on game startup. You can split rules across multiple files for organization.

## Quick Start

```json
{
  "$schema": "./schema.json",
  "rules": [
    {
      "icon": {
        "source": "DIII/myIcons.swf",
        "label": "QuestItemIcon"
      },
      "match": {
        "isQuestItem": true
      }
    }
  ]
}
```

This shows the `QuestItemIcon` export from `DIII/myIcons.swf` on every quest item in the inventory.

---

## Rule Structure

Each rule has two parts:

| Key | Description |
|---|---|
| `icon` | Which icon to display |
| `match` | Conditions the item must satisfy (AND logic) |

### Icon

| Field | Required | Description |
|---|---|---|
| `source` | ✅ | SWF file path relative to `Interface/` |
| `label` | ✅ | Export name inside the SWF |

### Match

All conditions within a single rule use **AND** logic — every condition must pass for the icon to show. Multiple rules can match the same item; all matching icons are displayed.

---

## Match Fields

### Common (All Items)

| Field | Type | Example | Description |
|---|---|---|---|
| `formType` | string | `"Weapon"` | Item category |
| `formId` | string / array | `"Skyrim.esm\|0x12EB7"` | Specific form(s) |
| `keywords` | string / array | `["VendorItemWeapon"]` | EditorID keywords (array = must have ALL) |
| `isStolen` | bool | `true` | Owned by someone other than the player |
| `isFavorited` | bool | `true` | Marked as favorite |
| `isQuestItem` | bool | `true` | Part of an active quest |
| `goldValue` | number / range | `{"min": 100}` | Item value in gold |
| `weight` | number / range | `{"max": 5.0}` | Item weight |

**Supported `formType` values:**
`Ammo`, `Armor`, `Book`, `Ingredient`, `Key`, `MiscItem`, `Potion`, `Scroll`, `SoulGem`, `Weapon`

### Numbers and Ranges

Numeric fields accept an exact value or a range object:

```json
"goldValue": 100
"goldValue": { "min": 50, "max": 200 }
"goldValue": { "min": 100 }
"goldValue": { "max": 500 }
```

### FormID Format

FormIDs use the format `"PluginName|0xRawID"`:

```json
"formId": "Skyrim.esm|0x12EB7"
"formId": ["Skyrim.esm|0x12EB7", "Dragonborn.esm|0x01CDB9"]
```

The plugin name is resolved at runtime via the load order.

---

### Weapon Fields

| Field | Type | Example |
|---|---|---|
| `weaponType` | string | `"OneHandSword"` |
| `baseDamage` | number / range | `{"min": 10}` |
| `speed` | number / range | `0.75` |
| `reach` | number / range | `{"min": 1.0}` |
| `stagger` | number / range | `0.5` |
| `critDamage` | number / range | `{"min": 5}` |

**Weapon types:**
`HandToHandMelee`, `OneHandSword`, `OneHandDagger`, `OneHandAxe`, `OneHandMace`, `TwoHandSword`, `TwoHandAxe`, `Bow`, `Staff`, `Crossbow`

### Armor Fields

| Field | Type | Example |
|---|---|---|
| `weightClass` | string | `"Heavy"` |
| `armorRating` | number / range | `{"min": 20}` |
| `mainPart` | integer (30–61) | `32` |

**Weight classes:** `Light`, `Heavy`, `Clothing`

**All slot numbers:**
* 32 - Body
* 31 - Hair
* 33 - Hands
* 34 - Forearms
* 37 - Feet
* 38 - Calves
* 39 - Shield
* 35 - Amulet
* 36 - Ring
* 41 - Long Hair
* 43 - Ears
* 30 - Head
* 42 - Circlet
* 40 - Tail
* 44 - Unnamed
* 45 - Unnamed
* 46 - Unnamed
* 47 - Unnamed
* 48 - Unnamed
* 49 - Unnamed
* 50 - Decapitate Head
* 51 - Decapitate
* 52 - Unnamed
* 53 - Unnamed
* 54 - Unnamed
* 55 - Unnamed
* 56 - Unnamed
* 57 - Unnamed
* 58 - Unnamed
* 59 - Unnamed
* 60 - Unnamed
* 61 - FX01

### Potion Fields

| Field | Type | Example |
|---|---|---|
| `potionType` | string | `"Poison"` |

**Potion types:** `Food`, `Poison`, `Medicine`

### Soul Gem Fields

| Field | Type | Example |
|---|---|---|
| `gemSize` | string | `"Grand"` |
| `soulSize` | string | `"Greater"` |

`gemSize` checks the gem's maximum capacity. `soulSize` checks the currently contained soul (via ExtraSoul data).

**Soul levels:** `None`, `Petty`, `Lesser`, `Common`, `Greater`, `Grand`

### Book Fields

| Field | Type | Example |
|---|---|---|
| `teachesSpell` | bool | `true` |
| `teachesSkill` | string | `"Enchanting"` |
| `isRead` | bool | `false` |

**Skills:** `OneHanded`, `TwoHanded`, `Marksman`, `Block`, `Smithing`, `HeavyArmor`, `LightArmor`, `Pickpocket`, `Lockpicking`, `Sneak`, `Alchemy`, `Speechcraft`, `Alteration`, `Conjuration`, `Destruction`, `Illusion`, `Restoration`, `Enchanting`

---

## Object-Field Conditions

These match against extra data attached to items at runtime.

### Enchantment

Matches items with an enchantment. Pass `{}` to match any enchanted item, or specify effect matchers:

```json
"enchantment": {}
"enchantment": {
  "school": "Destruction",
  "primaryValue": "Health"
}
```

Checks ExtraEnchantment first (player-applied), then falls back to the base form enchantment.

### Poison

Matches items with an applied poison:

```json
"poison": {}
"poison": { "school": "Restoration" }
```

### Magic Effect

use for potions and scrolls where you want to match their inherent effects:

```json
"magicEffect": { "archetype": "ValueModifier", "primaryValue": "Health" }
```

### Tempered

Matches items with tempering (ExtraHealth):

```json
"tempered": { "min": 1.5 }
```

---

## Effect Matchers

Used inside `enchantment`, `poison`, and `magicEffect`. All matchers use **AND** logic — the condition passes if **any single effect** on the item satisfies **all** specified matchers.

| Field | Type | Description |
|---|---|---|
| `effectId` | FormID | Specific magic effect form |
| `school` | string | `Alteration`, `Conjuration`, `Destruction`, `Illusion`, `Restoration` |
| `archetype` | string | Effect archetype (see list below) |
| `deliveryType` | string | `Self`, `Touch`, `Aimed`, `TargetActor`, `TargetLocation` |
| `castType` | string | `ConstantEffect`, `FireAndForget`, `Concentration`, `Scroll` |
| `primaryValue` | string | Actor value affected (e.g. `Health`, `Stamina`) |
| `resistance` | string | Resistance type (e.g. `FireResist`, `MagicResist`) |
| `magnitude` | number / range | Effect magnitude |
| `duration` | number / range | Effect duration in seconds |
| `area` | number / range | Effect area |
| `skillLevel` | number / range | Minimum skill level |
| `effectFlags` | string / array | Effect flags like `"Hostile"`, `"Detrimental"` |
| `effectKeywords` | string | Keyword EditorID on the base effect |

<details>
<summary><b>All archetype values</b></summary>

`ValueModifier`, `Script`, `Dispel`, `CureDisease`, `Absorb`, `DualValueModifier`, `Calm`, `Demoralize`, `Frenzy`, `Disarm`, `CommandSummoned`, `Invisibility`, `Light`, `Lock`, `Open`, `BoundWeapon`, `SummonCreature`, `DetectLife`, `Telekinesis`, `Paralysis`, `Reanimate`, `SoulTrap`, `TurnUndead`, `Guide`, `Cloak`, `SlowTime`, `Rally`, `EnhanceWeapon`, `SpawnHazard`, `Etherealize`, `Banish`, `Disguise`, `PeakValueModifier`

</details>

<details>
<summary><b>All effect flag values</b></summary>

`Hostile`, `Recover`, `Detrimental`, `NoDuration`, `NoArea`, `FXPersist`, `GoryVisuals`, `HideInUI`, `Painless`, `NoHitEffect`, `NoDeathDispel`, `PowerAffectsMagnitude`, `PowerAffectsDuration`

</details>

<details>
<summary><b>All resistance values</b></summary>

`DamageResist`, `PoisonResist`, `FireResist`, `ElectricResist`, `FrostResist`, `MagicResist`, `DiseaseResist`

</details>

---

## Negation

Use `not` to invert a group of conditions:

```json
{
  "icon": { "source": "DIII/icons.swf", "label": "UnenchantedIcon" },
  "match": {
    "formType": "Weapon",
    "not": {
      "enchantment": {}
    }
  }
}
```

This matches all weapons that are **not** enchanted. Multiple conditions inside `not` are AND-combined, then the result is negated.

---

## Examples

### Quest item icon
```json
{
  "icon": { "source": "DIII/questItemIcon.swf", "label": "QuestItemIcon" },
  "match": { "isQuestItem": true }
}
```

### Fire enchanted weapons
```json
{
  "icon": { "source": "DIII/elements.swf", "label": "FireIcon" },
  "match": {
    "formType": "Weapon",
    "enchantment": { "resistance": "FireResist" }
  }
}
```

### Valuable light armor
```json
{
  "icon": { "source": "DIII/quality.swf", "label": "PremiumIcon" },
  "match": {
    "formType": "Armor",
    "weightClass": "Light",
    "goldValue": { "min": 500 },
    "armorRating": { "min": 30 }
  }
}
```

### Grand soul gems with grand souls
```json
{
  "icon": { "source": "DIII/souls.swf", "label": "FullGrandIcon" },
  "match": {
    "formType": "SoulGem",
    "gemSize": "Grand",
    "soulSize": "Grand"
  }
}
```

### Poisoned favorited weapons
```json
{
  "icon": { "source": "DIII/status.swf", "label": "PoisonedFavIcon" },
  "match": {
    "formType": "Weapon",
    "isFavorited": true,
    "poison": {}
  }
}
```

### Healing potions (specific effect)
```json
{
  "icon": { "source": "DIII/potions.swf", "label": "HealingIcon" },
  "match": {
    "formType": "Potion",
    "potionType": "Medicine",
    "magicEffect": {
      "archetype": "ValueModifier",
      "primaryValue": "Health"
    }
  }
}
```

---

## Schema Validation

Point your editor to the included schema for autocompletion and validation:

```json
{
  "$schema": "./schema.json",
  "rules": [ ... ]
}
```

The schema file is located at `Data/SKSE/Plugins/DIII/schema.json`.
