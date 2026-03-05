This document will act as a specification for entities and AI behaviors.

## Effects

### Disease
- dmg reduced by 1

### Poison
- take 1 dmg each turn

### Webbed
- skip every other turn
- wears off in 5 turns

### Troll Blood
- come back to life 5 turns after death
- this effect goes away if damaged by fire

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

### Kamikaze
- move toward target
- when adjacent, explode dealing area damage

### Turret
- does not move
- attacks horizontally/vertically/diagonally aligned enemy within range

---

## Enemies

### Goblin Grunt
- 3hp, 2dmg
- basic melee

### Goblin Shaman
- 3hp, 1dmg
- follow nearest ally, follow range: 3-5
- when ally in range has less than max health, heal ally for 2hp, heal cooldown is 5 turns
- when no allies left on the level, flee from the player

### Goblin Warrior
- 5hp, 3dmg
- basic melee

### Slime
- 4hp, 1dmg
- basic melee
- on death, spawns 2 slimes with half max health
    - 4hp splits into 2hp splits into 1hp, then the 1hp slimes will not spawn new ones

### Troll
- 6hp, 4dmg
- basic melee
- has Troll Blood

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
- defend ranged, follow range: 3-5, defense range: 7
- attacks non-webbed targets and applies webbed on attack

### Bat
- 2hp, 2dmg
- can move up to 2 tiles per turn
- dash, cooldown: 5 turns
- follow player, follow range: 3-5

### Beetle
- 1hp, 3dmg
- kamikaze 3area
    - explosion deals fire dmg

### Beholder
- 10hp, 2dmg
- turrent 10range

---

## Items
- TBD...
