How to Use:
Open the .exe file under /Run/

 

Operations:
	press P to create player.
	press I to create item.
    	W, A, S, D to move the player
	press left mouse button to fire
	press ESC to quit (not X)
	F1 to enter debug mode
		blue box: camera window
		red circle: camera position (smoothed)
		black circle: goal camera position (not smoothed)
	F2 change camera window state:
		No Window: camera lock. Always follow player move
		Window:	Camera follow player move only when player out of the window 
	F3 change camera snapping state:
		If Use camera snapping, please enable camera windows by pressing "F2"
		Position snapping: Camera window follows player slowly
		Horizontal lock: Camera window follows player slowly, always set window pos.x = player pos.x
		Vertical lock:	Camera window follows player slowly, always set window pos.y = player pos.y
		Position lock: Camera window position = player position
	F4 change camera shake state:
		Position shake: camera shake positionally
		Rotation shake: camera shake rotationally
		Blend shake: camera shake both positionally and rotationally
		press "1" to shake the camera.
	F5 change camera frame state:
		if using camera frame, please enable camera position snapping
		Forward frame: camera always shows where player moving to
		Projectile frame: camera always shows where player facing to
		Cue frame: Camera always shows where item are (if item exist)
		    