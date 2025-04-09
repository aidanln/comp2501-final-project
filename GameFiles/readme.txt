COMP2501 Course Project Submission by Aidan Lalonde-Novales and Zander Azevedo
Developed on Windows 11
Extension of Aidan's Assignment 3 submission and Week 1's demo code (GameDemo and AudioDemo)
GitHub Repository: https://github.com/aidanln/comp2501-final-projects

=-=-= Controls =-=-=

* Mouse recommended *
- WASD to move
- F to interact and buy collectibles
- K to kill yourself
- Left click to shoot bullets
- Move mouse to aim

=-=-= Design Requirements =-=-=

1. [1] Game mechanic 
- In this game, you have to defeat waves of enemies in order to win. You gain points for shooting/killing enemies, but you lose points for getting hit. There are buyables like weapons and upgrades that you can spend your points on to make the game easier, but your score is tied to your rank if you win (D to S rank), so the less points you spend the harder the game is, but the higher your rank. There are 10 waves, and different amounts of enemies on each wave, which spawn at random spawn points around the map.

2. [1] Enemies
2.1 [ 0.2 ] - 3 different textures for gunner, chaser, and kamikaze enemies
2.2 [ 0.8 ] - 3 different behaviours - gunner orbits, chaser follows your position, and kamikaze uses pursuit (advanced steering behaviour)
- 3 different weapons - gunner shoots bullets, chaser has saw arm, kamikaze hits you directly (maybe add damage over time effect)

3. [1] Weapons
- Weapons player can use: 
- Pistol (first default weapon, semi-auto, green laser sprite)
- SMG (high fire rate, auto, yellow bullet sprite)
- Rifle (lower fire rate, high damage, auto, orange bullet sprite (with cool aura))
- Sniper (lowest fire rate, highest damage, semi-auto, blue bullet sprite)

4. [1] Collectible items
- There are 3 power-ups (that last 20 seconds each) and 4 permanent upgrades
- Power-ups:
- Double points (per kill), cold shock (slows enemies), bullet boost (increased damage)
- Permanent upgrades:
- Celestial augment (increased weapon damage, fire speed), regen coating (faster and greater health regen), nitro-infuser (faster movement speed), armor plating (increase health)
- There are also 3 buyable weapons, which were described above, that can be bought with points

5. [1] Movement and transformations 
- 5.1 [0.4] All movement is handled through setting user's position
- 5.2 [0.6] Movement is done with either physically-based or parametrically-based motion.

6. [0.5] Collision detection
- Direct collision between game objects handled with circle-to-circle collision detection, and all projectile collisions handled with ray-to-circle collision detection.

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
- Kamikaze enemies use pursuit advanced steering behaviour by moving based on the player's movement vector, making for snappy and predictive steering.

12. Bonus
- Implement a vignette lighting effect with an image and changing depth_test and blending methods
