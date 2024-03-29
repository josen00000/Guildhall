
## Checklist
- [x] *05pts*: Insantiate a physic system into the game.
- [x] *10pts*: Be able to create game objects with a collider based on button press,
    - [x] `1` should create random radius disc centered at cursor
- [x] *10pts*: Camera changes implemented
- [x] *05pts*: Can move around using `WASD` or equivalent (if different, make note in readme)
- [x] *10pts*: Can zoom in and out using mouse-wheel.
    - [x] Make sure this is clamped at a max and min "zoom" so zoom can't go negative.
- [x] *05pts*: Pressing 'O' (capitol letter 'Oh') should reset camera position and zoom to default (ie, move to origin)
- [x] *05pts*: Be sure to render the objects in your scene, by default...
    - [x] Render with a blue border
    - [x] Render with 50% alpha white interior
- [x] *10pts* If mouse is inside the object, render the border yellow.
- [x] *15pts* Be able to drag them around
    - [x] If the mouse is dragging the object, render the border dark green.
- [x] *10pts* If the object is touching another object, render the entire interior at 50% alpha red instead of white.
    - [x] Disc-vs-Disc collision should be same as MP1
- [x] *10pts* If dragging, and `delete` or `backspace` is pressed, destroy object.
- [x] *05pts* Press `Escape` to quit.
- [x] Add `mp2.a02.md` readme to root folder

Makes sure you mouse position is correct for your current view - converting the client position (offset from top-left of window) to a world location.  
