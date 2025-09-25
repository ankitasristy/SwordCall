#Sword Call
A multi-level action-adventure game built with C++ and OpenGL graphics, featuring puzzle-solving, platforming, and combat mechanics.
Overview
Sword Call is a three-level game that combines different gameplay mechanics:

Level 1: Word puzzle/synonym matching game
Level 2: Side-scrolling platformer with combat and trivia
Level 3: Hidden object finding and fireball dodging mini-game

Features

Multi-level progression system with level unlocking
Dynamic character animations (running, jumping, turning, attacking)
Enemy AI with different monster types and behaviors
Health system with visual HP bars
Weather effects (rain animation)
Background music and sound effects
Hidden object mechanics
Fireball dodging mini-game
Trivia/riddle system

Game Mechanics
Level 1: Word Puzzle

Find synonyms for given words in a 5x5 grid
5 rounds to complete
Point-based scoring system
Unlock Level 2 upon completion

Level 2: Platformer Combat

Side-scrolling action with animated backgrounds
Combat system using slash attacks ('0' key) and jumping (spacebar)
Different enemy types: slimes (defeat by jumping) and monsters (defeat by slashing)
Health system with damage feedback
Final riddle about Greek mythology
Rain weather effects

Level 3: Multi-phase Adventure

Phase 1: Hidden object finding (Atlas, Amulet, Flame)
Phase 2: Side-scrolling combat with enhanced enemy AI
Phase 3: Fireball dodging mini-game
Special amulet mechanic for invincibility
Advanced monster AI with attack patterns

Controls
KeyActionW/↑Move UpA/←Move LeftS/↓Move DownD/→Move RightSpacebarJump0Slash/AttackKActivate Amulet (Level 3)RRestart (when game over)
Technical Details
Dependencies

iGraphics library (custom graphics library)
OpenGL for rendering
Windows Multimedia for audio (mciSendString)
Standard C libraries (stdio.h, stdlib.h, time.h, string.h)

Assets Required
images/
├── main_menu.bmp
├── background.bmp
├── aboutUs.bmp
├── instruction.bmp
├── button1.bmp, button2.bmp, button3.bmp, button4.bmp
├── backarrow.bmp
├── Character sprites (r1.png, r2.png, j1.png, etc.)
├── Monster sprites (run1-5.png, round1-5.png, etc.)
├── Background images (level2_bg1.png, level2_bg2.png)
├── Hidden objects (atlas.png, amulet.png, flame.png)
├── UI elements (box.png, sword.png, reward.png)
└── Special effects (fire.png, ice.png)

Audios/
├── background.mp3
└── gameover.mp3
Game Structure
Core Systems

State Management: Multiple game states (menu, levels, game over)
Animation System: Frame-based sprite animation
Collision Detection: Rectangle-based collision system
Audio System: Background music and sound effects
Level Progression: Sequential level unlocking

Enemy Types

Slimes: Slow-moving, defeated by jumping
Monsters: Fast-moving, defeated by slashing
Assistant Monsters: Level 3 enemies requiring multiple hits
Main Monster: Boss-type enemy with complex AI patterns

Installation & Setup

Ensure you have the iGraphics library installed
Set up OpenGL development environment
Place all required assets in their respective folders
Compile with appropriate graphics libraries linked
Run the executable

Game Flow

Main Menu: Start, Instructions, About Us
Level Selection: Progressive unlocking system
Level 1: Complete word puzzle to unlock Level 2
Level 2: Survive platformer challenges and answer riddle to unlock Level 3
Level 3: Complete all phases to finish the game

Development Notes

Uses custom animation timing system
Implements state machine pattern for game flow
Modular enemy system with extensible AI
Scalable level progression system
Custom collision detection optimized for 2D gameplay

Future Enhancements

Additional levels and enemy types
Power-up system
High score tracking
Multiple difficulty modes
Enhanced visual effects


Note: This game requires the iGraphics library and proper asset file placement to run correctly.
