This document will act as a specification for entities and AI behaviors.

## Effects

### Disease
- dmg reduced by 1

### Poison
- take 1 dmg each turn

### Webbed
- skip every other turn

---

## Behaviors

### Follow
- stay out of range of enemies
- keep target within range

### Basic Melee
- move toward target
- attack target when adjacent

### Defend Melee
- use follow behavior when no enemies are near target
- basic melee behavior targeting nearest enemy when any enemies are near defense target

### Basic Ranged
- move toward target when out of range
- move away from target when too close
- attack target when in range
- range is defined by lower and upper bounds

### Defend Ranged
- use follow behavior when no enemies are near target
- basic ranged behavior targeting nearest enemy when any enemies are near defense target

### Dash
- move away from target when too close
- move towards the closest tile that is within a straight line of the target
- if target is in a straight line from current position, dash to target and attack

---

## Enemies

### Goblin Grunt
- 3hp, 1dmg
- basic melee behavior

### Goblin Shaman
- TBD...

---

## Familiars

### Rat
- 2hp, 1dmg
- basic melee
- applies disease on attack

### Cobra
- 2hp, 1dmg
- basic melee
- applies poison on attack

### Spider
- 1hp, 0dmg
- defend ranged, follow range: 3-5, defense range: 5
- attacks non-webbed targets and applies webbed on attack

### Bat
- 2hp, 2dmg
- can move up to 2 tiles per turn
- dash, cooldown: 5 turns
- follow player, follow range: 3-5

### Beetle
- 1hp, 1dmg
- basic melee
- explodes, doing 3dmg in an aoe of dist 3

---

## Items
- TBD...
