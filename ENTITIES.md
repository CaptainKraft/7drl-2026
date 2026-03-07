This document will act as a specification for entities and AI behaviors.

Each class, Warlock and Druid, will have access to three familiars.

Only one familiar can be alive at any given time, so the action bar should be disabled while a familiar is alive.

## Effects

Unless otherwise noted, these will wear off in 5 turns.

### Burn
- take 1 dmg each turn for each stack
- adding a stack of burn resets the time to wear off

### Disease
- dmg reduced by 1

### Poison
- take 1 dmg each turn

### Webbed
- skip every other turn

### Troll Blood
- come back to life 5 turns after death
- this effect is permanent until hit by fire

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
- if target starts/ends turn aligned, dash to current/last position to attack

### Kamikaze
- move toward target
- when adjacent, explode dealing area damage

### Turret
- does not move
- attacks horizontally/vertically/diagonally aligned enemy within range

---

## Enemies

### Rat
- 2hp, 1dmg
- target: nearest enemy
- basic melee
- applies disease on attack

### Spider
- 1hp, 1dmg
- target: nearest enemy
- basic ranged for web attacks, range: 4
- basic melee once target is webbed

### Goblin Grunt
- 3hp, 2dmg
- target: nearest enemy
- basic melee

### Cobra
- 2hp, 1dmg
- target: nearest enemy
- basic melee
- applies poison on attack

### Bat
- 2hp, 2dmg
- target: nearest aligned enemy
- can move up to 2 tiles per turn
- dash, cooldown: 5 turns

### Beetle
- 1hp, 3dmg
- target: nearest enemy
- kamikaze 3area
    - explosion deals fire dmg

### Goblin Shaman
- 3hp, 1dmg
- follow nearest ally, follow range: 3-5
- when ally in range has less than max health, heal ally for 2hp, heal cooldown is 5 turns
- when no allies left on the level, flee from the player

### Goblin Warrior
- 5hp, 3dmg
- target: nearest enemy
- basic melee

### Slime
- 4hp, 1dmg
- target: nearest enemy
- basic melee
- on death, spawns 2 slimes with half max health
    - 4hp splits into 2hp splits into 1hp, then the 1hp slimes will not spawn new ones

### Troll
- 6hp, 4dmg
- target: nearest enemy
- basic melee
- has Troll Blood

---

## Warlock Familiars

### Imp
- 5hp, 3dmg
- target: nearest enemy
- basic ranged
- applies increasing stacks of burn on each attack

### Beholder
- 10hp, 2dmg
- target: nearest aligned enemy
- turrent 10range

### Reaper
- 5hp, 3dmg
- target: nearest enemy

## Druid Familiars

### Pheonix
- 5hp, 3dmg
- target: nearest enemy
- basic melee

### Treant
- 5hp, 3dmg
- target: nearest enemy
- basic melee

### Griffon
- 5hp, 3dmg
- target: nearest enemy
- basic melee

---

## Items
- TBD...
