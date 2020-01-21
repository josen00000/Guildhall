Known Issues: 

How to Use:
Open the .exe file under /Run.
The operation is same as the demo and the instruction
 

Operations:
xbox: 
    left joystick to control the movement and the orientation.
    right joystick to control the barrel orientation.
    press A to shoot

keyboard:
    Press Esc to exit.
    Press P to pause/start the game.
    Holding T to slow the game.
    Holding Y to speed up the game.
    Press N to respawn the player.
    Press F1 to enable/disable debug draw of the game.
    Press F3 to enable/disable no collision of the game.
    Press F4 to enable/disable debug camera of the game.
    Press F8 to restart of the game.

Bonus Feature:
    Brick tile(destroied after hit).
    Water tile(Solid and bullet an pass with low speed).
    Lava tile(Player get hurt)

Deep Learning:
In this assignment, I find that I am not good at Object management. I met many bugs after I delete certain entity and 
try to add new entity. It should be checked in some moment, like before spawn, and it should not be checked in some time, 
like when I check the collision between entities.(Cause every entities must exist then it can occur collision) However,
I current check all objects in every step to make sure it exist. I have done namy redaudant work. 
I still find out my code reusebility is not good. When I am working on this assignment, I have to modify the code from 
last assignment and sometimes it is really hard because I need to modify the whole frame of the code. I should leave enough 
space for the future code.
I still find out some bad coding habit. When I was coding, I focus on the detail of the code. If I met some tiny bug, I tried 
to fix it directly without considering. However, sometimes the bugs appear because my struct is wrong or my idea is wrong. After
spend much time on it, I reconsider about the struct and the idea and fix it. As a result, it waste my time. I should focus more on
the whole project when I am coding.

