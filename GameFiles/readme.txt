COMP2501 Course Project Submission by Aidan Lalonde-Novales and Zander Azevedo
Developed on Windows 11
Extension of Aidan's Assignment 3 submission and Week 1's demo code (GameDemo and AudioDemo)


=-=-= Design Requirements =-=-=

1. Game mechanic ->

2. [1] Enemies
2.1 [ 0.2 ] - 3 different textures for gunner, chaser, and kamikaze enemies
2.2 [ 0.8 ] - 3 different behaviours - gunner orbits, chaser follows your position, and kamikaze uses pursuit (advanced steering behaviour)
- 3 different weapons - gunner shoots bullets, chaser has saw arm, kamikaze hits you directly (maybe add damage over time effect)

3. [1] Weapons
- Weapons player can use: 
- Pistol (first default weapon, semi-auto, default grey bullet sprite)
- SMG (high fire rate, auto, yellow bullet sprite)
- Rifle (lower fire rate, high damage, auto, orange bullet sprite (with cool aura))
- Sniper (lowest fire rate, highest damage, semi-auto, green laser sprite)

4. Collectible items

5. Movement and transformations

6. Collision detection

7. [0.5] Game world
- 7.1 [0.25] camera scrolling (updates based on player position)
- 7.2 [0.25] tiled game background (background is much bigger), uses seperate sprite geometry

8. Particle systems
- // still broken //
9. [1] Hierarchical transformation of a chain of at least three links
- Made new arm object class (3 sprites - base, link(middle object), saw), chaser object contains 3 children, each child's position is based off of parent
- e.g. child1 based off of parent, child2 based off of child1, child3 based off of child2
10. UI
- HUD displays different information like time elapsed, player health, enemies alive, wave number, fps (very cool)

11. Advanced method

12. Bonus