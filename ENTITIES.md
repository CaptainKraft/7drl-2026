This document will act as a specification for entities and AI behaviors.

Each class, Warlock and Druid, will have access to three familiars.

Only one familiar can be alive at any given time, so the action bar should be disabled while a familiar is alive.

## Effects

Status effects will not have an effect on the turn they are applied. For example, if a unit becomes burned, it will
take the first point of fire dmg on the turn after it was applied.

### Burn
- take 1 dmg every other turn for each stack
- adding a stack of burn resets the time to wear off
- wears off in 4 turns

### Disease
- dmg reduced by 1
- wears off in 10 turns

### Poison
- take 1 dmg every other turn
- wears off in 4 turns

### Webbed
- skip every other turn
- wears off in 10 turns

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

### Dash
- move away from target when too close
- move towards closest tile with an aligned enemy
- if target starts/ends turn aligned, dash to current/last position to attack

### Dash Through
- move towards closest tile with an aligned enemy
- dash to last tile at range, dealing damage to all enemies aligned
- if any tile in path of dash is blocked, end at the tile before the first blocked tile
- if target tile is occupied, move other unit to nearest open tile

### Basic Ranged
- move toward target when out of range
- move away from target when too close
- attack target when in range
- range is defined by lower and upper bounds

### Aligned Ranged
- move away from target when too close
- move towards the closest tile that is within a straight line of the target
- attack aligned target within range

### Kamikaze
- move toward target
- when adjacent, explode dealing area damage

### Turret
- does not move
- attacks horizontally/vertically aligned enemy within range

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
- dash: range5, cooldown5

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
- 3hp, 1dmg
- target: nearest enemy
- basic ranged
- applies increasing stacks of burn on attack
- rises from the ashes after 10 turns

### Beholder
- 5hp, 2dmg
- target: nearest aligned enemy
- aligned + diagonal, piercing ranged: 10range

### Reaper
- 10hp, 4dmg
- target: nearest enemy
- attacks all adjacent tiles
- heals self and player for 1hp per enemy hit during attack

## Druid Familiars

### Phoenix
- 5hp, 3dmg
- target: nearest enemy
- basic melee
- applies increasing stacks of burn on attack
- rises from the ashes after 10 turns

### Treant
- 10hp, 2dmg
- target: nearest enemy
- basic melee
- while knockback is on cooldown, avoids enemies and stays close to player
- while knockback is on cooldown, only attacks when an enemy is adjacent
- knocks back target on attack: cooldown5

### Griffon
- 7hp, 5dmg
- target: nearest enemy
- dash through: range5, cooldown7
- basic melee when dash on cooldown

---

## Items
- TBD...
